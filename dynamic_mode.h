#ifndef DYNAMIC_MODE_H
#define DYNAMIC_MODE_H

#include <Arduino.h>   // for bool / uint8_t on AVR & ESP cores
#include <stdint.h>    // for uint8_t on other toolchains

/* ----------------------------------------------------------------
 *                         CONSTANTS
 * ----------------------------------------------------------------*/
#define NUM_LOCATIONS_DBL   12     // full-court doubles
#define NUM_LOCATIONS_SNG    6     // half-court singles
#define HISTORY_SIZE_DBL     3
#define HISTORY_SIZE_SNG     2
#define OPTIONS_PER_LOCATION 4     // unchanged

/* ----------------------------------------------------------------
 *                     GLOBAL CONFIGURATION
 * ----------------------------------------------------------------*/
extern bool    currentIsDoubles;     // true = doubles, false = singles
extern uint8_t currentDifficulty;    // 1-hard, 2-medium, 3-easy

/* ----------------------------------------------------------------
 *                        HISTORY BUFFERS
 * ----------------------------------------------------------------*/
extern int      shot_history_dbl[HISTORY_SIZE_DBL];
extern int      shot_history_sng[HISTORY_SIZE_SNG];
extern uint8_t  hist_index_dbl;
extern uint8_t  hist_index_sng;

/* ----------------------------------------------------------------
 *                       TABLE ACCESSORS
 * (These are implemented in the .cpp; they hide the six priority
 *  arrays so user code never needs to touch them directly.)
 * ----------------------------------------------------------------*/
int  getOpt(uint8_t location, uint8_t option);          // read
void setOpt(uint8_t location, uint8_t option, int val); // write

/* ----------------------------------------------------------------
 *                         PUBLIC API
 * ----------------------------------------------------------------*/
void setCourtMode(bool isDoubles);
void setDifficulty(uint8_t difficulty);                      // recent-shot test
void shiftSelectedToBack(uint8_t location, int sel);
int  selectNextShot();                              // core decision engine
void addToHistory(int shot);                        // push into queue

#endif /* DYNAMIC_MODE_H */