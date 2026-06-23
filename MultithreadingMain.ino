#include "BluetoothSerial.h"
#include <ESP32Servo.h>
#include "dynamic_mode.h"
#include <unistd.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

int sharedState = 0;
int threadDelay = 20;
SemaphoreHandle_t mutex;

#define I2C_SDA 33
#define I2C_SCL 32

Adafruit_8x8matrix matrix1 = Adafruit_8x8matrix();
Adafruit_8x8matrix matrix2 = Adafruit_8x8matrix();
     
Adafruit_8x8matrix matrix3 = Adafruit_8x8matrix();
Adafruit_8x8matrix matrix4 = Adafruit_8x8matrix();


BluetoothSerial SerialBT;

int    currentAngle = 98; 
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


void position0();
void feed();
void drop();
void position1_fast();
void position2_fast();
void position3_fast();
void position4_fast();
void position5_fast();
void position6_fast();
void position7_fast();
void position8_fast();
void position9_fast();
void position10_fast();
void position11_fast();
void position12_fast();
void run_dynamic_mode(int rounds);
void run_dynamic_mode_singles(int rounds);
void run_random_mode(int rounds);
void run_random_mode_singles(int rounds);
int getButtonPressCount();
void mode1LEDon();
void moveServoSmooth(int target);

// void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
// void displayInteger(int number, int i2cFileDesc);


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



// Task on core 0: reads sharedState and prints it
 
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
	// mybrushless.attach(19, 1000, 2500);   
  // mybrushless2.attach(18,1000,2500);
mybrushless.attach(18, 1000, 2500);   
  mybrushless2.attach(19,1000,2500);  // mygripper.attach(27,1000,2500);
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
  mygripper.write(71); // 68
  mystopper.write(105);

  delay(2000); // delay to wait for confirmation beeps
  delay(3000);
  
  ///////////////////////////////////////////////////////
  // Initialize the output variables as outputs
  pinMode(2, OUTPUT);
  // Set outputs to LOW
  digitalWrite(2, LOW);
  
  SerialBT.begin("Angry Birdie");
  pinMode(ledPin, OUTPUT);
  // put your setup code here, to run once:
  Serial.println("Ready");
  
  xTaskCreatePinnedToCore(
    TaskReader,        // Task function
    "State Reader",    // Task name
    2048,              // Stack size
    NULL,              // Parameter
    1,                 // Priority
    NULL,              // Task handle
    0                  // Core 0
  );

  mutex = xSemaphoreCreateMutex();
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


void TaskReader(void *pvParameters) {
  // Serial.printf("TASKREADERCORE%d\n", xPortGetCoreID());
  for (;;) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    int localCopy = sharedState;
    xSemaphoreGive(mutex);
    
    switch (localCopy) {
      case 1:
// Serial.println("BLUETOOTH OFF");
        matrix1.clear();
        matrix1.drawBitmap(0, 0,  BLUETOOTH1_1_bmp , 8, 8, LED_ON);
        matrix1.writeDisplay();

        matrix3.clear();
        matrix3.drawBitmap(0, 0,  BLUETOOTH2_3_bmp, 8, 8, LED_ON);
        matrix3.writeDisplay();        
    vTaskDelay(pdMS_TO_TICKS(250));  // Delay 1 second
        // Serial.println("BLUETOOTH ON");
        matrix1.clear();
        matrix1.drawBitmap(0, 0,  ON1_bmp , 8, 8, LED_ON);
        matrix1.writeDisplay();

        matrix3.clear();
        matrix3.drawBitmap(0, 0,  ON3_bmp, 8, 8, LED_ON);
        matrix3.writeDisplay();
    vTaskDelay(pdMS_TO_TICKS(250));  // Delay 1 second

        
        break;
      case 2:
        // Serial.println("BLUETOOTH ON");
        matrix1.clear();
        matrix1.drawBitmap(0, 0,  BLUETOOTH1_1_bmp , 8, 8, LED_ON);
        matrix1.writeDisplay();

        matrix3.clear();
        matrix3.drawBitmap(0, 0,  BLUETOOTH2_3_bmp, 8, 8, LED_ON);
        matrix3.writeDisplay();
        break;
      case 3:
        // Serial.println("MODE 1 BIRDIES 6");
        break;
      case 4:
        // Serial.println("BIRDIES 1");
        matrix4.clear();
        matrix4.drawBitmap(0, 0, N1_4_bmp, 8, 8, LED_ON);
        matrix4.writeDisplay();
        break;
      case 5:
        // Serial.println("BIRDIES 2");
        matrix4.clear();
        matrix4.drawBitmap(0, 0, N2_4_bmp, 8, 8, LED_ON);
        matrix4.writeDisplay();
        break;
      case 6:
       // Serial.println("BIRDIES 3");
        matrix4.clear();
        matrix4.drawBitmap(0, 0, N3_4_bmp, 8, 8, LED_ON);
        matrix4.writeDisplay();
        break;
      case 7:
       // Serial.println("BIRDIES 4");
        matrix4.clear();
        matrix4.drawBitmap(0, 0, N4_4_bmp, 8, 8, LED_ON);
        matrix4.writeDisplay();
        break;
      case 8:
        //Serial.println("BIRDIES 5");
        matrix4.clear();
        matrix4.drawBitmap(0, 0, N5_4_bmp, 8, 8, LED_ON);
        matrix4.writeDisplay();        break;
      case 9:
        //Serial.println("BIRDIES 6");
        matrix4.clear();
        matrix4.drawBitmap(0, 0, N6_4_bmp, 8, 8, LED_ON);
        matrix4.writeDisplay();
        break;
      case 10:
        //Serial.println("MODE 3");
        matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay();  
        break;
      case 11:
        //Serial.println("MODE 2");
        matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode2_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay();
        break;
      case 12:
       // Serial.println("MODE 4 ");
        matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode4_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay();
        break;
      case 13:
        //Serial.println("MODE 1");
        matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode1_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay();

        break;
      case 14: // on rhs
        matrix2.clear();      
        matrix2.drawBitmap(0, 0,  ON2_bmp , 8, 8, LED_ON);
        matrix2.writeDisplay();
        break;
      case 15:
        matrix4.clear();
        matrix4.drawBitmap(0, 0,  ON4_bmp, 8, 8, LED_ON);
        matrix4.writeDisplay(); 
        break;
      default:
        Serial.println("Unknown state");
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(threadDelay/2));  // Delay 1 second
  }
}

