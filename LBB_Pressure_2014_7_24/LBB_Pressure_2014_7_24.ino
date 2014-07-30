/*
Designed to work with the light blue bean
as well as circuit with op-amp

Functionality:
1)If the user picks up the bottle, set pickup to true and reset time elapsed
2) if pickup is true, and pressure sensor value is non-zero, then read in the pressure value
3) translate value into ounces
4) calculate the difference in ounces - if the difference is positive, then transfer positive value to ios

if the user has not picked up bottle in TIMER s, then turn on LED.

*/

const int pressurePin = A0;

const int numReadings = 240;
int currWaterWeight[numReadings];
int average = 0;
int lastWaterWeight;
int currOunces;
int lastOunces;
uint16_t difference = 0;

uint8_t data[2]; //data to be sent over BLE

unsigned long time;

boolean pickup=false;
boolean ledON = false;

const int ZERO = 700;
const long TIMER = 30; //in seconds. need to convert to ms
long TIME_MS = timeInMs(TIMER);

void setup()
{
  Serial.begin(57600);
 
  lastWaterWeight = analogRead(pressurePin);
  lastOunces = translateToOz(lastWaterWeight); 
  
  time = millis();
  delay(500);
}

void loop()
{
  unsigned long timeDiff;
 
  for(int x=0; x<numReadings; x++)
  {
  currWaterWeight[x] = analogRead(pressurePin);
  delay(8);
  //Serial.println(currWaterWeight[x]);
  }
  for(int y = (numReadings-20); y<numReadings; y++)
  {
    average = average + currWaterWeight[y];
  }
  Serial.println(" ");
  Serial.print("Ave: ");

  average = average/20;
    Serial.println(average);


  
   if (pickup)
  {
     if(average < ZERO)
    {
      //means user put the bottle down
      pickup = false;
      currOunces = translateToOz(average);
      //Serial.println("OUNCES: ");
      //Serial.println(currOunces);
      //Serial.println(average);
      //Serial.println(" ");
      
      //calculate the difference:
      difference = lastOunces - currOunces;
      Serial.println("DIFFERENCE :");
      Serial.print(difference);
      Serial.println(" ounces");
      
      //communicate to ios
      data[1]=difference;
      data[0]=difference >> 8;
      Bean.setScratchData(1,data, 2);

      time = millis();  //"reset" time benchmark
      lastOunces = currOunces; 
      
      //turn off top LED
      Bean.setLed(0,0,0);
      delay(10);

    }
    
    //if pickup is true, and curr weight approximates zero, then do nothing
   
}
 else if(!pickup)
  {
     if(average < ZERO)
    {
       timeDiff = millis() - time;

       if(timeDiff > TIME_MS)
       {
         
         //turn on the light
         Bean.setLed(255,255,255);
      
       }
       
    }
    else
    {
      pickup = true;  //set pickup to true because weight approximates zero
      Serial.println("picked up.");
    }
  }
 
  delay(100);
  average = 0;
}


//translates to an oz value of remaining water in device
//assumes aluminum 16 oz bottle with 1M Ohm resistor
int translateToOz(int value)
{
   if(value <= 175)
   {
     return 20;
   }
   if(value > 175 && value <= 325)
  {
   return 16;
  }
   if(value > 325 && value <= 450)
  {
   return 12;
  }
   if(value > 450 && value <= 580)
  {
   return 6;
  } 
  else if(value > 580 && value < 700)
  {
    return 0;
  }
}


//translate Timer to ms
long timeInMs(int value)
{
  return value * 1000;
}


