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
  pinCode = _getString(90);                                                                               //*****************************************************
  if(PINEntered()){
    Serial.println("Password accepted. Please click in input box within 2 seconds. ");
    delay(2000);
    int passwordButtonPresses = getPasswordButtonPresses();
    Serial.println(passwordButtonPresses);
    password = _getString(passwordButtonPresses*2);                                                      //***************************************************************************
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
    i = Serial.read();
    if(i < 57){
      i = i - 48;
      while(Serial.available() > 0){
        result += (char)Serial.read();
        delay(5);
      } 
      Serial.print(F("Password set to: "));
      Serial.println(result);
      Serial.println(i);
      _addString(result, i*2);
      //stringToEEPROM(result, i*100);                                                              //****************************************************************************
      password=result;
    }
    else{
      if(i == (int)'k' || i == (int)'k'){ //k for setting keylogger use
        //not implemented yet
      }
      else{
        if(i == (int)'p' || i == (int)'P'){ //p for setting pin
          while(Serial.available() > 0){
            result += (char)Serial.read();
            delay(5);
          }
          Serial.print("Set pin to: ");
          Serial.println(result);
          _addString(result, 90);
          //stringToEEPROM(result, 1000);                                                         //*****************************************************************************************
        } 
      }
    }
  }
  else{
    Serial.println(F("Type in storage space followed by password (Must be less than 100 characters). E.g. \"2correcthorsebatterystaple\"."));
    Serial.println(F("Enter p followed by a password to change the password required to access this device. E.g. \"p1234\"")); 
  }
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

/*
   _____ _             _      ____   __   _      _ _                          
  / ____| |           | |    / __ \ / _| | |    (_) |                         
 | (___ | |_ __ _ _ __| |_  | |  | | |_  | |     _| |__  _ __ __ _ _ __ _   _ 
  \___ \| __/ _` | '__| __| | |  | |  _| | |    | | '_ \| '__/ _` | '__| | | |
  ____) | || (_| | |  | |_  | |__| | |   | |____| | |_) | | | (_| | |  | |_| |
 |_____/ \__\__,_|_|   \__|  \____/|_|   |______|_|_.__/|_|  \__,_|_|   \__, |
                                                                         __/ |
                                                                        |___/ 

*/
int _blankVal = 254;
int _startVal = 253;
int _endVal = 252;
int _addressSpace = 1024;
/*
initializes the object with an int, _addressSpace, which specifies the size of the EEPROM (in bytes)
*/


/*
Goes through the whole EEPROM and writes _blankVal to every single address
*/
void _blankEEPROM(){
  for (int i = 0; i < _addressSpace; i++){
    if(EEPROM.read(i) != _blankVal){
      if(i != EEPROMAddrButton){
        EEPROM.write(i, _blankVal);
      }
    } 
   }
}

/*
Is passed a string and an int. It will add the string at the earliest possible place. It will store the reference for that at stringplace and stringplace+1
stringPlace must be even! 
*/
void _addString(String stringToAdd, int stringPlace){
  _defrag();
  int earliestEmptyIndex = _findEarliestEmptyIndex();
  _stringToEEPROM(stringToAdd, earliestEmptyIndex);
  EEPROM.write(stringPlace, earliestEmptyIndex);
  //Serial.print(stringPlace);
  //Serial.print(",");
  //Serial.println(earliestEmptyIndex);
  EEPROM.write(stringPlace+1, earliestEmptyIndex+stringToAdd.length()+1);
}
/*
Iterates through filesystem, moves everything forward then updates indices at first 100
*/
void _defrag(){
  int amountShift = 0; 
  boolean didDefraggingOccur = false;
  for(int i = 100; i < _addressSpace; i++){
    if(EEPROM.read(i) == _blankVal){
      amountShift--;
    }
    else{
      if(amountShift != 0){ //means shift is needed
        _shifter(amountShift, i);
        didDefraggingOccur = true;
        amountShift = 0;
      }
    }
  }
  int indexOfStartOfLastStringFound = 100; 
  int indexOfEndOfLastStringFound = 100;
  boolean isInMiddleOfWord = false; 
  if(didDefraggingOccur){
    for(int i = 2; i < 100; i=i+2){
      boolean doneWithStart = false;
      boolean doneWithEnd = false;
      for(int j = EEPROM.read(i-1); j < _addressSpace; j++){ 
          if(EEPROM.read(j) == _startVal && doneWithStart == false){
            indexOfStartOfLastStringFound = j;
            EEPROM.write(i, j);
            doneWithStart == true;
          }
          if(EEPROM.read(j) == _endVal && doneWithEnd == false){
            indexOfEndOfLastStringFound = j;
            EEPROM.write(i+1, j);
            doneWithEnd == true;
          }
      }
    }
}
  _zeroEEPROMAfterLastIndex();
}


/*
Is sent a amount to shift (positive == right; negative == left) and the place the shift should start
*/
void _shifter(int amountToShift, int indexToShiftFrom){
  for(int i = indexToShiftFrom; i < _addressSpace; i++){//goes from index to shift from to address space
    EEPROM.write(i+amountToShift, EEPROM.read(i));
  }
}

/*
Finds the last index with data in it and then zeros data out after that point
*/
void _zeroEEPROMAfterLastIndex(){
  int lastIndex = 100;
  for(int i = 1; i < 100; i=i+2){
    if(EEPROM.read(i) > lastIndex){
      lastIndex = EEPROM.read(i);
    }
  }
  lastIndex++;                                
  for (int i = lastIndex; i < _addressSpace; i++){
    if(EEPROM.read(i) != _blankVal){
      if(i!=EEPROMAddrButton){
        EEPROM.write(i, _blankVal);
      }
    }
  }
}


/*
Returns the int value for the earliest empty address
*/
int _findEarliestEmptyIndex(){
  for(int i = 100; i < _addressSpace; i++){
    if(EEPROM.read(i) == _blankVal){
      return i;
    }
  }
}

/*
Adds a string to the EEPROM at the current address surrounded by _startVal and _endVal
*/
void _stringToEEPROM(String toE, int addr){
  EEPROM.write(addr, _startVal);
  for(int i = 0; i < toE.length(); i ++){
    EEPROM.write(addr+i+1, toE.charAt(i));
    delay(5);
  }
  EEPROM.write(addr+toE.length()+1, _endVal);
}

/*
stringNumber must be even and between 0 and 98
*/
String _getString(int stringNumber){
  String result = "";
  int startOfString = EEPROM.read(stringNumber) + 1;
  int endOfString = EEPROM.read(stringNumber+1) - 1;
  for(int i = startOfString; i <= endOfString; i++){
    result += (char)EEPROM.read(i);
  }
  return result;
}
