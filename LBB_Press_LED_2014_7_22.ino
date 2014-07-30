/*
NOTE - does not work 2014-7-24

Functionality:
1) push the pressure sensor, and output "2" in over ble
1) after TIMER elapsed, turn on both LED's (to make a circle)
2) if user clicks the button, turn off LED 1 (to make a smile)
*/


const int pressurePin = A0;
const int topLED = 5;
const int bottomLED = 4;

const int numReadings = 240;
int currWaterWeight[numReadings];
int average = 0;
int lastWaterWeight;
int currOunces;
int lastOunces;
uint8_t difference[1] = {0};

unsigned long time;

boolean pickedUp = false; //state of being picked up
boolean smileOn = false;  //state of the smile
boolean indicatorHigh = false; //state of indicator

const int ZERO = 200;  //give some leeway for what "zero" means
const long TIMER = 20000; //time before user is notified to drink
const long FADE_SMILE = 2000;

void setup()
{
  // Init. and start BLE library.

  
  pinMode(topLED, OUTPUT);
  pinMode(bottomLED, OUTPUT);
  
  Serial.begin(9600);
  
  lastWaterWeight = analogRead(pressurePin);
  lastOunces = translateToOz(lastWaterWeight);

  time = millis(); //set start time
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

  
   if(smileOn)
   {
      timeDiff = millis()-time;
      if(timeDiff > FADE_SMILE)
     {
      digitalWrite(bottomLED, LOW);
      smileOn=false;
     }
     //else, do nothing 
   }
  
   if (pickedUp)
  {
     if(average > ZERO)
    {
      //means user put the bottle down
      pickedUp = false;
      currOunces = translateToOz(average);
      //Serial.println("OUNCES: ");
      //Serial.println(currOunces);
      //Serial.println(average);
      //Serial.println(" ");
      
      //calculate the difference:
      difference[0] = lastOunces - currOunces;
      //Serial.println("DIFFERENCE :");
      //Serial.print(difference);
      //Serial.println(" ounces");
      
      
      //communicate to ios
      switch (difference[0])
      {
        case 0:
          //ble_write('1');
          //ble_write('e');
          break;
        case 8:
          Bean.setScratchData(1, difference, 1);
          //ble_write('8');
          //ble_write('e');
          break;
         case 16:
           Bean.setScratchData(1, difference, 1);
           break;
         case 22:
           Bean.setScratchData(1, difference, 1);
           break;
         default:
          // Serial.println(difference);
           break;
      }
      
      
      time = millis();  //"reset" time benchmark
      lastOunces = currOunces; 
      
      //turn off top LED
      digitalWrite(topLED, LOW);
      delay(10);
      
      
      //turn on smile
      digitalWrite(bottomLED, HIGH);
     
      smileOn = true;
    }
    

  }
  
  else if(!pickedUp)
  {
     if(average > ZERO)
    {
       timeDiff = millis() - time;
       //Serial.print("time diff: ");
       //Serial.println(timeDiff);
       

      
       if(timeDiff > TIMER)
       {
         
         //turn on the top light
         digitalWrite(topLED, HIGH);
      
       //make the yellow light appear, softly
       /*
       if(!indicatorHigh)
       {
         indicatorHigh=true;
         for(int x = 0; x<=125; x=x+5)
         {
           analogWrite(topLED, x);
           delay(100);
         }
       }
       else
       {
         indicatorHigh=false;
         for(int x = 125; x > 0; x=x-5)
         {
           analogWrite(topLED, x);
           delay(100);
         }
       }
       */
       }
       
    }
    else
    {
      pickedUp = true;  //set pickedUp to true because weight approximates zero
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
   if(value >= 460)
   {
     return 0;
   }
   if(value >460 && value<700)
  {
   return 8;
  }
   if(value>=700 && value <800)
  {
   return 16;
  }
   else if(value>=800)
  {
   return 22;
  } 
}


