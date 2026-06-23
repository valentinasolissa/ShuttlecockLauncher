#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* --------------------------- constants -------------------------- */
#define NUM_LOCATIONS_DBL      12
#define NUM_LOCATIONS_SNG       6
#define HISTORY_SIZE_DBL        3
#define HISTORY_SIZE_SNG        2
#define OPTIONS_PER_LOCATION    4

/* ---------------------- global configuration -------------------- */
bool     currentIsDoubles   = false;   // default court
uint8_t  currentDifficulty  = 3;      // 1-hard, 2-medium, 3-easy

/* -------------------------- history ----------------------------- */
int shot_history_dbl[HISTORY_SIZE_DBL] = { -1, -1, -1 };
int shot_history_sng[HISTORY_SIZE_SNG] = { -1, -1 };
uint8_t hist_index_dbl = 0;
uint8_t hist_index_sng = 0;

/* ---------------- doubles priority tables ---------------------- */
int opt_hard_dbl[NUM_LOCATIONS_DBL * OPTIONS_PER_LOCATION] = {
  12,  8,  6,  4,   11,  7,  5,  3,   9,  6,  2,  4,
   9,  5,  7,  2,    4, 10, 12,  7,   1, 11,  9,  8,
   2,  9, 10, 12,    1,  5, 10,  3,   3,  7,  6, 11,
   4,  8,  2,  9,    1,  6,  5, 10,   2,  7,  4,  9
};

int opt_med_dbl[NUM_LOCATIONS_DBL * OPTIONS_PER_LOCATION] = {
   6,  8,  4, 12,    7,  5,  3, 11,   6,  9,  2,  4,
   5,  9,  2,  7,   10,  4,  7, 12,  11,  1,  8,  9,
   9,  2, 12, 10,    5,  1,  3, 10,   7,  3, 11,  6,
   8,  4,  9,  2,    6,  1, 10,  5,   7,  2,  9,  4
};

int opt_easy_dbl[NUM_LOCATIONS_DBL * OPTIONS_PER_LOCATION] = {
   8, 12,  6,  4,    7, 11,  5,  3,   6,  9,  2,  4,
   5,  9,  2,  7,   10,  4,  7, 12,   1, 11,  8,  9,
   9,  2, 10, 12,    5,  1, 10,  3,   7,  3, 11,  6,
   8,  4,  9,  2,    6,  1,  5, 10,   7,  2,  4,  9
};

/* ---------------- singles priority tables ---------------------- */
int opt_hard_sng[NUM_LOCATIONS_SNG * OPTIONS_PER_LOCATION] = {
   11,  7,  6, 10,   // 2
   10,  6,  7, 11,   // 3
    3,  7, 11,  2,   // 6
    2,  6, 10,  3,   // 7
    3, 11,  7,  2,   // 10
    6,  2, 10,  3    // 11
};

/* ------------ singles medium (remapped) ---------- */
int opt_med_sng[NUM_LOCATIONS_SNG * OPTIONS_PER_LOCATION] = {
    7, 11,  6,  3,   // 2
    6, 10,  7,  2,   // 3
    7,  3, 11,  2,   // 6
    6,  2, 10,  3,   // 7
    7,  3, 11,  6,   // 10
    6,  2, 10,  7    // 11
};

/* ------------ singles easy (remapped) ------------ */
int opt_easy_sng[NUM_LOCATIONS_SNG * OPTIONS_PER_LOCATION] = {
    7,  3, 11,  2,   // 2
    6,  2, 10,  3,   // 3
    7,  3,  6, 11,   // 6
    6,  7,  2, 10,   // 7
   11,  3, 10,  7,   // 10
   10,  2, 11,  6    // 11
};
/* =============================================================== *
 *                 LOW-LEVEL ARRAY ACCESSORS                       *
 * =============================================================== */

int  numLocations(bool d) { return d ? NUM_LOCATIONS_DBL : NUM_LOCATIONS_SNG; }
int  historySize (bool d) { return d ? HISTORY_SIZE_DBL  : HISTORY_SIZE_SNG;  }

/* Return value at (location, option) for the current mode */
// int getOpt(uint8_t location, uint8_t option)
// {
//     uint8_t idx = (location - 1) * OPTIONS_PER_LOCATION + option;

//     if (currentIsDoubles) {
//         if (currentDifficulty == 1) return opt_hard_dbl[idx];
//         if (currentDifficulty == 2) return opt_med_dbl[idx];
//         return                       opt_easy_dbl[idx];
//     } else {
//         if (currentDifficulty == 1) return opt_hard_sng[idx];
//         if (currentDifficulty == 2) return opt_med_sng[idx];
//         return                       opt_easy_sng[idx];
//     }
// }
int getOpt(uint8_t location, uint8_t option)
{
    uint8_t idx;

    if (currentIsDoubles) {
        idx = (location - 1) * OPTIONS_PER_LOCATION + option;
        if (currentDifficulty == 1) return opt_hard_dbl[idx];
        if (currentDifficulty == 2) return opt_med_dbl[idx];
        return                       opt_easy_dbl[idx];
    } else {
        const uint8_t singlesLocations[] = {2, 3, 6, 7, 10, 11};
        const int numSingles = sizeof(singlesLocations) / sizeof(singlesLocations[0]);

        int mappedIdx = -1;
        for (int i = 0; i < numSingles; ++i) {
            if (singlesLocations[i] == location) {
                mappedIdx = i;
                break;
            }
        }

        if (mappedIdx == -1) {
            // Handle error: invalid location
            return -1;
        }

        idx = mappedIdx * OPTIONS_PER_LOCATION + option;

        if (currentDifficulty == 1) return opt_hard_sng[idx];
        if (currentDifficulty == 2) return opt_med_sng[idx];
        return                       opt_easy_sng[idx];
    }
}

