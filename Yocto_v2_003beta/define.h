//-------------------------------------------------
//                  Yocto v2.x
//                    define
//-------------------------------------------------

#ifndef define_h
#define define_h

//DEBUG
#define DEBUG 0

//MIDI
#define NOTE_ON_CMD 0x90
#define NOTE_OFF_CMD 0x80
#define CLOCK_CMD 0xF8
#define START_CMD 0xFA
#define STOP_CMD 0xFC
#define ALL_NOTE_OFF 0x7B
#define CONTINU_CMD 0xFB
#define MIDI_BAUD 31250

//Button
#define BTN_PLAY     B10000000
#define BTN_STOP     B1
#define BTN_LASTSTEP B1000000
#define BTN_SCALE    B10
#define BTN_SHUF     B100000
#define BTN_CLEAR    B100
#define BTN_INST     B10000
#define BTN_SHIFT    B1000
#define BTN_TRK  B1000
#define BTN_BACK     B100
#define BTN_FWD      B10
#define BTN_NUM      B1
#define BTN_PTRN B10000000
#define BTN_TAP      B1000000
#define BTN_DIR      B100000
#define BTN_GUIDE    B10000
#define BTN_BANK     B1
#define BTN_MUTE     B10
#define BTN_TEMPO    B100
#define BTN_ENTER    B1000
#define BTN_ENCODER  B100
#define ENC_SW_GET   PINB & BTN_ENCODER
#define ENC_PINA    0
#define ENC_PINB    1
#define JUSTPRESSED 1
#define JUSTRELEASE 0
#define HOLD_TIME 1000 //1s to trig hold 
#define NBR_BTN_STEP 16

//Led
#define LED_STOP  B1
/*#define LED_SHIFT B10
 #define LED_INST  B100
 #define LED_CLR   B1000
 #define LED_SHUF  B10000
 #define LED_LAST  B100000
 #define LED_SCALE B1000000*/
#define LED_PLAY  B10000000
#define LED_MASK  0xDD55 //mask to fade selected inst led
#define LED_MASK_OH  0xFD55 

//LCD
#define MAX_CUR_POS 4
#define MAX_CONF_PAGE 2

//Utility
#define TOGGLE 0
#define MOMENTARY 1
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0
#define BOOTLOADER_TIME 5000 // time staying in bootloader 

//Sequencer
#define NBR_INST 16
#define NBR_STEP_BTN 16
#define NBR_STEP 16
#define NBR_PATTERN 16 //Number of pattern in a bank
#define NBR_BANK 8
#define MID_VEL 40 //velocity 0 to 127
#define HIGH_VEL 80
#define MAX_VEL 127 
#define MAX_SEQ_DIR 3
#define FORWARD 0
#define BACKWARD 1
#define PING_PONG 2
#define RANDOM 3
#define MAX_BANK 7 //bank A to H
#define MAX_PTRN 128
#define MAX_TRACK 16
#define MAX_PTRN_TRACK 1024
#define MIN_BPM 30
#define MAX_BPM 250
#define DEFAULT_BPM 120
#define PPQN 96
#define SCALE_16 PPQN/4
#define SCALE_32 PPQN/8
#define SCALE_16t PPQN/6
#define SCALE_8t PPQN/3
#define MAX_SHUF_TYPE 7
#define DEFAULT_SHUF 1
#define DEFAULT_LEN 16
#define DEFAULT_SCALE 24
#define MASTER 0
#define SLAVE 1
#define MAX_BLOCK_LEN 16
#define POSITION 0
#define LENGTH 1
#define MAX_TOTAL_ACC 13
#define MIN_TOTAL_ACC 0
#define SYNC 1
#define FREE 0

//Ext inst
#define MAX_OCT 8
#define DEFAULT_OCT 3 //corresponding to +0
#define MAX_EXT_INST_NOTE 99

//trig out  and din synchro
#define TR1_HIGH PORTA |= 1 << 2
#define TR1_LOW  PORTA &= ~(1 << 2)
#define TR2_HIGH PORTA |= 1 << 3
#define TR2_LOW  PORTA &= ~(1 << 3)
//#define TRIG1_PIN 26
//#define TRIG2_PIN 27
#define DIN_START_PIN 24
#define DIN_CLK_PIN 25
#define TRIG_TIME_MS 10

