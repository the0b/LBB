/*
testing pin outs for LBB
*/

const int led1=5;
const int led2=4;

void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  Serial.begin(57600);
}

void loop()
{
   digitalWrite(led1, HIGH);
   digitalWrite(led2, HIGH);
   delay(1000);
    digitalWrite(led1, LOW);
   digitalWrite(led2, LOW);
   
   Bean.sleep(500);
}


