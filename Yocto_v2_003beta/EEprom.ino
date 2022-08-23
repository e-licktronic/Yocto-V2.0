//-------------------------------------------------
//                  Yocto v2.x
//                   
// The Drum sequencer has two 24M02 I2C EEPROM
// Each EEPROM is 4 x 65535 bytes so in total 2 x 4 x 65535 = 524280 bytes (512kB)
// A pattern is composed to:
//  -16 instruments  which is:
//              -Trig value   = 2 bytes  0 to 16 steps so it is 16bits value
//              -Length value = 1 byte   0 to 16 
//              -Scale value  = 1 byte   0 to 3  (1/16, 1/32, 1/8t or 1/16t)
//              -Shuffle value= 1 byte   
//              -Dir. value   = 1 byte
//              -Velocity     = 16 bytes  one for each step 0 to 127
//              -Probality    = 16 bytes  one for each step 0 to 100%
//              -uTiming      = 16 bytes  0 to 24  (in real -12 to +12 but can't store negative value)
//              -Retrig       = 16 bytes  TO BE DEFINE
//                     TOTAL  = 70 bytes per intruments
//
//  -Pattern External Instr= 128 bytes  (for 128 notes)
//  -Pattern global length = 1 byte
//  -Pattern global scale  = 1 byte
//  -Pattern global shuffle= 1 byte
//  -Pattern global dir.   = 1 byte
//  -Pattern group length  = 1 byte
//  -Pattern group pos.    = 1 byte
//  -Pattern active Acc    = 2 bytes 
//  -UNUSED pattern setup  = 24 bytes
// 
//  PATTERN_SIZE = (70 x 16) +  = 1280 bytes (9 pages of 128 bytes)
//  OFFSET_PATTERN = 0
//
// Setup size:
//  -OFFSET_SETUP = TRACK_OFFSET + (TRACK_SIZE * MAX_TRACK) = 73728;
//  -SETUP_SIZE = 64 bytes  only 4 bytes used NOW...
//
//-------------------------------------------------

#define PTRN_SIZE          (PTRN_TRIG_SIZE + PTRN_SETUP_SIZE + PTRN_VEL_SIZE + PTRN_PROB_SIZE + PTRN_uTIMING_SIZE + PTRN_RETRIG_SIZE + PTRN_EXT_SIZE) //1280 bytes
#define PTRN_OFFSET        (unsigned long)(0)
#define PTRN_TRIG_SIZE     (unsigned long)(96)
#define PTRN_TRIG_OFFSET   (PTRN_OFFSET + PTRN_TRIG_SIZE)
#define PTRN_SETUP_SIZE    (unsigned long)(32)
#define PTRN_SETUP_OFFSET  (PTRN_TRIG_OFFSET + PTRN_SETUP_SIZE)
#define PTRN_VEL_SIZE      (unsigned long)(256)
#define PTRN_VEL_OFFSET    (PTRN_SETUP_OFFSET + PTRN_VEL_SIZE)
#define PTRN_PROB_SIZE     (unsigned long)(256)
#define PTRN_PROB_OFFSET   (PTRN_VEL_OFFSET + PTRN_PROB_SIZE)
#define PTRN_uTIMING_SIZE  (unsigned long)(256)
#define PTRN_uTIMING_OFFSET (PTRN_PROB_OFFSET + PTRN_uTIMING_SIZE)
#define PTRN_RETRIG_SIZE   (unsigned long)(256)
#define PTRN_RETRIG_OFFSET (PTRN_uTIMING_OFFSET + PTRN_RETRIG_SIZE)
#define PTRN_EXT_SIZE      (unsigned long)(128)
#define PTRN_EXT_OFFSET    (PTRN_RETRIG_OFFSET + PTRN_EXT_SIZE)
#define TRACK_SIZE         (unsigned long)(1024)//unused at this time but could be use to store Pattern Chain
#define TRACK_OFFSET       (PTRN_SIZE * MAX_PTRN)
#define OFFSET_SETUP       (TRACK_OFFSET + (TRACK_SIZE * MAX_TRACK))
#define SETUP_SIZE         (unsigned long)(64)
#define OFFSET_GROUP       (unsigned long)(37) //save only .groupPos and .groupLength
#define GROUP_SIZE         (unsigned long)(2)
#define MAX_PAGE_SIZE      (unsigned long)(128)
#define HRDW_ADDRESS       0x50
#define HRDW_ADDRESS_UP    (0x50 | B100)
#define DELAY_WR           10
////////////////////////Function//////////////////////

