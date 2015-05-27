
typedef struct blinkStruct{
  int pin;
  unsigned long lastHit;
  int onTime;
  int state;
};

blinkStruct startBlink(blinkStruct led);
blinkStruct checkBlink(blinkStruct led);

