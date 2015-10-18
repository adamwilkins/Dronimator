/* 
 * created 10/17/2015
 * by Stanley Wang
 * 
 * Team Drominator
 */

/*
 * All triggers should be on the same pin
 * Each echo has it's own pin
*/
const int echoTrigger = 1;
const int echoNorth = 2;
const int echoNorthEast = 3;
const int echoNorthWest = 4;
const int echoEast = 5;
const int echoWest = 6;
const int echoUp = 7;
const int echoDown = 8;

/*
 * Distance for each echo
 * 0 = N, 1 = NE, 2 = NW, 3 = E, 4 = W, 5 = Up, 6 = Down
 */
long distance[7];
  
void setup() {
  Serial.begin(9600);
  setupEcho();
}

void loop() {
  getDistance(&distance[0]);

  for (int i = 0; i < 7; i++) {
    if (i == 0)
      Serial.print("North:     ");
    else if (i == 1)
      Serial.print("NorthEast: ");
    else if (i == 2)
      Serial.print("NorthWest: ");
    else if (i == 3)
      Serial.print("East:      ");
    else if (i == 4)
      Serial.print("West:      ");
    else if (i == 5)
      Serial.print("Up:        ");
    else if (i == 6)
      Serial.print("Down:      ");
      
    Serial.print(distance[i]);
    Serial.println();    
  }
  
  Serial.println();
}

//sets up the pins for the sonar sensors
void setupEcho() {
  pinMode(echoTrigger, OUTPUT);
  pinMode(echoNorth, INPUT);
  pinMode(echoNorthEast, INPUT);
  pinMode(echoNorthWest, INPUT);
  pinMode(echoEast, INPUT);
  pinMode(echoWest, INPUT);
  pinMode(echoUp, INPUT);
  pinMode(echoDown, INPUT);
  
  Serial.print("Sensor setup complete");
  Serial.println();
}

/*
 * Gets the distance of all 7 sonars
 * 0 = N, 1 = NE, 2 = NW, 3 = E, 4 = W, 5 = Up, 6 = Down
 * delay of 40 milliseconds to make sure there are no lingering triggers
 */
void getDistance(long *distance) {
    distance[0] = getDistance(echoNorth);
  delay(40);
    
    //do east/west to reduce interfering echos
    distance[3] = getDistance(echoEast);
  delay(40);
    distance[4] = getDistance(echoWest);
  delay(40);
    
    //do ne/nw next to reduce interfering echos
    distance[1] = getDistance(echoNorthEast);
  delay(40);
    distance[2] = getDistance(echoNorthWest);
  delay(40);

    //up and down should not interfere with each other
    distance[5] = getDistance(echoUp);
  delay(40);
    distance[6] = getDistance(echoDown);
  delay(40);
  
    Serial.print("All distances calculated");
    Serial.println();
}

/*
 * Gets a single distance back (in inches) from a selected echo pin
 */
long getDistance(int echoPin) {
  //HC-SR04 needs 10 micro seconds of trigger time
  digitalWrite(echoTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(echoTrigger, LOW);

  if (echoPin == echoNorth)
    Serial.print("Calculating North");
  else if (echoPin == echoNorthEast)
    Serial.print("Calculating NorthEast");
  else if (echoPin == echoNorthWest)
    Serial.print("Calculating NorthWest");
  else if (echoPin == echoEast)
    Serial.print("Calculating East");
  else if (echoPin == echoWest)
    Serial.print("Calculating West");
  else if (echoPin == echoUp)
    Serial.print("Calculating Up");
  else if (echoPin == echoDown)
    Serial.print("Calculating Down");
  else
    Serial.print("Calculating ERROR");

  Serial.println();
  //from the HC-SR04 specs, pulse width(μs)/148 = distance(inches)
  //40 milliseconds timeout, specs say 38ms or no signal
  return pulseIn(echoPin, HIGH, 40000) / 148;
}
