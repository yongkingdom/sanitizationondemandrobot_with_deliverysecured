#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

Servo servoX,servoY;
/* Put your SSID & Password */
const char* ssid = "DeliverySecured";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

uint8_t LED1pin = D0;
bool LED1status = LOW;

uint8_t LED2pin = D4;
bool LED2status = LOW;
long int count=0;
int open = 0;

//NodeMCU Label marking : GPIO #  //RFID
#define D0 16   // Built-in LED mid // magnetic lock
#define D1 5    //  Servo X Box1
#define D2 4    //  Servo Y Box2
#define D3 0  
#define D4 2    // LED wifi
#define D5 14   // Laser
#define D6 12   
#define D7 13   
#define D8 15   
#define D9 3    
#define D10 1 
#define D11 9   
#define D12 10

void client_status()
{
 
unsigned char number_client;
struct station_info *stat_info;
uint8_t mac_add[6];
struct ip_addr *IPaddress;
IPAddress address;
 
//WiFi.softAPgetStationNum();
WiFi.softAPmacAddress(mac_add);

Serial.print(" Total connected_client are = ");
Serial.println(WiFi.softAPgetStationNum());
Serial.print("Mac Address of the Device");
Serial.println(WiFi.softAPmacAddress());
}

void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);
  
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  servoX.attach(D1);
  servoX.write(30);
  Serial.println("Box1 Open");
  delay(1000);
  servoX.write(130);
  Serial.println("Box1 Close");
  delay(2000);
  
  servoY.attach(D2);
  servoY.write(10);
  Serial.println("Box2 Open");
  delay(1000);
  servoY.write(160);
  Serial.println("Box2 Close");
  delay(1000);

}

void loop() {
 // if(open==1) {
  //  count++;
   // if(count>500000){
   //   Serial.println("Automatic Close After certain duration");
     // servoX.write(130);   //close the box
   //   count=0;
   //   open =0;
   //  }
 // }

  server.handleClient();
  if(LED1status)
  {
    digitalWrite(LED1pin, HIGH);
  }
  else
  {
    digitalWrite(LED1pin, LOW);
  }
  
  if(LED2status)
  {digitalWrite(LED2pin, HIGH);}
  else
  {digitalWrite(LED2pin, LOW);}
}



void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  Serial.println("Connected");
  Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}

void handle_led1on() {
  LED1status = HIGH;
  Serial.println("Box1 Status: Open");
  server.send(200, "text/html", SendHTML(true,LED2status)); 
  servoX.write(30);
  open=1;
  count=0;
  
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("Box1 Status: Close");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
  servoX.write(130);
  open=0;

}

void handle_led2on() {
  LED2status = HIGH;
  Serial.println("Box2 Status: Open");
  server.send(200, "text/html", SendHTML(LED1status,true)); 
  servoY.write(10);
}

void handle_led2off() {
  LED2status = LOW;
  Serial.println("Box2 Status: Close");
  server.send(200, "text/html", SendHTML(LED1status,false)); 
  servoY.write(160);

}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #00d000;}\n";
  ptr +=".button-on:active {background-color: #00d000;}\n";
  ptr +=".button-off {background-color: #fd3e50;}\n";
  ptr +=".button-off:active {background-color: #fd3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Delivery Secured Box</h1>\n";
  ptr +="<h3>Please select to Open or Close the Delivery Box</h3>\n";
  
   if(led1stat)
  {ptr +="<p>Box1 Status: Open</p><a class=\"button button-off\" href=\"/led1off\">CLOSE</a>\n";}
  else
  {ptr +="<p>Box1 Status: Close</p><a class=\"button button-on\" href=\"/led1on\">OPEN</a>\n";}

  if(led2stat)
  {ptr +="<p>Box2 Status: Open</p><a class=\"button button-off\" href=\"/led2off\">CLOSE</a>\n";}
  else
  {ptr +="<p>Box2 Status: Close</p><a class=\"button button-on\" href=\"/led2on\">OPEN</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
