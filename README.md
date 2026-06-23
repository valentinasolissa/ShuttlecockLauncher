# Angry Birdie: Advanced Badminton Training Partner

### Project Overview
Angry Birdie is a portable, intelligent shuttlecock launcher designed to provide realistic, game-speed solo training for beginner-to-intermediate badminton athletes.
By bridging the gap between high-end club machines and basic hobby devices, it offers an adaptive, interactive training experience.


<img width="600" height="419" alt="Screenshot 2026-06-23 at 10 07 11 AM" src="https://github.com/user-attachments/assets/21f6c652-cb8c-46f8-9107-0b1eb9bbb49e" />


### Technical Architecture
Utilizes an integrated hardware and software stack to achieve game-realistic shot delivery.

### Hardware Design

<img width="571" height="281" alt="Screenshot 2026-06-23 at 10 08 27 AM" src="https://github.com/user-attachments/assets/703652c3-7afc-4fca-83b6-b12c9690fad5" />

- Launch Mechanism: The launcher utilizes twin high-speed counter-rotating wheels controlled by brushless motors to impart velocity and spin on the shuttlecock.
- Trajectory Control: The system employs servos to control horizontal and vertical rotation, facilitating delivery to 12 distinct locations on the opponent's court.
- Feeding System: A gravity-drop arm system is utilized to ensure jam-free feeding, using 1 servo for gripping and 1 servo for stopping/releasing the shuttlecock.
- Electronics & Control: The core control logic is handled by an ESP32 microcontroller, which communicates with an android mobile app to manage modes, and motor speeds and angles.


<img width="484" height="281" alt="Screenshot 2026-06-23 at 1 01 51 PM" src="https://github.com/user-attachments/assets/3338c6ca-7f09-4e4e-be13-af673d834ced" />

