/*
Designed to work with the light blue bean

NOTE: sensor reading is used relative to battery voltage

Functionality:
- State machine keeps track of the state of the bottle, see eSysStateEnum for
list of states. See switch statement in code for description of what happens 
during each state.

Assumptions:
- User drinks water every time the pick up the bottle


if the user has not picked up bottle in TIMER s, then turn on LED.

*/

#define NUM_READINGS 240
#define ZERO 41000U
#define ONE_MIN_MS 60000U
#define NOTIFICATION_PERIOD ONE_MIN_MS

#define PRESSURE_PIN A0

typedef enum eSysStateEnum{
  idle = 0,   // bottle is doing nothing, only in this state before system init
  on_table,   // bottle is sitting on a table
  picked_up,  // user has picked up the bottle
  empty,      // bottle is empty, seems like a useful state to know
} eSysState;

// Global variable to keep track of system state
eSysState system_state = idle;

// Current battery voltage, unit is 100*Volts
uint32_t battV = 0;

uint32_t currWaterWeight[NUM_READINGS];
uint32_t average = 0;
uint32_t lastWaterWeight;
uint16_t currOunces;
uint16_t lastOunces;
uint32_t drank_now = 0; // this flag will be set by iOS to 1 if the user just 
                        // drank water. Theo: Why don't we make this determination
                        // in firmware?
uint32_t last_drink_time;

boolean ledON = false;

//const int TIMER = 60; //in seconds. need to convert to ms

// Arduino initialization routine, executed once at boot
void setup()
{
  // Initiate serial communication between LBM313 and Arduino
  Serial.begin(57600);

  // Turn off LED
  Bean.setLed(0,0,0);

  // Initialize water weight readings
  lastWaterWeight = analogRead(PRESSURE_PIN);
  lastOunces = translateFlexiforceToOz(lastWaterWeight); 

  // Initialize last drink time to now
  last_drink_time = millis();

  // Initialize battery voltage reading
  battV = Bean.getBatteryVoltage();

  // Initialize system state, we assume that bottle starts on table
  system_state = idle;

  // Theo: why is this sleep necessary? Is it to give the user time to put the
  // bottle down after turning the system on?
  // Sleep for 5 seconds
  Bean.sleep(5000);
}

// Determines if the bottle is on a surface
bool OnSurface_Flexiforce(uint32_t sensor_reading)
{
  // TODO: (Theo) Should this be greater or less?
  return (sensor_reading > ZERO);
}


// Arduino main loop: runs repeatedly, forever
void loop()
{
  uint32_t timeDiff;
  uint32_t currReading;
  AccelerationReading cur_accel = {0, 0, 0};
  uint8_t cur_temp = 0;

  // ---------------- Update Sensor readings ----------------------------------
  // Get battery voltage reading, do this before pressure readings
  battV - Bean.getBatteryVoltage();

  // Get current pressure reading (from flexiforce)
  currReading = getFlexiforcePressureReading();

  // Get current accelerometer reading
  cur_accel = Bean.getAcceleration();

  // Get current temperature reading
  cur_temp = Bean.getTemperature();
  // ----------------- End sensor readings ------------------------------------

  // ---------------- Start state machine code --------------------------------
  switch(system_state){
    case idle:
      // Idle state means that the device was just turned on
      // TODO: Figure out when it is appropriate to leave the idel state.
      //   Perhaps we should make a determination about whether the bottle is
      //   on a table or not and if we think the bottle is empty. 
      // For now we start monitoring immediately
      system_state = on_table;
    break;

    case on_table:
      // If bottle is on a table, we're waiting for it to be picked up or 
      // for enough time to pass since they last drank water that we need
      // to notify the user that it is time to drink again.
      if(OnSurface_Flexiforce(currReading)){
        if(!ledON){ 
          timeDiff = millis() - last_drink_time;
          //Serial.println(timeDiff);

          if(timeDiff > NOTIFICATION_PERIOD){
            //turn on the light
            Bean.setLed(255,255,255);
            ledON = true;
          }
        }
      }
      else{
        // Bottle was picked up
        system_state = picked_up;
        //Serial.println("picked up.");
      }
    break;

    case picked_up:
      // If the user has picked the bottle up, we are now waiting for it to
      // be put back down
      if(OnSurface_Flexiforce(currReading)){
        // Change state to indicate bottle was put down
        system_state = on_table;
        //turn off top LED
        Bean.setLed(0,0,0);
        ledON = false;

        currOunces = translateFlexiforceToOz(currReading);
        //Serial.print("CURRENT ounces: ");
        //Serial.println(currOunces);
        //calculate the difference:
        //difference = lastOunces - currOunces;
        //Serial.print("DIFFERENCE :");
        //Serial.print(difference);
        //Serial.println(" ounces");

        // Send data to iOS via scratch bank 1
        // iOS app will process data to determine if user drank water
        Bean.setScratchNumber(1,(long)currOunces);
        Bean.sleep(1000);   // Delay to let the characteristic update?

        // Read data from iOS via scratch bank 2, indicates whether user drank water
        drank_now = Bean.readScratchNumber(2);

        if(drank_now >0){
          last_drink_time = millis();  //"reset" time benchmark
          //Serial.println("delayed");
        }

        lastOunces = currOunces; 


        Bean.sleep(10);
      }
    break;

    case empty:
      // If the bottle is empty, we should do something appropriate
    break;
  }
  // ---------------- End state machine code ----------------------------------

  drank_now = 0;   //reset the drank_now flag

  // Sleep until it is time for the next reading
  // TODO: (Theo) should we modify the sleep time based on the state of the
  // system? Probably a good idea to optimize power savings
  Bean.sleep(2500);
}


//translates to an oz value of remaining water in device
//assumes aluminum 16 oz bottle with 1M Ohm resistor
uint16_t translateFlexiforceToOz(uint32_t value)
{
  uint16_t a_value = value/100;
  Bean.sleep(20);
  uint16_t oz_value = 30; 

  // Theo: Seems like we could use an equation for this conversion
  if(a_value > 1000){
    oz_value = 20;
  }
  else if(a_value > 895){
    oz_value = 16;
  }
  else if(a_value > 850){
    oz_value = 12;
  }
  else if(a_value > 750){
    oz_value = 8;
  } 
  else if(a_value > 600){
    oz_value = 4;
  }
  else{
    if(a_value < 410){
      //Serial.println("WTF");
    }
    oz_value = 0;
  }

  return oz_value;
}



uint32_t getFlexiforcePressureReading()
{
  average = 0;

  // Theo: why take so many readings if we only care about the last 20?
  // maybe can just take 20 readings?
  // With 240 readings separated by 8ms, this routine will take 1920ms = 2 sec
  // This is a very long time to lock up the processor.
  for(int x=0; x<NUM_READINGS; x++) {
    currWaterWeight[x] = analogRead(PRESSURE_PIN);
    delay(8);
    //Serial.println(currWaterWeight[x]);
  }

  for(int y = (NUM_READINGS-20); y<NUM_READINGS; y++){
    average = average + currWaterWeight[y];
  }
  average = average/20;
  //Serial.println(average);


  uint32_t scaled = average*battV;
  //Serial.print(">>>>>>>> Scale:");
  //Serial.println(scaled);
  return scaled;
}