void loop() {
// Serial.printf("MAINCORE%d\n", xPortGetCoreID());
  static int state = 1;
  // put your main code here, to run repeatedly:
  static int lastHandledCount = 0;          // remembers the last count we acted on
  int count = getButtonPressCount(); 

  if ((count & 1) && count != lastHandledCount) {
    lastHandledCount = count;               // mark this odd press as handled
    bool abortLoop = false;

    vTaskDelay(pdMS_TO_TICKS(1000));
    xSemaphoreTake(mutex, portMAX_DELAY); //mode 4
    sharedState = 12;
    xSemaphoreGive(mutex);
    vTaskDelay(pdMS_TO_TICKS(500));  

    xSemaphoreTake(mutex, portMAX_DELAY); // 6 birdies
    sharedState = 9;
    xSemaphoreGive(mutex);
  
    vTaskDelay(pdMS_TO_TICKS(1000));

    if (!SerialBT.hasClient()){
    vTaskDelay(pdMS_TO_TICKS(threadDelay));  

    xSemaphoreTake(mutex, portMAX_DELAY); //  led flash if no bluetooth
    sharedState = 1;
    xSemaphoreGive(mutex);
    }

    for (int i = 0; i < 6 && !abortLoop; ++i) {
      Serial.println("RUNRANDOM");
      run_random_mode(1);

      for (int j = 0; j < 500; ++j) {
        if ((getButtonPressCount() & 1 == 0) || (digitalRead(buttonPin) == 0) ) {   // even ⇒ cancel
          abortLoop = true;
          Serial.println("STOOP");
          position0();
          break;
        }
        delay(10);
      }
    }
    position0();

  }else if (SerialBT.available()) {
    // matrix1.clear();
    // matrix1.drawBitmap(0, 0,  BLUETOOTH1_1_bmp , 8, 8, LED_ON);
    // matrix1.writeDisplay();

    // matrix3.clear();
    // matrix3.drawBitmap(0, 0,  BLUETOOTH2_3_bmp, 8, 8, LED_ON);
    // matrix3.writeDisplay();

    char c = SerialBT.read();
    Serial.println(c);
    // if(c=='1'){
    //   digitalWrite(ledPin, HIGH);
    // }else if (c=='0'){
    //   digitalWrite(ledPin,LOW);
    // }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////


    //if (header.indexOf("GET /Mode1/pos0") >= 0) {
    if (c == 'a') {
              mode1LEDon();

      Mode = "Mode1";
      mode2active = false;
      Status = "OFF";
      digitalWrite(2, HIGH);
      position0();
      Serial.println("MODE1off");

    }else if (c == 't') { //mode 3 of
      
      mode3active = true;
      // Mode = "Mode1";
      mode2active = false;
      Status = "OFF";
      digitalWrite(2, HIGH);
      position0();
      Serial.println("MODE1off");
      

    }else if (c == 'A') { //bluetooth on
    /* matrix1.clear();
    matrix1.drawBitmap(0, 0,  BLUETOOTH1_1_bmp , 8, 8, LED_ON);
    matrix1.writeDisplay();

    matrix3.clear();
    matrix3.drawBitmap(0, 0,  BLUETOOTH2_3_bmp, 8, 8, LED_ON);
    matrix3.writeDisplay(); */
       xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 2;
      xSemaphoreGive(mutex);
      


    //}else if (header.indexOf("GET /Mode2/pos0") >= 0) { //mode 2 off
    }else if (c == 'b') {
      Mode = "Mode2";
      mode2active = false;
      digitalWrite(2, HIGH);
      position0();
      Serial.println("000000000");
      
      // matrix2.clear();
      // matrix2.drawBitmap(0, 0, Mode2_2_bmp, 8, 8, LED_ON);
      // matrix2.writeDisplay();
      xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 11;
        xSemaphoreGive(mutex);

    //}else if (header.indexOf("GET /Mode1/slow") >= 0) {
    }else if (c == 'L') {
     
      digitalWrite(2, HIGH);
      position0();
     
      xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 12;
        xSemaphoreGive(mutex);

    //}else if (header.indexOf("GET /Mode1/slow") >= 0) {
    }else if (c == 'c') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);
      }else{
        mode1LEDon();
      }
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, HIGH);
      fast = false;
      FAST_SLOW = "Slow";
      
    //}else if (header.indexOf("GET /Mode1/fast") >= 0) {
    }else if (c == 'd') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex); 
      }else{
        mode1LEDon();
      }      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, HIGH);
      fast = true;
      FAST_SLOW = "Fast";
      
    // }else if (header.indexOf("GET /Mode1/pos10") >= 0) {
      }else if (c == 'e') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position10_fast();
                            Serial.println("dynamic 10 fast");

      }else{
        position10_slow();
                            Serial.println("dynamic 10 slow");

      }
                    Serial.println("10101010101010");

      /*
      myservo.write(35);
      mybrushless.write(0);
      myservoHORIZONTAL.write(140);
      */

    // }else if (header.indexOf("GET /Mode1/pos11") >= 0) {
      }else if (c == 'f') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);

      if(fast){
        position11_fast();
                            Serial.println("dynamic 11 fast");

      }else{
        position11_slow();
                            Serial.println("dynamic 11 slow");

      }                             Serial.println("11111111111");


    // }else if (header.indexOf("GET /Mode1/pos12") >= 0) {
      }else if (c == 'g') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      Serial.println("12");
      if(fast){
        position12_fast();
                            Serial.println("dynamic 12 fast");

      }else{
        position12_slow();
                            Serial.println("dynamic 12 slow");

      }
                      Serial.println("12121212121212");


    // }else if (header.indexOf("GET /Mode1/pos1") >= 0) {
      }else if (c == 'h') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position1_fast();
                            Serial.println("dynamic 1 fast");

      }else{
        position1_slow();
                            Serial.println("dynamic 1 slow");

      }                                                        
      Serial.println("111");

      /*
      myservo.write(10);
      myservoHORIZONTAL.write(120);
      mybrushless.write(25);
      */
    // }else if (header.indexOf("GET /Mode1/pos2") >= 0) {
      }else if (c == 'i') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      

      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, HIGH);
      if(fast){
        position2_fast();
                            Serial.println("dynamic 2 fast");

      }else{
        position2_slow();
                            Serial.println("dynamic 2 slow");

      }

                                                Serial.println("22222222");

      /*
      myservo.write(20);
      myservoHORIZONTAL.write(120);
      mybrushless.write(25);
      */
    // }else if (header.indexOf("GET /Mode1/pos3") >= 0) {
      }else if (c == 'j') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position3_fast();
                            Serial.println("dynamic 3 fast");

      }else{
        position3_slow();
                            Serial.println("dynamic 3 slow");

      }
                                                              Serial.println("333333333");

      /*
      myservo.write(25);
      myservoHORIZONTAL.write(120);
      mybrushless.write(30); 
      */
    // }else if (header.indexOf("GET /Mode1/pos4") >= 0) { //////////////////////////////////////////////////////
      }else if (c == 'k') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position4_fast();
                            Serial.println("dynamic 4 fast");

      }else{
        position4_slow();
                            Serial.println("dynamic 4 slow");

      }                                                        Serial.println("4444444");

      /*         
      myservo.write(25);
      mybrushless.write(30);
      myservoHORIZONTAL.write(140);
      */
    // }else if (header.indexOf("GET /Mode1/pos5") >= 0) {
      }else if (c == 'l') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position5_fast();
                            Serial.println("dynamic 5 fast");

      }else{
        position5_slow();
                            Serial.println(" 5 slow");

      }                                                        Serial.println("5555555");

      /*          
      myservo.write(10);
      mybrushless.write(25);
      myservoHORIZONTAL.write(100);
      */

    // }else if (header.indexOf("GET /Mode1/pos6") >= 0) {
      }else if (c == 'm') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
     Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position6_fast();
                            Serial.println("dynamic 6 fast");

      }else{
        position6_slow();
                            Serial.println("dynamic 6 slow");

      }                                                        Serial.println("6666666");

      /*
      myservo.write(20);
      mybrushless.write(25);
      myservoHORIZONTAL.write(100);
      */

    // }else if (header.indexOf("GET /Mode1/pos7") >= 0) {
      }else if (c == 'n') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
      
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position7_fast();
                            Serial.println("dynamic 7 fast");

      }else{
        position7_slow();
                            Serial.println("dynamic 7 slow");

      }                                          Serial.println("77777777777777777");

      /*
      myservo.write(25);
      mybrushless.write(30);
      myservoHORIZONTAL.write(100);
      */
    // }else if (header.indexOf("GET /Mode1/pos8") >= 0) {
      }else if (c == 'o') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      

      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position8_fast();
                            Serial.println("dynamic 8 fast");

      }else{
        position8_slow();
                            Serial.println("8 slow");

      }                                                        Serial.println("88888888");

      /*
      myservo.write(10);
      mybrushless.write(25);
      myservoHORIZONTAL.write(140);
      */

    // }else if (header.indexOf("GET /Mode1/pos9") >= 0) {
      }else if (c == 'p') {
      if(mode3active){
        /* matrix2.clear();
        matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
        matrix2.writeDisplay(); */  
        xSemaphoreTake(mutex, portMAX_DELAY);
        sharedState = 10;
        xSemaphoreGive(mutex);  
      }else{
        mode1LEDon();
      }      
  
      Mode = "Mode1";
      mode2active = false;
      digitalWrite(2, LOW);
      if(fast){
        position9_fast();
          Serial.println("9 fast");
                            Serial.println("dynamic 9 fast");

      }else{
        position9_slow();
          Serial.println("9 slow");
                              Serial.println("9 slow");

      }                                                        Serial.println("999999999999999999");

      /*
      myservo.write(20);
      mybrushless.write(25);
      myservoHORIZONTAL.write(140);
      */


    }else if (c == 's') { //mode3 led routine
      Serial.println("MODE 3 LED");
      /* matrix2.clear();
      matrix2.drawBitmap(0, 0, Mode3_2_bmp, 8, 8, LED_ON);
      matrix2.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 10;
      xSemaphoreGive(mutex);
      mode3active = true;
    }else if (c == 'B') { //mode2 led on
      // matrix2.clear();
      // matrix2.drawBitmap(0, 0, Mode2_2_bmp, 8, 8, LED_ON);
      // matrix2.writeDisplay();
      
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 11;
      xSemaphoreGive(mutex);
    vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  
            xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 9;
      xSemaphoreGive(mutex);

    }else if (c == 'C') { //mode1 led on
      // matrix2.clear();
      // matrix2.drawBitmap(0, 0, Mode1_2_bmp, 8, 8, LED_ON);
      // matrix2.writeDisplay();
      mode3active = false;

      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 13;
      xSemaphoreGive(mutex);   
       vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 9;
      xSemaphoreGive(mutex);
    }else if (c == 'D') { //mode3 led on
      mode3active = true;
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 10;
            xSemaphoreGive(mutex);

      vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 9;
      xSemaphoreGive(mutex);
      
    }else if (c == 'E') { //mode2 led on
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 12;
      xSemaphoreGive(mutex);
      vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 9;
      xSemaphoreGive(mutex);
   // }else if (header.indexOf("GET /Mode2running") >= 0) {
    }else if (c == 'q') {
      /*   matrix2.clear();
      matrix2.drawBitmap(0, 0, Mode2_2_bmp, 8, 8, LED_ON);
      matrix2.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 11;
      xSemaphoreGive(mutex);
        Mode = "Mode2";
        mode2active = true;
Serial.println("BEFORE numruns: " + String(numruns) + "");
      Serial.println("BeFORE DYMAIC");
        run_dynamic_mode(1);
         //run_dynamic_mode_singles(1);
              Serial.println("BeFORE DYMAIC");

        numruns++;
        Serial.println("AFTER numruns: " + String(numruns) + "");

        if(numruns >= birdieCountGlobal){
          numruns = 0;
                            Serial.println("BUTTONS ON ON ON ON ON ON");

        }

      digitalWrite(2, HIGH);
      
    }else if (c == 'F') {
      /*   matrix2.clear();
      matrix2.drawBitmap(0, 0, Mode2_2_bmp, 8, 8, LED_ON);
      matrix2.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 11;
      xSemaphoreGive(mutex);
        Mode = "Mode2";
        mode2active = true;
Serial.println("BEFORE numruns: " + String(numruns) + "");
      Serial.println("BeFORE DYMAIC");
        //run_dynamic_mode(1);
         run_dynamic_mode_singles(1);
              Serial.println("BeFORE DYMAIC");

        numruns++;
        Serial.println("AFTER numruns: " + String(numruns) + "");

        if(numruns >= birdieCountGlobal){
          numruns = 0;
                            Serial.println("BUTTONS ON ON ON ON ON ON");

        }

      digitalWrite(2, HIGH);
      
    }else if (c == 'r') {
    //     matrix2.clear();
    // matrix2.drawBitmap(0, 0, Mode4_2_bmp, 8, 8, LED_ON);
    // matrix2.writeDisplay();
    xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 12;
      xSemaphoreGive(mutex);

        Mode = "Mode3";
        mode2active = false;
Serial.println("BEFORE numruns: " + String(numruns) + "");
      Serial.println("BeFORE random");
        run_random_mode(1);

        numruns++;
        Serial.println("AFTER numruns: " + String(numruns) + "");

        if(numruns >= birdieCountGlobal){
          numruns = 0;
                            Serial.println("BUTTONS ON ON ON ON ON ON");

        }

      digitalWrite(2, HIGH);
      
    }else if (c == 'G') { //random singles
    //     matrix2.clear();
    // matrix2.drawBitmap(0, 0, Mode4_2_bmp, 8, 8, LED_ON);
    // matrix2.writeDisplay();
    xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 12;
      xSemaphoreGive(mutex);
        Mode = "Mode3";
        mode2active = false;
Serial.println("BEFORE numruns: " + String(numruns) + "");
      Serial.println("BeFORE random");
        run_random_mode_singles(1);

        numruns++;
        Serial.println("AFTER numruns: " + String(numruns) + "");

        if(numruns >= birdieCountGlobal){
          numruns = 0;
                            Serial.println("BUTTONS ON ON ON ON ON ON");

        }

      digitalWrite(2, HIGH);
      
    }else if(c == 'x') {
      Serial.println("Difficulty set to EASY");
      Difficulty = "EASY";
                      // Mode = "Mode2";

      setDifficulty(3);
    }
    else if (c == 'y') {
      Serial.println("Difficulty set to MEDIUM");
      Difficulty = "MEDIUM";
                      // Mode = "Mode2";

      setDifficulty(2);
    }
    else if (c == 'z') {
      Serial.println("Difficulty set to HARD");
      Difficulty = "HARD";
                      // Mode = "Mode2";

      setDifficulty(1);     

    }else if (c == '1') {
      
      Serial.println("1birdie");

      /* matrix4.clear();
      matrix4.drawBitmap(0, 0, N1_4_bmp, 8, 8, LED_ON);
      matrix4.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 4;
      xSemaphoreGive(mutex);
     
    }else if (c == '2') {
    
      Serial.println("2birdie");

      /* matrix4.clear();
      matrix4.drawBitmap(0, 0, N2_4_bmp, 8, 8, LED_ON);
      matrix4.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 5;
      xSemaphoreGive(mutex);

      
    }else if (c == '3') {
      Serial.println("3birdie");

      /* matrix4.clear();
      matrix4.drawBitmap(0, 0, N3_4_bmp, 8, 8, LED_ON);
      matrix4.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 6;
      xSemaphoreGive(mutex);

      
    }else if (c == '4') {
            Serial.println("4birdie");

      /* matrix4.clear();
      matrix4.drawBitmap(0, 0, N4_4_bmp, 8, 8, LED_ON);
      matrix4.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 7;
      xSemaphoreGive(mutex);

      
    }else if (c == '5') {
               Serial.println("5birdie");

      /* matrix4.clear();
      matrix4.drawBitmap(0, 0, N5_4_bmp, 8, 8, LED_ON);
      matrix4.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 8;
      xSemaphoreGive(mutex);

      
    }else if (c == '6') {
                     Serial.println("6birdie");

      /* matrix4.clear();
      matrix4.drawBitmap(0, 0, N6_4_bmp, 8, 8, LED_ON);
      matrix4.writeDisplay(); */
      xSemaphoreTake(mutex, portMAX_DELAY);
      sharedState = 9;
      xSemaphoreGive(mutex);
  
    }
  }else if (!SerialBT.hasClient()) {
    /* matrix1.clear();
    matrix1.drawBitmap(0, 0,  BLUETOOTH1OFF_1_bmp , 8, 8, LED_ON);
    matrix1.writeDisplay();

    matrix3.clear();
    matrix3.drawBitmap(0, 0,  BLUETOOTH2OFF_3_bmp, 8, 8, LED_ON);
    matrix3.writeDisplay(); */
  //     xSemaphoreTake(mutex, portMAX_DELAY);
  // sharedState = 14;
  // xSemaphoreGive(mutex);
    // vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  

xSemaphoreTake(mutex, portMAX_DELAY);
    sharedState = 14;
    xSemaphoreGive(mutex);

      vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  
xSemaphoreTake(mutex, portMAX_DELAY);
    sharedState = 15;
    xSemaphoreGive(mutex);
          vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  
    xSemaphoreTake(mutex, portMAX_DELAY);
    sharedState = 1;
    xSemaphoreGive(mutex);
  vTaskDelay(pdMS_TO_TICKS(threadDelay+1));  


  }

}
  /////////////////////////////////////////////////////////////////////////////////////////////////////////


