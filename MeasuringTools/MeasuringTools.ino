/*
  This Code uses 2 EncodersDoneProperly

*/

// Enable weak pullups
#define ENABLE_PULLUPS

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
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
String inputString = "";                 // string variable to hold incoming data
boolean stringCompleteFlag = false;          // variable to indicate inputString is complete (newline found)


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
  if ( (timeCurrent - timePrevious) > serialDataInterval ) { // Enter into this only when interval has elapsed
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

/*
  Create the Interrupt Service Routine (ISR)
  this is what happens when one of the pins
  changes state
*/

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

/*
   OUTGOING SERIAL DATA PROCESSING CODE-------------------------------------------------------------------
*/

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

/*
 * INCOMING SERIAL DATA PROCESSING CODE-------------------------------------------------------------------
 */

void processIncomingSerial()
{
  getSerialData();
  parseSerialData();
}

//Gather bits from serial port to build inputString
void getSerialData() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();      // get new byte
    inputString += inChar;                  // add it to input string
    if (inChar == '\n') {                   // if we get a newline...
      stringCompleteFlag = true;            // we have a complete string of data to process
    }
  }
}

void parseSerialData()
{
  if (stringCompleteFlag) { // process data from inputString to set program variables.
    //process serial data - set variables using: var = getValue(inputString, ',', index).toInt(); // see getValue function below

    resetCount          = getValue(inputString, ',', 0).toInt();   //Data Out worksheet cell A5

     pointsPerRevolution = getValue(inputString, ',', 1).toDouble();   //Data Out worksheet cell B5


    if(pointsPerRevolution > 0){
      incrementAmount     = degreesPerRevolution / pointsPerRevolution;
    }

    inputString = "";                           // reset inputString
    stringCompleteFlag = false;                 // reset stringComplete flag
  }
}

//Get value from inputString using a matching algorithm
String getValue(String mDataString, char separator, int index)
{ // mDataString is inputString, separator is a comma, index is where we want to look in the data 'array'
  int matchingIndex = 0;
  int strIndex[] = {0, -1};
  int maxIndex = mDataString.length()-1;
  for(int i=0; i<=maxIndex && matchingIndex<=index; i++){     // loop until end of array or until we find a match
    if(mDataString.charAt(i)==separator || i==maxIndex){       // if we hit a comma OR we are at the end of the array
      matchingIndex++;                                        // increment matchingIndex to keep track of where we have looked
      // set substring parameters (see return)
      strIndex[0] = strIndex[1]+1;                            // increment first substring index
      // ternary operator in objective c - [condition] ? [true expression] : [false expression]
      strIndex[1] = (i == maxIndex) ? i+1 : i;                // set second substring index
    }
  }
  return matchingIndex>index ? mDataString.substring(strIndex[0], strIndex[1]) : ""; // if match return substring or ""
}

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