//save pattern
void SavePattern(byte patternNbr)
{
  unsigned long adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE);
  WireBeginTX(adress); 
  // Serial.println(adress);
  //TRIG-----------------------------------------------
  for (byte i = 0; i < NBR_INST; i++){ 
    byte lowbyte =  (pattern[ptrnBuffer].inst[i] & 0xFF);
    byte highbyte = (pattern[ptrnBuffer].inst[i] >> 8) & 0xFF;
    Wire.write((byte)(lowbyte)); 
    Wire.write((byte)(highbyte));
    Wire.write((byte)(pattern[ptrnBuffer].length[i]));
    Wire.write((byte)(pattern[ptrnBuffer].scale[i]));
    Wire.write((byte)(pattern[ptrnBuffer].shuffle[i]));
    Wire.write((byte)(pattern[ptrnBuffer].dir[i]));
     MIDI.read();// to improve slave sync response
  }//96 bytes

  //GLOBAL SETUP-----------------------------------------------
  Wire.write((byte)(pattern[ptrnBuffer].globalLength));
  Wire.write((byte)(pattern[ptrnBuffer].globalScale));
  Wire.write((byte)(pattern[ptrnBuffer].globalShuffle));
  Wire.write((byte)(pattern[ptrnBuffer].globalDir));
  Wire.write((byte)(pattern[ptrnBuffer].groupPos));
  Wire.write((byte)(pattern[ptrnBuffer].groupLength));
  Wire.write((byte)(pattern[ptrnBuffer].totalAcc));
  //103 bytes => 25 byte left ;)

  Wire.endTransmission();//end page transmission
  delay(DELAY_WR);//delay between each write page

  MIDI.read();// to improve slave sync response

  //VELOCITY-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 2; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
    WireBeginTX(adress);
    for (byte i = 0; i < 8; i++){//loop as many instrument for a page
      for (byte j = 0; j < NBR_STEP; j++){
        Wire.write((byte)(pattern[ptrnBuffer].velocity[i + (nbrPage * 8)][j] & 0xFF)); 
      }
      MIDI.read();// to improve slave sync response
    }
    Wire.endTransmission();//end of 128 bytes transfer
    delay(DELAY_WR);//delay between each write page

    
  }//2 * 128 bytes = 256 bytes

  //======================================NOT USED YET=============================================
  /*
  //PROBALITY-----------------------------------------------
   for(int nbrPage = 0; nbrPage < 2; nbrPage++){
   adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_VEL_OFFSET);
   //Serial.println(adress);
   WireBeginTX(adress);
   for (byte i = 0; i < 8; i++){//loop as many instrument for a page
   for (byte j = 0; j < NBR_STEP; j++){
   Wire.write((byte)(pattern[ptrnBuffer].probality[i + (4*nbrPage)][j] & 0xFF)); 
   }
   }
   Wire.endTransmission();//end of 64 bytes transfer
   delay(DELAY_WR);//delay between each write page
   }//2 * 128 bytes = 256 bytes
   
   //uTIMING-----------------------------------------------
   for(int nbrPage = 0; nbrPage < 2; nbrPage++){
   adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_PROB_OFFSET);
   //Serial.println(adress);
   WireBeginTX(adress);
   for (byte i = 0; i < 8; i++){//loop as many instrument for a page
   for (byte j = 0; j < NBR_STEP; j++){
   Wire.write((byte)(pattern[ptrnBuffer].uTiming[i + (4*nbrPage)][j] & 0xFF)); 
   }
   }
   Wire.endTransmission();//end of 64 bytes transfer
   delay(DELAY_WR);//delay between each write page
   }//2 * 128 bytes = 256 bytes
   
   //RETRIG-----------------------------------------------
   for(int nbrPage = 0; nbrPage < 2; nbrPage++){
   adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_uTIMING_OFFSET);
   //Serial.println(adress);
   WireBeginTX(adress);
   for (byte i = 0; i < 8; i++){//loop as many instrument for a page
   for (byte j = 0; j < NBR_STEP; j++){
   Wire.write((byte)(pattern[ptrnBuffer].retrig[i + (4*nbrPage)][j] & 0xFF)); 
   }
   }
   Wire.endTransmission();//end of 64 bytes transfer
   delay(DELAY_WR);//delay between each write page
   }//2 * 128 bytes = 256 bytes
   */
  //========================================================================================================

  //EXT INST-----------------------------------------------
  adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + PTRN_RETRIG_OFFSET);
  WireBeginTX(adress);
  for (byte j = 0; j < MAX_PAGE_SIZE; j++){
    Wire.write((byte)(pattern[ptrnBuffer].extNote[j] & 0xFF));
  }//128 bytes  
  MIDI.read();// to improve slave sync response
  Wire.endTransmission();//end page transmission
  delay(DELAY_WR);//delay between each write page
  // 128 bytes

  
  
}//END OF SAVE PATTERN FUNCTION

