#include "BluetoothSerial.h"
#include <ESP32Servo.h>
//#include "dynamic_mode.h"
#include <unistd.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define I2C_SDA 33
#define I2C_SCL 32

Adafruit_8x8matrix matrix1 = Adafruit_8x8matrix();
Adafruit_8x8matrix matrix2 = Adafruit_8x8matrix();
     
Adafruit_8x8matrix matrix3 = Adafruit_8x8matrix();
Adafruit_8x8matrix matrix4 = Adafruit_8x8matrix();


BluetoothSerial SerialBT;

const int ledPin = 2;
Servo myservo, myservoHORIZONTAL, mybrushless, mygripper, mystopper, mybrushless2;  
bool mode2active;// Set web server port number to 80
bool fast = true;
bool mode3active = false;
int mode2iterations = 0;
int birdieCountGlobal = 1;
int numruns = 0;
int revupdelay = 2500;
String header;
String Mode = "None";
String Status = "off";
String FAST_SLOW = "Fast";
String Position = "0"; // 0 = off
String Difficulty = "";
// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output16 = 16;
const int output17 = 17;

//button
const int  buttonPin      = 25;      // active-low, pull-up enabled
const long debounceDelay  = 25;  

int speed = 0;

static const uint8_t PROGMEM
  ON1_bmp[] =
  { B11111111,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001 },
  ON4_bmp[] =
  { B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B11111111 },
  ON3_bmp[] =
  { B11111111,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000,
    B10000000 },
  ON2_bmp[] =
  { B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B11111111 },

  BLUETOOTH1_1_bmp[] = 
  {  B11111111,
   B00000001,
   B00100001,
   B01000001,
   B11111001,
   B01010001,
   B00100001,
   B00000001},
   BLUETOOTH2_3_bmp[] =
  {  B11111111,
   B10000000,
   B10000010,
   B10000001,
   B10001111,
   B10000101,
   B10000010,
   B10000000},

  BLUETOOTH1OFF_1_bmp[] =
  { B11111111,
    B00000001,
    B11000001,
    B00100001,
    B10100001,
    B01100001,
    B11100001,
    B00010001},

  BLUETOOTH2OFF_3_bmp[] =
  { B11111111,
    B10000100,
    B10000011,
    B10000011,
    B10000010,
    B10000010,
    B10000001,
    B10000000},
  M1_3_bmp[] =
  { B11100011,
    B11010101,
    B11001001,
    B10011100,
    B10001000,
    B10001000,
    B10011100,
    B11111111 },
  M2_3_bmp[] =
  { B11100011,
    B11010101,
    B11001001,
    B10111110,
    B10010100,
    B10010100,
    B10111110,
    B1111111 },
  M3_3_bmp[] =
  { B11100011,
    B11010101,
    B11001001,
    B11111111,
    B10101010,
    B10101010,
    B10101010,
    B11111111 },
  M4_3_bmp[] =
  { B11100011,
    B11010101,
    B11001001,
    B11111111,
    B10101010,
    B10100100,
    B11111111,
    B11111111 },
    
   N6_4_bmp[] =
  { B10000000,
    B10000000,
    B10111111,
    B10101001,
    B10101001,
    B10111001,
    B10000000,
    B11111111},
   N1_4_bmp[] =
  { B10000000,
    B10000000,
    B10100100,
    B10100010,
    B10111111,
    B10100000,
    B10000000,
    B11111111 },
    
    N2_4_bmp[] =
  { B10000000,
    B10000000,
    B10100110,
    B10110001,
    B10101001,
    B10100111,
    B10000000,
    B11111111 },
    
    N3_4_bmp[] =
  { B10000000,
    B10000000,
    B11001001,
    B11001001,
    B11001001,
    B11111111,
    B10000000,
    B11111111 },

  N4_4_bmp[] =
  { B10000000,
    B10000000,
    B10001111,
    B10001000,
    B10001000,
    B10111111,
    B10000000,
    B11111111 },
    N5_4_bmp[] =
  { B10000000,
    B11001111,
    B11001001,
    B11001001,
    B11001001,
    B11111001,
    B10000000,
    B11111111 },  

    Mode1_2_bmp[] =
  { B00000001,
    B00000001,
    B01000101,
    B01111101,
    B01000101,
    B00000001,
    B00000001,
    B11111111 },

    Mode2_2_bmp[] =
  { B00000001,
    B01000101,
    B01111101,
    B01000101,
    B01111101,
    B01000101,
    B00000001,
    B11111111 },
  Mode3_2_bmp[] =
  { B00000001,
    B01111101,
    B00000001,
    B01111101,
    B00000001,
    B01111101,
    B00000001,
    B11111111 },

  Mode4_2_bmp[] =
  { B00111001,
    B00000001,
    B00001001,
    B00010001,
    B00100001,
    B00010001,
    B00001001,
    B11111111 },
  // ON3_bmp[] =
  // { B10000000,
  //   B10000000,
  //   B10000000,
  //   B10000000,
  //   B10000000,
  //   B10000000,
  //   B10000000,
  //   B11111111 },
  // ON4_bmp[] =
  // { B00000001,
  //   B00000001,
  //   B00000001,
  //   B00000001,
  //   B00000001,
  //   B00000001,
  //   B00000001,
  //   B11111111 },
  OFF_bmp[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 };






