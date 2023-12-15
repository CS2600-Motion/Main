#include <ezButton.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>

const char *ssid = "YOURWIFINAME";   //name of your WiFi network
const char *password = "YOURWIFIPASSWORD"; //password of the WiFi network
const char *gcpIP = "tonyprojesp.duckdns.org"; //"tonyprojesp.duckdns.org"
const char *mqttTopic = "MudGameServ";
const int mqttPort = 1883;
const int controllerPort = 8888;

#define SDA 13
#define SCL 14

WiFiClient espClient;
WiFiUDP udp;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);

ezButton button1(15);
ezButton button2(2);
ezButton button3(0);
ezButton button4(4);

String RoomDesc = "Waiting To Start";
String RoomDesc2 = "";

void setup(){
  Serial.begin(115200);
  Serial.println("Esp32 is ready");

  button1.setDebounceTime(50);
  button2.setDebounceTime(50);
  button3.setDebounceTime(50);
  button4.setDebounceTime(50);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(gcpIP, mqttPort);
  client.setCallback(callback);
  
  while(!client.connected()){
    client.connect("ESP32Client");
    if(client.connect("ESP32Client")){
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqttTopic);
    }
    else{
      Serial.println("Failed to connect to MQTT broker, retrying in 5 seconds...");
      delay(5000);
    }
  }
  Wire.begin(SDA, SCL);
  if(!i2CAddrTest(0x27)){
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(RoomDesc);
}

void loop() {
  client.loop();
  button1.loop();
  button2.loop();
  button3.loop();
  button4.loop();
  
  if(button1.isReleased()){
    udp.beginPacket(gcpIP, controllerPort);
    udp.print("/west");
    udp.endPacket();
    Serial.println("1");
  }
  if(button2.isReleased()){
    udp.beginPacket(gcpIP, controllerPort);
    udp.print("/north");
    udp.endPacket();
    Serial.println("2");
  }
  if(button3.isReleased()){
    udp.beginPacket(gcpIP, controllerPort);
    udp.print("/south");
    udp.endPacket();
    Serial.println("3");
  }
  if(button4.isReleased()){
    udp.beginPacket(gcpIP, controllerPort);
    udp.print("/east");
    udp.endPacket();
    Serial.println("4");
    }
  printDesc();
  delay(500);
  }
  
  
void callback(char* topic, byte* payload, unsigned int length){
  String message = "";
  for(int i = 0; i < length; i++){
    message += (char)payload[i];
  }
  RoomDesc = "";
  RoomDesc2 = "";
  if(length <= 16){
    RoomDesc = message;
  }
  else{
    RoomDesc = message.substring(0,15) + "-";
    RoomDesc2 = message.substring(15, length);
  }
  Serial.println(message);
}

void printDesc(){
  lcd.clear();  
  lcd.setCursor(0,0);
  lcd.print(RoomDesc);
  lcd.setCursor(0,1);
  lcd.print(RoomDesc2);
  
}

bool i2CAddrTest(uint8_t addr){
  Wire.begin();
  Wire.beginTransmission(addr);
  if(Wire.endTransmission() == 0){
    return true;
  }
  return false;
}
