
typedef struct sensorStruct{
  int pin;
  int mappedVal;
  int minVal;
  int maxVal;
};

sensorStruct readSensor(sensorStruct s);
sensorStruct setSensor(sensorStruct s);

