#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "OneButton.h"
//#include <LCD.h>
//#define DiskAddr 0x50                               // Address of 24LC256 eeprom chip used in Eoo3-Project


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display



// ##################### Power and GND for Midi In and Midi OUT ####################
int gndPin = 10;            // set to LOW and connect GND to the midi circuit here. (no need to disconnect midi devices for sketch upload)
int powerPin = 11;          // set to HIGH and connect power to the midi circuit here. (no need to disconnect midi devices for sketch upload)

// ##################### Pins for Buttons ####################
OneButton SwitchA(2, true);           // Setup a new OneButton
OneButton SwitchB(3, true);           // Setup a new OneButton
OneButton SwitchC(4, true);           // Setup a new OneButton
OneButton SwitchD(5, true);           // Setup a new OneButton
OneButton SwitchE(6, true);           // Setup a new OneButton
OneButton SwitchF(7, true);           // Setup a new OneButton

//##################### Banks ####################
int bank = 1;
int maxbank = 5;
boolean BankHasChanged = true;
int bankchangedelay = 500;            // determines how fast you can scroll through the banks
String BankName [5] = {"mfunky", "Pre & Snap", "Stompbox", "HX Stomp FS Modes", "Looper"};

//##################### Display ####################
int displayline;
int displayrow;

//##################### Toggle Variables ####################
boolean ToggleState [10] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

void setup()
{
  Serial.begin(31250); //MIDI:31250 // SERIAL MONITOR:9600 OR 115200

  //Setup Display
  lcd.begin (20, 4);                                          // initialize the lcd
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 1);
  lcd.print (bank);
  lcd.print (": ");
  lcd.print (BankName [bank - 1]);

  //pins for Midi (including power supply pins, so midi is not screwed up when uploading a new sketch)
  pinMode (powerPin, OUTPUT);
  pinMode (gndPin, OUTPUT);
  digitalWrite(powerPin, HIGH);                               // Power up
  digitalWrite(gndPin, LOW);                                  // Power up

  // link the button functions
  SwitchA.attachClick(SwitchAclick);
  SwitchA.attachDuringLongPress(SwitchAlongPress);
  SwitchB.attachClick(SwitchBclick);
  SwitchB.attachDuringLongPress(SwitchBlongPress);
  SwitchC.attachClick(SwitchCclick);
  SwitchC.attachDuringLongPress(SwitchClongPress);
  SwitchD.attachClick(SwitchDclick);
  SwitchD.attachDuringLongPress(SwitchDlongPress);
  SwitchD.attachLongPressStop(SwitchDlongPressStop);          // To change to the selected bank, after browsing through banks
  SwitchE.attachClick(SwitchEclick);
  SwitchE.attachDuringLongPress(SwitchElongPress);
  SwitchF.attachClick(SwitchFclick);
  SwitchF.attachDuringLongPress(SwitchFlongPress);
  SwitchF.attachLongPressStop(SwitchFlongPressStop);          // To change to the selected bank, after browsing through banks
}

void loop()
{
  // ********************Monitor the Buttons***************
  SwitchA.tick();
  SwitchB.tick();
  SwitchC.tick();
  SwitchD.tick();
  SwitchE.tick();
  SwitchF.tick();

  //*********** Bank Change Actions (Display setup, Midi Events) ******************
  //On a 20x4 Display, each text block can be 6 letters long (with a space between the blocks)

  if (bank == 1 && BankHasChanged == true)
  {
    FillBankDisplay ("Prest-", "Prest+", "solo", "Snap 1", "Snap 2", "Snap 3");
    StompFSMode();                                 //Set HX Stomp Footswitches to Stomp Mode
    SetSnapshot(1);                                 //Set Snapshot 1
    BankHasChanged = false;
  }

  if (bank == 2  && BankHasChanged == true)
  {
    FillBankDisplay ("Pres 1", "Pres 2", "Pres 3", "Snap 1", "Snap 2", "Snap 3");
    BankHasChanged = false;
  }

  if (bank == 3 && BankHasChanged == true)
  {
    FillBankDisplay ("drive", "delay", "reverb", "wah", "chorus", "eq");
    BankHasChanged = false;
  }

  if (bank == 4 && BankHasChanged == true)
  {
    FillBankDisplay ("Next", "Prev", "Preset", "Scroll", "Snapsh", "Stomp");
    BankHasChanged = false;
  }

  if (bank == 5 && BankHasChanged == true)
  {
    FillBankDisplay ("UnRedo", "", "Stop", "Record", "Dub", "Play");
    BankHasChanged = false;
  }
}// Loop

