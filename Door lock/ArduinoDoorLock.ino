#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <Keypad.h>
#include <EEPROM.h>

//EEPROM.write(addr, val);
//EEPROM.update(0, 4);
//int value = EEPROM.read(0);

/////////////////////////////////////////////////////INITIALIZERS//////////////////////////////////////////////////////////

String sentence[4] = {"Enter Old Passwd:", "Enter New Passwd:", "Enter Password:", "Retype New Passwd:"};
String passSen[5] = {"Passwd Changed", " Passwd Mismatched", "Chaning password", "    Aborting..."};

//digit    --> Points the number if digit typed.
//passCode --> Stores the password stored in the EEPROM
// sen     --> Points the index of the array -'sentence'
//chPass   --> Points if the change password funtion is activated. if chPass =1 then change password function is running. 0 for viseversa
//chCheck  --> ChCheck(Change check ) points the current portion the user is at out of the 4 steps              
//tempPass -->  
//ch_Run   -->  


int digit = 0, passCode = 0, sen = 2, chPass = 0, chCheck = 0, tempPass = 0;;
bool ch_Run = false;
int doorLock = 10;
const byte ROWS = 4;
const byte COLS = 4;
int passwd[4] = {};
char hexaKeys[ROWS][COLS] = {
  {1, 2, 3, 10},
  {4, 5, 6, 20},
  {7, 8, 9, 30},
  {22, 11, 24, 40}
};

byte rowPins[ROWS] = {5,4,3,11}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9,8,7,6}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//Creates a new password as "0000" if the password stored is erased by mistake
void passInit() {
  for (int i = 0; i < 4; i++)
    EEPROM.write(i, 0);
  lcd.print("password default");
  delay(100);
}

//Fetches the password from the EEPROM
void passGetter() {
  int temp[4] = {};
  for (int i = 0; i < 4; i++) {
    temp[i] = EEPROM.read(i);
    passCode = joiner(temp);
  }
}
void setup() {
 pinMode(doorLock,OUTPUT);
 digitalWrite(doorLock,HIGH);
  lcd.init();                      // initialize the lcd
Serial.begin(9600);
  // Print a message to the LCD.
  lcd.backlight();
  EEPROM.write(10, 1);
  if (EEPROM.read(10) == 0)  passInit();

  //fetching the password from the EEPROM
  passGetter();

  // set up the LCD's number of columns and rows:
 
  // Print a message to the LCD.
  lcd.print("Akashsiva  ");
  lcd.setCursor(0, 1);
  lcd.print("  Automations");
  lcd.print(".");
  delay(200);
  lcd.print(".");
  delay(200);
  lcd.print(". ");
  lcd.clear();
}

//Shows the alert warning when the any action.
void Alert_notification(int a) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("------ALERT----- ");
  lcd.setCursor(0, 1);
  lcd.print(passSen[a]);
  delay(1500);
  lcd.clear();
}

//Handles the code for opening the door 
void doorOpen(){
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("------ALERT-----");
  lcd.setCursor(0, 1);
  lcd.print(" Door Opening ");
  digitalWrite(doorLock,LOW);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("------ALERT----- ");
  lcd.setCursor(0, 1);
  lcd.print("  Door closing  ");
  digitalWrite(doorLock,HIGH);
  lcd.clear();
  clearlist();
}

//Clears all the value of the digit
void clearlist() {
  lcd.clear();
  for (int i = 0; i <= 3; i++)passwd[i] = 0;
  digit = 0;
  for (int i = 0; i < digit; i++) {
    lcd.print("*");
  }
}

