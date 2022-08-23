 //-------------------------------------------------
//                  Yocto v2.x
//                  Test Mode
//-------------------------------------------------

/////////////////////Function//////////////////////

//Voices terst mode send a trig each second on all analog Voices
void VoicesTestMode()
{
  while(1){
    lcd.setCursor(0, 0);
    lcd.print("Voices Test Mode");
    lcd.setCursor(0,1);
    lcd.print("                 ");
    SetMuxTest();//Set velocity to 127
    SetDoutTrig(0xFFFF);//Trig all instrument    
    delayMicroseconds(1000);
    ResetDoutTrig();
    delay(1000);
  }
}

//
void UserInterfaceTestMode()
{
  byte ledOrderConf[16] ={ 
    5, 6, 7, 0, 9, 10, 11, 12, 13, 14, 15, 8, 1, 2, 3, 4    };
  byte ledOrderMenu[8] = { 
    2, 1, 4, 3, 5, 6, 7, 0    };
  while(1){
    lcd.setCursor(0, 0);
    lcd.print(" User Interface ");
    lcd.setCursor(0,1);
    lcd.print("   Test mode    ");
    for (byte b = 0; b < 2; b++){
      for (byte a = 0; a < 16; a++){
        SetDoutLed(1 << a, 0, 0);
        delay(500);
      }
      for (byte a = 0; a < 16; a++){
        SetDoutLed(0, 1 << ledOrderConf[a], 0);
        delay(500);
      }
      for (byte a = 0; a < 8; a++){
        SetDoutLed(0, 0, 1 << ledOrderMenu[a]);
        delay(500);
      }
    }
    while(1){
      unsigned int tempMenuLed = 0;
      unsigned int tempConfigLed = 0;
      byte menuBtnToLed[8] = {0, 6, 3, 1, 2, 4, 5, 7};
      byte configBtnToLed[12] = {0, 7, 6, 5, 12, 11, 10, 9, 13, 14, 15, 8};
      ScanDin();
      unsigned int configBtns = (dinSr[4] <<8) | dinSr[3];
      byte menuBtns = dinSr[2];
      for (byte a = 0; a < 12; a++){
        if (bitRead(configBtns, a)) bitSet(tempConfigLed, configBtnToLed[a]);
        else bitClear(tempConfigLed, configBtnToLed[a]);      
      }
      for (byte a = 0; a < 8; a++){
       if (bitRead(menuBtns, a)) bitSet(tempMenuLed, menuBtnToLed[a]);
       else bitClear(tempMenuLed, menuBtnToLed[a]);
      }
      SetDoutLed((dinSr[1] <<8) | dinSr[0], tempConfigLed, tempMenuLed);
    }
  }
}