//****** AAAA ************ AAAA ************ AAAA ************ AAAA ************ AAAA ************ AAAA ************ AAAA ************ AAAA ******
void SwitchAclick()
{
  if (bank == 1)
  {
    Footswitch4Press();                                 // Footswitch 4 = Preset Down
    SetToggleState(0, 11, LOW, "C", "solo");        //Set State of a specific toggle parameter: Variable [0..9], CC Number [4, 5, 6,..], LOW/HIGH, Text Block [A..F], Text [String]
  }
  if (bank == 2)
  {
    SetPreset(1);                                       //Set Preset 1
  }
  if (bank == 3)
  {
    Toggle(0, 4, 0, 127, "A", "drive", "DRIVE");        //Toggle State Variable to toggle [0..9], CC Number [4, 5, 6,..], Low Value [0..127], High Value [0..127], Text Block [A..F], LowText [String], HighText [String]
  }
  if (bank == 4)
  {
    NextFSMode();
  }
  if (bank == 5)
  {
    //looper undo
    Serial.write(176);
    Serial.write(63);
    Serial.write(127);
  }
}
void SwitchAlongPress()
{
  if (bank == 1)
  {
    Footswitch4Press();
    SetToggleState(0, 11, LOW, "C", "solo");        //Set State of a specific toggle parameter: Variable [0..9], CC Number [4, 5, 6,..], LOW/HIGH, Text Block [A..F], Text [String]
    delay(300);
  }
  if (bank == 2)
  {
  }
  if (bank == 3)
  {
  }
  if (bank == 4)
  {
  }
  if (bank == 5)
  {
  }
}

//****** BBBB ************ BBBB ************ BBBB ************ BBBB ************ BBBB ************ BBBB ************ BBBB ************ BBBB ******
void SwitchBclick()
{
  if (bank == 1)
  {
    Footswitch5Press();                             // Footswitch 5 = Preset Up
    SetToggleState(0, 11, LOW, "C", "solo");        //Set State of a specific toggle parameter: Variable [0..9], CC Number [4, 5, 6,..], LOW/HIGH, Text Block [A..F], Text [String]
  }
  if (bank == 2)
  {
    SetPreset(2);                                       //Set Preset 2
  }
  if (bank == 3)
  {
    Toggle(1, 5, 0, 127, "B", "delay", "DELAY");        //Toggle State Variable to toggle [0..9], CC Number [4, 5, 6,..], Low Value [0..127], High Value [0..127], Text Block [A..F], LowText [String], HighText [String]
  }
  if (bank == 4)
  {
    PrevFSMode();
  }
  if (bank == 5)
  {
  }
}

void SwitchBlongPress()
{
  if (bank == 1)
  {
    Footswitch5Press();
    SetToggleState(0, 11, LOW, "C", "solo");        //Set State of a specific toggle parameter: Variable [0..9], CC Number [4, 5, 6,..], LOW/HIGH, Text Block [A..F], Text [String]
    delay(300);
  }
  if (bank == 2)
  {
  }
  if (bank == 3)
  {
  }
  if (bank == 4)
  {
  }
  if (bank == 5)
  {
  }
}

//****** CCCC ************ CCCC ************ CCCC ************ CCCC ************ CCCC ************ CCCC ************ CCCC ************ CCCC ******
void SwitchCclick()
{
  if (bank == 1)
  {
    Toggle(0, 11, 0, 127, "C", "solo", "SOLO");        //Toggle State Variable to toggle [0..9], CC Number [4, 5, 6,..], Low Value [0..127], High Value [0..127], Text Block [A..F], LowText [String], HighText [String]
  }
  if (bank == 2)
  {
    SetPreset(3);                                         //Set Preset 3
  }
  if (bank == 3)
  {
    Toggle(2, 6, 0, 127, "C", "reverb", "REVERB");        //Toggle State Variable to toggle [0..9], CC Number [4, 5, 6,..], Low Value [0..127], High Value [0..127], Text Block [A..F], LowText [String], HighText [String]
  }
  if (bank == 4)
  {
    PresetFSMode();
  }
  if (bank == 5)
  {
    //Stop Looper
    Serial.write(176);
    Serial.write(61);
    Serial.write(63);
  }
}
void SwitchClongPress()
{
  if (bank == 1)
  {
  }
  if (bank == 2)
  {
  }
  if (bank == 3)
  {
  }
  if (bank == 4)
  {
  }
  if (bank == 5)
  {
  }
}