void feed()
{
 // myservo.write(6);
 // delay(100);
 // myservo.write(20);
  mygripper.write(42);
  //delay(speed);
  delay(500);
  // mygripper.write(72);
  mygripper.write(71);
}

void drop()
{
  mystopper.write(50);
  delay(700);
  mystopper.write(105);

  // myservoHORIZONTAL.write(110);
  // mybrushless.write(0);
}

void position0()
{
  myservo.write(0);
  moveServoSmooth(99);
  mybrushless.write(0);
      mybrushless2.write(0);

}

void position1_fast()
{
  mybrushless.write(25); //init writes
  mybrushless2.write(25);
 // moveServoSmooth(100);
  delay(750);
 // moveServoSmooth(120);
  // delay(750);


  myservo.write(0);
  // moveServoSmooth(140); //old valus that turn too much
  // mybrushless.write(21);
  // mybrushless2.write(21);

  // diff wheel speed values
  moveServoSmooth(125); 
  mybrushless.write(20);
  mybrushless2.write(35);
  feed();
  delay(revupdelay);
  drop();
}

void position2_fast()
{
  mybrushless.write(25); //init writes
  mybrushless2.write(25);
 //   moveServoSmooth(100);

  delay(750);
 // moveServoSmooth(105);
  // delay(750);

  myservo.write(0);
  moveServoSmooth(110);
  mybrushless.write(19);
  mybrushless2.write(19);

  feed();
  delay(revupdelay);
  drop();
}

