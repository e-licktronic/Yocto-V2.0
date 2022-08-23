//-------------------------------------------------
//                  Yocto v2.x
//              Midi Function
//-------------------------------------------------

/////////////////////Function//////////////////////

//initialize midi real time variable
void  InitMidiRealTime()
{
  midiStart = LOW;
  midiStop = LOW;
  midiContinue = LOW;
}

//intialize note off when stop or patternChanged  or ext instrument loop back
void InitMidiNoteOff()
{
  if(midiNoteOnActive){
    midiNoteOnActive = FALSE;
    if (noteIndexCpt) MidiSendNoteOff(seq.TXchannel, pattern[ptrnBuffer].extNote[noteIndexCpt - 1]);
    else MidiSendNoteOff(seq.TXchannel, pattern[ptrnBuffer].extNote[pattern[ptrnBuffer].extLength]);
  }
}

//Send note OFF
void MidiSendNoteOff (byte channel, byte value)
{
  MIDI.sendNoteOff(value + 12, 0, channel);
  /* MidiSend(channel + 0x80);
   MidiSend(value);
   MidiSend(0x00);*/
}
//Send note ON
void MidiSendNoteOn (byte channel, byte value, byte velocity)
{
  MIDI.sendNoteOn(value + 12, velocity, channel);
  /* MidiSend(channel + 0x90);
   MidiSend(value);
   MidiSend(velocity);*/
}

//Handle clock
void HandleClock()
{
  DIN_CLK_HIGH;
  CountPPQN();//execute 4x because internal sequencer run as 96 ppqn
  CountPPQN();
  delayMicroseconds(2000);
  CountPPQN();
  CountPPQN();
  DIN_CLK_LOW;

}

//handle start
void HandleStart()
{
  midiStart = HIGH;
}

//handle stop
void HandleStop()
{
  midiStop = HIGH;
}

//handle Continue
void HandleContinue()
{
  midiContinue = HIGH;
}

//Disconned midi real time callback
void DisconnectMidiHandleRealTime()
{
  MIDI.disconnectCallbackFromType(Clock);
  MIDI.disconnectCallbackFromType(Start);
  MIDI.disconnectCallbackFromType(Stop);
  MIDI.disconnectCallbackFromType(Continue);
}

//Handle noteON
void HandleNoteOn(byte channel, byte pitch, byte velocity)
{
  //Midi note On with 0 velocity as Midi note Off
  if (velocity == 0){
    if (channel == seq.RXchannel){
      switch (pitch){
      case 35:
      case 36:
        MidiTrigOff(BD);
        break;
      case 38:
      case 40:
        MidiTrigOff(SD);
        break;
      case 41:
        MidiTrigOff(LT);
        break;
      case 45:
      case 47:
      case 48:
        MidiTrigOff(MT);
        break;
      case 50:
        MidiTrigOff(HT);
        break;
      case 34:
        MidiTrigOff(RM);
        break;
      case 39:
        MidiTrigOff(HC);
        break;
      case 42:
        MidiTrigOff(CH);
        break;
      case 46:
        MidiTrigOff(OH);
        break;
      case 51:
        MidiTrigOff(CY);
        break;
      case 49:
        MidiTrigOff(CB);
        break;
      case 55:
      case 54:
      case 53:
        MidiTrigOff(PC2);
        break;
      case 60:
        TR1_HIGH;
        break;
      }
    }
  }
  else{
    if (channel == seq.RXchannel){
      switch (pitch){
      case 35:
      case 36:
        MidiTrigOn(BD, velocity);
        break;
      case 38:
      case 40:
        MidiTrigOn(SD, velocity);
        break;
      case 41:
        MidiTrigOn(LT, velocity);
        break;
      case 45:
      case 47:
      case 48:
        MidiTrigOn(MT, velocity);
        break;
      case 50:
        MidiTrigOn(HT, velocity);
        break;
      case 34:
        MidiTrigOn(RM, velocity);
        break;
      case 39:
        MidiTrigOn(HC, velocity);
        break;
      case 42:
        MidiTrigOn(CH, velocity);
        break;
      case 46:
        MidiTrigOn(OH, velocity);
        break;
      case 51:
        MidiTrigOn(CY, velocity);
        break;
      case 49:
        MidiTrigOn(CB, velocity);
        break;
      case 55:
      case 54:
      case 53:
        MidiTrigOn(PC2, velocity);
        //Serial.println(velocity);
        break;
      case 60:
        TR1_LOW;
        break;
      }
    }
  }
}

//Handle noteOFF
void HandleNoteOff(byte channel, byte pitch, byte velocity)
{
  if (channel == seq.RXchannel){
    switch (pitch){
    case 35:
    case 36:
      MidiTrigOff(BD);
      break;
    case 38:
    case 40:
      MidiTrigOff(SD);
      break;
    case 41:
      MidiTrigOff(LT);
      break;
    case 45:
    case 47:
    case 48:
      MidiTrigOff(MT);
      break;
    case 50:
      MidiTrigOff(HT);
      break;
    case 34:
      MidiTrigOff(RM);
      break;
    case 39:
      MidiTrigOff(HC);
      break;
    case 42:
      MidiTrigOff(CH);
      break;
    case 46:
      MidiTrigOff(OH);
      break;
    case 51:
      MidiTrigOff(CY);
      break;
    case 49:
      MidiTrigOff(CB);
      break;
    case 55:
    case 54:
    case 53:
      MidiTrigOff(PC2);
      break;
    case 60:
      TR1_HIGH;
      break;
    }
  }   
}


//MidiTrigOn insturment
void MidiTrigOn(byte inst, byte velocity)
{
  static byte hhSelect;//OH selected = 0, CH selected = 1
  byte trigInst;//instrument trigged

  if (instWasMidiTrigged[inst] == FALSE){
    SetMuxTrigMidi(inst, velocity);
    SetDoutTrig(1 << inst );
    delayMicroseconds(1000);
    SetDoutTrig(0);
    instWasMidiTrigged[inst] = TRUE;
  }
}

//MidiTrigOff insturment
void MidiTrigOff(byte inst)
{
  instWasMidiTrigged[inst] = FALSE;
}

//Midi send all note off
void SendAllNoteOff()
{
  MIDI.sendControlChange(ALL_NOTE_OFF , 0, seq.TXchannel);	
}












