#include <BlockNot.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include "htmlfiles.h"
//  Cooldowns in ms
const int SMUDGE_SPIRIT = 180000;
const int SMUDGE_NORMAL = 90000;
const int SMUDGE_DEMON = 60000;

//  Storage for WiFi credentials
const int EEPROM_SIZE = 64;

//  LED Pins
const int LVL1 = 15;
const int LVL2 = 16;
const int LVL3 = 17;
const int LVL4 = 18;
const int LVL5 = 8;

//  Memory Clear Block
const uint8_t zero[32] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};

//  Web Servers
WebServer server(80);
WebServer control(8080);

//  Signals the Timer Functions
int Flag = 0;

//  Saves the current EMF Level
int Level = 0;

//  Subroutines
void StartAPMode(){
  WiFi.softAP("ESP32-EMF","PhasmoEMF");
  server.on("/",handleRoot);
  server.on("/save",handleSave);
  server.begin();
}
void StartSTAMode(){
  WiFi.begin(EEPROM.readString(0),EEPROM.readString(32));
  delay(3000);
  if(WiFi.status() == WL_NO_SSID_AVAIL){
      EEPROM.writeBytes(0, zero, 32);
      EEPROM.writeBytes(32, zero, 32);
      EEPROM.commit();
      ESP.restart();
  }
  else{
      control.on("/",handleRootControl);
      control.begin();
  }
}

//  Smudge Timer Modules
void smudgeDemon(){
  setLevel3();
}
void smudgeNormal(){
  setLevel4();
}
void smudgeSpirit(){
  setLevel5();
}

/// Sends the "Setup WiFi" Webpage to the device requesting it
void handleRoot() 
{
  server.send(200,"text/html",FPSTR(SetupPage));
}
/// Saves the response from the "Setup WiFi" Webpage into this devices memory and restarts
void handleSave()
{
  String ssid = server.arg(0);
  String password = server.arg(1);
  EEPROM.writeString(0,ssid);
  EEPROM.writeString(32,password);
  EEPROM.commit();
  server.send(200,"text/plain","WiFi Credentials Provided\nDevice Will Reset in 3 seconds to connect to WiFi");
  delay(3000);
  ESP.restart();
}

// Checks the request to see if the requester has provided a correct argument
void handleRootControl()
{
  if(control.args() > 0 && control.args() == 1 && control.method() == HTTP_GET){
    if(control.arg(0) == "RESET"){
      control.send(200,"text/plain","Device Reset Code Recieved\nDevice will reboot in 3 seconds");
      EEPROM.writeBytes(0,zero,32);
      EEPROM.writeBytes(32,zero,32);
      EEPROM.commit();
      delay(3000);
      ESP.restart();
    }
    if(control.arg(0) == "EMF0"){
      clearAll();
      control.send(200,"text/plain","Lights Off Command");
    }
    if(control.arg(0) == "EMF1"){
      setLevel1();
      control.send(200,"text/plain","Level 1 Command");
    }
    if(control.arg(0) == "EMF2"){
      setLevel2();
      control.send(200,"text/plain","Level 2 Command");
    }
    if(control.arg(0) == "EMF3"){
      setLevel3();
      control.send(200,"text/plain","Level 3 Command");
    }
    if(control.arg(0) == "EMF4"){
      setLevel4();
      control.send(200,"text/plain","Level 4 Command");
    }
    if(control.arg(0) == "EMF5"){
      setLevel5();
      control.send(200,"text/plain","Level 5 Command");
    }
    if(control.arg(0) == "TIMER"){
      clearAll();
      Flag = 1;
      control.send(200,"text/plain","Smudge Timer Started");
      
    }
    else{
      control.send(200,"text/plain","Unknown Command");
    }
  }
  else{
    control.send(200,"text/plain","Please Enter Command");
  }
}


///  Initializes the device ready for use
void setup(){
  pinMode(LVL1,OUTPUT);
  pinMode(LVL2,OUTPUT);
  pinMode(LVL3,OUTPUT);
  pinMode(LVL4,OUTPUT);
  pinMode(LVL5,OUTPUT);
  //  LED Check Routine
  testLights();
  setLevel1();
  EEPROM.begin(EEPROM_SIZE);
  setLevel2();
  // Checks if the memory is blanked out
  if(EEPROM.read(0) == 0xFF){
    setLevel3();
    delay(500);
    clearAll();
    StartAPMode();
  }
  else{
    setLevel4();
    delay(500);
    clearAll();
    StartSTAMode();
  }
}

/// Runs through the main loop of the program
void loop(){
  //  Tells the webserver to allow connections and to handle their requests
  server.handleClient();
  control.handleClient();

  //  A request for smudge timer function was invoked
  if(Flag == 1){
    static BlockNot demonSmudge(60,SECONDS);
    static BlockNot normalSmudge(90,SECONDS);
    static BlockNot spiritSmudge(180,SECONDS);
    static BlockNot returnToTimer(240,SECONDS);
    if(demonSmudge.FIRST_TRIGGER){
      smudgeDemon();
    }
    if(normalSmudge.FIRST_TRIGGER){
      smudgeNormal();
    }
    if(spiritSmudge.FIRST_TRIGGER){
      smudgeSpirit();
      Flag = 0;
    }
    if(returnToTimer.FIRST_TRIGGER){
      switch(Level){
        case 0:
          clearAll();
          break;
        case 1:
          setLevel1();
          break;
        case 2:
          setLevel2();
          break;
        case 3:
          setLevel3();
          break;
        case 4:
          setLevel4();
          break;
        case 5:
          setLevel5();
          break;
      }
    }
  }
}

void setLevel1(){
  digitalWrite(LVL1,HIGH);
  digitalWrite(LVL2,LOW);
  digitalWrite(LVL3,LOW);
  digitalWrite(LVL4,LOW);
  digitalWrite(LVL5,LOW);
}

void setLevel2(){
  digitalWrite(LVL1,HIGH);
  digitalWrite(LVL2,HIGH);
  digitalWrite(LVL3,LOW);
  digitalWrite(LVL4,LOW);
  digitalWrite(LVL5,LOW);
}

void setLevel3(){
  digitalWrite(LVL1,HIGH);
  digitalWrite(LVL2,HIGH);
  digitalWrite(LVL3,HIGH);
  digitalWrite(LVL4,LOW);
  digitalWrite(LVL5,LOW);
}

void setLevel4(){
  digitalWrite(LVL1,HIGH);
  digitalWrite(LVL2,HIGH);
  digitalWrite(LVL3,HIGH);
  digitalWrite(LVL4,HIGH);
  digitalWrite(LVL5,LOW);
}

void setLevel5(){
  digitalWrite(LVL1,HIGH);
  digitalWrite(LVL2,HIGH);
  digitalWrite(LVL3,HIGH);
  digitalWrite(LVL4,HIGH);
  digitalWrite(LVL5,HIGH);
}

void clearAll(){
  digitalWrite(LVL1,LOW);
  digitalWrite(LVL2,LOW);
  digitalWrite(LVL3,LOW);
  digitalWrite(LVL4,LOW);
  digitalWrite(LVL5,LOW);
}

void testLights(){
  delay(100);
  setLevel1();
  delay(100);
  setLevel2();
  delay(100);
  setLevel3();
  delay(100);
  setLevel4();
  delay(100);
  setLevel5();
  delay(100);
  clearAll();
}



