//-------------------------------------------------
//                  Yocto v2.x
//                 SEQ Functions
//-------------------------------------------------

/////////////////////Function//////////////////////
//Initialyse the sequencer before to run
void InitSeq()
{
  LoadSeqSetup();
  ppqn = 0;
  globalStepCount = 0;
  seq.configPage = 0;
  seq.configMode = FALSE;
  randomSeed(analogRead(0));
  seq.dir = FORWARD;
  seq.bpm = seq.defaultBpm;

  switch (seq.sync){
  case MASTER:
    DisconnectMidiHandleRealTime();
    TimerStart();//cf timer
    break;
  case SLAVE:
    TimerStop();
    MIDI.setHandleClock(HandleClock);
    MIDI.setHandleStart(HandleStart);
    MIDI.setHandleStop(HandleStop);
    MIDI.setHandleContinue(HandleContinue);
    break;
    seq.syncChanged = FALSE;
  }
}

//copy pattern to buffer
void CopyPatternToBuffer(byte patternNum)
{
  for (byte i = 0; i < NBR_INST; i++){ 
    bufferedPattern.inst[i] =  pattern[ptrnBuffer].inst[i] ;
    bufferedPattern.length[i] =  pattern[ptrnBuffer].length[i] ;
    bufferedPattern.scale[i] =  pattern[ptrnBuffer].scale[i] ;
    bufferedPattern.shuffle[i] =  pattern[ptrnBuffer].shuffle[i] ;
    bufferedPattern.dir[i] =  pattern[ptrnBuffer].dir[i] ;
    for (byte j = 0; j < NBR_STEP; j++){
      bufferedPattern.velocity[i][j] = pattern[ptrnBuffer].velocity[i][j]; 
      bufferedPattern.probality[i][j] = pattern[ptrnBuffer].probality[i][j];
      bufferedPattern.uTiming[i][j] = pattern[ptrnBuffer].uTiming[i][j]; 
      bufferedPattern.retrig[i][j] = pattern[ptrnBuffer].retrig[i][j];
    }
  }
  bufferedPattern.globalLength = pattern[ptrnBuffer].globalLength;
  bufferedPattern.globalScale = pattern[ptrnBuffer].globalScale;
  bufferedPattern.globalShuffle = pattern[ptrnBuffer].globalShuffle;
  bufferedPattern.globalDir = pattern[ptrnBuffer].globalDir;
  bufferedPattern.groupPos = pattern[ptrnBuffer].groupPos;
  bufferedPattern.groupLength = pattern[ptrnBuffer].groupLength;
  bufferedPattern.extLength = pattern[ptrnBuffer].extLength;
  bufferedPattern.totalAcc = pattern[ptrnBuffer].totalAcc;

  for ( byte j = 0; j < pattern[ptrnBuffer].extLength; j++){
    bufferedPattern.extNote[j] = pattern[ptrnBuffer].extNote[j];
  }
}

//paste buffer to current pattern
void PasteBufferToPattern(byte patternNum)
{
  for (byte i = 0; i < NBR_INST; i++){ 
    pattern[ptrnBuffer].inst[i] =  bufferedPattern.inst[i];
    pattern[ptrnBuffer].length[i] =   bufferedPattern.length[i] ;
    pattern[ptrnBuffer].scale[i] =   bufferedPattern.scale[i] ;
    pattern[ptrnBuffer].shuffle[i] =   bufferedPattern.shuffle[i] ;
    pattern[ptrnBuffer].dir[i] =   bufferedPattern.dir[i] ;
    for (byte j = 0; j < NBR_STEP; j++){
      pattern[ptrnBuffer].velocity[i][j] = bufferedPattern.velocity[i][j]; 
      pattern[ptrnBuffer].probality[i][j] =  bufferedPattern.probality[i][j];
      pattern[ptrnBuffer].uTiming[i][j] =  bufferedPattern.uTiming[i][j]; 
      pattern[ptrnBuffer].retrig[i][j] =  bufferedPattern.retrig[i][j]; 
    }
  }
  pattern[ptrnBuffer].globalLength =  bufferedPattern.globalLength;
  pattern[ptrnBuffer].globalScale =  bufferedPattern.globalScale;
  pattern[ptrnBuffer].globalShuffle =  bufferedPattern.globalShuffle;
  pattern[ptrnBuffer].globalDir =  bufferedPattern.globalDir;
  pattern[ptrnBuffer].groupPos =  bufferedPattern.groupPos;
  pattern[ptrnBuffer].groupLength =  bufferedPattern.groupLength;
  pattern[ptrnBuffer].extLength = bufferedPattern.extLength;
  pattern[ptrnBuffer].totalAcc = bufferedPattern.totalAcc;

  for ( byte j = 0; j < bufferedPattern.extLength; j++){
    pattern[ptrnBuffer].extNote[j] = bufferedPattern.extNote[j];
  }
}

