//===------__ Hacking STEM – MeasuringToolsCode.X.X.X.ino – Arduino __-----===//
// For use with the "MEASURING TOOLS Using the Pythagorean Theorem to Explore
// Topography in 2D/3D Space" lesson plan available from Microsoft Education
// Workshop at http://aka.ms/hackingSTEM
//
// Overview:
// This project uses Rotary Encoders to find angles.
//
// Output to Serial reports angle, count, and revolutions of two encoders, in
// format: "angleOne,countOne,revolutionsOne,angleTwo,countTwo,revolutionsTwo"
// example:
// "353,-1087,-4,45,45,0,"
//
// For encoder one:
// AngleOne: Angle, in degrees. Can be positive or negative.
// CountOne: Count from the rotary encoder signal, can be positive or negative.
// RevolutionsOne: Count of full rotations, can be positive or negative.
//
// For encoder two:
// AngleTwo: Angle, in degrees. Can be positive or negative.
// CountTwo: Count from the rotary encoder signal, can be positive or negative.
// RevolutionsOne: Count of full rotations, can be positive or negative.
//
// How rotary encoders work:
// Rotary encoders interface through two digital pins on the arduino. State
// changes on these pins will progress through four combinations per step
//
// So to transition from step 0 to step 1, there will be four state changes:
//
// Step 0:   LOW LOW
// Step 1/4: HIGH LOW
// Step 2/4: HIGH HIGH
// Step 3/4: LOW HIGH
// Step 1:   LOW LOW
//
//   Direction:
//   To go the other direction, from 1 to 0, the pins will go through the same
//   four state changes in reverse. The direction of change can be understood by
//   comparing the current state to the last state.
//
//   Step Count:
//   To keep track of the step count, we add 1 for every step encountered in
//   the forward rotation and subtract one for each step encountered in the
//   backward direction. For example, the step count of four steps forward and
//   one steps back is 3, or (1 + 1 + 1 + 1 - 1). The step count of 3 steps
//   backward, two steps forward, and one step backward is negative 2, or
//   (0 - 1 - 1 - 1 + 1 + 1 -1)
//
//   Degree:
//   With an encoder that has 360 steps per revoltion, degree is simply the
//   remainder left over by dividing step count by 360. For example,
//   The remainder of 792 Steps / 360 Steps Per Revolution is 72, so the
//   current degree is 72.
//
//   Rotation Count:
//   To count rotations simply sum every positive and negative step and divide
//   by the steps per revolution of the encoder and round down to get a whole
//   number. For example: 792 Steps / 360 Steps Per Revolution is 2 steps
//   (that's 2.2 rounded down).
//
// With the 360 step rotary encoder degree translation is easy, the
// current step position will be
//
// This project uses an Arduino UNO microcontroller board, information at:
// https://www.arduino.cc/en/main/arduinoBoardUno
//
// This project uses a 360 steps per revoltion rotary encoder, such as:
// lpd3806-360bm-g5-24c
//
// Based on project by Ben Buxton, who has a wonderful post and video
// explaination of rotary encoders on arduino:
// http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
//
// Original Source
// https://github.com/buxtronix/arduino/tree/master/libraries/Rotary
//
// Comments, contributions, suggestions, bug reports, and feature requests
// are welcome! For source code and bug reports see:
// http://github.com/[TODO github path to Hacking STEM]
//
// Copyright [year], [your name] Microsoft EDU Workshop - HackingSTEM
// MIT License terms detailed in LICENSE.md
//===----------------------------------------------------------------------===//
// Enable weak pullups
#define ENABLE_PULLUPS

// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20


//Create a Rotary class in order to create more than one instance of the Encoder
class Rotary
{
  public:
    Rotary(char, char);
    // Process pin(s)
    unsigned char process();
  private:
    unsigned char state;
    unsigned char pin1;
    unsigned char pin2;
};

Rotary distanceTool =  Rotary(2, 3);
Rotary angleTool = Rotary(4, 5);

// Degrees Per Revolution. There are 360 whole degrees per revolution.
const double  degreesPerRevolution = 360;

// Points Per Revolution of encoder
double pointsPerRevolution = 360;
volatile int revolutionsOne = 0;
volatile int revolutionsTwo = 0;

// count that will be incremented or decremented by rotation.
volatile float countOne;
volatile float countPreviousOne;
volatile float angleOne;
volatile float countTwo;
volatile float countPreviousTwo;
volatile float angleTwo;
boolean resetCount = 0;

double incrementAmount = degreesPerRevolution / pointsPerRevolution;

// Serial string data delimeter
const String DELIMETER = ",";

// Serial data interval
const int serialDataInterval = 5;
int timeCurrent, timePrevious;

// Incoming Serial Data
String inputString = "";   // string variable to hold incoming data
boolean stringCompleteFlag = false;  // inputString is complete (newline found)


void setup() {
  Serial.begin(9600);
  cli();
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21);
  sei();
  resetVariables();
}

void loop () {
  processIncomingSerial();
  if (resetCount){
    resetVariables();
  }
  timeCurrent = millis();
   // send data when interval has elapsed AND either count has changed
  if ( (timeCurrent - timePrevious) > serialDataInterval ) {
    if (countOne != countPreviousOne || countTwo != countPreviousTwo) {
      sendDataToSerial();
      timePrevious = timeCurrent;
      countPreviousOne = countOne;
      countPreviousTwo = countTwo;
    }
  }
}