/////////////////////////////////////////////////-LOAD-////////////////////////////////
//Load Pattern
void LoadPattern(byte patternNbr)
{
  unsigned long adress = (unsigned long)(PTRN_OFFSET + patternNbr * PTRN_SIZE);
  WireBeginTX(adress); 
  Wire.endTransmission();
  byte hrdwAddress = HRDW_ADDRESS + (adress >> 16);
  Wire.requestFrom(hrdwAddress ,MAX_PAGE_SIZE); //request a 128 bytes page
  //TRIG-----------------------------------------------
  for(int i =0; i<NBR_INST;i++){
    pattern[!ptrnBuffer].inst[i] = (unsigned long)((Wire.read() & 0xFF) | (( Wire.read() << 8) & 0xFF00));
    pattern[!ptrnBuffer].length[i]  = Wire.read();
    pattern[!ptrnBuffer].scale[i]  = Wire.read();
    pattern[!ptrnBuffer].shuffle[i]  = Wire.read();
    pattern[!ptrnBuffer].dir[i]  = Wire.read();
  }
  //96 bytes requested
  //SETUP-----------------------------------------------
  pattern[!ptrnBuffer].globalLength = Wire.read();
  pattern[!ptrnBuffer].globalScale = Wire.read();
  pattern[!ptrnBuffer].globalShuffle = Wire.read();
  pattern[!ptrnBuffer].globalDir = Wire.read();
  pattern[!ptrnBuffer].groupPos = Wire.read();
  pattern[!ptrnBuffer].groupLength = Wire.read();
  pattern[!ptrnBuffer].totalAcc = Wire.read();
  //103 bytes requested

    //VELOCITY-----------------------------------------------
  for(int nbrPage = 0; nbrPage < 2; nbrPage++){
    adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
    //Serial.println(adress);
    WireBeginTX(adress);
    Wire.endTransmission();
    hrdwAddress = HRDW_ADDRESS + (adress >> 16);
    Wire.requestFrom(hrdwAddress,MAX_PAGE_SIZE); //request of  128 bytes
    for (byte i = 0; i < 8; i++){//loop as many instrument for a page
      for (byte j = 0; j < NBR_STEP; j++){
        pattern[!ptrnBuffer].velocity[i + (nbrPage * 8)][j] = (Wire.read() & 0xFF);
      }
    }
  }

  //======================================NOT USED YET=============================================
  /*
  //PROBALITY-----------------------------------------------
   for(int nbrPage = 0; nbrPage < 2; nbrPage++){
   adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_VEL_OFFSET);
   WireBeginTX(adress);
   Wire.endTransmission();
   hrdwAddress = HRDW_ADDRESS + (adress >> 16);
   Wire.requestFrom(hrdwAddress,MAX_PAGE_SIZE); //request of  128 bytes
   for (byte i = 0; i < 8; i++){//loop as many instrument for a page
   for (byte j = 0; j < NBR_STEP; j++){
   pattern[!ptrnBuffer].probality[i + (4*nbrPage)][j] = (Wire.read() & 0xFF);
   }
   }
   }
   
   //uTIMING-----------------------------------------------
   for(int nbrPage = 0; nbrPage < 2; nbrPage++){
   adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_PROB_OFFSET);
   WireBeginTX(adress);
   Wire.endTransmission();
   hrdwAddress = HRDW_ADDRESS + (adress >> 16);
   Wire.requestFrom(hrdwAddress,MAX_PAGE_SIZE); //request of  128 bytes
   for (byte i = 0; i < 8; i++){//loop as many instrument for a page
   for (byte j = 0; j < NBR_STEP; j++){
   pattern[!ptrnBuffer].uTiming[i + (4*nbrPage)][j] = (Wire.read() & 0xFF);
   }
   }
   }
   
   //RETRIG-----------------------------------------------
   for(int nbrPage = 0; nbrPage < 2; nbrPage++){
   adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_uTIMING_OFFSET);
   WireBeginTX(adress);
   Wire.endTransmission();
   hrdwAddress = HRDW_ADDRESS + (adress >> 16);
   Wire.requestFrom(hrdwAddress,MAX_PAGE_SIZE); //request of  128 bytes
   for (byte i = 0; i < 8; i++){//loop as many instrument for a page
   for (byte j = 0; j < NBR_STEP; j++){
   pattern[!ptrnBuffer].retrig[i + (4*nbrPage)][j] = (Wire.read() & 0xFF);
   }
   }
   }
   */
  //======================================================================================================

  //EXT INST-----------------------------------------------
  adress = (unsigned long)(PTRN_OFFSET + (patternNbr * PTRN_SIZE) + PTRN_RETRIG_OFFSET);
  WireBeginTX(adress);
  Wire.endTransmission();
  Wire.requestFrom(HRDW_ADDRESS,MAX_PAGE_SIZE); //request of  128 bytes

  for (byte j = 0; j < MAX_PAGE_SIZE; j++){
    pattern[!ptrnBuffer].extNote[j] = Wire.read();
  }
}

