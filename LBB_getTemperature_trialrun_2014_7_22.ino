
// Notify the client over serial when a temperature reading changes
//ADDED
/* also pulse led1 high then low if the temp changes upwards
and pulse led2 high then low if the temp changes downwards
*/

static int8_t temp = 0;
  const int led1=5;
  const int led2 = 4;
  const int pressurePin = A0;

// the setup routine runs once when you press reset:
void setup() 
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  // initialize serial communication at 57600 bits per second:
  Serial.begin(57600); 
}

void loop()
{
 bool notify = false;
 
 int8_t newTemp = Bean.getTemperature();
 
 if ( newTemp != temp )
 {
   if(newTemp > temp)
   {
     digitalWrite(led1, HIGH);
     delay(500);
     digitalWrite(led1, LOW);
   } else if(newTemp < temp)
   {
     digitalWrite(led2, HIGH);
     delay(500);
     digitalWrite(led2, LOW);
   }
   temp = newTemp;
   
   Serial.print("Temperature: ");
   Serial.println(temp);
   
 }
  
 // Sleep for a second before reading the temperature again  
 Bean.sleep(1000);  
}