void position3_fast()
{
  mybrushless.write(25); //init writes
  mybrushless2.write(25);
 //   moveServoSmooth(100);
  delay(750);
 // moveServoSmooth(95);
  //delay(750);

  myservo.write(0);
  moveServoSmooth(90);
  mybrushless.write(19);
  mybrushless2.write(19);

  feed();
  delay(revupdelay);
  drop();
}
void position4_fast()
{
  mybrushless.write(25); //init writes
  mybrushless2.write(25);
   // moveServoSmooth(100);

   // delay(750);
  //moveServoSmooth(80);
  delay(750);

  myservo.write(0);
  // moveServoSmooth(62);
  // mybrushless.write(20);
  // mybrushless2.write(20);

  // diff wheel speed values
  moveServoSmooth(65); 
  mybrushless.write(27);
  mybrushless2.write(18);

  feed();
  delay(revupdelay);
  drop();
}

void position5_fast()
{
  mybrushless.write(25); //init writes
  mybrushless2.write(25);
  //  moveServoSmooth(100);

  delay(750);
 // moveServoSmooth(110);
 // delay(750);
  myservo.write(5);
  moveServoSmooth(125);
  mybrushless.write(27);
  mybrushless2.write(27);
  feed();
  delay(revupdelay);
  drop();
}
void position6_fast()
{
  mybrushless.write(25); //init writes
  mybrushless2.write(25);
  //  moveServoSmooth(100);

  delay(750);
 //moveServoSmooth(105);
//  delay(750);
  myservo.write(5);
  moveServoSmooth(110);
  mybrushless.write(25);
  mybrushless2.write(25);


  feed();
  delay(revupdelay);
  drop();
}

