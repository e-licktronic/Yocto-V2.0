//-------------------------------------------------
//                  Yocto v2.x
//                  BPM
//-------------------------------------------------

/////////////////////Function//////////////////////
//Timer interrupt
ISR(TIMER1_COMPA_vect) { 
  CountPPQN(); 
}

//Tick proceed each pulse
void CountPPQN()
{
  blinkVeryFast =! blinkVeryFast;
  if (ppqn % (PPQN/2) == 0) blinkTempo = !blinkTempo;
  if (ppqn  % (pattern[ptrnBuffer].scale[curInst]/2) == 0) blinkFast[curInst] = LOW;
  //if (ppqn % 4 == 0) MIDI.sendRealTime(Clock);//MidiSend(CLOCK_CMD);//as NAVA seq is 96ppqn we need to send clock each 4 internal ppq

  if (ppqn % 4 == 0) {                                
    while (!(UCSR1A & (1 << UDRE1))) {
    };                                            // [zabox] directly adressing the uart fixes the midi clock lag
    UDR1 = CLOCK_CMD; //Tick
  }
  if (seq.sync == MASTER){
    if (ppqn % 4 == 0){
      DIN_CLK_HIGH;
      dinClkState = HIGH;
    }
    else if (ppqn % 4 == 2) {
      DIN_CLK_LOW;
      dinClkState = LOW;
    }    
  }

  if (isRunning)
  {
    if (ppqn % pattern[ptrnBuffer].globalScale == (pattern[ptrnBuffer].globalScale/2)) tapStepCount++;

    if (tapStepCount > pattern[ptrnBuffer].globalLength) tapStepCount = 0;

    // Initialize the step value
    stepValue = 0; 

    for (byte z = 0; z < NBR_INST; z++){
      if (ppqn % pattern[ptrnBuffer].scale[z] == 0) stepChanged[z] = TRUE;//Step changed ?

      //-----------------------------------------------------------------------------
      //Here is everything happen each step
      //-----------------------------------------------------------------------------
      if (((ppqn + shuffle[(pattern[ptrnBuffer].shuffle[z])-1][shufPolarity[z]]) % pattern[ptrnBuffer].scale[z] == 0) && stepChanged[z])
      {//Each Step
        stepChanged[z] = FALSE;//flag that we already trig this step
        shufPolarity[z] = !shufPolarity[z];
        blinkFast[z] = HIGH;// TO BE REMOVED OUT OF THIS FOR LOOP

        //sequencer direction for each instruments-----------
        switch (pattern[ptrnBuffer].dir[z]){
        case FORWARD:
          curStep[z] = stepCount[z];
          break;
        case BACKWARD:
          curStep[z] = pattern[ptrnBuffer].length[z] - stepCount[z];
          break;
        case PING_PONG:
          if (curStep[z] == pattern[ptrnBuffer].length[z] && changeDir[z] == 1) changeDir[z] = 0;
          else if (curStep[z] == 0 && changeDir[z] == 0)  changeDir[z] = 1;
          if (changeDir[z]) curStep[z] = stepCount[z];
          else curStep[z] = pattern[ptrnBuffer].length[z] - stepCount[z];
          break;
        case RANDOM:
          curStep[z] = random(0, pattern[ptrnBuffer].length[z]+1);
          break;
        }
        //Set each step value in a 16bit word depending step counter and direction of each instrument
        //Compare the current step of each instrument and store in "stepValue"
        if ( z != TOTAL_ACC ){//&& z != TR1 && z != TR2){
          stepValue |= ((bitRead(pattern[ptrnBuffer].inst[z],curStep[z])) << z);
        }

        //Set the multiplexer for velocity out just before Trig
        stepCount[z]++;
      }
    }

    //incremente global step count according the longer instrument length
    if (ppqn % pattern[ptrnBuffer].scale[longerInstLength] == 0){
      globalStepCount++;
    }

    //Set Metronome value to be trigged
    if (ppqn % pattern[ptrnBuffer].globalScale == 0){
      stepValue |= (bitRead(metronome,(globalStepCount-1)) << RM);
    }

    //Add Mute value to StepValue
    stepValue &= ~muteInst;

    //Is there something to trig ?
    if (stepValue) {
      if (bitRead(stepValue, TR1)){
        TR1_LOW;//Trigout pulse 
        trig1State = HIGH;
      }
      if (bitRead(stepValue, TR2)){
        TR2_HIGH;//Trigout pulse 
        trig2State = HIGH;
      }
      SetMux();
      //Trig according mute value
      SetDoutTrig(stepValue);      
      delayMicroseconds(2000);
      ResetDoutTrig();

      //Trig external instrument-------------------------------------
      if (bitRead(stepValue, EXT_INST))//pattern[ptrnBuffer].inst[EXT_INST], curStep[EXT_INST]))
      {
        InitMidiNoteOff();
        MidiSendNoteOn(seq.TXchannel, pattern[ptrnBuffer].extNote[noteIndexCpt], HIGH_VEL);
        midiNoteOnActive = TRUE;
        noteIndexCpt++;//incremente external inst note index
      }
      if (noteIndexCpt > pattern[ptrnBuffer].extLength){
        noteIndexCpt = 0;
      }
    }//END IF STEP VALUE

    for (byte z = 0; z < NBR_INST; z++){
      // if(stepChanged[z]){
      //  stepChanged[z] = FALSE;//flag that we already trig this step
      //  stepCount[z]++;
      if (stepCount[z] > pattern[ptrnBuffer].length[z]){
        stepCount[z] = 0;
      }
      //}
    }

    /* //init each instrument step counter
     for (byte z = 0; z < NBR_INST; z++){
     if (stepCount[z] > pattern[ptrnBuffer].length[z]){
     stepCount[z] = 0;
     }
     }*/

    if (globalStepCount > pattern[ptrnBuffer].globalLength){// && (ppqn % 24 == pattern[ptrnBuffer].scale - 1))
      endMeasure = TRUE;
      trackPosNeedIncremante = TRUE;
      globalStepCount = 0;
      //In pattern play mode this peace of code execute in the PPQNCount function
      if(nextPatternReady && curSeqMode == PTRN_PLAY){
        nextPatternReady = FALSE;
        keybOct = DEFAULT_OCT;
        noteIndex = 0;
        InitMidiNoteOff();
        //When pattern changed init all instrument step counter
        for (byte z = 0; z < NBR_INST; z++){
          stepCount[z] = 0;
        }
        ptrnBuffer = !ptrnBuffer;
        InitPattern();//SHOULD BE REMOVED WHEN EEPROM WILL BE INITIALIZED
        //SetHHPattern();
        InstToStepWord();
      }
    }
    if (ppqn % pattern[ptrnBuffer].globalScale == 4 && globalStepCount == 0){ 
      endMeasure = FALSE;
    }
  }

  if (ppqn++ >= PPQN ) ppqn = 1;
}// END COUNT FUNCTION

//
void Metronome(boolean state)
{
  if (state){
    metronome = 0x1111;//trig RM every time
    pattern[ptrnBuffer].velocity[RM][0] = 127;
    pattern[ptrnBuffer].velocity[RM][4] = 30;
    pattern[ptrnBuffer].velocity[RM][8] = 30;
    pattern[ptrnBuffer].velocity[RM][12] = 30;
  }
  else{
    metronome = 0;
    pattern[ptrnBuffer].velocity[RM][0] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][4] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][8] = HIGH_VEL;
    pattern[ptrnBuffer].velocity[RM][12] = HIGH_VEL;
  }
}








