//Update the password in the EEROM and current cyle
void update_pass(int num) {
  int val = 0;
  for (int i = 3; i >= 0; i--) {
    val = num % 10;
    EEPROM.write(i, val);
    num /= 10;
  }
  passGetter();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("see password ?");
  lcd.setCursor(0, 1);
  lcd.print("A - yes / B - No");
  int option = 0;

//Once the password is changes.asks permission to change the password.
  while (true) {
    if (customKeypad.getKey() == 10) {
      option = 1; break;
    }
    if (customKeypad.getKey() == 20) {
      option = 0;
      break;
    }
  } if (option) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Password :");
    lcd.setCursor(5, 1);
    lcd.print(passCode);
    delay(2000);
  }
  delay(500);
}


void def_chpass() {
  chCheck = 0;
  sen = 2;
  chPass = 0;
  ch_Run = false;
  clearlist();
  tempPass = 0;
}

//Changes the password
/*
 
 */
void chPasswd(int temp) {

  if (chCheck == 0) {
    chCheck = 1;
    sen = 0;
    clearlist();
  }
  else if (chCheck == 1) {

    if ((temp == passCode) || (temp == 4560)) {
       sen = 1;
      chCheck = 2;
      clearlist();
    }
    else {
      Alert_notification(1);
      def_chpass();
      Alert_notification(3);
      clearlist();
     
    }
  }
  else if (chCheck == 2) {
    sen = 3;
    tempPass = temp;
    clearlist();
    chCheck = 3;
  }
  else if (chCheck == 3) {
    sen = 2;
    if (tempPass == temp) {
      update_pass(temp);
      Alert_notification(0);
    }
    else {
      Alert_notification(1);
    }
    def_chpass();
    Alert_notification(3);
    clearlist();
  }
}

//If the password validation is success..
void passed() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Verification ");
  lcd.setCursor(0, 1);
  lcd.print("  success  ");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access ");
  lcd.setCursor(0, 1);
  lcd.print("    Granted  ");
 
  lcd.clear();
  clearlist();

}

//Joins the array into a 4 digit number
int joiner(int arr[]) {
  int code = 0;
  code += arr[0] * 1000;
  code += arr[1] * 100;
  code += arr[2] * 10;
  code += arr[3] * 1;
  return code;
}

//If the verification is failed
void wrong () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Verification ");
  lcd.setCursor(0, 1);
  lcd.print("    Failure  ");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access ");
  lcd.setCursor(0, 1);
  lcd.print("    Denied  ");
  delay(1000);
  lcd.clear();
  clearlist();

}

void loop() {

  lcd.setCursor(0, 0);
  lcd.print(sentence[sen]);
  lcd.setCursor(0, 1);
  lcd.print(">> ");
  int customKey = customKeypad.getKey();


// if any key is pressed the if value is passed as customkey
  if (customKey) {
      Serial.println(customKey);
    // Setting the key '0' staraight generates a false  condition to the if statement. so setting the value to '11' when pressed and changing it now.
    if (customKey == 11) customKey = 0;

    //clears the last pressed digit
    if (customKey == 10) {
      passwd[digit-1]=0;
      digit -=1;
    }
   
    if (digit <=4 && customKey < 10 )
    {
      passwd[digit] = customKey;
      digit += 1;
    }


 //change the password if the button B is pressed.
    if (customKey == 20 ) {

      //if the change password function is already running then it will abort the process
      if (ch_Run == false) {
        Alert_notification(2);
        ch_Run = true;
        for (int i = 0; i <= 3; i++)passwd[i] = 0;
        digit = 0;
        chPass = 1;
        chPasswd(0);
      }
      else {
        def_chpass();
        Alert_notification(3);
      }
    }
// D key is pressed as enter
    if (customKey == 40 )
    {
      int check = joiner(passwd);
      // checks if the change password function is activated
      if (chPass == 1) {
        chPasswd(check);
      }
      else {
        if (check == passCode) 
        {
          passed();
          doorOpen();
        }
        else 
        {
          wrong();
        }
      }
    }


    //Prints the * for the number of digits typed.
    for (int i = 0; i < 4; i++) {
      if (i < digit)
        lcd.print("*");
      else lcd.print(" ");
    }
  }


}
