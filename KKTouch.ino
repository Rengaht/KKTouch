#include <MPR121.h>
#include <Wire.h>

#define numElectrodes 12
#define TouchThreshold 40
#define ReleaseThreshold 30

int numProx=1;
int proxPin[1]={0};

void setup()
{
  Serial.begin(19200);
  while(!Serial);  // only needed if you want serial feedback with the
         // Arduino Leonardo or Bare Touch Board

  Wire.begin();
  
  if(!MPR121.begin(0x5A)){ 
    Serial.println("error setting up MPR121");  
    switch(MPR121.getError()){
      case NO_ERROR:
        Serial.println("no error");
        break;  
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;      
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;      
    }
    while(1);
  }
    
  // pin 4 is the MPR121 interrupt on the Bare Touch Board
  MPR121.setInterruptPin(4);

  // this is the touch threshold - setting it low makes it more like a proximity trigger
  // default value is 40 for touch
  MPR121.setTouchThreshold(TouchThreshold);
  
  // this is the release threshold - must ALWAYS be smaller than the touch threshold
  // default value is 20 for touch
  MPR121.setReleaseThreshold(ReleaseThreshold);  

  // initial data update
  MPR121.updateTouchData();

  Serial.println("done set up!");
}

void loop(){

  MPR121.updateBaselineData();
  MPR121.updateFilteredData();

  for(int i=0;i<numProx;i++){
    int diff=MPR121.getBaselineData(proxPin[i])-MPR121.getFilteredData(proxPin[i]);
    if(diff>0) Serial.println("#P|"+String(proxPin[i])+"|"+String(diff)+"#");
  }
  if(MPR121.touchStatusChanged()){
    MPR121.updateTouchData();
    
    for(int i=0; i<numElectrodes; i++){     
      if(MPR121.isNewTouch(i)){
          Serial.println("#T|"+String(i)+"#");
      } else if(MPR121.isNewRelease(i)){
          Serial.println("#R|"+String(i)+"#");          
      }
    } 
  }
}