//Dincsync out
#define DIN_START_HIGH PORTA |= 1
#define DIN_START_LOW  PORTA &= ~(1)
#define DIN_CLK_HIGH   PORTA |= 1 << 1
#define DIN_CLK_LOW    PORTA &= ~(1 << 1)

//Dio
#define SW_CS_LOW PORTD&=~(1<<4)
#define SW_CS_HIGH PORTD|=(1<<4) 
#define LED_CS_LOW   PORTB&=~(1<<4)
#define LED_CS_HIGH  PORTB|=(1<<4)
#define TRIG_CS_LOW   PORTB&=~(1<<3)
#define TRIG_CS_HIGH  PORTB|=(1<<3)
#define DAC_CS_HIGH  PORTD|=(1<<5)
#define DAC_CS_LOW  PORTD &=~(1<<5)

#define DEBOUNCE_TIME 5

//Mux
#define MUX_ADDR_PORT PORTA
#define NBR_MUX_OUT 5
#define NBR_MUX 2
#define MUX_INH_PORT PORTD 
#define MUX_INH1_BIT 6
#define MUX_INH2_BIT 7
#define MUX_INH1_HIGH PORTD|=(1<<6)
#define MUX_INH1_LOW PORTD &=~(1<<6)
#define MUX_INH2_HIGH PORTD|=(1<<7)
#define MUX_INH2_LOW PORTD &=~(1<<7)
#define MUX_ADDR_0 PORTA &=~(1<<7)&~(1<<6)&~(1<<5)
#define MUX_ADDR_1 PORTA |=(1<<7)&~(1<<6)&~(1<<5)

//Inst button
#define BD_BTN 0
#define SD_BTN 1
#define LT_BTN 2
#define MT_BTN 3
#define HT_BTN 4
#define RM_BTN 5
#define HC_BTN 6
#define CB_BTN 7
#define CY_BTN 8
#define OH_BTN 9
#define CH_BTN 10
#define PC2_BTN 11

//Inst number  Match with IC113 and IC112 bit shift register out (cf schematic)
#define BD 0
#define SD 1
#define LT 2
#define MT 3
#define HT 4
#define RM 5
#define HC 6
#define CB 7
#define CY 8
#define OH 9
#define CH 10
#define PC2 11

#define TOTAL_ACC 12 //
#define EXT_INST 13 //unsused shift register out but used in the pattern
#define TR1 14 //
#define TR2 15


enum SeqMode
{
  TRACK_PLAY, TRACK_WRITE, PTRN_PLAY, PTRN_STEP, PTRN_TAP, MUTE
};

//////////////////////////////variable////////////////////////////////
//Dio---------------------------------------------
byte dinSr[5]={
  0};//Store Din value of the five shift register
byte tempDin[5][2];//Used for the bounce
unsigned long debounceTimer;

//Buttons-----------------------------------------
typedef struct Button Button;
struct Button
{
  boolean curState;//1 is ON, 0 is OFF
  boolean prevState;
  boolean pressed;
  boolean justPressed;
  boolean justRelease;
  boolean hold;// 1 is holding
  byte counter;//count number of push on the button
  unsigned long curTime;
};
Button stepBtn[NBR_STEP_BTN];
Button muteStepBtn[NBR_STEP_BTN];
Button playBtn;
Button stopBtn;
Button encBtn;
Button guideBtn;
Button scaleBtn;
Button tapBtn;
Button dirBtn;
Button tempoBtn;
Button shufBtn;
Button shiftBtn, enterBtn, numBtn, lastStepBtn, backBtn, fwdBtn, muteBtn, bankBtn, stepsBtn, ptrnBtn, trkBtn, clearBtn;

boolean instBtn;
boolean doublePush = 0; //flag that CH and CH_LOW button are pressed together
byte instOut[NBR_INST]=  { 
  BD, SD, LT, MT, HT,RM, HC, CB, CY, OH, CH, PC2};
unsigned int readButtonState;

//Led-----------------------------------------------
unsigned int stepLeds;
unsigned int stepLedsHigh;
unsigned int stepLedsLow;
byte menuLed;
unsigned int configLed;
volatile boolean blinkTempo = 1;
volatile boolean blinkFast[NBR_INST] = {
  1};//each instrument could blink in different scale
