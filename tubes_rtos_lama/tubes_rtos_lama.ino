//include library
#include <TM1637.h>
#include <Arduino_FreeRTOS.h>

//declare variable
int secs;
int mins;
int hours;
int alarmMins=0;
int alarmHours=0;

//declare function
void addSecs();
void addMins();
void addHours();
void displayTime(int hours, int minutes);

//declare pin
int CLKPin = 5;
int DIOPin = 6;
TM1637 tm1637(CLKPin,DIOPin);
int button1 = 2;
int button2 = 3;
int button3 = 4;
int buzzer = 8;

void setup() {
  Serial.begin(9600);

  //waktu awal
  secs = 50;
  mins = 59;
  hours = 23;
  
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
 
  attachInterrupt(digitalPinToInterrupt(button1), tekan1, RISING);
  attachInterrupt(digitalPinToInterrupt(button2), tekan2, RISING);

  //inisialisasi thread
  xTaskCreate(displayClock, "Task 1", 128, NULL, 2, NULL);
  xTaskCreate(CallAlarm, "Task 2" , 128, NULL, 1, NULL);
  
}

void loop(){
}

void displayClock(void* pvParameters){
  (void) pvParameters;
  while(1){
    if(digitalRead(button3)==HIGH){
      displayTime(alarmHours, alarmMins);
    }
    else{
      displayTime(hours, mins);
    }
    //delay
    vTaskDelay(20/portTICK_PERIOD_MS);  
  }
}


void CallAlarm(void* pvParameters){
  (void) pvParameters;
  while(1){
    if((mins == alarmMins) && (hours == alarmHours)) {
      digitalWrite(buzzer, HIGH);
    }
    else{
      digitalWrite(buzzer, LOW);
    }
    vTaskDelay(20/portTICK_PERIOD_MS);
  }
}


ISR(TIMER1_COMPA_vect){
  TCNT1  = 0;
  addSecs();
}

void addSecs()
{
  if (secs < 59 && secs >= 0) {
    secs++;
  }
  else if (secs == 59) {
    secs = 0;
    addMins();
  }
}

void addMins()
{
  if (mins < 59 && mins >= 0) {
    mins++;
  }
  else if (mins == 59) {
    mins = 0;
    addHours();
  }
}

void addHours()
{
  if (hours < 23 && hours >= 0) {
    hours++;
  }
  else if (hours == 23) {
    hours = 0;
  }
}

void addMinsAlarm()
{
  if (alarmMins < 59 && alarmMins >= 0) {
    alarmMins++;
  }
  else if (alarmMins == 59) {
    alarmMins = 0;
  }
}

void addHoursAlarm()
{
  if (alarmHours < 23 && alarmHours >= 0) {
    alarmHours++;
  }
  else if (alarmHours == 23) {
    alarmHours = 0;
 }
}

void displayTime(int hours, int minutes) {
  tm1637.display(0, (hours/10)%10); //puluhan jam
  tm1637.display(1, hours%10);  //satuan jam
  tm1637.point(1);
  tm1637.display(2, (minutes/10)%10); //puluhan menit
  tm1637.display(3, minutes%10);  //satuan menit
}

void tekan1(){
  if (digitalRead(button3)==LOW){
    addMins();
  }
  else{
    addMinsAlarm();
  }
}

void tekan2(){
  if (digitalRead(button3)==LOW){
    addHours();
  }
  else{
    addHoursAlarm();
  }
}
