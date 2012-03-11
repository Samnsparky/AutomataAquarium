/**
 * Name: digital_aquarium_sketch.ino
 * Desc: Logic for the DigitalAquarium
 * Auth: Jessica Ebert, Sam Pottinger, DJ Sutton
**/

#define NUM_CONT_ROT_SERVOS 1
#define NUM_LIM_ROT_SERVOS 1
#define NUM_PIEZO_SENSORS 1
#define NUM_LIGHT_SENSORS 1
#define NUM_LED 2
#define NUM_JELLYFISH 1
#define NUM_FISH 1
#define NUM_PIEZO_SENSOR_GROUPS 1
#define NUM_AQUARIUMS 1

#define MIN_LIGHT_VAL 100
#define PIEZO_MIN_TAP_VAL 50

#define NONE -1

#define JELLYFISH_RAISED_ANGLE 0
#define JELLYFISH_LOWERED_ANGLE 180

#define FISH_SUB_STEPS_TO_GOAL 10

#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 3

#define WIGGLE_AMPLITUDE 10
#define WIGGLE_SPEED 3.14159 // rad / sec

#define LONG_TIME_STEP 100
#define SHORT_TIME_STEP 10

#include <Arduino.h>
#include <math.h>
#include <stdarg.h>

// Abstraction for continuous rotation servos

typedef struct
{
  int controlLine;
  int potLine;
  int zeroValue;
  long position;
  long targetPosition;
  int targetVel;
} ContinuousRotationServo;

// Continuous rotation servo behavior

/**
 * Name: crs_init(int id, byte controlLine, byte potLine)
 * Desc: Calibrates and enumerates a cont. rotation servo
 * Para: id, The unique id of the servo to operate on
 *       controlLine, Which line to use for PWM to control the device
 *       potLine, The line where the potentiometer is installed
 *       calibrate, If true, servo is calibrated and position reset. If false, loaded from EEPROM
**/
void crs_init(int id, byte controlLine, byte potLine, boolean calibrate);

/**
 * Name: crs_startMovingTo(int id, long targetPosition)
 * Desc: Moves the given servo to the given position
 * Para: id, The unique numerical id of the servo to operate on
 *       targetPosition, The position the servo should go to
**/
void crs_startMovingTo(int id, long targetPosition);

/**
 * Name: crs_startMovingToDegrees(int id, double angle)
 * Desc: Has this servo start moving to a given angle
 * Para: id, The unique numerical id of the servo to operate on
 *       angle, The angle to move to in degrees (zero due right)
**/
void crs_startMovingToDegrees(int id, double angle);

/**
 * Name: crs_startMovingToAngle(int id, double angle)
 * Desc: Has this servo start moving to a given angle
 * Para: id, The unique numerical id of the servo to operate on
 *       angle, The angle to move to in radians (zero due right)
**/
void crs_startMovingToAngle(int id, double angle);

/**
 * Name: crs_step(int id, long ms)
 * Desc: Update this servo's state and check to see if its goal was reached
 * Para: id, The id of the servo to operate on
 *       ms, The time (milliseconds) since this was last called
**/
void crs_step(int id, long ms);

/**
 * Name: crs_setVelocity(int id, int targetVelocity)
 * Desc: Sets the velocity this servo should use to reach its goal point
 * Para: id, The unique numerical id of the servo to operate on
 *       targetVelocity, The velocity this servo should use
**/
void crs_setVelocity(int id, int targetVelocity);

/**
 * Name: crs_onGoalReached(int id)
 * Desc: The function called when a continuous rotation servo hits its
 *       goal positoin
 * Para: id, The unique id of the servo this should operate on
**/
void crs_onGoalReached(int id);

/**
 * Name: crs_getPos(int id)
 * Desc: Get the current position of this servo
 * Para: id, The unique id of the servo to get the position for
 * Retr: Current estimated position of this servo
**/
long crs_getPos(int id);

/**
 * Name: crs_convertToVelocityRaw(float vel)
 * Desc: Converts the given velocity to a raw value that can be sent
 *       to the servo
 * Para: vel, The velocity to convert
 * Note: Should be treated as private member of ContinuousRotationServo
**/
int crs_convertVelocityToRaw_(float vel);

/**
 * Name: crs_loadCalConstants_(int id)
 * Desc: Load position and calibration information from EEPROM
 * Para: id, The id of the servo to operate on
 * Note: Should be treated as private member of ContinuousRotationServo
**/
void crs_loadCalConstants_(int id);

/**
 * Name: crs_calibrate_(int id)
 * Desc: Generate calibration constants and zero position
 * Para: id, The id of the servo to operate on
 * Note: Should be treated as private member of ContinuousRotationServo
**/
void crs_calibrate_(int id);

/**
 * Name:crsh_save_(int id)
 * Desc: Saves this servo's position and calibration information to EEPROM
 * Para: id, The unique numerical id of the servo to save to mem
 * Note: Should be treated as private member of ContinuousRotationServo
**/
void crs_save_(int id);

// Logic for simple limited rotation servos

typedef struct
{
  byte controlLine;
} LimitedRotationServo;