//****** DDDD ************ DDDD ************ DDDD ************ DDDD ************ DDDD ************ DDDD ************ DDDD ************ DDDD ******
void SwitchDclick()
{
  if (bank == 1)
  {
    SetSnapshot(1);                                 //Set Snapshot 1
    SetToggleState(0, 11, LOW, "C", "solo");        //Set State of a specific toggle parameter: Variable [0..9], CC Number [4, 5, 6,..], LOW/HIGH, Text Block [A..F], Text [String]
  }
  if (bank == 2)
  {
    SetSnapshot(1);                                 //Set Snapshot 1
  }
  if (bank == 3)
  {
    Toggle(3, 7, 0, 127, "D", "wah", "WAH");        //Toggle State Variable to toggle [0..9], CC Number [4, 5, 6,..], Low Value [0..127], High Value [0..127], Text Block [A..F], LowText [String], HighText [String]
  }
  if (bank == 4)
  {
    ScrollFSMode();
  }
  if (bank == 5)
  {
    //Record Looper
    Serial.write(176);
    Serial.write(60);
    Serial.write(64);
  }
}
void SwitchDlongPress()                             //Switch D longpress will ALWAYS be "Bank Down", so we dont have to distinguish the banks here
{
  //Count Bank down
  {
    if (bank > 1)
    {
      bank = bank - 1;
    }
    else bank = 5;
    //    BankHasChanged = true;
    PrintBankName();
  }
}

void SwitchDlongPressStop()
{
  BankHasChanged = true;
}

//****** EEEE ************ EEEE ************ EEEE ************ EEEE ************ EEEE ************ EEEE ************ EEEE ************ EEEE ******
void SwitchEclick()
{
  if (bank == 1)
  {
    SetSnapshot(2);                                 //Set Snapshot 2
    SetToggleState(0, 11, LOW, "C", "solo");        //Set State of a specific toggle parameter: Variable [0..9], CC Number [4, 5, 6,..], LOW/HIGH, Text Block [A..F], Text [String]
  }
  if (bank == 2)
  {
    SetSnapshot(2);                                     //Set Snapshot 2
  }
  if (bank == 3)
  {
    Toggle(4, 8, 0, 127, "E", "chorus", "CHORUS");        //Toggle State Variable to toggle [0..9], CC Number [4, 5, 6,..], Low Value [0..127], High Value [0..127], Text Block [A..F], LowText [String], HighText [String]
  }
  if (bank == 4)
  {
    SnapshotFSMode();
  }
  if (bank == 5)
  {
    //Overdub Looper
    Serial.write(176);
    Serial.write(60);
    Serial.write(63);
  }
}
void SwitchElongPress()
{
  if (bank == 1)
  {
  }
  if (bank == 2)
  {
  }
  if (bank == 3)
  {
  }
  if (bank == 4)
  {
  }
  if (bank == 5)
  {
  }
}


//****** FFFF ************ FFFF ************ FFFF ************ FFFF ************ FFFF ************ FFFF ************ FFFF ************ FFFF ******
void SwitchFclick()
{
  if (bank == 1)
  {
    SetSnapshot(3);                                 //Set Snapshot 3
    SetToggleState(0, 11, LOW, "C", "solo");        //Set State of a specific toggle parameter: Variable [0..9], CC Number [4, 5, 6,..], LOW/HIGH, Text Block [A..F], Text [String]
  }
  if (bank == 2)
  {
    SetSnapshot(3);                               //Set Snapshot 3
  }
  if (bank == 3)
  {
    Toggle(5, 9, 0, 127, "F", "eq", "EQ");        //Toggle State Variable to toggle [0..9], CC Number [4, 5, 6,..], Low Value [0..127], High Value [0..127], Text Block [A..F], LowText [String], HighText [String]
  }
  if (bank == 4)
  {
    StompFSMode();
  }
  if (bank == 5)
  {
    //Play Looper
    Serial.write(176);
    Serial.write(61);
    Serial.write(64);
  }
}
void SwitchFlongPress()                           //Switch F longpress will ALWAYS be "Bank Up", so we dont have to distinguish the banks here
{
  //count bank down
  {
    if (bank < maxbank)
    {
      bank = bank + 1;
    }
    else bank = 1;
    // BankHasChanged = true;
    PrintBankName();
  }
}

