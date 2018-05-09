/*  This Arduino program reads acceleration data, calculates speed,
 *   and displays it on an LCD.
 *  Derek Snyder
 */

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <SFE_MMA8452Q.h>

/* 0x27 is the I2C bus address for an unmodified backpack
 * TODO what do the rest of these parameters mean?
 */
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); 
MMA8452Q accel;

void setup() 
{
  /* 16x2 display
   */
  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);

  /* SCALE_4G:  +- 4 gravity acceleration scale, 
   * ODR_800:   sensor updates at 800 hz/ 1.25 ms
   */
  accel.init(SCALE_4G, ODR_800); 

  Serial.begin(9600);
}

const int sensor_interval = 2; //ms
const int display_interval = 250; //ms
int last_display_update = 0;

float v;
float vx = 0, vy = 0, vz = 0;
float t1 = 0, t2;

void loop() 
{
  t2 = (float)millis();
  if (t2 - t1 >= 2) 
  {
    v = get_velocity((t2 - t1)/1000, vx, vy, vz);
    t1 = t2;
  }
  if (millis() - last_display_update >= 250)
  {
    Serial.println(v);
    last_display_update = millis();
  }
}

const float G = 9.81; // m/s^2
float get_velocity(float dt, float vx, float vy, float vz) 
{
  while (!accel.available()) ;
  accel.read();
  vx += G*accel.cx*dt;
  vy += G*accel.cy*dt;
  vz += G*(accel.cz - 1)*dt; //correct for constant acceleration due to gravity
  
  return (vx*vx + vy*vy + vz*vz);
}