volatile boolean blinkVeryFast;
boolean stopLed, trackLed, backLed, fwdLed, numLed, ptrnLed, tapLed, dirLed, guideLed, bankLed, muteLed, tempoLed, instLed, shiftLed, clearLed, shufLed, scaleLed, lastStepLed;
/*boolean eighttLed;//tempo scale Leds
 boolean sixteentLed;
 boolean sixteenLed;
 boolean threetwoLed;*/
byte scaleLeds;
boolean enterLed;
unsigned int instSlctLed;//[NBR_INST]={0x00, 0x00, 0x300, 0x400, 0x800, 
byte flagLedIntensity;

//Sequencer-----------------------------------------
struct SeqConfig {
  boolean ptrnChangeSync;// 0 = free pattern change  1 = sync end measure pattern change
  boolean sync;//0 = MASTER  1 = SLAVE
  boolean syncChanged;
  byte TXchannel;//MIDI transmit channel
  byte RXchannel;
  unsigned int bpm;
  unsigned int defaultBpm;// stored in the eeprom
  byte dir;
  byte configPage;
  boolean configMode;
  boolean setupNeedSaved;
}
seq;

volatile byte ppqn = 0;
volatile byte globalCurStep = 0;//current step of the pattern, egal the length of the longer instrument 16 max
volatile byte curStep[NBR_INST] = {
  0};//current played step of each instrument
volatile int globalStepCount = -1;
volatile byte metronomeStepCount = -1;
volatile int stepCount[NBR_INST] = {
  -1};//counter of each instrument
volatile byte tapStepCount;//this counter is used to get a better tap response
volatile boolean stepChanged[NBR_INST] = {
  FALSE};
volatile unsigned int stepValue = 0;//value of each trig step used in "Clock"
volatile byte noteIndexCpt = 0;
boolean isRunning = FALSE;
boolean isStop = TRUE;
SeqMode curSeqMode = PTRN_STEP;
SeqMode prevSeqMode;
byte curInst = BD;//0 is BD trig out shift register
byte curBank = 0;//0 to 7 banks
byte curPattern = 0;// 0 to 255
byte nextPattern;
//byte seqDirMode;
boolean changeDir[NBR_INST]; //use to PING PONG change dir
volatile boolean endMeasure;
unsigned int metronome;//value of metronome trig
int shuffle[MAX_SHUF_TYPE][2]={
  {
    0                    }
  ,{
    0,-1                  }
  ,{
    0,-2                  }
  ,{
    0,-3                  }
  ,{
    0,-4                  }
  ,{
    0,-5                  }
  ,{
    0,-6                  }
};
volatile boolean shufPolarity[NBR_INST];

//Pattern-------------------------------------------
typedef struct Pattern Pattern;
struct Pattern
{
  byte globalLength;//gmlobal length of the pattern
  byte globalScale;
  byte globalDir;//0=>forward, 1=>backword, 2=>ping-pong, 3=>random
  byte globalShuffle;
  byte flam;
  unsigned int inst[NBR_INST];
  unsigned int step[NBR_STEP];
  byte velocity[NBR_INST][NBR_STEP];
  byte probality[NBR_INST][NBR_STEP];
  byte uTiming[NBR_INST][NBR_STEP];
  byte retrig[NBR_INST][NBR_STEP];
  byte extNote[128];// DUE to EEPROM 64 bytes PAGE WRITE
  byte extLength;
  byte groupPos;
  byte groupLength;
  byte totalAcc;//Total accent value
  byte dir[NBR_INST];
  byte length[NBR_INST];
  byte scale[NBR_INST];
  byte shuffle[NBR_INST];
  //byte bank;
};
Pattern pattern[2];//current pattern and next pattern in the buffer
Pattern bufferedPattern;//to copy paste pattern
boolean ptrnBuffer = 0;
boolean patternWasEdited = FALSE;
boolean selectedPatternChanged = FALSE;
boolean nextPatternReady = FALSE;
boolean patternNeedSaved = FALSE;
byte longerInstLength;
//Scale value correspond to the scale button counter
byte scaleValue[]={
  SCALE_16, SCALE_32, SCALE_8t, SCALE_16t};

