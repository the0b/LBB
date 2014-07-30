/*
read in sensor output to serial [numReadings] number of times at [x] delay in between
to be used with mac built in functionality to write serial input to mac to file

this uses blend micro board
*/




const int pressurePin = A0;

const int numReadings = 300;
int weight[numReadings];
const int x = 5; //milliseconds to pause between readings
const int sleep = 1000; //milliseconds to pause between numReadings cycles

void setup()
{
  Serial.begin(9600);
  
}

void loop()
{
  //do 300 readings, x ms apart
  for(int y=0; y<numReadings; y++)
  {
    weight[y]=analogRead(pressurePin);
    delay(x);
  }
  
  //now print to serial
  for(int i=0; i<numReadings; i++)
  {
    Serial.println(weight[i]);
  }
  delay(sleep);
}
