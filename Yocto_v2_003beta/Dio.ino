//-------------------------------------------------
//                  Yocto v2.x
//                  Dio utility
//-------------------------------------------------

/////////////////////Function//////////////////////
//Initialise IO PORT and libraries
void InitIO()
{
  DDRB = DDRB | B11000;
  DDRD = DDRD | B11110000;
  DDRA = DDRA | B11101100;
  TR1_HIGH;
  pinMode(DIN_START_PIN, OUTPUT);
  pinMode(DIN_CLK_PIN, OUTPUT);
  pinMode (ENC_PINA, INPUT);
  pinMode (ENC_PINB, INPUT);
  SPI.begin();
  SPI.usingInterrupt(255);
  Wire.begin();
}

//Pre init IO for Test MODE
// Init Test mode pins as Input PA3 (27) adn PA4 (28)
void InitIOTestMode()
{ 
 pinMode(27, INPUT);           // set pin to input
digitalWrite(27, HIGH);       // turn on pullup resistors
pinMode(28, INPUT);           // set pin to input
digitalWrite(28, HIGH);       // turn on pullup resistors
}

//Scan DIN shift register with debounce time-------------------------------
void ScanDin()
{
  if ((millis() - debounceTimer) > DEBOUNCE_TIME)
  {
    debounceTimer = millis();
    //shiftregisted switches
    SPI.beginTransaction(SPIset); 
    SW_CS_HIGH;
    tempDin[0][0] = SPI.transfer(0);
    tempDin[1][0] = SPI.transfer(0);
    tempDin[2][0] = SPI.transfer(0);
    tempDin[3][0] = SPI.transfer(0);
    tempDin[4][0] = SPI.transfer(0);
    SW_CS_LOW;
    SPI.endTransaction();

    //µC direct pin switches
    encSwValue[0] = ENC_SW_GET;

    //Compare
    for (int a = 0; a < 5; a++){
      if (tempDin[a][0] == tempDin[a][1]) dinSr[a] = tempDin[a][0];
    }
    if (encSwValue[0] == encSwValue[1]) encSwState = encSwValue[0];
  }

  //shiftregisted switches
  SPI.beginTransaction(SPIset);
  SW_CS_HIGH;
  tempDin[0][1] = SPI.transfer(0);
  tempDin[1][1] = SPI.transfer(0);
  tempDin[2][1] = SPI.transfer(0);
  tempDin[3][1] = SPI.transfer(0);
  tempDin[4][1] = SPI.transfer(0);
  SW_CS_LOW; 
  SPI.endTransaction();

  //µC direct pin switches
  encSwValue[1] = ENC_SW_GET;
}

//Set Dout LED------------------------------------------------------------
void SetDoutLed(unsigned int stepLed, unsigned int configLed, byte menuLed)
{
  SPI.beginTransaction(SPIset);
  LED_CS_LOW;
  SPI.transfer(byte(stepLed >> 8));
  SPI.transfer(byte(stepLed));
  SPI.transfer(byte(configLed >>8));
  SPI.transfer(byte(configLed));
  SPI.transfer(byte(menuLed));
  LED_CS_HIGH;
  SPI.endTransaction(); 
}

//Set Dout Trig------------------------------------------------------------
void SetDoutTrig(unsigned int value)
{
  //No analog voices doesn't have to be trigged by the shift register
  //It prevent "TICK" sound when muting
  value &= ~((1 << TOTAL_ACC) | (1 << EXT_INST) | (1 << TR1) | (1 << TR2));
  SPI.beginTransaction(SPIset);
  TRIG_CS_LOW;
  SPI.transfer(byte(value >> 8));
  SPI.transfer(byte(value));
  TRIG_CS_HIGH;
  SPI.endTransaction(); 
}

//Reset Dout Trig---------------------------------------------------------
void ResetDoutTrig()
{
  SPI.beginTransaction(SPIset);
  TRIG_CS_LOW;
  SPI.transfer(0);
  SPI.transfer(0);
  TRIG_CS_HIGH;
  SPI.endTransaction();
}

//Set Dac voltage---------------------------------------------------------
void SetDacA(byte velocity)
{
  if (velocity >= 127) velocity = 127;
  unsigned int milliVolt = map(velocity, 0, 127, 0, 4090);
  unsigned int command = 0;
  command |= 0x1000;// Active DAC out A
  command |= (milliVolt & 0x0FFF);
  SPI.beginTransaction(SPIset);
  DAC_CS_LOW;
  delayMicroseconds(5);
  SPI.transfer(command >> 8);
  SPI.transfer(command & 0xFF);
  DAC_CS_HIGH;
  SPI.endTransaction();
}

//Scan Din bootloader
void ScanDinBoot()
{
  SPI.beginTransaction(SPIset); 
  SW_CS_HIGH;
  SPI.transfer(0);
  SPI.transfer(0);
  btnPlayStopByte = SPI.transfer(0);
  SPI.transfer(0);
  btnEnterByte = SPI.transfer(0);
  SW_CS_LOW;
  SPI.endTransaction();
}