struct GroupPattern
{
  byte length;//length of the pattern bloc
  byte firstPattern;//first pattern of the bloc
  byte pos;
  boolean priority;//use to know if volatile group have priority on stored pattern group
}
group;

unsigned int tempInst[NBR_INST]={
  0};
unsigned int muteInst = 0;
byte muteOut[NBR_STEP_BTN]=  { 
  BD, SD, LT, MT, HT,RM, HC, CB, CY, OH, CH, PC2};
//Velocity table match with original TR909
//BD, SD, LT, MT, HT, RM, HC, CB, CY, OH, CH, PC2
byte instVelHigh[NBR_INST]={
  80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80};
byte instVelLow[NBR_INST]={
  40, 40, 40, 40, 40, 40, 40, 40, 60, 60, 60, 20, 40, 40, 40, 40};

//Track----------------------------------------------
typedef struct Track Track;
struct Track
{
  unsigned int length;//lenght before loop 0 to 999
  byte patternNbr[MAX_PTRN_TRACK];//Should be 999 
};
Track track[2];

typedef struct TrackSetup TrackSetup;
struct TrackSetup
{
  byte next;
  byte current;//number of the track 0 to 15
  unsigned int pos;//track position 0 to 999
  volatile byte indexCpt;//incrementer each pattern end
  boolean wasEdited;
};
TrackSetup trk;
boolean selectedTrackChanged = FALSE;
boolean trkBuffer = 0;
boolean trackNeedSaved = FALSE;
boolean trackJustSaved = FALSE;
volatile boolean trackPosNeedIncremante = FALSE;
unsigned long timeSinceSaved;

//Ext inst-------------------------------------------
boolean keyboardMode;
byte keybOct = DEFAULT_OCT;
byte noteIndex = 0;//external inst note index

//SPI------------------------------------------------
SPISettings SPIset(4000000, MSBFIRST, SPI_MODE0);

//Encoder--------------------------------------------
#define ENCSTATE 3
byte encoder_A;
byte encoder_B;
byte encoder_A_prev=0;
boolean encSwValue[2];
boolean encSwState;
byte oldStateEnc = 3;
byte encPosition = 0;
const int8_t knobDir[] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
  -1,  0,  0,  1,
  0, 1, -1, 0 };

//Mux------------------------------------------------
byte muxAddr[8]={
  B111, B11, B101, B1, B110, B10, B100, B0};// Due to the  A, B, C pin on PORTA sequence
byte muxInst[12]={
  BD, SD, LT, MT, HT, RM, HC, CB, CY, OH, CH, PC2};//To match with DAC mux OUT

//LCD------------------------------------------------
boolean needLcdUpdate = TRUE;
byte curIndex;//position of the cursor
byte curPage = 0;
byte cursorPos[MAX_CUR_POS]={
  0, 4, 8, 12};
const char *letterUpTrackWrite[MAX_CUR_POS]={
  "P", "P", "L", "N"};
const char *letterUpExtInst[MAX_CUR_POS]={
  "I", "N", "L", "O"};
const char *letterUpConfPage1[MAX_CUR_POS]={
  "S", "B", "M", "M"};
const char *letterUpConfPage2[MAX_CUR_POS]={
  "P", "N", "N", "N"};

//MIDI-----------------------------------------------
volatile boolean midiNoteOnActive = FALSE;
boolean midiStart;
boolean midiStop;
boolean midiContinue;
boolean instWasMidiTrigged[NBR_INST] ={
  FALSE};
byte midiVelocity[NBR_INST]={
  100};

//Din synchro----------------------------------------
boolean dinStartState = LOW;
boolean dinClkState = LOW;

//trigState----------------------------------------
boolean trig1State = LOW;
boolean trig2State = LOW;
unsigned long trigTime = 0;
boolean trigTimeOk;

//Bootloader-----------------------------------------
unsigned long bootElapseTime = 0;
byte btnPlayStopByte = 0;
byte btnEnterByte = 0;

//Test Mode varibale
boolean userInterfaceTestMode = LOW;
boolean voicesTestMode = LOW;

#endif//end if define_h


