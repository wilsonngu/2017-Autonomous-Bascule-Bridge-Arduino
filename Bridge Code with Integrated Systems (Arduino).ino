
#include <NewPing.h>

//Bridge Boat Checker
bool boat = true; //false = no boats true = boats
int PING_PIN = 11; // First ULTRA
int PING_PINtwo = 10; //Second ULTRA
int PING_PINthree = 7; // Middle ULTRA
#define LEDPinR A3// RED traffic for boat
int insideMaxrange = 40; //Max range for boats underneat the brigde
int insideMinrange = 5; //Min range for boats underneat the brigde
int maximumRange = 50; // Maximum range needed
int minimumRange = 1; // Minimum range needed
int checker = 0; //Checker Value 
long distance;  //Undeclared ultrasonic sensor values
long distance2;
long distance3;
NewPing sonar(PING_PIN, PING_PIN );  //Declaring all three ultrasonic sensors for the arduino
NewPing sonar2(PING_PINtwo,PING_PINtwo);
NewPing sonar3(PING_PINthree,PING_PINthree);
int const marineRed = 8;   // Waterway actuators 
int const marineYellow = 9;

//Pedestrian detection 
#define leftside A4  //-1 push button
#define rightside A5 //+1 push botton
bool nodetection = true; //bool variable used for a loop check
int leftsideval = 0;  //undeclared values for the button sensors read
int rightsideval = 0;
int counter = 0;  //counter value to de-activate or activate the loop check

//Motor Function Setup
int motorR = 3; // Motor driver Right -->
int motorL = 4; // Motor drier Left <--

//Bridge State LED
int const bopen = A0; // Sensor Input to indicate if the bridge is fully open 
int const bclose = A1; // Sensor Input to indicate if the bridge is fully closed
int const red = 12; // Digital Led to indicate if the bridge is fully open or in the process of closing/open
int const green = 13; // Digital Led to indicate if the bridge is fully closed
int fullyC = 0; // Analogread Value for the closing sensor
int fullyO = 0; // Analogread Value for the opening sensor


//Setup
//two digital outputs for the led indication of the bridge
//4 analog inputs to sense the bridge state
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  pinMode(red,OUTPUT); // Setup for the open bridge state indicator 
  pinMode(green,OUTPUT); // Setup for the close bridge state indicator 
  pinMode(LEDPinR, OUTPUT); // Indicator if they're no boats in the water way
  pinMode(marineRed,OUTPUT); //Indicator for marine waterway to make the boats wait till the bridge is open
  pinMode(marineYellow,OUTPUT); //Indicator for marine waterway to tell the boats it is okay to proceed through the bridge
  
}