/* Overwrite value at that position */
// void setOpt(uint8_t location, uint8_t option, int value)
// {
//     uint8_t idx = (location - 1) * OPTIONS_PER_LOCATION + option;

//     if (currentIsDoubles) {
//         if (currentDifficulty == 1) opt_hard_dbl[idx] = value;
//         else if (currentDifficulty == 2) opt_med_dbl[idx] = value;
//         else opt_easy_dbl[idx] = value;
//     } else {
//         if (currentDifficulty == 1) opt_hard_sng[idx] = value;
//         else if (currentDifficulty == 2) opt_med_sng[idx] = value;
//         else opt_easy_sng[idx] = value;
//     }
// }
void setOpt(uint8_t location, uint8_t option, int value)
{
    uint8_t idx;

    if (currentIsDoubles) {
        idx = (location - 1) * OPTIONS_PER_LOCATION + option;

        if (currentDifficulty == 1) opt_hard_dbl[idx] = value;
        else if (currentDifficulty == 2) opt_med_dbl[idx] = value;
        else                           opt_easy_dbl[idx] = value;
    } else {
        const uint8_t singlesLocations[] = {2, 3, 6, 7, 10, 11};
        const int numSingles = sizeof(singlesLocations) / sizeof(singlesLocations[0]);

        int mappedIdx = -1;
        for (int i = 0; i < numSingles; ++i) {
            if (singlesLocations[i] == location) {
                mappedIdx = i;
                break;
            }
        }

        if (mappedIdx == -1) {
            // Handle error: invalid location
            return;
        }

        idx = mappedIdx * OPTIONS_PER_LOCATION + option;

        if (currentDifficulty == 1) opt_hard_sng[idx] = value;
        else if (currentDifficulty == 2) opt_med_sng[idx] = value;
        else                            opt_easy_sng[idx] = value;
    }
}
/* =============================================================== *
 *                      PUBLIC HELPERS                             *
 * =============================================================== */

/* Select court & difficulty (NO POINTERS NEEDED) */
void setCourtMode(bool isDoubles)
{
    // change court only if it’s different
    if (currentIsDoubles != isDoubles) {
        currentIsDoubles = isDoubles;

        /* clear the history that will now be active */
        if (isDoubles) {
            for (uint8_t i = 0; i < HISTORY_SIZE_DBL; ++i) shot_history_dbl[i] = -1;
            hist_index_dbl = 0;
        } else {
            for (uint8_t i = 0; i < HISTORY_SIZE_SNG; ++i) shot_history_sng[i] = -1;
            hist_index_sng = 0;
        }
    }
}

/* ---------------- difficulty selector ---------------- */
void setDifficulty(uint8_t difficulty)
{
    /* clamp to 1-3; default to 1 (hard) on bad input  */
    if (difficulty < 1 || difficulty > 3) difficulty = 1;
    currentDifficulty = difficulty;
}
/* True if SHOT is in recent history */
bool isInHistory(int shot)
{
    if (currentIsDoubles) {
        for (uint8_t i = 0; i < HISTORY_SIZE_DBL; ++i)
            if (shot_history_dbl[i] == shot) return true;
    } else {
        for (uint8_t i = 0; i < HISTORY_SIZE_SNG; ++i)
            if (shot_history_sng[i] == shot) return true;
    }
    return false;
}

/* Rotate priority list so SELECTED becomes last */
void shiftSelectedToBack(uint8_t location, int selected)
{
    int temp[OPTIONS_PER_LOCATION];
    uint8_t t = 0;

    /* copy all others first */
    for (uint8_t i = 0; i < OPTIONS_PER_LOCATION; ++i) {
        int v = getOpt(location, i);
        if (v != selected) temp[t++] = v;
    }
    temp[t] = selected;                     // put chosen one last

    /* write back */
    for (uint8_t i = 0; i < OPTIONS_PER_LOCATION; ++i)
        setOpt(location, i, temp[i]);
}

/* Add shot to history */
void addToHistory(int shot)
{
    if (currentIsDoubles) {
        shot_history_dbl[hist_index_dbl] = shot;
        hist_index_dbl = (hist_index_dbl + 1) % HISTORY_SIZE_DBL;
    } else {
        shot_history_sng[hist_index_sng] = shot;
        hist_index_sng = (hist_index_sng + 1) % HISTORY_SIZE_SNG;
    }
}

/* Core decision engine */
int selectNextShot()
{
    uint8_t hSize = historySize(currentIsDoubles);
    int*     hist = currentIsDoubles ? shot_history_dbl : shot_history_sng;
    uint8_t  hIdx = currentIsDoubles ? hist_index_dbl   : hist_index_sng;

    int last = hist[(hIdx + hSize - 1) % hSize];

    /* First shot → random */
    if (last == -1) {
        if (currentIsDoubles) {
            int r = random(1, numLocations(currentIsDoubles) + 1);
            return r;
        } else {
            int options[] = {2, 3, 6, 7, 10, 11};
            int r = options[random(0, 6)]; // random index from 0 to 5
            return r;
        }
    }

    /* Walk priority list */
    for (uint8_t i = 0; i < OPTIONS_PER_LOCATION; ++i) {
        int cand = getOpt(last, i);
        if (!isInHistory(cand)) {
            shiftSelectedToBack(last, cand);
            return cand;
        }
    }

    /* All were in history: take top priority */
    return getOpt(last, 0);
}

