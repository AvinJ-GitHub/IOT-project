#include <ThingSpeak.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
const int MPU_addr=0x68; // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float ax=0, ay=0, az=0, gx=0, gy=0, gz=0;
boolean fall = false; //stores if a fall has occurred
boolean trigger1=false; //stores if 1st trigger (lower threshold) has occurred
boolean trigger2=false; //stores if 2nd trigger (upper threshold) has occurred
boolean trigger3=false; //stores if 3rd trigger (orientation change) has occurred
byte trigger1count=0; //stores the counts past since trigger 1 was set true
byte trigger2count=0; //stores the counts past since trigger 2 was set true
byte trigger3count=0; //stores the counts past since trigger 3 was set true
int angleChange=0;
// WiFi network info.
WiFiClient client;
const char *ssid = "Galaxy M31DF30"; // Enter your WiFi Name
const char *pass = "zyqd8685"; // Enter your WiFi Password

void send_event(const char *event);
const char *host = "maker.ifttt.com";
const char *privateKey = "h3R6TSCL-qSqYzB1z1hISx1hpv6KYt1IFmeXX3Q93eL"; //IFTTT API Key
unsigned long myChannelNumber = 1918312;
const char * myWriteAPIKey = "XZ7LXB1TED79UMOO"; //ThingSpeak API
void setup(){
Serial.begin(115200);
ThingSpeak.begin(client);
Wire.begin();
Wire.beginTransmission(MPU_addr);
Wire.write(0x6B); // PWR_MGMT_1 register
Wire.write(0); // set to zero (wakes up the MPU-6050)
Wire.endTransmission(true);
Serial.println("Wrote to IMU");
Serial.println("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print("."); // print ... till not connected
}
Serial.println("");

Serial.println("WiFi connected");
}
void loop(){
mpu_read();
ax = (AcX-2050)/16384.00;
ay = (AcY-77)/16384.00;
az = (AcZ-1947)/16384.00;
gx = (GyX+270)/131.07;
gy = (GyY-351)/131.07;
gz = (GyZ+136)/131.07;
// calculating Amplitute vactor for 3 axis
float Raw_Amp = pow(pow(ax,2)+pow(ay,2)+pow(az,2),0.5);
int Amp = Raw_Amp * 10;
//Serial.println(Amp);
Serial.print("X:");
Serial.println(ax);
Serial.print("Y:");
Serial.println(ay);
Serial.print("Z:");
Serial.println(az);
ThingSpeak.setField(1, ax);
ThingSpeak.setField(2, ay);
ThingSpeak.setField(3, az);
ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

/*if (client.connect(thingSpeakAddress,80)) // "184.106.153.149" or api.thingspeak.com
{
String postStr = apiKey;
String postStr2 = apiKey;
String postStr3 = apiKey;
//Field1
postStr +="&field1=";
postStr2 +="&field2=";
postStr3 +="&field3=";
postStr += String(ax);
postStr += "\r\n\r\n";
postStr2 += String(ay);
postStr2 += "\r\n\r\n";
postStr3 += String(az);
postStr3 += "\r\n\r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());

client.print("\n\n");
client.print(postStr);
client.print(postStr2);
client.print(postStr3);
}
client.stop();*/
if (Amp<=2 && trigger2==false){ //if AM breaks lower threshold (0.4g)
trigger1=true;
Serial.println("TRIGGER 1 ACTIVATED");
}
if (trigger1==true){
trigger1count++;
if (Amp>=12){ //if AM breaks upper threshold (3g)
trigger2=true;
Serial.println("TRIGGER 2 ACTIVATED");
trigger1=false; trigger1count=0;
}
}
if (trigger2==true){
trigger2count++;
angleChange = pow(pow(gx,2)+pow(gy,2)+pow(gz,2),0.5); Serial.println(angleChange);
if (angleChange>=30 && angleChange<=400){ //if orientation changes by between 80-100 degrees

trigger3=true; trigger2=false; trigger2count=0;
Serial.println(angleChange);
Serial.println("TRIGGER 3 ACTIVATED");
}
}
if (trigger3==true){
trigger3count++;
if (trigger3count>=10){
angleChange = pow(pow(gx,2)+pow(gy,2)+pow(gz,2),0.5);
//delay(10);
Serial.println(angleChange);
if ((angleChange>=0) && (angleChange<=50)){ //if orientation changes remains between 0-10 degrees
fall=true; trigger3=false; trigger3count=0;
Serial.println(angleChange);
}
/*else{ //user regained normal orientation
trigger3=false; trigger3count=0;
Serial.println("TRIGGER 3 DEACTIVATED");
}*/
}
}
if (fall==true){ //in event of a fall detection
Serial.println("FALL DETECTED");

send_event("ESP_EVENT");
fall=false;
}
if (trigger2count>=6){ //allow 0.5s for orientation change
trigger2=false; trigger2count=0;
Serial.println("TRIGGER 2 DECACTIVATED");
}
if (trigger1count>=6){ //allow 0.5s for AM to break upper threshold
trigger1=false; trigger1count=0;
Serial.println("TRIGGER 1 DECACTIVATED");
}
delay(100);
}
void mpu_read(){
Wire.beginTransmission(MPU_addr);
Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
Wire.endTransmission(false);
Wire.requestFrom(MPU_addr,14,true);
AcX=Wire.read()<<8|Wire.read();
AcY=Wire.read()<<8|Wire.read();
AcZ=Wire.read()<<8|Wire.read();
Tmp=Wire.read()<<8|Wire.read();
GyX=Wire.read()<<8|Wire.read();
GyY=Wire.read()<<8|Wire.read();

GyZ=Wire.read()<<8|Wire.read();
}
void send_event(const char *event)
{
Serial.print("Connecting to ");
Serial.println(host);
// Use WiFiClient class to create TCP connections
WiFiClient client;
const int httpPort = 80; //http Port
if (!client.connect(host, httpPort)) {
Serial.println("Connection failed");
return;
}
// We now create a URI for the request
String url = "/trigger/";
url += event;
url += "/with/key/";
url += privateKey;
Serial.print("Requesting URL: ");
Serial.println(url);
// This will send the request to the server
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"Connection: close\r\n\r\n");

while(client.connected())
{
if(client.available())
{
String line = client.readStringUntil('\r');
Serial.print(line);
} else {
// No data yet, wait a bit
delay(50);
};
}
Serial.println();
Serial.println("closing connection");
client.stop();
}