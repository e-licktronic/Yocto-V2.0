//-------------------------------------------------
//                  Yocto v2.x
//                  main program
//-------------------------------------------------

/////////////////////Include/////////////////////
#include <SPI.h>
#include <LiquidCrystal.h>
#include <WireN.h>
#include "define.h"
#include "string.h"
#include <MIDI.h>
#include <EEPROM.h>

#include <MemoryFree.h>

LiquidCrystal lcd(18, 19, 20, 21, 22, 23);

////////////////////////Setup//////////////////////
void setup()
{

  InitIOTestMode();
  //Voices test Mode
  if(bitRead(PINA, 3)== LOW){
    voicesTestMode = HIGH;
  }
  //User interface Test Mode
  if(bitRead (PINA, 4) == LOW){
    userInterfaceTestMode = HIGH;
  }

  InitIO();//cf Dio
  InitButtonCounter();//cf Button

#if DEBUG
  Serial.begin(115200);
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
#endif


  lcd.begin(16, 2);
  lcd.createChar(0,font0);
  lcd.createChar(1,font1);
  lcd.createChar(2,font2);
  lcd.createChar(3,font3);
  lcd.createChar(4,font4);
  lcd.createChar(5,font5);



  //==================================================================================================
  //Auto Init EEprom
  //if the last (address 262137, 262138, 262139)  bytes of the first EEprom is != to respectivly 0, 1, 2
  // is that the external EEPROMs wasn't init  (Should be execute only ONE time)
  //================================================================================================== 

  WireBeginTX(262137); 
  Wire.endTransmission();
  Wire.requestFrom(0X53, 3); //
  byte data1 = (Wire.read() & 0xFF);
  byte data2 = (Wire.read() & 0xFF);
  byte data3 = (Wire.read() & 0xFF);
  if (data1 + data2 + data3 != 3){
    LcdPrintEEpromInit();
    InitEEprom();
  }

  ScanDinBoot();

  //TM2 adjustement for velocity
  if (btnEnterByte == BTN_ENTER){
    LcdPrintTM2Adjust();
    while (1){
      SetDacA(MAX_VEL);
    }
  }


  InitSeq();// cf Seq
  //Load default track
  LoadTrack(0);
  //Load default pattern
  LoadPattern(0);
  ptrnBuffer = !ptrnBuffer;
  InitPattern();
  //SetHHPattern();
  InstToStepWord();
  SetDoutTrig(0);

  MIDI.begin();//Serial1.begin(MIDI_BAUD);
  MIDI.setHandleNoteOn(HandleNoteOn);
  MIDI.setHandleNoteOff(HandleNoteOff);
  MIDI.setInputChannel(seq.RXchannel);

  sei();

  //Testing Modes -------------------------------
  if(voicesTestMode){
    VoicesTestMode();
  }
  if(userInterfaceTestMode){
    UserInterfaceTestMode();
  }
  //-----------------------------------------------

  lcd.setCursor(0, 0);
  lcd.print("Yocto v2.003beta");
  lcd.setCursor(0,1);
  lcd.print("by e-licktronic ");
  delay(1000);

}

////////////////////////Loop///////////////////////
void loop()
{  
  InitMidiRealTime();
  MIDI.read();
  ButtonGet(); 
  EncGet();  
  SetLeds(); 
  SeqConfiguration(); 
  SeqParameter();
  KeyboardUpdate();
  LcdUpdate();
  SetTrigTime();
}




