void SwitchFlongPressStop()
{
  BankHasChanged = true;
}

//*********** General Functions *************************
void FillBankDisplay (String A, String B, String C, String D, String E, String F)
{
  SetDisplayCoordinates("A");
  lcd.print(A);
  SetDisplayCoordinates("B");
  lcd.print(B);
  SetDisplayCoordinates("C");
  lcd.print(C);
  SetDisplayCoordinates("D");
  lcd.print(D);
  SetDisplayCoordinates("E");
  lcd.print(E);
  SetDisplayCoordinates("F");
  lcd.print(F);
}

void PrintBankName()
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print (bank);
  lcd.print (": ");
  lcd.print (BankName [bank - 1]);
  delay (bankchangedelay);
}

void SetDisplayCoordinates(String TextBlock)
{
  if (TextBlock == "A")
  {
    displayline = 0;
    displayrow = 0;
  }
  if (TextBlock == "B")
  {
    displayline = 0;
    displayrow = 7;
  }
  if (TextBlock == "C")
  {
    displayline = 0;
    displayrow = 14;
  }
  if (TextBlock == "D")
  {
    displayline = 3;
    displayrow = 0;
  }
  if (TextBlock == "E")
  {
    displayline = 3;
    displayrow = 7;
  }
  if (TextBlock == "F")
  {
    displayline = 3;
    displayrow = 14;
  }
  lcd.setCursor(displayrow, displayline);
}

//**************** Device Specific Functions below this point (e.g. for Line6 HX Stomp) **************************
void SetSnapshot(int n)
{
  Serial.write(176);
  Serial.write(69);
  Serial.write(n - 1);
}

void SetPreset(int n)
{
  Serial.write(192);
  Serial.write(n);
}

void Toggle(int ts, int cc, int low, int high, String TextBlock, String LowText, String HighText)
{
  Serial.write(176);              //Write first of three bytes of CC Message
  Serial.write(cc);               //Write second of three bytes of CC Message
  if (ToggleState[ts] == LOW)     //Write third byte of CC Message (high or low value)
  {
    Serial.write(high);
    ToggleState[ts] = HIGH;
    SetDisplayCoordinates(TextBlock);
    lcd.print(HighText);
  }
  else
  {
    Serial.write(low);
    ToggleState[ts] = LOW;
    SetDisplayCoordinates(TextBlock);
    lcd.print(LowText);
  }
}

void SetToggleState(int ts, int cc, boolean state, String TextBlock, String Text)
{
  Serial.write(176);              //Write first of three bytes of CC Message
  Serial.write(cc);               //Write second of three bytes of CC Message
  ToggleState[ts] = state;
  if (ToggleState[ts] == LOW) Serial.write(0);
  if (ToggleState[ts] == HIGH) Serial.write(127);
  SetDisplayCoordinates(TextBlock);
  lcd.print(Text);
}

void NextFSMode()
{
  Serial.write(176);
  Serial.write(71);
  Serial.write(4);
}

void PrevFSMode()
{
  Serial.write(176);
  Serial.write(71);
  Serial.write(5);
}

void PresetFSMode()
{
  Serial.write(176);
  Serial.write(71);
  Serial.write(2);
}

void ScrollFSMode()
{
  Serial.write(176);
  Serial.write(71);
  Serial.write(1);
}

void SnapshotFSMode()
{
  Serial.write(176);
  Serial.write(71);
  Serial.write(3);
}

void StompFSMode()
{
  Serial.write(176);
  Serial.write(71);
  Serial.write(0);
}

void SnapshotUp()
{
  Serial.write(176);
  Serial.write(69);
  Serial.write(8);
}

void SnapshotDown()
{
  Serial.write(176);
  Serial.write(69);
  Serial.write(9);
}

void Footswitch4Press()
{
  Serial.write(176);
  Serial.write(52);
  Serial.write(127);
}

void Footswitch5Press()
{
  Serial.write(176);
  Serial.write(53);
  Serial.write(127);
}
