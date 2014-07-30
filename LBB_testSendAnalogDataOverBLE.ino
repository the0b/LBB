/*
testing pressure sensor to LBB
*/

const int pressurePin = A0;
uint16_t value; //stores pressure value
uint8_t blue_data[2]; //breaks ints into bytes to transfer to LBB

void setup()
{
  Serial.begin(57600);
}

void loop()
{
  value = analogRead(pressurePin);
  
  if(value > 0)
  {  
    Serial.println(value);
    blue_data[0] = value >>8;
    blue_data[1]=value;
    Bean.setScratchData(1, blue_data, 2);
  }
  
  Bean.sleep(1000);
  delay(1000);
}