//init pattern
void InitPattern()
{
  if (!group.priority){
    group.length = pattern[ptrnBuffer].groupLength;
    group.firstPattern = curPattern - pattern[ptrnBuffer].groupPos;
  }
  /////////////////////COULD BE REMOVED/////////////////////////////
  //Init Ride, Crash velocity to HIGH_VEL
  /*for (int stp = 0; stp < NBR_STEP; stp++){
   pattern[ptrnBuffer].velocity[CH][stp] = instVelHigh[CH];//
   pattern[ptrnBuffer].velocity[OH][stp] = instVelHigh[OH];//
   pattern[ptrnBuffer].velocity[PC2][stp] = instVelHigh[PC2];//
   pattern[ptrnBuffer].velocity[TOTAL_ACC][stp] = HIGH_VEL;//TOTAL_ACC
   pattern[ptrnBuffer].velocity[TRIG_OUT][stp] = HIGH_VEL;//TRIG_OUT
   pattern[ptrnBuffer].velocity[EXT_INST][stp] = HIGH_VEL;//EXT_INST
   }*/
  ////////////////////////////////////////////////////////////////////
  switch (pattern[ptrnBuffer].scale[curInst]){
  case  SCALE_16:
    scaleBtn.counter = 0;
    break;
  case  SCALE_32:
    scaleBtn.counter = 1;
    break;
  case  SCALE_8t:
    scaleBtn.counter = 2;
    break;
  case  SCALE_16t:
    scaleBtn.counter = 3;
    break;
  }
}

//Convert Instrument Word to Step Word
void InstToStepWord()
{
  for (int a = 0; a < NBR_STEP; a++){
    pattern[ptrnBuffer].step[a] = 0;
    for (int b = 0; b < NBR_INST; b++){
      if (bitRead(pattern[ptrnBuffer].inst[b],a)) bitSet(pattern[ptrnBuffer].step[a],b);
    }
  }
}