void position7_fast()
{
  mybrushless.write(25); //init writes
  mybrushless2.write(25);
  //  moveServoSmooth(105);

  delay(750);
  //moveServoSmooth(100);
 // delay(750);
  myservo.write(5);
  moveServoSmooth(97);
  mybrushless.write(25);
  mybrushless2.write(25);


  feed();
  delay(revupdelay);
  drop();
}

void position8_fast()
{

  mybrushless.write(25); //init writes
  mybrushless2.write(25);
  //  moveServoSmooth(100);

  delay(750);
  //moveServoSmooth(90);
 // delay(750);
  myservo.write(5);
  moveServoSmooth(80);
  mybrushless.write(29);
  mybrushless2.write(29);


  feed();
  delay(revupdelay);
  drop();
}

void position9_fast()
{

  mybrushless.write(25); //init writes
  mybrushless2.write(25);
  //  moveServoSmooth(100);

  delay(750);
  //moveServoSmooth(110);
 // delay(750);
  myservo.write(0);
  moveServoSmooth(122);
  mybrushless.write(35);
    mybrushless2.write(35);


  feed();
  delay(revupdelay);
  drop();
}

void position10_fast()
{

    mybrushless.write(25); //init writes
  mybrushless2.write(25);
 //   moveServoSmooth(100);

  delay(750);
 // moveServoSmooth(102);
  //delay(750);  
  myservo.write(0);
  moveServoSmooth(105);
  mybrushless.write(35);
      mybrushless2.write(35);


  feed();
  delay(revupdelay);
  drop();
}

