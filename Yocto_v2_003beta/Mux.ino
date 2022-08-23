//-------------------------------------------------
//                  Yocto v2.x
//                  Dio utility
//-------------------------------------------------

/////////////////////Function//////////////////////
//Update Velocity of each instruments
void SetMux()
{
  //Select second  Multiplexer IC111 CY, OH, CH and PC2 Accent
  SelectSecondMux();
  for (byte a = 0; a < 4; a++){
    PORTA &= 0b00011111;//Mask to clear last 3 bits of the PORTA
    PORTA |= (muxAddr[a] << 5);//Set bits A, B and of the multiplexer
    SetDacA(pattern[ptrnBuffer].velocity[8 + a][curStep[8 + a]] + (((bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep[TOTAL_ACC])& ~(bitRead(muteInst, TOTAL_ACC)))) ? (pattern[ptrnBuffer].totalAcc * 4) : 0));
    delayMicroseconds(5);
  }
  //Select First Multiplexer IC114 BD, SD, LT, MT, HT, RM, HC and  CB Accent
  SelectFirstMux();
  for (byte a = 0; a < 8; a++){
    PORTA &= 0b00011111;//Mask to clear last 3 bits of the PORTA
    PORTA |= (muxAddr[a] << 5);//Set bits A, B and of the multiplexer
    SetDacA(pattern[ptrnBuffer].velocity[a][curStep[a]] + (((bitRead(pattern[ptrnBuffer].inst[TOTAL_ACC], curStep[TOTAL_ACC]) & ~(bitRead(muteInst, TOTAL_ACC)))) ? (pattern[ptrnBuffer].totalAcc * 4) : 0));
    delayMicroseconds(5);
  }
}

//Set ACC to max in Voice test mode
void SetMuxTest()
{
  //Select second  Multiplexer IC111 CY, OH, CH and PC2 Accent
  SelectSecondMux();
  for (byte a = 0; a < 4; a++){
    PORTA &= 0b00011111;//Mask to clear last 3 bits of the PORTA
    PORTA |= (muxAddr[a] << 5);//Set bits A, B and of the multiplexer 
    SetDacA(127);
    delayMicroseconds(5);
  }
  //Select First Multiplexer IC114 BD, SD, LT, MT, HT, RM, HC and  CB Accent
  SelectFirstMux();
  for (byte a = 0; a < 8; a++){
    PORTA &= 0b00011111;//Mask to clear last 3 bits of the PORTA
    PORTA |= (muxAddr[a] << 5);//Set bits A, B and of the multiplexer
    SetDacA(127);
    delayMicroseconds(5);
  }
}

void SetMuxTrigMidi(byte inst, byte velocity)
{

  midiVelocity[inst] = velocity;

  //Select second  Multiplexer IC111 CY, OH, CH and PC2 Accent
  SelectSecondMux();
  for (byte a = 0; a < 4; a++){
    PORTA &= 0b00011111;//Mask to clear last 3 bits of the PORTA
    PORTA |= (muxAddr[a] << 5);//Set bits A, B and of the multiplexer
    SetDacA(midiVelocity[8 + a]);
    delayMicroseconds(5);
  }
  //Select First Multiplexer IC114 BD, SD, LT, MT, HT, RM, HC and  CB Accent
  SelectFirstMux();
  for (byte a = 0; a < 8; a++){
    PORTA &= 0b00011111;//Mask to clear last 3 bits of the PORTA
    PORTA |= (muxAddr[a] << 5);//Set bits A, B and of the multiplexer
    SetDacA(midiVelocity[a]);
    delayMicroseconds(5);
  }
}

void SelectFirstMux(){
  delayMicroseconds(15);                                          //Wait stabilisation  [zabox] moved for better visibility, 15us are enough
  bitWrite(MUX_INH_PORT, MUX_INH2_BIT, 0);
  bitWrite(MUX_INH_PORT, MUX_INH1_BIT, 1); 
  delayMicroseconds(5);                                           // [zabox] charge s/h caps
}

void SelectSecondMux(){
  delayMicroseconds(15);                                          //Wait stabilisation  [zabox] moved for better visibility, 15us are enough
  bitWrite(MUX_INH_PORT, MUX_INH1_BIT, 0);
  bitWrite(MUX_INH_PORT, MUX_INH2_BIT, 1); 
  delayMicroseconds(5);                                           // [zabox] charge s/h caps
}



