// Limited rotation servo behavior

/**
 * Name: piezo_getInstance(int id)
 * Desc: Get the limited rotation servo instance with the given id
 * Para: id, The unique numerical id of the desired servo
**/
LimitedRotationServo * lrs_getInstance(int id);

/**
 * Name: lrs_init(int id, byte controlLine)
 * Desc: Initializes this limited rotation servo with starting values
 * Para: id, The id of the servo to operate on
 *       controlLine, The line to which this servo is attached
**/
void lrs_init(int id, byte controlLine);

/**
 * Name: lrs_setAngle(int id, int angle)
 * Desc: Makes this servo rotate to the given angle
**/
void lrs_setAngle(int id, int angle);

/**
 * Name: lrs_step(int id)
 * Desc: Has this limited rotation servo update interal state
 * Para: id, The unique id of the servo to operate on
**/
void lrs_step(int id);

// Piezo sensor abstraction
typedef struct
{
  byte line;
} PiezoSensor;

// Piezo sensor behavior

/**
 * Name: piezo_getInstance(int id)
 * Desc: Get the piezo instance with the given id
 * Para: id, The unique numerical id of the desired piezo sensor
**/
PiezoSensor * piezo_getInstance(int id);

/**
 * Name: piezo_init(int id, byte line)
 * Desc: Creates internal state for piezo sensor
 * Para: id, The unique id of the piezo sensor to initalize
 *       line, The line number (analog) to which this sensor
 *             is attached
**/
void piezo_init(int id, byte line);

/**
 * Name: piezo_isFired(int id)
 * Desc: Determines if the given piezo sensor reigsters a "tapped" state
 * Para: id, The unique id of the piezeo sensor to check
 * Retr: 0 if not reigstering a tap (below threshold) and raw non-zero value
 *       positive reading otherwise
**/
int piezo_isFired();

// Light sensor abstraction
typedef struct
{
  byte line;
} LightSensor;

// Light sensor behavior

/**
 * Name: ls_init(int id, byte line)
 * Desc: Initalizes this light sensors' state
 * Para: id, The unique numerical id for this light sensor
 *       line, The line this sensor is attached to
**/
void ls_init(int id, byte line);

/**
 * Name: ls_isLight(int id)
 * Desc: Determines if this light sensor indicates that the 
 *       environment is light or dark
 * Para: id, The unique numerical id of the light sensor to
 *           check
 * Retr: True if light and False if dark
**/
boolean ls_isLight(int id);

// LED abstraction

typedef struct
{
  byte line;
} LEDAbstraction;

// LED abstraction behavior

/**
 * Name: led_getInstance(int id)
 * Desc: Gets the structure instance corresponding to
 *       the given id
 * Para: id, The unique numerical id of the instance to
 *           get
 * Retr: Pointer to instance
**/
LEDAbstraction * led_getInstance(int id);

/**
 * Name: led_init(int id, byte line)
 * Desc: Creates internal state for given LED
 * Para: id, The unique numerical id of the LED to initalize
 *       port, Which line this led is installed on
**/
void led_init(int id, byte line);

/**
 * Name: led_turnOn(int id)
 * Desc: Turns on the given LED
 * Para: id, The unique numerical id of the LED to turn on
**/
void led_turnOn(int id);

/**
 * Name: led_turnOff(int id)
 * Desc: Turns off the given LED
 * Para: id, The unique numerical id of the LED to turn off
**/
void led_turnOff(int id);

// Jellyfish abstraction

typedef struct
{
  int servoNum;
  int ledNum;
} Jellyfish;

/**
 * Name: jellyfish_getInstance(int id)
 * Desc: Gets the jellyfish instance corresponding to
 *       the given id
 * Para: id, The unique numerical id of the instance to
 *           get
 * Retr: Pointer to instance
**/
Jellyfish * jellyfish_getInstance(int id);

/**
 * Name: jellyfish_init(int id, int servoNum, int ledNum)
 * Desc: Initalize state of given jellyfish
 * Para: id, The unqiue numerical id of the jellyfish to initalize
 *       servoNum, The unique numerical id of the limited roatation
 *                 servo controlling this jellyfish
 *       ledNum, The unique numerical id of the LED inside of this
 *               jellyfish
**/
void jellyfish_init(int id, int servoNum, int ledNum);

/**
 * Name: jellyfish_lower(int id)
 * Desc: Lower this jellyfish into view and turn on its led
 * Para: id, The unique numerical id of the jellyfish to lower
**/
void jellyfish_lower(int id);

/**
 * Name: jellyfish_raise(int id)
 * Desc: Raise this jellyfish out of view and turn of its led
 * Para: id, The unique numerical id of the jellyfish to raise
**/
void jellyfish_raise(int id);

// Fish abstraction

typedef struct
{
  int xServo;
  int yServo;
  int zServo;
  int thetaServo;
  int velocity;
  long targetX;
  long targetY;
  long targetZ;
  long startX;
  long startY;
  long startZ;
  long startMS;
  float xSpeedPortion;
  float ySpeedPortion;
  float zSpeedPortion;
  int subStepsLeftToGoal;
  double moveAngle;
} Fish;