///////////////////////shift left pattern///////////////////////////////
void ShiftLeftPattern()
{

  patternWasEdited = TRUE;

  //only shift selected instruments
  if (instBtn && !shiftBtn.pressed){
    //remember first step velocity value
    byte temp = pattern[ptrnBuffer].velocity[curInst][0];

    //Shift Left trig value
    if (bitRead(pattern[ptrnBuffer].inst[curInst],0))pattern[ptrnBuffer].inst[curInst] = pattern[ptrnBuffer].inst[curInst] >> 1 | (1 << pattern[ptrnBuffer].length[curInst]);
    else pattern[ptrnBuffer].inst[curInst] = pattern[ptrnBuffer].inst[curInst] >> 1;

    //shift left velocity value
    for (int stp = 0; stp <= pattern[ptrnBuffer].length[curInst]; stp++){
      if (stp == pattern[ptrnBuffer].length[curInst]) pattern[ptrnBuffer].velocity[curInst][stp] = temp;
      else pattern[ptrnBuffer].velocity[curInst][stp] = pattern[ptrnBuffer].velocity[curInst][stp+1];
    }
  }
  // shift full pattern
  else{
    byte temp[NBR_INST];
    //Loop as many instrument
    for (int nbrInst = 0; nbrInst < NBR_INST; nbrInst++){
      //remember first step velocity value of each instrument
      temp[nbrInst] = pattern[ptrnBuffer].velocity[nbrInst][0];

      //Shift Left trig value of all instrument
      if (bitRead(pattern[ptrnBuffer].inst[nbrInst],0))pattern[ptrnBuffer].inst[nbrInst] = pattern[ptrnBuffer].inst[nbrInst] >> 1 | (1 << pattern[ptrnBuffer].length[nbrInst]);
      else pattern[ptrnBuffer].inst[nbrInst] = pattern[ptrnBuffer].inst[nbrInst] >> 1;

      //shift left velocity value of all instrument
      for (int stp = 0; stp <= pattern[ptrnBuffer].length[nbrInst]; stp++){
        if (stp == pattern[ptrnBuffer].length[nbrInst]) pattern[ptrnBuffer].velocity[nbrInst][stp] = temp[nbrInst];
        else pattern[ptrnBuffer].velocity[nbrInst][stp] = pattern[ptrnBuffer].velocity[nbrInst][stp + 1];      
      }
    }
  }
}

///////////////////////////Shift Right pattern/////////////////////////////////////////
void ShiftRightPattern()
{
  patternWasEdited = TRUE; 

  //shift right only selected instrument
  if (instBtn && !shiftBtn.pressed){
    //---------------Variable---------------------
    //Instrument Trig value
    unsigned int trigValue = pattern[ptrnBuffer].inst[curInst];
    //Last step variable
    byte lastStep = pattern[ptrnBuffer].length[curInst];
    //remember last step velocity value
    byte temp = pattern[ptrnBuffer].velocity[curInst][lastStep];

    //Shift right trig value
    if (bitRead(trigValue,lastStep)){
      trigValue = trigValue << 1 | 1;
      bitClear(trigValue, lastStep + 1);
    }
    else trigValue = trigValue << 1;

    //Set new trig value
    pattern[ptrnBuffer].inst[curInst] = trigValue;

    //shift right velocity
    for (int stp = lastStep; stp >= 0; stp--){
      if (stp == 0) pattern[ptrnBuffer].velocity[curInst][stp] = temp;
      else pattern[ptrnBuffer].velocity[curInst][stp] = pattern[ptrnBuffer].velocity[curInst][stp-1];
    }
  }

  //shift right all pattern
  else{
    //---------------Variable---------------------
    //Instrument Trig value
    unsigned int trigValue[NBR_INST]; 
    //Last step variable
    byte lastStep[NBR_INST]; 
    //remember last step velocity value
    byte temp[NBR_INST]; 

    //Loop as many instrument
    for (int a = 0; a < NBR_INST; a++){
      //Update variable
      trigValue[a] = pattern[ptrnBuffer].inst[a];
      lastStep[a] = pattern[ptrnBuffer].length[a];
      temp[a] = pattern[ptrnBuffer].velocity[a][lastStep[a]];

      //Shift right full pattern
      if (bitRead(trigValue[a],lastStep[a])){
        trigValue[a] = trigValue[a] << 1 | 1;
        bitClear(trigValue[a], lastStep[a] + 1);
      }
      else trigValue[a] = trigValue[a] << 1;

      //Set news trig values
      pattern[ptrnBuffer].inst[a] = trigValue[a];

      //Shift right full pattern velocity values 
      for (int stp = lastStep[a]; stp >= 0; stp--){
        if (stp == 0) pattern[ptrnBuffer].velocity[a][stp] = temp[a];
        else pattern[ptrnBuffer].velocity[a][stp] = pattern[ptrnBuffer].velocity[a][stp-1];      
      }
    }
  }

}





















