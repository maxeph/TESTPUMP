const boolean DEBUG = true; // DEBUG mode

// Set up the global variables linked to the number of level sensors TO BE REWORKED
const byte nblevels = 3;
byte levels[nblevels];
byte empt=0,low,med,hi;

// START config Variables (pins, type of sensors, etc)
// pins
const int currentPin = A0; // which pin for ACS712
int currentPin = nblevels+2
//config
int mVperAmp = 185; // Which kind of ACS712 use 185 for 5A Module, 100 for 20A Module, 66 for 30A Module
const float peakPump = 0.20; // minimum current at which we are sure that the pump is running.
// END config Variables

unsigned long chronoPump, lTime=0;

void setup() {
  // Set up of levels id MIGHT need to be reworked
  levels[0] = 2;
  levels[1] = 3;
  levels[2] = 4;
  low = levels[0]; //2
  med = low + levels[1]; //5
  hi = med + levels[2]; //9

  // Init pin level sensors
  for (int i = 2; i < nblevels+2; i++) {
    pinMode(i, INPUT);
  }
  // Init Serial
  Serial.begin(9600);
}

void loop() {
  // Creating temporary variable required for loop
  unsigned long cTime;
  float cAmpere;
  byte cLevel;

  // Getting required measures
  cTime = millis();
  cAmpere = getAmpere();
  cLevel =  getLevel();

  if (cAmpere > peakPump) {
      chronoPump += (cTime - lTime);
  }

  // DEBUG
  if (DEBUG) {
    Serial.print("timer : ");
    Serial.println(cTime);
    Serial.print("Ampère :");
    Serial.println(cAmpere);
    Serial.print("cLevel : ");
    Serial.println(cLevel);
    Serial.print("L arduino fonctionne depuis ");
    Serial.println(milli()/1000);
    Serial.print("La pompe a fonctionné ");
    Serial.println(chronoPump/1000);
  }


// Preparing next loop
  lTime=cTime; // time of this loop is last time of next loop
  delay(2000);
}

byte getLevel() {
  // Check the levels and give back a value
  byte level = 0;
  for (int i = 2; i < nblevels+2; i++) {
    level += levels[i-2]*digitalRead(i);
  }
  return level;
}

float getAmpere()
{
  double AmpsRMS;

  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  uint32_t start_time = millis();
  while((millis()-start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(currentPin);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the maximum sensor value*/
      minValue = readValue;
    }
  }

  // Subtract min from max
  AmpsRMS = ((((maxValue - minValue) * 5.0)/1024.0)/2*0.707)*1000/mVperAmp;
  return AmpsRMS;
}
