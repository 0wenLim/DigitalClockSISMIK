/*  Program Jam Digital Superloop
    By Wifal Inola (13219002) and Owen Limvin (13219064)
    Kelompok 13    
*/

//include library
#include <TM1637.h>
#include <PinChangeInterrupt.h>

//declare variable
int secs=50;
int mins=58;
int hours=23;
int alarmMins=59;
int alarmHours=23;
int stopwatchSecs=0;
int stopwatchMins=0;
int state = 0;

//declare function
void addSecs(); 
void addMins();
void addHours();
void addMinsAlarm();
void addHoursAlarm();
void addSecsStopwatch();
void addMinsStopwatch();
void resetStopwatch();
void pauseStopwatch();
void displayTime(int biggerTime, int smallerTime);
void tekan1();
void tekan2();
void changeMode();

//declare pin
int CLKPin = 5;
int DIOPin = 6;
TM1637 tm1637(CLKPin,DIOPin);
int button1 = 2;
int button2 = 3;
int button3 = 4;
int buzzer = 8;

void setup() {  
  cli();  // Disable Interrupt
 
  //initialize timer register
  TCCR1A = 0; // initialise register TCCR1A
  TCCR1B = 0; // initialise register TCCR1B
  OCR1A = 62500; // trigger interrupt A at 1s
  TCCR1B |= (1<<CS12) | (1<<WGM12); // set pre-scalar 256 and CTC mode
  TIMSK1 |= (1<<OCIE1A) ; // enable OCR1A
   
  sei();  // Enable Interrupt

  //initialize TM1637
  tm1637.init();
  tm1637.set();

  //IO
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(buzzer, OUTPUT);

 //Interrupt Initialization
  attachInterrupt(digitalPinToInterrupt(button1), tekan1, RISING);
  attachInterrupt(digitalPinToInterrupt(button2), tekan2, RISING);
  attachPCINT(digitalPinToPCINT(button3), changeMode, RISING);
}

void loop() {  
  if(state==0){
    displayTime(hours, mins);
  }    
  else if(state==1){
    displayTime(alarmHours, alarmMins);
  }
  else if(state==2){
    displayTime(stopwatchMins, stopwatchSecs);
  }
  
  if((mins == alarmMins) && (hours == alarmHours)) {
    digitalWrite(buzzer, HIGH);
  }
  else{
    digitalWrite(buzzer, LOW);
  }
}

// Interrupt Service Routine 
ISR(TIMER1_COMPA_vect){
  TCNT1  = 0;
  addSecs();
  addSecsStopwatch();
}

// Fungsi menambah detik jam digital
void addSecs(){
  if (secs < 59 && secs >= 0) {
    secs++;
  }
  else if (secs == 59) {
    secs = 0;
    addMins();
  }
}

// Fungsi menambah menit jam digital
void addMins(){
  if (mins < 59 && mins >= 0) {
    mins++;
  }
  else if (mins == 59) {
    mins = 0;
    addHours();
  }
}

// Fungsi menambah jam di jam digital
void addHours(){
  if (hours < 23 && hours >= 0) {
    hours++;
  }
  else if (hours == 23) {
    hours = 0;
  }
}

// Fungsi menambah menit alarm jam digital
void addMinsAlarm(){
  if (alarmMins < 59 && alarmMins >= 0) {
    alarmMins++;
  }
  else if (alarmMins == 59) {
    alarmMins = 0;
  }
}

// Fungsi menambah jam alarm di jam digital
void addHoursAlarm(){
  if (alarmHours < 23 && alarmHours >= 0) {
    alarmHours++;
  }
  else if (alarmHours == 23) {
    alarmHours = 0;
 }
}

// Fungsi menambah detik stopwatch 
void addSecsStopwatch(){
  if (stopwatchSecs < 59 && stopwatchSecs >= 0) {
    stopwatchSecs++;
  }
  else if (stopwatchSecs == 59) {
    stopwatchSecs = 0;
    addMinsStopwatch();
  }
}

// Fungsi menambah menit stopwatch
void addMinsStopwatch(){
  if (stopwatchMins < 59 && stopwatchMins >= 0) {
    stopwatchMins++;
  }
  else if (stopwatchSecs == 59) {
    stopwatchMins = 0;
  }
}

// Fungsi reset stopwatch
void resetStopwatch(){
  stopwatchMins=0;
  stopwatchSecs=0;
}

// Fungsi pause stopwatch
void pauseStopwatch(){
  while(digitalRead(button2)){
    stopwatchSecs=stopwatchSecs;
    stopwatchMins=stopwatchMins;  
  }
}

// Fungsi display jam, alarm, dan stopwatch
void displayTime(int biggerTime, int smallerTime) {
  tm1637.display(0, (biggerTime/10)%10); //puluhan waktu yang lebih besar (jam atau menit)
  tm1637.display(1, biggerTime%10);  //satuan waktu yang lebih besar (jam atau menit)
  tm1637.point(1);
  tm1637.display(2, (smallerTime/10)%10); //puluhan waktu yang lebih kecil (jam atau menit)
  tm1637.display(3, smallerTime%10);  //satuan waktu yang lebih kecil (jam atau menit)
}

// Fungsi apabila button1 ditekan
void tekan1(){
  if(state==0){   //mode jam digital
    addMins();
  }
  else if (state==1){ // mode alarm
    addMinsAlarm();
  }
  else if (state==2){ // mode stopwatch
    resetStopwatch();
  }
}

// Fungsi apabila button2 ditekan
void tekan2(){
  if(state==0){ // mode jam digital
    addHours();
  }
  else if (state==1){ // mode alarm
    addHoursAlarm();
  }
  else if (state==2){ // mode stopwatch
    pauseStopwatch();
  }
}

// Fungsi apabila button3 ditekan untuk mengubah mode
void changeMode(){
  if(state==0){   //jam digital -> alarm
      state = 1;  
  }    
  else if(state==1){  // alarm -> stopwatch
      state = 2;
  }
  else if(state==2){  // stopwatch -> jam digital
      state = 0;
  }  
}
