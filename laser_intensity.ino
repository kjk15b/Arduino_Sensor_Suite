/*
 * Kolby Kiesling
 * Abilene Buoy Systems
 * 11-11-2019
 * This program handles sample analog reading with an Arduino Uno
 */


// Global Variables
int laser = 13;
float pl = 0;
float pa = 0;
float laser_int = 0;
int R = 0;
// Parameters for the thermistor
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float tmp=0;
int i = 1;
float w0 = 0.002; // initial beam waist



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(laser, OUTPUT);
  pinMode(A0, INPUT); // laser photocell
  pinMode(A1, INPUT); // ambient photocell
  pinMode(A2, INPUT); // thermistor
}

float convert_voltage (float in) {
  float v1;
  v1 = in * (5.0 / 1023.0);
  //Serial.print("Input voltage:\t");
  //Serial.println(v1);
  return v1; 
}

float temp_calc (float an) {
  // We want to bring in our data, determine R2 from R1 in our voltage divider
  // Then we want to pass it through a quick formula
  float R2, R1 = 100000.0, LogR2, T, Tc;
  R2 = R1 * (1023.0 / (float)an - 1.0); // find our resistance
  LogR2 = log(R2); // take the natural logarithm
  T = (1.0 / (c1 + c2*LogR2 + c3*LogR2*LogR2*LogR2));
  Tc = T - 273.15;
  Tc += 73.1289; // scaling issues with the thermistor for our equations...
  //Serial.print("Temperature:\t");
  //Serial.println(Tc);
  //Serial.println(" [C]");
  return Tc;
}

float laser_intensity (float Vl, float Va) {
  // From this function we want to find the intensity of our laser hitting the photocell
  // we also want to get rid of any ambient signals too.  
  // Laser reading followed by ambient reading
  float R1 = 10000, R2, Vdiff, intensity; // resistor values
  //Serial.print("Vdiff:\t");
  //Serial.println(Vl-Va);
  Vdiff = Vl - Va; // subtract ambient-ness
  R2 = abs(R1 * (Vdiff / 5.0 - 1));
  //Serial.print("R2:\t");
  //Serial.println(R2);
  intensity = (2 * Vdiff * Vdiff) / (3.14 * R2 * w0 * w0); // our derivation using power laws
  //Serial.print("Intensity:\t");
  //Serial.println(intensity);
  return intensity;
  
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(laser, HIGH);
  delay(5000); // wait 5 seconds
  // Main sensor collections here
  laser_int = laser_intensity(convert_voltage(analogRead(A0)), convert_voltage(analogRead(A1))); // determine our laser intensity
  tmp = temp_calc((float)analogRead(A2));
  // Transmission of data to follow here...
  Serial.print("Temperature: ");
  Serial.println(tmp);
  delay(300);
  Serial.print("Intensity: ");
  Serial.println(laser_int);
  digitalWrite(laser, LOW);
  delay(5000);
  i++;
}