//Track save
void SaveTrack(byte trackNbr)
{
  //Last two bytes of the track corresponding to the length 1 to 999
  //Have to be 1020 and 1021 in the array index !!
  byte lowbyte =  (byte)(track[trkBuffer].length & 0xFF);
  byte highbyte = (byte)((track[trkBuffer].length >> 8) & 0xFF);
  track[trkBuffer].patternNbr[1020] = lowbyte;
  track[trkBuffer].patternNbr[1021] = highbyte;

  unsigned long adress;
  //Need 8 pages to save 1024 bytes track size
  for(int nbrPage = 0; nbrPage < 8 ; nbrPage++){
    adress = (unsigned long)(TRACK_OFFSET + (trackNbr * TRACK_SIZE) + (MAX_PAGE_SIZE * nbrPage));
    WireBeginTX(adress);
    for (byte i = 0; i < MAX_PAGE_SIZE; i++){//loop as many instrument for a page
      Wire.write((byte)(track[trkBuffer].patternNbr[i + (MAX_PAGE_SIZE * nbrPage)] & 0xFF)); 
    }
    Wire.endTransmission();//end of 128 bytes transfer
    delay(DELAY_WR);//delay between each write page
  }

}

//Load track
void LoadTrack(byte trackNbr)
{
  unsigned long adress;
  //Need 8 pages to load 1024 bytes track size
  for(int nbrPage = 0; nbrPage < 8 ; nbrPage++){
    adress = (unsigned long)(TRACK_OFFSET + (trackNbr * TRACK_SIZE) + (MAX_PAGE_SIZE * nbrPage));
    WireBeginTX(adress);
    Wire.endTransmission();
    byte hrdwAddress = HRDW_ADDRESS + (adress >> 16);
    Wire.requestFrom(hrdwAddress,MAX_PAGE_SIZE); //request of  128 bytes
    for (byte i = 0; i < MAX_PAGE_SIZE; i++){//loop as many instrument for a page
      track[trkBuffer].patternNbr[i + (MAX_PAGE_SIZE * nbrPage)] = (Wire.read() & 0xFF);
    }
  }
  byte lowbyte = track[trkBuffer].patternNbr[1020];
  byte highbyte = track[trkBuffer].patternNbr[1021];
  track[trkBuffer].length =  (unsigned int)(lowbyte | (highbyte << 8));

}

