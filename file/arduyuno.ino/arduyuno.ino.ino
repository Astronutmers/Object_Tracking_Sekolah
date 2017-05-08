/*
   TA Elektro 064
   Arduino Master (Modul Unit Sentral)
   Penghubung Modul Pengendali Kamera dan Modul Pendeteksi Guru
*/

#include <Wire.h>

//Define i2c address
#define SLAVE_ADDRESS_FRONT 0x04
#define SLAVE_ADDRESS_BACK 0x07

// Variabel Modul Pengendali Kamera
String cmd;    //command to be sent to slaves
String cmm;
int ans = 0;    //monitoring whether cmd is received right or not (by listening to reply)

// Variabel Modul Pendeteksi Guru
int ledPin = 13;        // choose the pin for the LED
int inputPin = 2;      // choose the input pin (for PIR sensor)
int val = 0;           // variable for reading the pin status
int count = 0;         // count "ada" (HIGH) state

int addr;
char cmdc;

void setup() {
  Serial.begin(9600);

  //Modul Pendeteksi Guru
  //----------------------
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input

  // Waktu kalibrasi modul: 12 detik
  Serial.println("Calibration start");
  delay(1000);
  Serial.println("Calibration end!");
  delay(20);

  /*
  cli();
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 46875;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();*/

  //Modul Pengendali Kamera
  //---------------------
  // initialize i2c as master
  Wire.begin();
  delay(100);

  //disable internal pull-up
  digitalWrite(A4, 0);
  digitalWrite(A5, 0);

  //set SCL to low frequency
  Wire.setClock(10000L);
}

void loop() {
 
  while (Serial.available()>0){

    cmdc = Serial.read();
    cmd = String(cmdc);
    char buffer[2];
    cmm = cmd.substring(1);
    cmm.toCharArray(buffer, 2);
  
    
    if(cmd.substring(0)=="1"){
    Wire.beginTransmission(SLAVE_ADDRESS_FRONT);
    Wire.write(buffer);
    delay(10);
    Wire.endTransmission();
      
    }

    if(cmd.substring(0)=="2"){
    Wire.beginTransmission(SLAVE_ADDRESS_BACK);
    Wire.write(buffer);
    delay(10);
    Wire.endTransmission();
  
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    count++;
    Serial.println("1");
  }
  else {
    count = 0;
    digitalWrite(ledPin, LOW); // turn LED OFF
    //Serial.println("0");
  }

  //second reading achieved, 4 secs after first reading (each read takes 3 secs)
  if (count == 4) {
    count = 0;
    digitalWrite(ledPin, HIGH);
    Serial.println("bambam");
  }
}