void position11_fast()
{
    mybrushless.write(25); //init writes
  mybrushless2.write(25);
  //  moveServoSmooth(100);

  delay(750);
  //moveServoSmooth(98);
 // delay(750);
  myservo.write(0);
  moveServoSmooth(95);
  mybrushless.write(35);
      mybrushless2.write(35);


  feed();
  delay(revupdelay);
  drop();
}

void position12_fast()
{

    mybrushless.write(25); //init writes
  mybrushless2.write(25);
   // moveServoSmooth(100);

  delay(750);
 // moveServoSmooth(90);
  //delay(750);
  myservo.write(0);
  moveServoSmooth(83);
  mybrushless.write(35);
      mybrushless2.write(35);


  feed();
  delay(revupdelay);
  drop();
}


void position1_slow()
{
  myservo.write(10);
  moveServoSmooth(150);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position2_slow()
{
  myservo.write(10);
  moveServoSmooth(130);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position3_slow()
{
  myservo.write(10);
  moveServoSmooth(110);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}
void position4_slow()
{
  myservo.write(10);
  moveServoSmooth(90);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position5_slow()
{
  myservo.write(20);
  moveServoSmooth(150);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}
void position6_slow()
{
  myservo.write(20);
  moveServoSmooth(130);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position7_slow()
{
  myservo.write(20);
  moveServoSmooth(110);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position8_slow()
{
  myservo.write(20);
  moveServoSmooth(90);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position9_slow()
{
  myservo.write(20);
  moveServoSmooth(150);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position10_slow()
{
  myservo.write(20);
  moveServoSmooth(130);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position11_slow()
{
  myservo.write(20);
  moveServoSmooth(110);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}

void position12_slow()
{
  myservo.write(20);
  moveServoSmooth(90);
  mybrushless.write(0);

  feed();
  delay(revupdelay);
  drop();
}


void execute_shot(int position, bool isfast)
{
  switch (position) {
    case 1:
        if(isfast){
          position1_fast();
          Serial.println("dynamic 1 fast");
        }else{
          position1_slow();
                    Serial.println("dynamic 1 slow");

        }
        break;
    case 2:
        if(isfast){
          position2_fast();
                    Serial.println("dynamic 2 fast");

        }else{
          position2_slow();
                              Serial.println("dynamic 2 slow");

        }        
        break;
    case 3:
        if(isfast){
          position3_fast();
                    Serial.println("dynamic 3 fast");

        }else{
          position3_slow();
                              Serial.println("dynamic 3 slow");

        }        
        break;
    case 4:
        if(isfast){
          position4_fast();
                    Serial.println("dynamic 4 fast");

        }else{
          position4_slow();
                              Serial.println("dynamic 4 slow");

        }          
        break;
    case 5:
        if(isfast){
          position5_fast();
                    Serial.println("dynamic 5 fast");

        }else{
          position5_slow();
                              Serial.println("dynamic 5 slow");

        }        
        break;
    case 6:
        if(isfast){
          position6_fast();
                    Serial.println("dynamic 6 fast");

        }else{
          position6_slow();
                              Serial.println("dynamic 6 slow");

        }                
        break;
    case 7:
        if(isfast){
          position7_fast();
                    Serial.println("dynamic 7 fast");

        }else{
          position7_slow();
                              Serial.println("dynamic 7 slow");

        }        
        break;
    case 8:
        if(isfast){
          position8_fast();
                    Serial.println("dynamic 8 fast");

        }else{
          position8_slow();
                              Serial.println("dynamic 8 slow");

        }                
        break;
    case 9:
        if(isfast){
          position9_fast();
                    Serial.println("dynamic 9 fast");

        }else{
          position9_slow();
                              Serial.println("dynamic 9 slow");

        }         
        break;
    case 10:
        if(isfast){
          position10_fast();
                    Serial.println("dynamic 10 fast");

        }else{
          position10_slow();
                              Serial.println("dynamic 10 slow");

        }         
        break;
    case 11:
        if(isfast){
          position11_fast();
                    Serial.println("dynamic 11 fast");

        }else{
          position11_slow();
                              Serial.println("dynamic 11 slow");

        }         
        break;
    case 12:
        if(isfast){
          position12_fast();
                    Serial.println("dynamic 12 fast");

        }else{
          position12_slow();
                              Serial.println("dynamic 12 slow");

        }         
        break;
    default:
        position0();
        break;
  }
}

bool random_true_false() {
    int random_number = rand() % 100;  // Random number between 0 and 99
    if (random_number < 100) {
        return true;  // 60% chance
    } else {
        return false; // 40% chance
    }
}



void run_dynamic_mode(int rounds) {
      setCourtMode(true); //is doubles
      for (int i = 0; i < rounds; i++) {
        int next_shot = selectNextShot();
        
        addToHistory(next_shot);
        delay(5);
        execute_shot(next_shot, random_true_false());
        delay(5);
        
        Serial.print("DOUBLES ");
        Serial.print("Round ");
        Serial.print(i + 1);
        Serial.print(": Shot to location ");
        Serial.print(next_shot);
        Serial.print(" | History: [");

        for (int j = 0; j < HISTORY_SIZE_DBL; j++) {
            Serial.print(shot_history_dbl[j]);
            if (j < HISTORY_SIZE_DBL - 1) Serial.print(", ");
        }

        Serial.println("]");

    }
}

void run_dynamic_mode_singles(int rounds) {
      setCourtMode(false); //is doubles
      for (int i = 0; i < rounds; i++) {
        int next_shot = selectNextShot();
        
        addToHistory(next_shot);
        delay(5);
        execute_shot(next_shot, random_true_false());
        delay(5);
        
        Serial.print("SINGLES ");
        Serial.print("Round ");
        Serial.print(i + 1);
        Serial.print(": Shot to location ");
        Serial.print(next_shot);
        Serial.print(" | History: [");

        for (int j = 0; j < HISTORY_SIZE_SNG; j++) {
            Serial.print(shot_history_sng[j]);
            if (j < HISTORY_SIZE_SNG - 1) Serial.print(", ");
        }

        Serial.println("]");

    }
}

void run_random_mode(int rounds) {
    for (int i = 0; i < rounds; i++) {
        int next_shot = random(1, 13); // different from dynamic       
        delay(5);
        execute_shot(next_shot, random_true_false());
        delay(5);
        

        Serial.print("Round ");
        Serial.print(i + 1);
        Serial.print(": Random Shot to location ");
        Serial.print(next_shot);

    }
}

void run_random_mode_singles(int rounds) {
    static const uint8_t choices[6] = { 2, 3, 6, 7, 10, 11 };
              
    for (int i = 0; i < rounds; i++) {
        int next_shot = choices[random(0, 6)];// different from dynamic       
        delay(5);
        execute_shot(next_shot, random_true_false());
        delay(5);
        

        Serial.print("Round ");
        Serial.print(i + 1);
        Serial.print(": Random Shot to location ");
        Serial.print(next_shot);

    }
}

int getButtonPressCount()
{
  static int  stableState      = HIGH;              // debounced level
  static int  readingState     = HIGH;              // last raw read
  static unsigned long lastToggleTimeMs = 0;        // debounce timer
  static int  pressCount       = 0;                 // what we return

  int current = digitalRead(buttonPin);             // raw reading

  if (current != readingState) {                    // edge detected
    readingState     = current;
    lastToggleTimeMs = millis();                    // restart timer
  }

  if ((millis() - lastToggleTimeMs) > debounceDelay // stable long enough
      && current != stableState) {

    stableState = current;                          // accept new state
    if (stableState == LOW) {                       // button *pressed*
      ++pressCount;
    }
  }
  return pressCount;
}

void mode1LEDon(){
    // matrix2.clear();
    // matrix2.drawBitmap(0, 0, Mode1_2_bmp, 8, 8, LED_ON);
    // matrix2.writeDisplay();
    
    xSemaphoreTake(mutex, portMAX_DELAY); //mode 4
    sharedState = 13;
    xSemaphoreGive(mutex);
}

void moveServoSmooth(int target)
{
    /* --- clamp to valid Servo angles (0-180) just in case --- */
    // if (target < 0)   target = 0;
    // if (target > 180) target = 180;

    if (target > currentAngle) {
        for (int a = currentAngle + 1; a <= target; ++a) {
            myservoHORIZONTAL.write(a);
            delay(10);
        }
    } else if (target < currentAngle) {
        for (int a = currentAngle - 1; a >= target; --a) {
            myservoHORIZONTAL.write(a);
            delay(10);
        }
    } else {
        /* Already there */
        myservoHORIZONTAL.write(target);
    }

    currentAngle = target;     // update global reference
}

// void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value) {
//     unsigned char buff[2];
//     buff[0] = regAddr;
//     buff[1] = value;

//     int res = write(i2cFileDesc, buff, 2);
//     if (res != 2) {
//         perror("Unable to write i2c register");
// 		exit(-1);
//     }
// }


// void displayInteger(int number, int i2cFileDesc) {
//     int tens = number/10; //Left digit
//     int ones = number%10; //Right digit

//     write2Digits(tens, ones, i2cFileDesc);

//     //Display a space in the last row
//     writeI2cReg(i2cFileDesc, REG_ROW_BASE - 2*(8-1), 0b00000000);
// }
