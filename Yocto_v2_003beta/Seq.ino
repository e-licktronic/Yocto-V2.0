//-------------------------------------------------
//                  Yocto v2.x
//                 SEQ Parameter
//-------------------------------------------------

/////////////////////Function//////////////////////
void SeqParameter()
{  

 /* if(lastStepBtn.justPressed){
    //Serial.println(nextPattern);
    for (byte a = 0; a < 16; a++){
      //Serial.print(a);
      // Serial.print(" :");
      // Serial.println(nextPattern);
    }
  }*/
  readButtonState = StepButtonGet(MOMENTARY);
  //can not access config when isRunning
  if (isRunning && seq.configMode){
    seq.configMode = FALSE;
    needLcdUpdate = TRUE;
  }
  if (curSeqMode != MUTE) muteBtn.counter = 0;
  if (!seq.configMode) seq.configPage = 0;

  //-------------------Encoder button---------------------------
  if(encBtn.justPressed){
    curIndex++;
    if (curIndex >= MAX_CUR_POS)  curIndex = 0;
    needLcdUpdate = TRUE;
  }
  //-------------------play button---------------------------
  if (playBtn.justPressed || midiStart){
    isRunning = TRUE;
    isStop = FALSE;
    ppqn = 0;
    globalStepCount = 0;
    for(byte z = 0; z < NBR_INST; z++){
      stepCount[z] = 0;
      shufPolarity[z] = 0;//Init shuffle polarity
      changeDir[z] = 1;//restart Forward direction on each instrument
    }
    tapStepCount = 0;
    stopBtn.counter = 0;
    noteIndexCpt = 0;//init ext instrument note index counter
    MIDI.sendRealTime(Start);//;MidiSend(START_CMD);
    DIN_START_HIGH;
    dinStartState = HIGH;
    DIN_CLK_HIGH;
    dinClkState = HIGH;
  }

  //-------------------stop button------------------------------
  if ((stopBtn.justPressed && !instBtn) || midiStop || midiContinue){
    //Init Midi note off
    SendAllNoteOff();//InitMidiNoteOff();
    if (midiStop) stopBtn.counter = 0;
    else if (midiContinue) stopBtn.counter = 1;
    stopBtn.counter++;
    switch (stopBtn.counter){
    case 1:
      isStop = TRUE;
      isRunning = FALSE;
      MIDI.sendRealTime(Stop);//;MidiSend(STOP_CMD);
      DIN_START_LOW;
      dinStartState = LOW;
      break;
    case 2:
      isStop = FALSE;
      isRunning = TRUE;
      stopBtn.counter = 0;
      ppqn = 0;
      MIDI.sendRealTime(Continue);//MidiSend(CONTINU_CMD);
      DIN_START_HIGH;
      dinStartState = HIGH;
      break;
    }
  }

  //-------------------Shift button pressed------------------------------
  if (shiftBtn.pressed){
    /*if (trkBtn.justPressed){
     needLcdUpdate = TRUE;
     curSeqMode = TRACK_WRITE;
     keyboardMode = FALSE;
     seq.configMode  = FALSE;
     }*/
    /* if (ptrnBtn.justPressed) {
     needLcdUpdate = TRUE;
     if (curSeqMode == PTRN_STEP) curSeqMode = PTRN_PLAY;
     else curSeqMode = PTRN_STEP;
     seq.configMode  = FALSE;
     trackNeedSaved = FALSE;
     }*/
    if (tapBtn.justPressed){
      curSeqMode = PTRN_TAP;
      needLcdUpdate = TRUE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
      trackNeedSaved = FALSE;
    }
    if (bankBtn.justPressed){
      CopyPatternToBuffer(curPattern);
    } // copy current pattern to the buffer
    if (muteBtn.justPressed){
      PasteBufferToPattern(curPattern);
      patternWasEdited = TRUE;
    } //paste copy buffered pattern to the current pattern number

    //sequencer configuration page
    if (tempoBtn.justPressed && !isRunning){
      seq.configMode  = TRUE;
      seq.configPage++;
      curIndex = 0;
      if (seq.configPage > MAX_CONF_PAGE){
        seq.configPage = 0;
        seq.configMode  = FALSE;
      }
      needLcdUpdate = TRUE;
    }
  }
  //-------------------Shift button released------------------------------
  else {
    if (trkBtn.justPressed){
      if(curSeqMode == TRACK_WRITE) curSeqMode = TRACK_PLAY;
      else curSeqMode = TRACK_WRITE;
      needLcdUpdate = TRUE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
    }
    // if (backBtn.justPressed) ;//back  track postion
    //if (fwdBtn.justPressed) ;//foward track postion
    if (numBtn.pressed) ;//select Track number
    if (ptrnBtn.justPressed){
      if (curSeqMode == PTRN_STEP) curSeqMode = PTRN_PLAY;
      else curSeqMode = PTRN_STEP;
      needLcdUpdate = TRUE;
      keyboardMode = FALSE;
      seq.configMode  = FALSE;
      trackNeedSaved = FALSE;
    }
    if (tapBtn.justPressed) ShiftLeftPattern();
    if (dirBtn.justPressed) ShiftRightPattern();
    if (guideBtn.justPressed){
      guideBtn.counter++;
      switch (guideBtn.counter){
      case 1:
        Metronome(TRUE);
        break;
      case 2:
        Metronome(FALSE);
        guideBtn.counter = 0;
        break;
      }
    }
    //if (bankBtn && readButtonState) curBank = FirstBitOn();
    if (muteBtn.justPressed){
      muteBtn.counter++;
      switch (muteBtn.counter){
      case 1:
        prevSeqMode = curSeqMode;
        curSeqMode = MUTE; //paste copy buffered pattern to the current pattern number
        break;
      case 2:
        curSeqMode = prevSeqMode;
        muteBtn.counter = 0;
        break;
      }
    }
    if (curSeqMode != MUTE) muteBtn.counter = 0;
    if (!seq.configMode) seq.configPage = 0;
    if (tempoBtn.justRelease) needLcdUpdate = TRUE;
  }

  //==============================================================================
  //////////////////////////MODE PATTERN EDIT/////////////////////////////////

  if (curSeqMode == PTRN_STEP || curSeqMode == PTRN_TAP){

    static boolean curInstChanged;//flag that curInstchanged to not update LCD more than one time

    //-------------------Select instrument------------------------------
    //Match with trig shift register out (cf schematic)
    if(readButtonState == 0) doublePush = 0; //init double push if all step button  released 

    if (instBtn && readButtonState){
      curInstChanged = TRUE;
      keyboardMode = FALSE;
      curInst = FirstBitOn();
    }
    if (curInstChanged && stepsBtn.justRelease){
      needLcdUpdate = TRUE;
      curInstChanged = FALSE;
    }
    if (instBtn && enterBtn.justPressed){
      curInst = TOTAL_ACC;
      needLcdUpdate = TRUE;
    }
    /* if (instBtn && stopBtn.justPressed){
     curInst = TRIG_OUT;
     needLcdUpdate = TRUE;
     }*/
    if (shiftBtn.pressed && guideBtn.justPressed){
      curInst = EXT_INST;
      needLcdUpdate = TRUE;
    }
    //Serial.println(curInst);

    //-------------------Clear Button------------------------------
    if (clearBtn.pressed && !keyboardMode && curSeqMode != PTRN_TAP && isRunning){
      bitClear (pattern[ptrnBuffer].inst[curInst], curStep[curInst]);
      pattern[ptrnBuffer].velocity[curInst][curStep[curInst]] = instVelLow[curInst];
      //if (curInst == CH) pattern[ptrnBuffer].velocity[CH][curStep] = instVelHigh[HH];//update HH velocity that OH is trigged correctly   

      patternWasEdited = TRUE;   
    }
    if (clearBtn.justPressed && !keyboardMode && curSeqMode != PTRN_TAP && !isRunning){
      //clear full pattern
      for (int a = 0; a < NBR_INST; a++){
        pattern[ptrnBuffer].inst[a] = 0;
        pattern[ptrnBuffer].shuffle[a] = DEFAULT_SHUF;
        pattern[ptrnBuffer].length[a] = NBR_STEP - 1;
        pattern[ptrnBuffer].scale[a] = SCALE_16;
        pattern[ptrnBuffer].dir[a] = 0;//0 is forward
      }
      ////////////////////////////////////////////////////////////////////////
      //init all intrument velocity ||   PEUT ETRE SUPPRIMER, A TESTER !!!  ||
      /* for (int b = 0; b < NBR_STEP; b++){
       pattern[ptrnBuffer].velocity[BD][b] = instVelLow[BD];
       pattern[ptrnBuffer].velocity[SD][b] = instVelLow[SD];
       pattern[ptrnBuffer].velocity[LT][b] = instVelLow[LT];
       pattern[ptrnBuffer].velocity[MT][b] = instVelLow[MT];
       pattern[ptrnBuffer].velocity[HT][b] = instVelLow[HT];
       pattern[ptrnBuffer].velocity[RM][b] = instVelLow[RM];
       pattern[ptrnBuffer].velocity[HC][b] = instVelLow[HC];
       pattern[ptrnBuffer].velocity[CB][b] = instVelLow[CB];
       pattern[ptrnBuffer].velocity[CY][b] = instVelLow[CY];
       }*/
      ////////////////////////////////////////////////////////////////////////

      //pattern[ptrnBuffer].globalShuffle = DEFAULT_SHUF;
      //pattern[ptrnBuffer].globalLength = NBR_STEP - 1;
      //pattern[ptrnBuffer].globalScale = SCALE_16;
      keybOct = DEFAULT_OCT;
      patternWasEdited = TRUE;
      needLcdUpdate = TRUE;
    }


    //-------------------shuffle Button------------------------------
    if (shufBtn.justPressed || shufBtn.justRelease) needLcdUpdate = TRUE;

    //-------------------scale button------------------------------
    if (scaleBtn.justPressed && !keyboardMode){
      byte a;
      needLcdUpdate = TRUE;
      patternWasEdited = TRUE;
      scaleBtn.counter++;
      if (scaleBtn.counter == 4) scaleBtn.counter = 0;
      a = scaleValue[scaleBtn.counter];
      if (shiftBtn.pressed){
        pattern[ptrnBuffer].globalScale = a;
        //init all scale of each instrument when globalscale changed
        for (byte z = 0; z < NBR_INST; z++){
          pattern[ptrnBuffer].scale[z] = pattern[ptrnBuffer].globalScale;
        }
      }
      else pattern[ptrnBuffer].scale[curInst] = a;
    } 
    //Updte scaleBtn counter if pattern change with a different scale
    if (pattern[ptrnBuffer].scale[curInst] != scaleValue[scaleBtn.counter]){
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

    //-------------------last step button------------------------------
    if (lastStepBtn.pressed && stepsBtn.justPressed){//readButtonState) {
      if (!shiftBtn.pressed){
        pattern[ptrnBuffer].length[curInst] = FirstBitOn();
        pattern[ptrnBuffer].globalLength = 0;// init global length
        //set the highest length value as global length and remember which instrument is the longer length
        for (byte z = 0; z < NBR_INST; z++){
          if (pattern[ptrnBuffer].length[z] > pattern[ptrnBuffer].globalLength){
            pattern[ptrnBuffer].globalLength = pattern[ptrnBuffer].length[z];
            longerInstLength = z;
          }
        }
      }
      else if (shiftBtn.pressed){
        pattern[ptrnBuffer].globalLength = FirstBitOn();
        for (byte z = 0; z < NBR_INST; z++){
          pattern[ptrnBuffer].length[z] = pattern[ptrnBuffer].globalLength;
        }
      }
      needLcdUpdate = TRUE;
      patternWasEdited = TRUE;
      //Serial.println(pattern[ptrnBuffer].globalLength);
    }
    //-------------------Steps buttons------------------------------
    /////////////////////////////STEP EDIT///////////////////////////
    if (curSeqMode == PTRN_STEP){
      if (stepsBtn.justRelease) doublePush = FALSE;
      if(!lastStepBtn.pressed && !instBtn && !keyboardMode){
        if (isRunning)
        {
          pattern[ptrnBuffer].inst[curInst] = InstValueGet(pattern[ptrnBuffer].inst[curInst]);//cf InstValueGet()
          nextPattern = curPattern;//To prevent undesired poattern change during switching PLAY and STEP mode
        }
        else if (!isRunning)
        {//Return pattern number
          if (stepsBtn.pressed){
            if (bankBtn.pressed){
              if(FirstBitOn() >= MAX_BANK) curBank = MAX_BANK;
              else curBank = FirstBitOn();
              nextPattern = curBank * NBR_PATTERN + (curPattern % NBR_PATTERN);
              if(curPattern != nextPattern) selectedPatternChanged = TRUE;
              group.length = 0;
            }
            else{//pattern group edit------------------------------------------------------
              if (SecondBitOn())
              {
                trackPosNeedIncremante = FALSE;                                                               // [zabox] fixes group bug
                group.length = SecondBitOn() - FirstBitOn();
                nextPattern = group.firstPattern = FirstBitOn() + curBank * NBR_PATTERN;
                doublePush = TRUE;
                group.priority = TRUE;
                //Store groupe in eeprom
                if(enterBtn.justPressed){
                  group.priority = FALSE;
                  byte tempLength;
                  byte tempPos;
                  //Test if one the  selected pattern is already in a Group
                  for (int a = 0; a <= group.length; a++){
                    tempLength = LoadPatternGroup(group.firstPattern + a, LENGTH);
                    if (tempLength){
                      tempPos = LoadPatternGroup(group.firstPattern + a, POSITION);
                      ClearPatternGroup(group.firstPattern + a - tempPos, tempLength);
                    }
                  }
                  SavePatternGroup(group.firstPattern, group.length);
                }
              }
              else if (!doublePush){
                group.priority = FALSE;
                nextPattern = FirstBitOn() + curBank * NBR_PATTERN;
                if(enterBtn.justPressed){
                  ClearPatternGroup(nextPattern - pattern[ptrnBuffer].groupPos, pattern[ptrnBuffer].groupLength);
                  group.length = 0;
                }
                group.pos = pattern[ptrnBuffer].groupPos;
              }
              if(curPattern != nextPattern) selectedPatternChanged = TRUE;
            }
          }
        }
      }
      if (trackPosNeedIncremante && group.length ){//&& stepCount > 0)
        group.pos++;
        if (group.pos > group.length) group.pos = 0;
        nextPattern = group.firstPattern + group.pos;
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        trackPosNeedIncremante = FALSE;
        needLcdUpdate = TRUE;
      }

      //--------------------------------sequencer run direction-----------------------
      if (shiftBtn.pressed && dirBtn.justPressed){
        if(instBtn){
          //Change current instrument direction
          if (pattern[ptrnBuffer].dir[curInst]++ >= MAX_SEQ_DIR) pattern[ptrnBuffer].dir[curInst] = FORWARD;
        }
        else {
          byte dir;
         if (pattern[ptrnBuffer].dir[curInst]++ >= MAX_SEQ_DIR) pattern[ptrnBuffer].dir[curInst] = FORWARD; 
          dir = pattern[ptrnBuffer].dir[curInst];
          //Change all instrument direction in PATTERN STEP MODE
          for (byte z =0; z < NBR_INST; z++){
             pattern[ptrnBuffer].dir[z] = dir;
          }
        }
      }
    }
    //////////////////////////////TAP EDIT///////////////////////////////
    else if (curSeqMode == PTRN_TAP)
    {
      //static byte lastHHtrigged;// remmeber last OH or CH was trigged to prevent OH noise when trigget other instruments

      if (clearBtn.pressed){
        bitClear (pattern[ptrnBuffer].inst[curInst], curStep[curInst]);
        // if (curInst == CH) pattern[ptrnBuffer].velocity[CH][curStep[curInst]] = HIGH_VEL;//update HH velocity that OH is trigged correctly
        patternWasEdited = TRUE;
      } 
      if (!lastStepBtn.pressed && !instBtn)
      {
        static boolean doublePushOH;

        if (bitRead(readButtonState,12) && bitRead(readButtonState,13)) doublePushOH = 1;
        else doublePushOH = 0;

        for (byte a = 0; a < NBR_STEP_BTN; a++){
          stepBtn[a].curState = bitRead(readButtonState,a);


          if (stepBtn[a].curState != stepBtn[a].prevState){
            if ((stepBtn[a].pressed == LOW) && (stepBtn[a].curState == HIGH)){
              static byte tempVel[16];//store temp instrument velocity
              switch (a){
              case BD_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[BD];
                tempVel[BD] = instVelHigh[BD];
                break;              
              case SD_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[SD];
                tempVel[SD] = instVelHigh[SD];
                break;
              case LT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[LT];
                tempVel[LT] = instVelHigh[LT];
                break;
              case MT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[MT];
                tempVel[MT] = instVelHigh[MT];
                break;
              case HT_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[HT];
                tempVel[HT] = instVelHigh[HT];
                break;
              case PC2_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[PC2];
                tempVel[PC2] = instVelHigh[PC2];
                break;
              case CH_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[CH];
                tempVel[CH] = instVelHigh[CH];
                break;
              case RM_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[RM];
                tempVel[RM] = instVelHigh[RM];
                break;
              case HC_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[HC];
                tempVel[HC] = instVelHigh[HC];
                break;
              case CY_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[CY];
                tempVel[CY] = instVelHigh[CY];
                break;
              case OH_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[OH];
                tempVel[OH] = instVelHigh[OH];
                break;
              case CB_BTN:
                if (isRunning) pattern[ptrnBuffer].velocity[instOut[a]][tapStepCount] = instVelHigh[CB];
                tempVel[CB] = instVelHigh[CB];
                break;
              }


              //-----SET Velocity Values-----//
              //Set first multiplexer IC111 value
              bitWrite(MUX_INH_PORT, MUX_INH1_BIT, 0);
              bitWrite(MUX_INH_PORT, MUX_INH2_BIT, 1);   
              SetDacA(tempVel[CY]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B111 << 5;
              SetDacA(tempVel[OH]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B11 << 5;
              SetDacA(tempVel[CH]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B101 << 5;
              SetDacA(tempVel[PC2]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B1 << 5;
              ////////////////////////////////////////////
              delayMicroseconds(100);//Coudl BE REMOVE !!!!
              ////////////////////////////////////////////
              //Set Second multiplexer values
              bitWrite(MUX_INH_PORT, MUX_INH1_BIT, 1);
              bitWrite(MUX_INH_PORT, MUX_INH2_BIT, 0);   
              SetDacA(tempVel[BD]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B111 << 5;
              SetDacA(tempVel[SD]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B11 << 5;
              SetDacA(tempVel[LT]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B101 << 5;
              SetDacA(tempVel[MT]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B1 << 5;
              SetDacA(tempVel[HT]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B110 << 5;
              SetDacA(tempVel[RM]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B10 << 5;
              SetDacA(tempVel[HC]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B100 << 5;
              SetDacA(tempVel[CB]);//
              PORTA = dinStartState | dinClkState << 1 | 1 << 2 | B0 << 5;

              SetDoutTrig(1 << instOut[a]);
              if(isRunning) bitSet(tempInst[instOut[a]],tapStepCount);
              delayMicroseconds(2000);
              SetDoutTrig(0);// Reset Trig
            }      
          }
          stepBtn[a].prevState = stepBtn[a].curState;
        }//END FOR LOOP
      }
      if (endMeasure){//Update pattern at the end of measure to not get a double trig
        for (int inst = 0; inst < NBR_INST; inst++){
          if (tempInst[inst])//if instruments was edited
          {
            pattern[ptrnBuffer].inst[inst] |= tempInst[inst];
            tempInst[inst] = 0;// init tempInst
            patternWasEdited = TRUE;
          }
        }
      }
    }//END IF PTRN_TAP MODE
  }//ENDIF MODE EDIT

  //=======================================================================================
  //////////////////////////MODE PATTERN PLAY.../////////////////////////////////

  if (curSeqMode == PTRN_PLAY){
    //-------------------------------select pattern-----------------------------------
    if (stepsBtn.justRelease) doublePush = FALSE;
    if (readButtonState){
      if (bankBtn.pressed){
        if(FirstBitOn() >= MAX_BANK) curBank = MAX_BANK;
        else curBank = FirstBitOn();
        nextPattern = curBank * NBR_PATTERN + (curPattern % NBR_PATTERN);
        group.length = 0;//should be 0 to play the right next pattern
        if((curPattern != nextPattern) && !isRunning) selectedPatternChanged = TRUE;
      }
      else{
        //Group selected
        if (SecondBitOn())
        {
          group.length = SecondBitOn() - FirstBitOn();
          nextPattern = group.firstPattern = FirstBitOn() + curBank * NBR_PATTERN;
          if (isRunning && (seq.ptrnChangeSync == SYNC)) {                                                    // [zabox] fixes group bug while running
            group.pos = group.length;                                                                           
          }
          doublePush = TRUE;
          group.priority = TRUE;
        }
        //Only one pattern selected
        else if (!doublePush){
          group.priority = FALSE;
          group.length = 0;//should be 0 to play the right next pattern
          nextPattern = FirstBitOn() + curBank * NBR_PATTERN;
          group.pos = pattern[ptrnBuffer].groupPos;
        }
        if(curPattern != nextPattern && stepsBtn.justPressed) {  // [zabox] [1.027] fixes pattern change bug in slave mode
          if(seq.ptrnChangeSync == SYNC){
            if (isRunning) needLcdUpdate = TRUE;//selected pattern changed so we need to update display 
            else selectedPatternChanged = TRUE;
          }
          if(seq.ptrnChangeSync == FREE){
            selectedPatternChanged = TRUE;
          }
        }
      }
    }

    //--------------------------------pattern next update---------------------------
    if (trackPosNeedIncremante && group.length ){// && stepCount > 0)
      if (group.pos > group.length) group.pos = 0;
      nextPattern = group.firstPattern + group.pos;
      group.pos++;
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      trackPosNeedIncremante = FALSE;
      needLcdUpdate = TRUE;
    }
    else if (trackPosNeedIncremante && group.length == 0){
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      trackPosNeedIncremante = FALSE;
    }

  }//END IF MODE PLAY PATTERN

  //===================================================================================================================================

  //===================================================================================================================================

  //////////////////////////MODE TRACK WRITE/////////////////////////////////

  if (curSeqMode == TRACK_WRITE){
    //-------------------------------select pattern-----------------------------------
    if (stepsBtn.justPressed){

      if (bankBtn.pressed){
        if(FirstBitOn() >= MAX_BANK) curBank = MAX_BANK;
        else curBank = FirstBitOn();
        nextPattern = curBank * NBR_PATTERN + (curPattern % NBR_PATTERN);
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      }
      else if(numBtn.pressed){
        trk.next = FirstBitOn();
        selectedTrackChanged = TRUE;
        needLcdUpdate = TRUE;
      }
      else{
        nextPattern = FirstBitOn() + curBank * NBR_PATTERN;
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      }
    }
    //decremente track position
    if (backBtn.justPressed){
      trk.pos--;
      if (trk.pos < 0 || trk.pos > MAX_PTRN_TRACK) trk.pos = 0;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      needLcdUpdate = TRUE;
    }
    //incremente track position
    if (fwdBtn.justPressed){
      trk.pos++;
      if (trk.pos > MAX_PTRN_TRACK) trk.pos = MAX_PTRN_TRACK;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      needLcdUpdate = TRUE;
    }
    //go to first measure
    if (clearBtn.justPressed){
      trk.pos = 0;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      needLcdUpdate = TRUE;
    }
    if(shiftBtn.pressed){
      //go to last measure
      if (numBtn.pressed){
        trk.pos = track[trkBuffer].length;
        nextPattern = track[trkBuffer].patternNbr[trk.pos];
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        needLcdUpdate = TRUE;
      }
      //delete current pattern in the current position
   // if(backBtn.justPressed) {
      if (backBtn.justPressed && track[trkBuffer].length){                                                  // [zabox] fixes crashs when deleting the last track pos
        if (trk.pos < (track[trkBuffer].length - 1)) {                                                      // [zabox] delete only valid track pos
        for (int a = trk.pos + 1; a < track[trkBuffer].length; a++){         
          track[trkBuffer].patternNbr[a] = track[trkBuffer].patternNbr[a + 1]; 
        }
        trk.pos +=1;//to stay in the same position
        track[trkBuffer].length = track[trkBuffer].length - 1;//decremente length by 1 du to deleted pattern
        nextPattern = track[trkBuffer].patternNbr[trk.pos];
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        trackNeedSaved = TRUE;
        needLcdUpdate = TRUE;
        }
        else {
          trk.pos +=1;//to stay in the same position
        }
      }
      //insert a pattern
      if (fwdBtn.justPressed) {                                                                              
        if (trk.pos < (track[trkBuffer].length + 1)) {                                                     // [zabox] insert only inside track
        for (int a = track[trkBuffer].length + 1; a >= trk.pos; a--){                                             
          track[trkBuffer].patternNbr[a] = track[trkBuffer].patternNbr[a - 1]; 
        }
        trk.pos -=1;//to stay in the same position
        track[trkBuffer].patternNbr[trk.pos] = curPattern;
        track[trkBuffer].length = track[trkBuffer].length + 1;//decremente length by 1 du to deleted pattern
        nextPattern = track[trkBuffer].patternNbr[trk.pos];
        if(curPattern != nextPattern) selectedPatternChanged = TRUE;
        trackNeedSaved = TRUE;
        needLcdUpdate = TRUE;
        }
        else {
          trk.pos -=1;//to stay in the same position
        }
      }
    }//end shift

    //write selected pattern in the current track position
    if (enterBtn.justRelease && !trackJustSaved){
      track[trkBuffer].patternNbr[trk.pos] = curPattern;
      trk.pos++;
      if (trk.pos > MAX_PTRN_TRACK) trk.pos = MAX_PTRN_TRACK;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      track[trkBuffer].length = trk.pos;
      trackNeedSaved = TRUE;
      needLcdUpdate = TRUE;
    }
  }//END IF MODE TRACK WRITE

    //////////////////////////MODE TRACK PLAY/////////////////////////////////
  if (curSeqMode == TRACK_PLAY)
  {
    if (trackPosNeedIncremante){//(endMeasure)
      trk.pos++;
      if (trk.pos >= track[trkBuffer].length) trk.pos = 0;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      trackPosNeedIncremante = FALSE;
      needLcdUpdate = TRUE;

    }
    //go to first measure
    if (clearBtn.justPressed){
      trk.pos = 0;
      nextPattern = track[trkBuffer].patternNbr[trk.pos];
      if(curPattern != nextPattern) selectedPatternChanged = TRUE;
      needLcdUpdate = TRUE;
    }
    if(readButtonState){
      trk.next = FirstBitOn();
      selectedTrackChanged = TRUE;
      needLcdUpdate = TRUE;
    }
  }

  //=======================================================================================
  //------------------------------------Update pattern track load/save-----------------------
  if (selectedTrackChanged)
  {
    selectedTrackChanged = FALSE;
    needLcdUpdate = TRUE;
    trackNeedSaved = FALSE;
    LoadTrack(trk.next);
    trk.current = trk.next;
    nextPattern = track[trkBuffer].patternNbr[trk.pos];
    if(curPattern != nextPattern) selectedPatternChanged = TRUE;
    //trkBuffer = !trkBuffer;
  }
  if (trackNeedSaved && enterBtn.hold)
  {
    trackNeedSaved = FALSE;
    SaveTrack(trk.current);
    LcdPrintSaved();
    trackJustSaved = TRUE;
    timeSinceSaved = millis();
  }
  //this function is to not incremente trk.pos when released enterBtn after Saved track
  if (millis() - timeSinceSaved > HOLD_TIME){
    trackJustSaved = FALSE;
  }

  if (selectedPatternChanged)
  {
    //Serial.println("changed!!");
    selectedPatternChanged = FALSE;
    needLcdUpdate = TRUE;//selected pattern changed so we need to update display
    patternNeedSaved = FALSE;
    LoadPattern(nextPattern);
    curPattern = nextPattern;
    nextPatternReady = TRUE;
  }

  if(nextPatternReady){///In pattern play mode this peace of code execute in the PPQ Count function
    //Serial.println("Ready!!");
    //if ((isRunning && endMeasure) || !isRunning ){//|| (curSeqMode != PTRN_PLAY))
    // Serial.println("endMeasure!!");
    nextPatternReady = FALSE;
    keybOct = DEFAULT_OCT;
    noteIndex = 0;
    InitMidiNoteOff();
    ptrnBuffer = !ptrnBuffer;
    //Serial.println("switched!!");
    InitPattern();//SHOULD BE REMOVED WHEN EEPROM WILL BE INITIALIZED
    //SetHHPattern();
    InstToStepWord();
  }

  if (patternWasEdited)
  {//update Pattern
    patternWasEdited = FALSE;
    //SetHHPattern();
    InstToStepWord();
    patternNeedSaved = TRUE;
    // Serial.println("patternupdated");
  }

  if (patternNeedSaved && enterBtn.justPressed && !instBtn)
  {
    patternNeedSaved = FALSE;
    SavePattern(curPattern);//pattern saved
    LcdPrintSaved();
  }

  if (enterBtn.justRelease) needLcdUpdate = TRUE;

  //////////////////////////MODE MUTE//////////////////////////////////////

  if (curSeqMode == MUTE){
    MuteButtonGet();
    if (encBtn.pressed){
      muteInst = 0;
      InitMuteBtnCounter();
    }
  }  
}










































































