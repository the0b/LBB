/*
Testing LBB bluetooth capabilities

set the scratch data and see if you can read it off the light blue app
*/

void setup()
{
  Serial.begin(57600);
}

void loop()
{
  uint16_t x = 800;
  
  uint8_t i[2];
  i[1] = x & 0xFF;
  i[0] = x >> 8;

  Bean.setScratchData(1, i, 2);
  
  Bean.sleep(1000);
}
  
