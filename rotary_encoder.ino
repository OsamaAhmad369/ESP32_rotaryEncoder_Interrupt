
#define encoderAPin 32
#define encoderBPin 21


int16_t lastReadEncoder0Pos;
uint8_t encoderSteps=4;
bool _circleValues = false;
volatile int16_t encoder0Pos = 0;
uint8_t old_AB;
int8_t enc_states[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

void IRAM_ATTR isr()
{    
    /**/
    old_AB <<= 2;                   //remember previous state
    int8_t ENC_PORT = ((digitalRead(encoderBPin)) ? (1 << 1) : 0) | ((digitalRead(encoderAPin)) ? (1 << 0) : 0);        
    old_AB |= ( ENC_PORT & 0x03 );  //add current state
    encoder0Pos += (enc_states[(old_AB & 0x0f )]);     
}

int16_t readEncoder()
{
  return (encoder0Pos / encoderSteps);
}
int16_t encoderChanged() {
  int16_t _encoder0Pos = readEncoder(); 
  int16_t encoder0Diff = _encoder0Pos - lastReadEncoder0Pos;

  lastReadEncoder0Pos = _encoder0Pos;
  
  return encoder0Diff;
}
void rotary_loop() {
  //lets see if anything changed
  int16_t encoderDelta = encoderChanged();
  
  //optionally we can ignore whenever there is no change
  if (encoderDelta == 0) return;
  
  //for some cases we only want to know if value is increased or decreased (typically for menu items)
  if (encoderDelta>0) Serial.print("+");
  if (encoderDelta<0) Serial.print("-");

  //for other cases we want to know what is current value. Additionally often we only want if something changed
  //example: when using rotary encoder to set termostat temperature, or sound volume etc
  
  //if value is changed compared to our last read
  if (encoderDelta!=0) {
    //now we need current value
    int16_t encoderValue =readEncoder();
    //process new value. Here is simple output.
    Serial.print("Value: ");
    Serial.println(encoderValue);
  }
}

void setup() {
  
  Serial.begin(115200);
  old_AB = 0;
  pinMode(encoderAPin, INPUT_PULLDOWN);
  pinMode(encoderBPin, INPUT_PULLDOWN);
  
 attachInterrupt(digitalPinToInterrupt(encoderAPin), isr, CHANGE);
 attachInterrupt(digitalPinToInterrupt(encoderBPin), isr, CHANGE);
  
}

void loop() {

rotary_loop();
}
