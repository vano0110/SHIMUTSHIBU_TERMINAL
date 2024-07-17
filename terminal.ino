#include <AltSoftSerial.h>
#define rxPin 1 
#define txPin 0
#define K_IN  8
#define K_OUT 9
#define READ_ATTEMPTS 125
AltSoftSerial altSerial =  AltSoftSerial(K_IN, K_OUT);
char command;
void setup()  {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  delay(500);
  pinMode(K_IN, INPUT);
  pinMode(K_OUT, OUTPUT);
  Serial.begin(9600);
  Serial.println("-=Shimutshibu MUT-II Terminal=-");
}
void loop() {
  command = Serial.read();
  switch (command) {
  case '1':
    obdConnect();
    break;
  case '2':
    Serial.println("Voltage:");
    get_pid(0x14);
    break;
  case '3':
    Serial.println("PRM:");
    get_pid(0x21);
    break;
  }
}
int obdConnect(){
  Serial.println("Attempting ECU initialization...");
  if (iso_init() == 0){
    Serial.println("PASS");
    return 0;
  }
  else{
   Serial.println("FAIL");
    return 1;
  }
}
byte iso_init()
{
  byte b;
  alt_serial_tx_off(); 
  alt_serial_rx_off(); 
  digitalWrite(K_OUT, LOW);
  delay(2000);
  digitalWrite(K_OUT, HIGH);
  delay(300);
  digitalWrite(K_OUT, LOW);
  delay(200);
  b=0x00;
  for (byte mask = 0x01; mask != 0; mask <<= 1)
  {
    if (b & mask)
      digitalWrite(K_OUT, HIGH);
    else
      digitalWrite(K_OUT, LOW);
    delay(200);
  }
  digitalWrite(K_OUT, HIGH);
  delay(260);
  altSerial.begin(15625);
  byte i=0;
  while(i<3 && !iso_read_byte(&b)){
        i++;
  }
  Serial.print("Return Value - ");
  Serial.println(b);
  if(b == 0){
        return 1;
  }
    if(b == 85){
        return 0;
  }
}
void alt_serial_rx_off() {
  altSerial.end();  
}
void alt_serial_tx_off() {
   altSerial.end();  
   delay(20);
}
void alt_serial_rx_on() {
  altSerial.begin(15625);
}
boolean iso_read_byte(byte * b)
{
  int readData;
  boolean success = true;
  byte t=0;
  while(t != READ_ATTEMPTS  && (readData=altSerial.read())==-1) {
    delay(1);
    t++;
  }
  if (t>=READ_ATTEMPTS) {
    success = false;
  }
  if (success)
  {
    *b = (byte) readData;
  }
  return success;
}
int get_pid(byte pid) { 
  int result = 0;
  byte t, buf[2];
  buf[1]=0x00;
  
   altSerial.write(pid); 
  if (!altSerial.readBytes(buf, 2));
  switch (pid) {
  case 0x14:
    result = int(buf[1])*73/100;
    break;
  case 0x21:
    result = int(buf[1])*31.25;
    break;
  }
  Serial.print("Return Value - ");
  Serial.println(result);
  return result;
}