void setup() {
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservoHORIZONTAL.setPeriodHertz(50);
  mybrushless.setPeriodHertz(50);
  mybrushless2.setPeriodHertz(50);
  mygripper.setPeriodHertz(50);
  mystopper.setPeriodHertz(50);
	myservo.attach(13, 1000, 2500); //
  myservoHORIZONTAL.attach(12,1000,2500);
	mybrushless.attach(14, 1000, 2500);   
  mybrushless2.attach(27,1000,2500);
  // mygripper.attach(27,1000,2500);
  mygripper.attach(17,1000,2500);
	mystopper.attach(26, 1000, 2500);   

  //button
  pinMode(buttonPin, INPUT_PULLUP); //pin 25

  //i2c LED matrix mapping
  Wire.begin(I2C_SDA,I2C_SCL);
  
  ///////////////////////////////////////////////////////
  matrix4.begin(0x70);  // pass in the address
  matrix2.begin(0x73);  // pass in the address
  matrix3.begin(0x72);
  matrix1.begin(0x71);

  ///////////////////////////////////////////////////////

  // delay(3000); // delay to plug power in 
  // myservo.write(0);
  // myservoHORIZONTAL.write(120);
  // mybrushless.write(0);
  // mygripper.write(24);
  // mystopper.write(90);

 delay(3000); // delay to plug power in 
  myservo.write(20);
  myservoHORIZONTAL.write(99);
  mybrushless.write(0);
  mybrushless2.write(0);
  mygripper.write(72);
  mystopper.write(65);

  delay(2000); // delay to wait for confirmation beeps
  delay(3000);
  
  ///////////////////////////////////////////////////////
  // Initialize the output variables as outputs
  pinMode(2, OUTPUT);
  // Set outputs to LOW
  digitalWrite(2, LOW);
  
  SerialBT.begin("ESP32-Bluetooth");
  pinMode(ledPin, OUTPUT);
  // put your setup code here, to run once:
  Serial.println("Ready");
  

  // vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  

  // xSemaphoreTake(mutex, portMAX_DELAY);
  // //sharedState = 14;
  // sharedState = 11;
  // xSemaphoreGive(mutex);
  
  // vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  
  
  // xSemaphoreTake(mutex, portMAX_DELAY);
  // sharedState = 1;
  // xSemaphoreGive(mutex);

  
/*   matrix1.clear();
  matrix1.drawBitmap(0, 0,  BLUETOOTH1OFF_1_bmp , 8, 8, LED_ON);
  matrix1.writeDisplay();

  matrix3.clear();
  matrix3.drawBitmap(0, 0,  BLUETOOTH2OFF_3_bmp, 8, 8, LED_ON);
  matrix3.writeDisplay();

  matrix2.clear();
  matrix2.drawBitmap(0, 0,  ON2_bmp , 8, 8, LED_ON);
  matrix2.writeDisplay();

  matrix4.clear();
  matrix4.drawBitmap(0, 0,  ON4_bmp, 8, 8, LED_ON);
  matrix4.writeDisplay(); */


}



// ─────────── Helper: wait for user, grab one integer ───────────
int readAngle() {
  digitalWrite(ledPin, HIGH);           // LED on → waiting
  while (Serial.available() == 0) { }   // block until at least one byte arrives
  digitalWrite(ledPin, LOW);            // LED off → something received

  int angle = Serial.parseInt();        // read the integer
  while (Serial.available()) Serial.read();  // flush any extra chars
  angle = constrain(angle, 0, 270);     // keep it legal
  Serial.println(angle);                // echo back

  return angle;
}
void feed()
{
 // myservo.write(6);
 // delay(100);
 // myservo.write(20);
  mygripper.write(45);
  //delay(speed);
  delay(500);
  mygripper.write(65);
}
void drop()
{
  mystopper.write(24);
  delay(700);
  mystopper.write(95);

  // myservoHORIZONTAL.write(110);
  // mybrushless.write(0);
}
// ───────────────────────── MAIN LOOP ─────────────────────────
void loop() {

  Serial.println("\nEnter angle for VERTICAL SERVO 1 (0-20)(20 flat, 0 up):");
  int a1 = readAngle();

  Serial.println("Enter angle for HORIZONTAL SERVO 2 (0-270): (center is 98)");
  int a2 = readAngle();

  Serial.println("Enter speed for BRUSHLESS 3 (0-180):"); //lhs motor looking at from back sping more to go right
  int a3 = readAngle();

  Serial.println("Enter speed for BRUSHLESS 4 (0-180):"); ///rhs motor looking at from back sping more to go left
  int a4 = readAngle();

  Serial.println("✓ All servos updated, running");
  delay(50);                      // small pause before next cycle
  
  myservo.write(a1);
  
  myservoHORIZONTAL.write(a2);
  
   mybrushless.write(25);
  mybrushless2.write(25);
  delay(2000);

  mybrushless.write(a3);
  mybrushless2.write(a4);
  delay(5000);

  feed();
  delay(2500);
  drop();
  delay(2500);
  mybrushless.write(0);
  mybrushless2.write(0);
}