//Save Setup
void SaveSeqSetup()
{
  unsigned long adress = (unsigned long)(OFFSET_SETUP);
  WireBeginTX(adress);
  Wire.write((byte)(seq.sync)); 
  Wire.write((byte)(seq.defaultBpm));
  Wire.write((byte)(seq.TXchannel));
  Wire.write((byte)(seq.RXchannel));
  Wire.write((byte)(seq.ptrnChangeSync));
  Wire.endTransmission();//end page transmission
  delay(DELAY_WR);//delay between each write page
}

//Load Setup
void LoadSeqSetup()
{
  unsigned long adress = (unsigned long)(OFFSET_SETUP);
  WireBeginTX(adress); 
  Wire.endTransmission();
  byte hrdwAddress = HRDW_ADDRESS + (adress >> 16);
  Wire.requestFrom(hrdwAddress,SETUP_SIZE); //
  seq.sync = (Wire.read() & 0xFF);
  seq.sync = constrain(seq.sync, 0, 1);
  seq.defaultBpm = (Wire.read() & 0xFF);
  seq.defaultBpm = constrain(seq.defaultBpm, MIN_BPM, MAX_BPM);
  seq.TXchannel = (Wire.read() & 0xFF);
  seq.TXchannel = constrain(seq.TXchannel, 1 ,16);
  seq.RXchannel = (Wire.read() & 0xFF);
  seq.RXchannel = constrain(seq.RXchannel, 1 ,16);
  seq.ptrnChangeSync = (Wire.read() & 0xFF);
  seq.ptrnChangeSync = constrain(seq.ptrnChangeSync, 0, 1);
}

//Save pattern group
void SavePatternGroup(byte firstPattern, byte length)
{
  for (int a = 0; a <= length ; a++){
    unsigned long adress = (unsigned long)(PTRN_OFFSET + ((firstPattern + a) * PTRN_SIZE) + OFFSET_GROUP);
    WireBeginTX(adress);
    /* Serial.print("adresse=");
     Serial.println(adress);
     Serial.print("groupPos=");
     Serial.println(a);
     Serial.print("groupLeght=");
     Serial.println(length);*/
    Wire.write((byte)(a)); //pattern[ptrnBuffer].groupPos
    Wire.write((byte)(length));//pattern[ptrnBuffer].groupLength
    Wire.endTransmission();//end page transmission
    delay(DELAY_WR);//delay between each write page
  }
}

//Clear pattern group
void ClearPatternGroup(byte firstPattern, byte length)
{
  for (int a = 0; a <= length ; a++){
    unsigned long adress = (unsigned long)(PTRN_OFFSET + ((firstPattern + a) * PTRN_SIZE) + OFFSET_GROUP);
    WireBeginTX(adress);
    Wire.write((byte)(0)); //pattern[ptrnBuffer].groupPos
    Wire.write((byte)(0));//pattern[ptrnBuffer].groupLength
    Wire.endTransmission();//end page transmission
    delay(DELAY_WR);//delay between each write page
  }
}

//Load pattern group type => POSITION = 0 or LENGTH = 1
byte LoadPatternGroup(byte patternNum, byte type)
{
  unsigned long adress = (unsigned long)(PTRN_OFFSET + (patternNum * PTRN_SIZE) + OFFSET_GROUP + type);
  WireBeginTX(adress);
  Wire.endTransmission();
  Wire.requestFrom(HRDW_ADDRESS,(unsigned long)(1)); //
  byte data = (Wire.read() & 0xFF);
  return data;
}