/**
 * Name: jellyfish_getInstance(int id)
 * Desc: Gets the fish instance corresponding to
 *       the given id
 * Para: id, The unique numerical id of the instance to
 *           get
 * Retr: Pointer to instance
**/
Fish * fish_getInstance(int id);

/**
 * Name: fish_init(int id, int xServoNum, int yServoNum, int zServoNum, int thetaServo, boolean calibrate)
 * Desc: Initalizes the state of a fish
 * Para: id, The unique numerical id of the instance to initalize
 *       xServoNum, ID of continuous rotation servo on x axis
 *       yServoNum, ID of continuous rotation servo on y axis
 *       zServoNum, ID of continuous rotation servo on z axis
 *       thetaServo, ID of the servo rotating this fish
**/
void fish_init(int id, int xServoNum, int yServoNum, int zServoNum, int thetaServo);

/**
 * Name: fish_goTo(long id, long x, long y, long z)
 * Desc: Has the fish go to the given position
 * Para: id, The id of the fish to move
 *       x, The x position to take this fish to
 *       y, The y position to take this fish to
 *       z, The z position to take this fish to
**/
void fish_goTo(long id, long x, long y, long z);

/**
 * Name: fish_onGoalReached(int id)
 * Desc: Event handler for when a fish reaches its goal position
 * Para: id, The id of the fish that reached its goal position
**/
void fish_onGoalReached(int id);

/**
 * Name: fish_goToNextInternalGoal_(int id)
 * Desc: Has this fish go to the next sub goal in its larger goal position
 * Para: id, The unique numerical id of the fish to operate on
 * Note: Should be treated as a private member of Fish
**/
void fish_goToNextInternalGoal_(int id);

// Piezo sensor group abstraction

typedef struct
{
  int numSensors;
  int * sensorNums;
} PiezoSensorGroup;

/**
 * Name: pse_getInstance(int id)
 * Desc: Gets the piezeo sensor group instance corresponding to
 *       the given id
 * Para: id, The unique numerical id of the instance to
 *           get
 * Retr: Pointer to instance
**/
PiezoSensorGroup * pse_getInstance(int id);

/**
 * Name: pse_addToSensorList(int id, int sensorID)
 * Desc: Adds a new piezo sensor to this group
 * Para: id, The unique numerical id of the group to operate on
 *       sensorID, The id of the sensor to add to this group
**/
void pse_addToSensorList(int id, int sensorID);

/**
 * Name: pse_getTapped(int id)
 * Desc: Get the sensor that shows a "tapped state"
 * Para: id, The unique numerical id of the group to operate on
 * Retr: NONE or id of sensor that indicatd a tap
**/
int pse_getTapped(int id);

// Aquarium abstraction

typedef struct
{
  int fishNum;
  int jellyfishNum;
  int lightSensorNum;
  int piezeoSensorGroupNum;
} Aquarium;

/**
 * Name: pse_getInstance(int id)
 * Desc: Gets the aquarium instance corresponding to
 *       the given id
 * Para: id, The unique numerical id of the instance to
 *           get
 * Retr: Pointer to instance
**/
Aquarium * aquarium_getInstance(int id);

/**
 * Name: aquarium_init(int id, int fishNum, int jellyfishNum, int lightSensorNum, piezeoSensorGroupNum)
 * Desc: Initalizes state of the given aquarium
 * Para: id, THhe unique numerical id of this aquarium
 *       jellyfishNum, The unique numerical id of the jellyfish in use in this aquarium
 *       lightSensorNum, The unique numerical id of the light sensor for this aquarium
 *       piezoSensorGroupNum, The unique numerical id of the piezo sensor group for this aquarium
**/
void aquarium_init(int id, int fishNum, int jellyfishNum, int lightSensorNum, int piezeoSensorGroupNum);

/**
 * Name: aquarium_shortStep(int id, long ms)
 * Desc: Executes the low latency loop operations (excuted more frequently)
 * Para: id, The unique numerical id of this aquarium
 *       ms, The number of milliseconds since this was last called
**/
void aquarium_shortStep(int id, long ms);

/**
 * Name: aquarium_longStep(int id, long ms)
 * Desc: Executes the high latency loop operations (excuted less frequently)
 * Para: id, The unique numerical id of this aquarium
 *       ms, The number of milliseconds since this was last called
**/
void aquarium_longStep(int id, long ms);

/**
 * Name: aquarium_onFishReachedGoal(int fishID)
 * Desc: Event handler for when a fish reaches its goal position
 * Para: fishID, The id of the fish that reached its goal
**/
void aquarium_onFishReachedGoal(int fishID);

/**
 * Name: aquarium_onFishReachedGoal_(int id, int fishID)
 * Desc: Event handler for when a fish reaches its goal position
 * Para: id, The id of the aquarium who is responding to this event
 *       fishID, The id of the fish taht reached its goal
 * Note: Should be treated as private member of Aquarium
**/
void aquarium_onFishReachedGoal_(int id, int fishID);
