//TEST SKETCH

/*
This sketch decrements ounces every 30 seconds and sends over scratch
*/

int ounces = 24;
unsigned long time;


void setup()
{
   Serial.begin(57600); 
   time = millis();
   //Serial.println("START");
   //Serial.println(time);
}

void loop()
{
   unsigned long later = millis();
   unsigned long timeDiff = later - time;
   //Serial.println(timeDiff);
   
   if(ounces <= 0)
   {
     //reset to 24!
     ounces = 24; 
   }
   if(timeDiff > 1000)
   {
      Bean.setScratchNumber(1, (long)ounces);
      ounces = ounces - 4; 
      time = millis(); //reset the time
      //Serial.println(ounces);
   }
   
   Bean.sleep(1000);
  
}
