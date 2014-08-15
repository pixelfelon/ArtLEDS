//Headery stuff
#include <Adafruit_NeoPixel.h>
#include <DMXSerial.h>
#define DMX_USE_PORT1

#define USED_CHANNELS 16
#define LEDS 150
#define S1_PIN 51

//Create our arrays
int prevDMX[USED_CHANNELS];
int curDMX[USED_CHANNELS];
int DMX[USED_CHANNELS];
int emberIntensity[LEDS];

//Define pi, because apparently it isn't predefined.
const float pi = 3.14;

//Globals for keeping track of things
int rStep = 0;
int tlDMX = 0;

//Define (?) our strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, S1_PIN, NEO_GRB + NEO_KHZ800);

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