void loop() {
  
  fullyC = analogRead(bclose); //Begin reading the sensor to indicate if the bridge is fully close
  fullyO = analogRead(bopen);  //Begin reading the sensor to indicate if the bridge is fully open


  //Initialize the first reading of what bridge state it is in (open/closed/midway)

  //If the bridge is fully closed, turn on the green led and notify the serial monitor
  //Begin checking for boats along with tramway/vehicle/pedestrian detection
  if ( fullyC == 0 && fullyO > 400 ){
  //Turn on the marine traffic sensor
  digitalWrite(marineRed,HIGH);
  digitalWrite(marineYellow,LOW);
  digitalWrite(red,LOW);
  blinkGreen(500);
  Serial.println("it is fully closed~");
  //Begin boat checker loop with bridge detection
  bool boatcheckerC = false;
  
  while (boatcheckerC == false) {
    //Initialize counter system for bridge detection
    leftsideval = analogRead(leftside);
    rightsideval = analogRead(rightside);
    if (( leftsideval > 500) && (rightsideval < 10)){
       counter ++;
    } else if (( rightsideval > 500) && (leftsideval < 10)){
       counter --;
    }
    Serial.print("Counter is:");
    Serial.println(counter);
    blinkGreen(500);
    //Initialize the ultrasonics for reading the boats within a fixed vicinity
    unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
    unsigned int uS2 = sonar2.ping(); // Send ping, get ping time in microseconds (uS).
    unsigned int uS3 = sonar3.ping(); // Send ping, get ping time in microseconds (uS).
    distance = (uS / US_ROUNDTRIP_CM);
    distance2 = (uS2 / US_ROUNDTRIP_CM);
    distance3 = (uS3 / US_ROUNDTRIP_CM);
    Serial.print("Ping1: ");
    Serial.print(distance); // convert time into distance
    Serial.println("cm"); 
    Serial.print("Ping2: ");
    Serial.print(distance2); // convert time into distance
    Serial.println("cm"); 
    Serial.print("Ping3: ");
    Serial.print(distance3); // convert time into distance
    Serial.println("cm");
    //If the distance is met withing the waterway vicinity, the checker will increase by one untill it meets the
    //max checker requirement indicating boats
    if ((distance >= maximumRange || distance <= minimumRange) && (distance2 >= maximumRange || distance2 <= minimumRange)/*&& (distance3 >= insideMaxrange || distance3 <= insideMinrange)*/) {
      checker++;
      Serial.println("checker is");
      Serial.println(checker);
    //If the checker value is greater than 0 turn off the LED for detection of boats
      if (checker > 0) {
         digitalWrite(LEDPinR, LOW);
      }
    //If it has reached the max check counter restart the value at zero 
      if (checker == 3){
        Serial.println("start over");
        checker = 0;
      boat = true;
      }

    }
     //If the distance is met within the waterway vicinity, the checker will 'decrease' by one until it meets the
    //max checker requirement indicating no boats
    else {
      //subtract a value from the checker and turn off the led if it is less than 0
      checker--;
      if (checker < 0) {
      digitalWrite(LEDPinR, HIGH);
    }
    Serial.println("checker is");
    Serial.println(checker);
    if (checker == -3){
    //start checker again
    boat = false;
    Serial.println("Start over");
    checker = 0;
    }
    }
    if (boat == false ) {
      boatcheckerC = true;
    } else {
      boatcheckerC = false;
    }
    //Delay 1/2 sec before next reading.
    delay(500);
    Serial.print("boat value is:");
    Serial.println(boat);
    }

    //There was a boat detected and now this while loop will begin to wait untill the counter
    //position has reached '0' from a -1 or +1 for bridge detection

    bool detectionloop = false;
    while (detectionloop == false){
       //reinitialize counter sensors to read
       leftsideval = analogRead(leftside);
       rightsideval = analogRead(rightside);
       if (( leftsideval > 500) && (rightsideval < 10)){
       counter ++;
       } else if (( rightsideval > 500) && (leftsideval < 10)){
       counter --;
       }

       if (counter != 0){
       detectionloop = false;
       Serial.println("detected ");
       } else {
       Serial.println("nothing on the bridge");
       detectionloop = true;
       }
       Serial.print("counter is");
       Serial.println(counter);
       delay(500);

    }
    
    
    //If the bridge is fully closed and all tests are passed then begin to move the motor
    if ( fullyC == 0 && fullyO > 400 ) {

      digitalWrite(motorR,HIGH);
      digitalWrite(motorL,LOW);

      //Make a while loop untill the opening bridge circuit is complete (analogreadings), this will
      //continuously check untill the circuit is complete will moving the motor untill it is ready to stop (circuit completes)
      bool bridgest = true;
      while(bridgest == true) {
        //bridge is midway of opening or closing
        digitalWrite(green,LOW);
        blinkRed(500);
        Serial.print("fully open is:");
        Serial.println(fullyO);
        Serial.print("fully close is:");
        Serial.println(fullyC);
         fullyC = analogRead(bclose);
         fullyO = analogRead(bopen);
         //Bridge is fully open, stop the motor
        if ( fullyC > 500 && fullyO == 0){
           digitalWrite(green,LOW);
           blinkRed(500);
           Serial.println("It is fully open~");
           bridgest = false;
        }
       
      }
      digitalWrite(motorR,LOW);
      digitalWrite(motorL,LOW);
    }

    
  
  //if the bridge is fully open, turn on the red led / green led off and notify the serial monitor 
 } else if ( fullyC > 400 && fullyO == 0){
  //Indicate the bridge is passable and activate marin traffic actuators
  digitalWrite(marineRed,LOW);
  digitalWrite(marineYellow,HIGH);
  digitalWrite(green,LOW);
  blinkRed(500);
  Serial.println("It is fully open~");

  bool boatcheckerO = false;
  //Begin a boat checker loop to detect if the boat has passed through
  //All three ultrasonic sensors will read values and if there is no value within the vicinity it will assume there is no boat
  // and will move the bridge back to its closed position
  while (boatcheckerO == false) {
    //run the blinking function for the actuator on the bridge to indicate it is not passable 
    blinkRed(500);
    //initialize ultrasonic sensors to read values for the boat detection
    unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
    unsigned int uS2 = sonar2.ping(); // Send ping, get ping time in microseconds (uS).
    unsigned int uS3 = sonar3.ping();
    distance = (uS / US_ROUNDTRIP_CM);
    distance2 = (uS2 / US_ROUNDTRIP_CM);
    distance3 = (uS3 / US_ROUNDTRIP_CM);
    Serial.print("Ping1: ");
    Serial.print(distance); // convert time into distance
    Serial.println("cm"); 
    Serial.print("Ping2: ");
    Serial.print(distance2); // convert time into distance
    Serial.println("cm"); 
    Serial.print("Ping3: ");
    Serial.print(distance3); // convert time into distance
    Serial.println("cm"); 

    //If the distance is met within the waterway vicinity, the checker will increase by one until it meets the
    //max checker requirement indicating boats
    if ((distance >= maximumRange || distance <= minimumRange) && (distance2 >= maximumRange || distance2 <= minimumRange)/*&& (distance3 >= insideMaxrange || distance3 <= insideMinrange)*/) {
      checker++;
      Serial.println("checker is");
      Serial.println(checker);
    //If the checker value is greater than 0 turn on the LED for detection of boats
      if (checker > 0) {
         digitalWrite(LEDPinR, LOW);
      }
      
    //If it has reached the max check counter restart the value at zero 
      if (checker == 3){
        Serial.println("start over");
        checker = 0;
      boat = true;
      }

    }
     //If the distance is met within the waterway vicinity, the checker will 'decrease' by one until it meets the
    //max checker requirement indicating no boats
    else {
      //subtract a value from the checker and turn off the led if it is less than 0
      checker--;
      if (checker < 0) {
      digitalWrite(LEDPinR, HIGH);
    }
    Serial.println("checker is");
    Serial.println(checker);
    if (checker == -3){
    //start checker again
    boat = false;
  
    Serial.println("Start over");
    checker = 0;
    }
    }

    if (boat == true ) {
      boatcheckerO = true;
    } else {
      boatcheckerO = false;
    }
    //Delay 1/2 sec before next reading.
    delay(500);
    Serial.print("boat value is:");
    Serial.println(boat);
    }
    
     //If the bridge is fully open and there is no boat, begin the motor movement
    if ( fullyC > 400 && fullyO ==0){
    // switch the actuators on the marin traffic to indicate boats cannot pass underneath the brigde
    digitalWrite(marineRed,HIGH);
    digitalWrite(marineYellow,LOW);
    digitalWrite(green,LOW);
    blinkRed(500);
    Serial.println("It is fully open~ and about to close");
    digitalWrite(motorL,HIGH);
    digitalWrite(motorR,LOW);
    bool bridgest = false;
    //check the bridge untill its fully closed with a while loop
    while (bridgest ==false){
      fullyC = analogRead(bclose);
      fullyO = analogRead(bopen);
      //Bridge is fully closed, stop the motor
      if( fullyC == 0 && fullyO > 400 ){
      blinkGreen(500);
      digitalWrite(red,LOW);
      Serial.println("it is fully closed~");
      bridgest = true;
    } else  if (fullyC > 400 && fullyO > 400 ){
      digitalWrite(green,LOW);
      blinkRed(500);
      Serial.println("it's in the process of opening or closing");
    }
    
    } 
      digitalWrite(motorL,LOW);
      digitalWrite(motorR,LOW);
    }



  //if the bridge is mid way in the process of opening or closing, turn on the red led and the green led off
 } else if (fullyC > 300 && fullyO > 300 ) {
  digitalWrite(marineRed,HIGH);
  digitalWrite(marineYellow,LOW);
  digitalWrite(green,LOW);
  blinkRed(500);
  Serial.println("it's in the process of opening or closing");
  
 }
  delay(1000); 
}

void blinkGreen (int interval){
  static long prevMill = 0;
  if ((millis() - prevMill) >= interval){ 
   prevMill = millis(); 
   digitalWrite(green, !digitalRead(green));
 } 
}

void blinkRed (int interval){
   static long prevMill = 0;
   if (((long)millis() - prevMill) >= interval){ 
    prevMill = millis(); 
    digitalWrite(red, !digitalRead(red));
  }
 }






