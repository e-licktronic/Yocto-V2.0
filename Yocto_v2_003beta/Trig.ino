//-------------------------------------------------
//                  Yocto v2.x
//                 Trig.c
//-------------------------------------------------

////////////////////////Function//////////////////////

void SetTrigTime()
{
   if (trig1State && !trigTimeOk){
    trigTime = millis();
    trigTimeOk = HIGH;
  }
  if (millis() - trigTime >= TRIG_TIME_MS){
    TR1_HIGH;//Reset trig1 out
    trig1State = trigTimeOk = LOW;
  }
}