//==================================================================================================
//==================================================================================================
//init pattern in the EEprom
void InitEEprom()
{
  unsigned long adress;

  //Pattern init
  for (byte nbrPattern = 0; nbrPattern < MAX_PTRN ; nbrPattern++)
  {
    unsigned long adress = (unsigned long)(PTRN_OFFSET + nbrPattern * PTRN_SIZE);
    WireBeginTX(adress); 
    // Serial.println(adress);
    //TRIG-----------------------------------------------
    for (byte i = 0; i < NBR_INST; i++){  
      Wire.write((byte)(0));//(pattern[ptrnBuffer].inst[i] & 0xFF);
      Wire.write((byte)(0));//(pattern[ptrnBuffer].inst[i] >> 8) & 0xFF;
      Wire.write((byte)(DEFAULT_LEN - 1));//(pattern[ptrnBuffer].length[i]));
      Wire.write((byte)(DEFAULT_SCALE));//(pattern[ptrnBuffer].scale[i]));
      Wire.write((byte)(DEFAULT_SHUF));//(pattern[ptrnBuffer].shuffle[i]));
      Wire.write((byte)(0));//(pattern[ptrnBuffer].dir[i]));
    }//96 bytes

    //GLOBAL SETUP-----------------------------------------------
    Wire.write((byte)(DEFAULT_LEN - 1));//(pattern[ptrnBuffer].globalLength));
    Wire.write((byte)(DEFAULT_SCALE));//(pattern[ptrnBuffer].globalScale));
    Wire.write((byte)(DEFAULT_SHUF));//(pattern[ptrnBuffer].globalShuffle));
    Wire.write((byte)(0));//(pattern[ptrnBuffer].globalDir));
    Wire.write((byte)(0));//(pattern[ptrnBuffer].groupPos));
    Wire.write((byte)(0));//(pattern[ptrnBuffer].groupLength));
    //102 bytes
    for(int a =0; a < 26; a++){
      Wire.write( (byte)(0));//unused parameters
    }//128 bytes FIRST PAGE

    Wire.endTransmission();//end page transmission
    delay(DELAY_WR);//delay between each write page

    //VELOCITY-----------------------------------------------
    for(int nbrPage = 0; nbrPage < 2; nbrPage++){
      adress = (unsigned long)(PTRN_OFFSET + (nbrPattern * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_SETUP_OFFSET);
      //Serial.println(adress);
      WireBeginTX(adress);
      for (byte i = 0; i < 8; i++){//loop as many instrument for a page
        for (byte j = 0; j < NBR_STEP; j++){
          Wire.write((byte)(40));//(pattern[ptrnBuffer].velocity[i + 4*nbrPage][j] & 0xFF)); 
        }
      }
      Wire.endTransmission();//end of 64 bytes transfer
      delay(DELAY_WR);//delay between each write page
    }//2 * 128 bytes = 256 bytes

    //PROBALITY-----------------------------------------------
    for(int nbrPage = 0; nbrPage < 2; nbrPage++){
      adress = (unsigned long)(PTRN_OFFSET + (nbrPattern * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_VEL_OFFSET);
      //Serial.println(adress);
      WireBeginTX(adress);
      for (byte i = 0; i < 8; i++){//loop as many instrument for a page
        for (byte j = 0; j < NBR_STEP; j++){
          Wire.write((byte)(0));//(pattern[ptrnBuffer].probality[i + 4*nbrPage][j] & 0xFF)); 
        }
      }
      Wire.endTransmission();//end of 64 bytes transfer
      delay(DELAY_WR);//delay between each write page
    }//2 * 128 bytes = 256 bytes

    //uTIMING-----------------------------------------------
    for(int nbrPage = 0; nbrPage < 2; nbrPage++){
      adress = (unsigned long)(PTRN_OFFSET + (nbrPattern * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_PROB_OFFSET);
      //Serial.println(adress);
      WireBeginTX(adress);
      for (byte i = 0; i < 8; i++){//loop as many instrument for a page
        for (byte j = 0; j < NBR_STEP; j++){
          Wire.write((byte)(0));//(pattern[ptrnBuffer].uTiming[i + 4*nbrPage][j] & 0xFF)); 
        }
      }
      Wire.endTransmission();//end of 64 bytes transfer
      delay(DELAY_WR);//delay between each write page
    }//2 * 128 bytes = 256 bytes

    //RETRIG-----------------------------------------------
    for(int nbrPage = 0; nbrPage < 2; nbrPage++){
      adress = (unsigned long)(PTRN_OFFSET + (nbrPattern * PTRN_SIZE) + (MAX_PAGE_SIZE * nbrPage) + PTRN_uTIMING_OFFSET);
      //Serial.println(adress);
      WireBeginTX(adress);
      for (byte i = 0; i < 8; i++){//loop as many instrument for a page
        for (byte j = 0; j < NBR_STEP; j++){
          Wire.write((byte)(0));//(pattern[ptrnBuffer].uTiming[i + 4*nbrPage][j] & 0xFF)); 
        }
      }
      Wire.endTransmission();//end of 64 bytes transfer
      delay(DELAY_WR);//delay between each write page
    }

    //EXT INST-----------------------------------------------
    adress = (unsigned long)(PTRN_OFFSET + (nbrPattern * PTRN_SIZE) + PTRN_RETRIG_OFFSET);
    //Serial.println(adress);
    WireBeginTX(adress);
    for (byte j = 0; j < MAX_PAGE_SIZE; j++){
      Wire.write((byte)(0));
    }//128 bytes -> fisrt page
    Wire.endTransmission();//end page transmission
    delay(DELAY_WR);//delay between each write page

    //Show init statut on steps Leds
    static unsigned int tempInitLeds;
    tempInitLeds |= bitSet(tempInitLeds, (nbrPattern/8));
    SetDoutLed(tempInitLeds, 0, 0);
  }//END PATTERN INIT

  //TRACK INST-----------------------------------------------

  for (byte nbrTrack = 0; nbrTrack < MAX_TRACK ; nbrTrack++)
  {   
    for (byte nbrPage = 0; nbrPage < TRACK_SIZE/MAX_PAGE_SIZE; nbrPage++){
      adress = (unsigned long)(TRACK_OFFSET + (nbrTrack * TRACK_SIZE) + (nbrPage * MAX_PAGE_SIZE));
      WireBeginTX(adress);
      for (byte j = 0; j < MAX_PAGE_SIZE; j++){
        Wire.write((byte)(0));
      }//128 bytes -> fisrt page
      Wire.endTransmission();//end page transmission
      delay(DELAY_WR);//delay between each write page
    }
    //Show init statut on steps Leds
    static unsigned int tempInitLeds;
    tempInitLeds |= bitSet(tempInitLeds, nbrTrack);
    SetDoutLed(tempInitLeds, 0, 0);
  }//END TRACK INIT

  //Sequencer Setup initialize----------------------------------
  adress = (unsigned long)(OFFSET_SETUP);
  WireBeginTX(adress);
  Wire.write((byte)(MASTER));//seq.sync)); 
  Wire.write((byte)(DEFAULT_BPM));//seq.defaultBpm));
  Wire.write((byte)(1));//seq.TXchannel));
  Wire.write((byte)(1));//seq.RXchannel));
  Wire.endTransmission();//end page transmission
  delay(DELAY_WR);//delay between each write page

  //Set the 3 last byte of the first EEprom to 0, 1, 2 value to NEVER init EEPROM again
  WireBeginTX(262137);
  Wire.write((byte)(0));//seq.defaultBpm));
  Wire.write((byte)(1));//seq.TXchannel));
  Wire.write((byte)(2));//seq.RXchannel));
  Wire.endTransmission();//end page transmission
  delay(DELAY_WR);//delay between each write page

  /*  //Set the 3 last byte of the internal Atmega1284P EEprom to 0, 1, 2 value to NEVER init EEPROM again
   EEPROM.write(4093, 0);
   EEPROM.write(4094, 1);
   EEPROM.write(4095, 2);*/

}

void DebugEEprom(){
  Serial.println(OFFSET_SETUP);
}

//==================================================================================================
//Wire begin Function to set address on the I2C bus
//==================================================================================================
void WireBeginTX(unsigned long address)
{
  byte hardwareAddress;
  hardwareAddress = HRDW_ADDRESS + (address >> 16); 
  Wire.beginTransmission(hardwareAddress);
  Wire.write((byte)(address >> 8));
  Wire.write((byte)(address & 0xFF));
}
























































