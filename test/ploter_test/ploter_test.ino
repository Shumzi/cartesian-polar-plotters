#include <EncoderButton.h>
#include <AccelStepper.h>


// The X Stepper pins
#define STEPPER1_DIR_PIN 2
#define STEPPER1_STEP_PIN 5
#define STEPPER1_U_STEP 1
// The Y stepper pins
#define STEPPER2_DIR_PIN 3
#define STEPPER2_STEP_PIN 6
#define STEPPER2_U_STEP 1

#define ENC_TO_STEP_MULT 8
#define MINIMAL_MS_UPDATE_TARGET  10

unsigned long ticks = 0;
unsigned long last_st1_movement_ms = 0;
unsigned long last_st2_movement_ms = 0;
int new_target = 0;
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
const int max_speed = 2000;
const int acceleration = 300;



EncoderButton eb1(A0, 9, A2);
EncoderButton eb2(A1, 12, A3);
AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);

void onEb1Clicked(EncoderButton& eb) {
    Serial.print("eb1 clickCount: ");
    Serial.println(eb.clickCount());
}

void onEb2Clicked(EncoderButton& eb) {
    Serial.print("eb2 clickCount: ");
    Serial.println(eb.clickCount());
}

/**
 * A function to handle the 'encoder' event
 */
void onEb1Encoder(EncoderButton& eb) {
    Serial.print("eb1 position is: ");
    Serial.println(eb.position());
    // stepper1.moveTo(eb.position() * ENC_TO_STEP_MULT * STEPPER1_U_STEP); 

    
}

void onEb2Encoder(EncoderButton& eb) {
    Serial.print("eb2 position is: ");
    Serial.println(eb.position());
    // stepper2.moveTo(eb.position() * ENC_TO_STEP_MULT * STEPPER2_U_STEP);
}


void setup() {
  // put your setup code here, to run once:

  eb1.setClickHandler(onEb1Clicked);
  eb1.setEncoderHandler(onEb1Encoder);
  eb2.setClickHandler(onEb2Clicked);
  eb2.setEncoderHandler(onEb2Encoder);
  eb1.useQuadPrecision(true);
  eb2.useQuadPrecision(true);
  stepper1.setMaxSpeed(max_speed * STEPPER1_U_STEP);
  stepper1.setAcceleration(2000.0 * STEPPER1_U_STEP);
    
  stepper2.setMaxSpeed(max_speed * STEPPER2_U_STEP);
  stepper2.setAcceleration(2000.0 * STEPPER2_U_STEP);

  Serial.begin(115200);
  delay(500);
  Serial.println("Ploter Teser");
}

void loop() {
  // put your main code here, to run repeatedly:
    eb1.update();
    eb2.update();

    stepper1.run();
    stepper2.run();
}
