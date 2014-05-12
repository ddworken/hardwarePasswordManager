
#include <EEPROM.h>
int led = 13; //on arduino micro there is a led attached to pin 13
int method = 1;//0 is best (keylogger proof); 1 uses delete to trash gibberish data; 2 uses new tabs for temp storage of gibberish; 3 has no keylogger security;-1=nothing
String password = "correcthorsebatterystaple"; //default password (see http://xkcd.com/936)
int EEPROMAddrButton = 499; //EEPROM address for storing number of times reset before running
boolean requireSerialUnlock = true; //set to true to require the user to type in pinCode into serial before it outputs data. false means no authentication. 
String pinCode = "1234"; //unlock code. 

/*
-Setup Function
-Initializes serial, keyboard, and mouse control and sets the seed for the random number generator to analogRead(0)
*/
void setup() {
  Serial.begin(115200);
  EEPROM.write(EEPROMAddrButton, (int)EEPROM.read(EEPROMAddrButton) + 1);
  Keyboard.begin();
  Mouse.begin();
  randomSeed(analogRead(0));
  pinMode(led, OUTPUT);
}

/*
-Loop Function
-Delays for a set period of time before it enters the password (Can be removed in final version)
-Does below once: 
  -gets password from eeprom based off of number of button presses
  -resets the counter for the button presses
  -if debug option: no password is not enabled, it types in the password with trash data
  -detaches keyboard and mouse
-Then continually prompts the user to enter a new password using programNewPassword();
*/
void loop() {
  delay(250);
  if(PINEntered()){
    Serial.println("Password accepted. Please click in input box within 2 seconds. ");
    delay(2000);
    int passwordButtonPresses = getPasswordButtonPresses();
    Serial.println(passwordButtonPresses);
    password = retrievePasswordFromEEPROM(passwordButtonPresses*100);
    EEPROM.write(EEPROMAddrButton, (int)0);
    if(method!=-1){
      for(int i = 0; i < password.length(); i++){
        typeCharAt(i); 
        trashData(method);
      }
    }
    Keyboard.end();
    Mouse.end();
    while(true){
      delay(500);
      programNewPassword();
    }
  }
}

/*
-Pass it a character and it types in that character of the string password (which is initialized by loop())
*/
void typeCharAt(int i){
  Keyboard.write(password.charAt(i)); 
  delay(10);
}

/*
-Generates trash data for keylogger-proofing the program. 
*/
void trashData(int type){
  if(type == 0){ //moves mouse to left, types gibberish into nothing, then goes back to field for next char then repeates
    Mouse.move(-100,0,0);
    Mouse.click();
    int randomN = (int)random(0,5);
    for(int i = 0; i < randomN; i++){
      //Serial.println("mouse method");
      int randomNN = (int)random(97,122);
      Keyboard.write(randomNN);
      delay(10);
    }
    Mouse.move(100,0,0);
    Mouse.click();
  }
  else if(type == 1){//types gibberish into password field then deletes it (very insecure)
    int randomN = (int)random(0,5);
    for(int i = 0; i < randomN; i++){
      //Serial.println("Doing delete style random trash"); 
      int randomNN = (int)random(97,122);
      Keyboard.write(randomNN);
      delay(5);
    }
    for(int i = 0; i < randomN; i++){
      Keyboard.write(178);
      delay(5);
    }
  }
  else if(type == 2){//only works in chrome/firefox. works by opening new tab for gibberish then deleting new tab
    int randomN = (int)random(0,5);
    Keyboard.press(128);
    Keyboard.write(116);
    Keyboard.releaseAll();
    for(int i = 0; i < randomN; i++){
      //Serial.println("Doing chrome/firefox style random trash"); 
      int randomNN = (int)random(97,122);
      Keyboard.write(randomNN);
    }
    delay(1000);
    Keyboard.press(128);
    Keyboard.write(119);
    Keyboard.releaseAll();
  }
  else if(type == 3){
    //do nothing
  }
}
/*
-When called it checks if serial is available. If serial is available it parses the serial input to retrieve a password (String) and EEPROM index (int). 
-It sends the string and EEPROM index to the stringToEEPROM method. 
*/
void programNewPassword(){
  int i = 0;
  String whichPassword = "";
  String result = password;
  if(Serial.available()){
    result = "";
    int i;
    i = Serial.read() - 48;
    while(Serial.available() > 0){
      result += (char)Serial.read();
      delay(5);
    } 
    Serial.print(F("Password set to: "));
    Serial.println(result);
    Serial.println(i);
    stringToEEPROM(result, i*100);
    password=result;
  }
  else{
    Serial.println(F("Type in storage space followed by password (Must be less than 100 characters). E.g. \"2correcthorsebatterystaple\"")); 
  }
}

/*
-Receives a string and a address to store that string in. 
-Stores the string in the EEPROM by first setting the byte at the address specified to the length of the string. 
-Then stores the rest of the string in each additional place (one character to one byte).
*/
void stringToEEPROM(String toE, int addr){
  EEPROM.write(addr, toE.length());
  for(int i = 0; i < toE.length(); i ++){
    EEPROM.write(addr+i+1, toE.charAt(i));
    delay(5);
  }
}

/*
-Returns a string that is the password retrieved from the EEPROM at the address that is passed to the method. 
-Works by reading byte at EEPROM address to determine length of password and then iterating through the EEPROM that many bytes. 
*/
String retrievePasswordFromEEPROM(int addr){
  String result = "";
  for(int i = 0; i < EEPROM.read(addr); i++){
    result+=(char)EEPROM.read(i+addr+1);
    delay(5);
  }
  return result;
}

/*
-Returns the number of times the button dedicated to choosing which password is pressed. 
-Currently works by reading a value from the EEPROM. Each time the Arduino turns on it increments that value. After outputting the password it sets that value to 0. By reseting the arduino before it outputs the password one can change which password the arduino will output. 
*/
int getPasswordButtonPresses(){
  return EEPROM.read(EEPROMAddrButton) - 1;
}

/*
-If the pin has been entered, return true. Otherwise return false. 
*/
boolean PINEntered(){
  if(requireSerialUnlock){ //forces user to enter password stored in first index. 
    Serial.println(F("Please input your password to unlock this device."));
    String result = "";
    if(Serial.available()){
      while(Serial.available() > 0){
        result += (char)Serial.read();
        delay(5);
      } 
      if(result.equals(pinCode)){
        return true;
      }
      return false;
    }
  }
  else{
    return true; 
  }
  return false;
}
