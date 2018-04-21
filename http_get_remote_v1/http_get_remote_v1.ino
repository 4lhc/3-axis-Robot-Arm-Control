#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "robotarm";
const char* password = "thereisnospoon";



ESP8266WebServer server(80);


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

uint8_t baseServo = 0;  // Max 135, Min 20
uint8_t horiServo = 1;  // Max 165, Min 80
uint8_t vertServo = 2;  // Max 140, Min 80 reverse
uint8_t grioServo = 3;  // Max 120, Min 60
char servo;
int servoNum;

uint8_t servoPos[] = {90, 100, 120, 0, 0}; //each servoPositions
int servoResolution = 4;
int degree;
int degIncr = 5; 



String input;
int pulselen = 0;



void handleRotate()
{
  Serial.print("servo = ");
  Serial.println(server.arg("servo"));
  Serial.print("angle = ");
  Serial.println(server.arg("angle"));
  //TODO: CLeanup
  int rotServoNum = server.arg("servo").toInt();
  int rotServoDegree = server.arg("angle").toInt();

  server.send ( 200, "text/plain", "1" );

  rotateServo(rotServoNum, rotServoDegree);
  
}


void setup() {
  Serial.begin(9600);
  pwm.begin();  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  WiFi.mode ( WIFI_STA );
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  
  //WiFi.softAP( ssid, password );
  Serial.println ("");
  Serial.print ("SSID: ");
  Serial.println (ssid);
  Serial.print ("Password: ");
  Serial.println (password);
  Serial.print ("IP address: ");
  Serial.println (WiFi.localIP());


  server.on ( "/rotate", handleRotate );
  server.begin();
  delay(10);
  /*//waiting for station to connect
  while(!WiFi.softAPgetStationNum())
  {
    Serial.print(".");
    delay(500);
  }*/
}



void loop() {
  server.handleClient();
/*
  while (Serial.available())
  {
    input = Serial.readStringUntil(';');
    if (input.indexOf(":") == -1) break;
  
    Serial.print("Input: ");
    Serial.println(input);
    
    servo = input[0];
    if (servo == 'b')  servoNum = 0;
    else if (servo == 'h') servoNum = 1;
    else if (servo == 'v') servoNum = 2;
    else if (servo == 'g') servoNum = 3;
    else servoNum = 4;
    
    if (input.indexOf("-") != -1)
    { //decrease servo angle by degIncr
      rotateServo(servoNum, servoPos[servoNum] - degIncr);
      break;  
    }
    if (input.indexOf("+") != -1)
    {
      rotateServo(servoNum, servoPos[servoNum] + degIncr);
      break;        
    }
    
    input.remove(0, 2);
    degree = input.toInt();

    Serial.print("Degrees: ");
    Serial.println(input);

    rotateServo(servoNum, degree);
    
  }
 */ 
}


void rotateServo(int servoNum, int degree)
{
 
  pulselen = map(degree, 0, 180, SERVOMIN, SERVOMAX);
  int curr_pulse = map(servoPos[servoNum], 0, 180, SERVOMIN, SERVOMAX);

  if (degree-servoPos[servoNum] > servoResolution)
  {
    for (int i = curr_pulse; i <= pulselen; i++)
    {
      pwm.setPWM(servoNum, 0, i);
      delay(10);
    }
  }
  
  if (degree-servoPos[servoNum] < -servoResolution)
  {
    for (int i = curr_pulse; i >= pulselen; i--)
    {
      pwm.setPWM(servoNum, 0, i);
      delay(10);
    }
  }

  if (abs(degree-servoPos[servoNum] < servoResolution))
  {
    pwm.setPWM(servoNum, 0, pulselen);
    delay(20);
  }
  
  servoPos[servoNum] = degree;
  /*Serial.print("Servo: ");
  Serial.println(servoNum);
  Serial.print("Degree: ");
  Serial.println(degree);
  */
}

