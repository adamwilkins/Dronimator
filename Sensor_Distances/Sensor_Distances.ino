/* 
 * modified 10/27/2015
 * by Stanley Wang
 *  
 * created 10/17/2015
 * by Stanley Wang
 * 
 * Team Drominator
 * 
 * Get sonar sensor distances
 * Using HC-SR04 sonar sensors
 */

/*
 * Each direction has its own trigger and echo pin
 * Pin sharing for trigger is allowed but not for echo
 * We are sharing trigger pins so echoes do not interfere
 * Each trigger has a 15 degree effectual angle
 */
const int NORTH_ECHO_PIN = 1;
const int NORTH_TRIGGER_PIN = 8;

const int EAST_ECHO_PIN = 2;
const int EAST_TRIGGER_PIN = 8;

const int WEST_ECHO_PIN = 3;
const int WEST_TRIGGER_PIN = 8;

const int NORTH_EAST_ECHO_PIN = 4;
const int NORTH_EAST_TRIGGER_PIN = 9;

const int NORTH_WEST_ECHO_PIN = 5;
const int NORTH_WEST_TRIGGER_PIN = 9;

const int UP_ECHO_PIN = 6;
const int UP_TRIGGER_PIN = 9;

const int DOWN_ECHO_PIN = 7;
const int DOWN_TRIGGER_PIN = 9;

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
      
    Serial.println(distance[i]);
  }
  
  Serial.println();
}

/*
 * sets up the pins for the sonar sensors
 */
void setupEcho() {
  pinMode(NORTH_ECHO_PIN, INPUT);
  pinMode(NORTH_EAST_ECHO_PIN, INPUT);
  pinMode(NORTH_WEST_ECHO_PIN, INPUT);
  pinMode(EAST_ECHO_PIN, INPUT);
  pinMode(WEST_ECHO_PIN, INPUT);
  pinMode(UP_ECHO_PIN, INPUT);
  pinMode(DOWN_ECHO_PIN, INPUT);
  
  pinMode(NORTH_TRIGGER_PIN, OUTPUT);
  pinMode(NORTH_EAST_TRIGGER_PIN, OUTPUT);
  pinMode(NORTH_WEST_TRIGGER_PIN, OUTPUT);
  pinMode(EAST_TRIGGER_PIN, OUTPUT);
  pinMode(WEST_TRIGGER_PIN, OUTPUT);
  pinMode(UP_TRIGGER_PIN, OUTPUT);
  pinMode(DOWN_TRIGGER_PIN, OUTPUT);

  //make sure trigger pins are off first
  digitalWrite(NORTH_TRIGGER_PIN, LOW);
  digitalWrite(NORTH_EAST_TRIGGER_PIN, LOW);
  digitalWrite(NORTH_WEST_TRIGGER_PIN, LOW);
  digitalWrite(EAST_TRIGGER_PIN, LOW);
  digitalWrite(WEST_TRIGGER_PIN, LOW);
  digitalWrite(UP_TRIGGER_PIN, LOW);
  digitalWrite(DOWN_TRIGGER_PIN, LOW);
  
  Serial.println("Sensor setup complete");
}

/*
 * Gets the distance of all 7 sonars
 * 0 = N, 1 = NE, 2 = NW, 3 = E, 4 = W, 5 = Up, 6 = Down
 * delay of 40 milliseconds to make sure there are no lingering triggers
 * Worst case senario delay of up to roughly half a second (280ms)
 * Worst case only happens if there is nothing
 */
void getDistance(long *distance) {
  //do north/east/west to reduce interfering echos
  distance[0] = getDistance(NORTH_TRIGGER_PIN, NORTH_ECHO_PIN);
  distance[3] = getDistance(EAST_TRIGGER_PIN, EAST_ECHO_PIN);
  distance[4] = getDistance(WEST_TRIGGER_PIN, WEST_ECHO_PIN);
  
  //up and down should not interfere with any other
  distance[5] = getDistance(UP_TRIGGER_PIN, UP_ECHO_PIN);
  distance[6] = getDistance(DOWN_TRIGGER_PIN, DOWN_ECHO_PIN);
  
  //do ne/nw next to reduce interfering echos
  distance[1] = getDistance(NORTH_EAST_TRIGGER_PIN, NORTH_EAST_ECHO_PIN);
  distance[2] = getDistance(NORTH_WEST_TRIGGER_PIN, NORTH_WEST_ECHO_PIN);
  
  Serial.println("All distances calculated");
}

/*
 * Gets a single distance back (in inches) from a selected trigger/echo pin
 */
long getDistance(int triggerPin, int echoPin) {
  //HC-SR04 needs 10 micro seconds of trigger time
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  //from the HC-SR04 specs, pulse width(μs)/148 = distance(inches)
  //40 milliseconds timeout, specs say 38ms or no signal
  return pulseIn(echoPin, HIGH, 40000) / 148;
}

