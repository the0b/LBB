/*
Designed to work with the light blue bean
as well as circuit with op-amp

NOTE: sensor reading is used relative to battery voltage

Functionality:
1)If the user picks up the bottle, set pickup to true and reset time elapsed
2) if pickup is true, and pressure sensor value is non-zero, then read in the pressure value
3) translate value into ounces
4)communicate value to ios

if the user has not picked up bottle in TIMER s, then turn on LED.

*/

const int pressurePin = A0;

const int numReadings = 240;
uint32_t currWaterWeight[numReadings];
uint32_t average = 0;
uint32_t lastWaterWeight;
uint32_t currReading;
uint16_t currOunces;
uint16_t lastOunces;
long delayLight = 0; //this will be set by iOS to 1 if the user drank water
//uint32_t difference = 0;


unsigned long time;

boolean pickup=false;
boolean ledON = false;

const uint32_t ZERO = 41000;
//const int TIMER = 60; //in seconds. need to convert to ms
uint32_t TIME_MS = 60000;


void setup()
{
  Serial.begin(57600);
  Bean.setLed(0,0,0);
 
  lastWaterWeight = analogRead(pressurePin);
  lastOunces = translateToOz(lastWaterWeight); 
  
  time = millis();
  Bean.sleep(5000);
}

void loop()
{
  unsigned long timeDiff;
 
  currReading = getPressureReading();
  
   if (pickup)
  {
     if(currReading > ZERO)
    {
      //means user put the bottle down
      pickup = false;
      //turn off top LED
      Bean.setLed(0,0,0);
      ledON = false;
      
      currOunces = translateToOz(currReading);
      //Serial.print("CURRENT ounces: ");
      //Serial.println(currOunces);
      //calculate the difference:
      //difference = lastOunces - currOunces;
      //Serial.print("DIFFERENCE :");
      //Serial.print(difference);
      //Serial.println(" ounces");
      
      //communicate to ios
      Bean.setScratchNumber(1,(long)currOunces);
      
      Bean.sleep(1000);
      delayLight = Bean.readScratchNumber(2);
      
      if(delayLight >0)
      {
        time = millis();  //"reset" time benchmark
        //Serial.println("delayed");
      }
      
      lastOunces = currOunces; 
      
      
      Bean.sleep(10);

    }
    
    //if pickup is true, and curr weight approximates zero, then do nothing
   
}
 else if(!pickup)
  {
     if(currReading > ZERO)
    {
    
      if(!ledON){ 
       timeDiff = millis() - time;
       //Serial.println(timeDiff);

       if(timeDiff > TIME_MS)
       {
         
         //turn on the light
         Bean.setLed(255,255,255);
         ledON = true;
       }
      }
    
       
    }
    else
    {
      pickup = true;  //set pickup to true because weight approximates zero
      //Serial.println("picked up.");
    }
  }
 
  //delay(100);
  delayLight = 0;//reset the delayLight bool
  Bean.sleep(2500);
}


//translates to an oz value of remaining water in device
//assumes aluminum 16 oz bottle with 1M Ohm resistor
uint16_t translateToOz(uint32_t value)
{

  uint16_t a_value = value/100;
  Bean.sleep(20);
  uint16_t r_value = 30; 
  if(a_value > 1000)
   {
     r_value = 20;
     return r_value;
   }
   if(a_value > 895 && a_value <= 1000)
  {
   r_value = 16;
     return r_value;
  }
   if(a_value > 850 && a_value <= 895)
  {
   r_value = 12;
     return r_value;
  }
   if(a_value > 750 && a_value <= 850)
  {
   r_value = 8;
     return r_value;
  } 
  if(a_value > 600 && a_value <= 750)
  {
    r_value = 4;
     return r_value;
  }
  else if(a_value >410 && a_value <=600)
  {
    r_value = 0;
    return r_value;
  }
  else{
    //Serial.println("WTF");
    return r_value;
  }
}



uint32_t getPressureReading()
{
  average = 0;
  
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
  average = average/20;
    //Serial.println(average);

   uint32_t battV = 0;
   battV = Bean.getBatteryVoltage();
  
   uint32_t scaled = average*battV;
    //Serial.print(">>>>>>>> Scale:");
    //Serial.println(scaled);
    return scaled;
}