void resetVariables(){
  countOne = 0;
  countPreviousOne = -1;
  angleOne = 0;
  revolutionsOne = 0;
  countTwo = 0;
  countPreviousTwo = -1;
  angleTwo = 0;
  revolutionsTwo = 0;
  resetCount = 0;
}

// Create the Interrupt Service Routine (ISR) this is what happens when one
// of the pins changes state. Interrupt is an important approach to avoid
// missing state changes from the rotary encoder!
ISR(PCINT2_vect) {
unsigned char result = distanceTool.process();
  if (result == DIR_CW) {
    countOne += incrementAmount;
    angleOne += incrementAmount;
    if( (int) angleOne == (int) degreesPerRevolution){
      angleOne = 0;
      revolutionsOne++;
    }

  } else if (result == DIR_CCW) {
    countOne -= incrementAmount;
    angleOne -= incrementAmount;
     if( (int) angleOne == -1 ){
      angleOne = degreesPerRevolution - 1;
      revolutionsOne--;
     }
  }

  result = angleTool.process();
  if (result == DIR_CW) {
    countTwo += incrementAmount;
    angleTwo += incrementAmount;
    if( (int) angleTwo == (int) degreesPerRevolution){
      angleTwo = 0;
      revolutionsTwo++;
    }
  } else if (result == DIR_CCW) {
    countTwo -= incrementAmount;
     angleTwo -= incrementAmount;
     if( (int) angleTwo == -1 ){
      angleTwo = degreesPerRevolution - 1;
      revolutionsTwo--;
     }
  }
}

// Send each of our variables out on Serial (to Excel)
void sendDataToSerial() {
  Serial.print( (int) angleOne );
  Serial.print( DELIMETER );

  Serial.print( (int) countOne );
  Serial.print( DELIMETER );

  Serial.print( revolutionsOne );
  Serial.print( DELIMETER );

  Serial.print( (int) angleTwo );
  Serial.print( DELIMETER );

  Serial.print( (int) countTwo );
  Serial.print( DELIMETER );

  Serial.print( revolutionsTwo );
  Serial.print( DELIMETER );


  Serial.println();
}


// Receive commands via Serial (from Excel)
void processIncomingSerial() {
  getSerialData();
  parseSerialData();
}

// Gather characters from serial port to build inputString
void getSerialData() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();  // get new byte
    inputString += inChar;              // add it to input string
    if (inChar == '\n') {               // if we get a newline...
      stringCompleteFlag = true;        // we have a complete string of data
    }
  }
}

// Use the values from serial input to set program variables
void parseSerialData() {
  // When string is complete, we can process data from inputString
  if (stringCompleteFlag) { 
    // Zero out encoders
    resetCount = getValue(inputString, ',', 0).toInt();

    // Override steps per revoltion (normally 360)
    pointsPerRevolution = getValue(inputString, ',', 1).toDouble();
    if(pointsPerRevolution > 0){
      incrementAmount  = degreesPerRevolution / pointsPerRevolution;
      Serial.println(incrementAmount);
    }

    inputString = "";               // reset inputString
    stringCompleteFlag = false;     // reset stringComplete flag
  }
}

// Gets value from mDataString using an index and separator
// Example:
// given 'alice,bob,dana' and separator ','
// index 0 returns 'alice'
// index 1 returns 'bob'
// index 2 returns 'dana'
//
// mDataString: String as read from Serial (mInputString)
// separator: Character used to separate values (a comma)
// index: where we want to look in the data 'array' for value
String getValue(String mDataString, char separator, int index) {
  int matchingIndex = 0;
  int strIndex[] = {0, -1};
  int maxIndex = mDataString.length()-1;
  // loop until end of array or until we find a match
  for(int i=0; i<=maxIndex && matchingIndex<=index; i++){
    if(mDataString.charAt(i)==separator || i==maxIndex){ // if we hit a comma
                                                         // OR end of the array
      matchingIndex++;   // increment to track where we have looked
      strIndex[0] = strIndex[1]+1;   // increment first substring index
      strIndex[1] = (i == maxIndex) ? i+1 : i;   // set second substring index
    }
  }
  // if match return substring or ""
  if (matchingIndex > index) {
    return mDataString.substring(strIndex[0], strIndex[1]);
  } else {
    return "";
  }
}

//===----------------------------------------------------------------------===//
//    Below here is adaptation of Ben Buxton's 'Rotary Encoders Done Well'
//===----------------------------------------------------------------------===//

/*
   The below state table has, for each state (row), the new state
   to set based on the next encoder output. From left to right in,
   the table, the encoder outputs are 00, 01, 10, 11, and the value
   in that position is the new state to set.
*/

#define R_START 0x0

#ifdef HALF_STEP
// Use the half-step state table (emits a code at 00 and 11)
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
// Use the full-step state table (emits a code at 00 only)
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

Rotary::Rotary(char _pin1, char _pin2) {
  // Assign variables.
  pin1 = _pin1;
  pin2 = _pin2;
  // Set pins to input.
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
#ifdef ENABLE_PULLUPS
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
#endif
  // Initialise state.
  state = R_START;
}

unsigned char Rotary::process() {
  // Grab state of input pins.
  unsigned char pinstate = (digitalRead(pin2) << 1) | digitalRead(pin1);
  // Determine new state from the pins and state table.
  state = ttable[state & 0xf][pinstate];
  // Return emit bits, ie the generated event.
  return state & 0x30;
}
