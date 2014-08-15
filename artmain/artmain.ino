//Headery stuff
#include <Adafruit_NeoPixel.h>
#include <DMXSerial.h>
#define DMX_USE_PORT1

//Assorted definitions
#define USED_CHANNELS 16
#define LEDS 148
#define OFFSET 0
#define S1_PIN 51
#define LAYERS 4

//Channel Definitions
#define BG_RED                           1
#define BG_BLU                           2
#define BG_GRN                           3
#define PLAYER_SHIELDS_ON                4
#define RED_ALERT                        5
#define SHIP_DAMAGE_20                   6
#define SHIP_DAMAGE_40                   7
#define SHIP_DAMAGE_60                   8
#define TRACTORED_FOR_DOCKED             9
#define JUMP_INITIATED                   10
#define JUMP_EXECUTED                    11
#define FRONT_SHIELD_LOW                 12
#define REAR_SHIELD_LOW                  13
#define WITHIN_NEBULA                    14
#define HELM_IN_REVERSE                  15
#define SOMETHING_HITS_PLAYER            16
#define PLAYER_TAKES_INTERNAL_DAMAGE     17
#define PLAYER_TAKES_FRONT_SHIELD_DAMAGE 18
#define PLAYER_TAKES_REAR_SHIELD_DAMAGE  19
#define PLAYER_DESTROYED                 20
#define GAME_OVER                        21
#define ENERGY_LOW                       22
#define ENERGY_20                        23
#define ENERGY_40                        24
#define ENERGY_60                        25
#define ENERGY_80                        26
#define ENERGY_100                       27
#define ENERGY_200                       28
//#define LOADING_TUBE1                    29
//#define LOADING_TUBE2                    30
//#define LOADING_TUBE3                    31
//#define LOADING_TUBE4                    32
#define NUKE_READY_TO_FIRE               29
#define TORP_NUKE_FIRED                  30
#define TORP_FIRED                       31

//Create our arrays
int prevDMX[USED_CHANNELS];
int curDMX[USED_CHANNELS];
int DMX[USED_CHANNELS];
int emberIntensity[LEDS];
//layers[layer][led][1:r,2:g,3:b,4:a]=value
byte layers[LAYERS][LEDS][4];

//Define pi, because apparently it isn't predefined.
const float pi = 3.14;

//Globals for keeping track of things
int rStep = 0;
int tlDMX = 0;
int rightA,rightB,upA,upB,leftA,leftB,downA,downB;

//Define (?) our strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS+OFFSET, S1_PIN, NEO_GRB + NEO_KHZ800);

void calculateSides(int aspect,int ratio) { //i.e. calculateSides(16,9); or calculateSides(4,3);
  int basePerimeter = (aspect*2)+(ratio*2);
  int multiplier = LEDS/basePerimeter;
  int modulo = LEDS % basePerimeter;
  int adder = modulo/4;
  int modulooo = modulo % 4;
  rightA=OFFSET+(adder*0);
  rightB=rightA+(aspect*multiplier)+(adder*1);
  upA=rightB+1+(adder*1);
  upB=upA+(ratio*multiplier)+(adder*2);
  leftA=upB+1+(adder*2);
  leftB=leftA+(aspect*multiplier)+(adder*3);
  downA=leftB+1+(adder*3);
  downB=downA+(ratio*multiplier)+(adder*4);
  switch(modulooo){
    case 1:
      rightA+=0;
      rightB+=0;
         upA+=0;
         upB+=1;
       leftA+=1;
       leftB+=1;
       downA+=1;
       downB+=1;
    case 2:
      rightA+=0;
      rightB+=1;
         upA+=1;
         upB+=1;
       leftA+=1;
       leftB+=2;
       downA+=2;
       downB+=2;
    case 3:
      rightA+=0;
      rightB+=1;
         upA+=1;
         upB+=2;
       leftA+=2;
       leftB+=3;
       downA+=3;
       downB+=3;
  }
}
  
  

void setup() {
  //Initialize our communications
  DMXSerial.init(DMXReceiver);
  Serial.begin(9600);
  strip.begin();
  strip.show();
  //Initialize DMX buffers/arrays
  for(int i=0;i<USED_CHANNELS;i++){
    prevDMX[i]=0;
    curDMX[i]=0;
    DMX[i]=0;
  }
  calculateSides(16,9);
}

//Make our DMX input more usable
void processDMX() {
  for(int i=1;i<USED_CHANNELS;i++){
    //int DMXIN = DMXSerial.read(i);
    int DMXIN = DMX[i];
    //Removal of false zeros due to data loss
    if(DMXIN==0 && prevDMX[i]!=0){
      DMX[i]=prevDMX[i];
    }else{
      DMX[i]=DMXIN;
    }
    prevDMX[i]=curDMX[i];
    curDMX[i]=DMXIN;
  }
}

//Put the DMX into an array
void readDMX() {
  for(int i=1;i<USED_CHANNELS;i++){
    DMX[i]=DMXSerial.read(i);
    Serial.println(DMX[i]);
  }
}

//Return a rainbow color based on progress from 0 to 255
uint32_t rainbow(int progress){
  float pf = progress*(pi/128);
  float rm=sin(pf);
  float bm=sin(pf+((2*pi)/3));
  float gm=sin(pf+((4*pi)/3));
  int  rv=128+(rm*128);
  int  bv=128+(bm*128);
  int  gv=128+(gm*128);
  return strip.Color(rv,bv,gv);
}

void loop() {
  int cmils=millis();
  if(((cmils-tlDMX)>40)||(cmils<tlDMX)){
  //if((cmils % 40) == 0){
    tlDMX=cmils;
    while(DMXSerial.noDataSince()==0){}
    readDMX();
    processDMX();
  //  Serial.print("Ran, millis=");
  //  Serial.println(cmils);
  }
  
  //readDMX();
  
  //Debugging
  //Serial.print("Got value on DMX Channel 1:");
  //Serial.println(ch_one);
  //Serial.print("Got value on DMX Channel 2:");
  //Serial.println(ch_two);
  //Serial.print("Got value on DMX Channel 3:");
  //Serial.println(ch_three);
  
  uint32_t cCol = 0;
  
  if(DMX[4]>128){
    if(rStep>255){rStep=0;}
    cCol = rainbow(rStep);
    rStep++;
    delay(10);
  }else{
    cCol = strip.Color(DMX[1],DMX[2],DMX[3]);
  }
  
  //Set whole strip as RGB light
  for(int i=0;i<150;i++){
    //strip.setPixelColor(i,strip.Color(ch_one,ch_two,ch_three));
    //strip.setPixelColor(i,strip.Color(DMXSerial.read(1),DMXSerial.read(2),DMXSerial.read(3)));
    //strip.setPixelColor(i,strip.Color(DMX[1],DMX[2],DMX[3]));
    strip.setPixelColor(i,cCol);
  }
  strip.show();
  //DMX DELAY!
  //delay(40);
  Serial.println(millis());
  //Rainbow test delay
  //delay(10);
  //delay(5);
}
