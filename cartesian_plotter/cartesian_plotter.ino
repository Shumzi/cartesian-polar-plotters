#include <Arduino.h>
#include "StepperController.h"
#include "JoystickInterface.h"
#include "Settings.h"

// DEFINITIONS:
void print_current_position();

sys_state state = {IDLE, micros()};
//Servo pen_controller;
StepperController stepper_c = StepperController();
int current_steps_mask = 0;
int current_direction_mask = 0;
int target[N_INSTRUCTIONS] = {0, 0, 0};
const int *current_position = stepper_c.get_steps_count();
int current_drawing = 0;

// USER INTERFACE OBJECTS
Encoder encoder_a = Encoder(ENCODER_A_BIT_0, ENCODER_A_BIT_1, ENCODER_A_BUTTON,'A');
Encoder encoder_b = Encoder(ENCODER_B_BIT_0, ENCODER_B_BIT_1, ENCODER_B_BUTTON,'B');
unsigned long test_timer = 0;
int UV_state = UV_OFF;


void state_handler(int current_steps_mask, int UV_state, StepperController *stepper_c)
{
    // if movement was deteced
    if (current_steps_mask ||  UV_state)
    {

        stepper_c->set_enable(true);
        state.sys_mode = MOVE;
        state.last_move_time_stamp = micros();
    }
    else
        state.sys_mode = IDLE;
}

void toggle_UV_state(StepperController *stepper_c,int UV_state)
{
  if(UV_state){
    int new_val = UV_ON;
    if(stepper_c->get_UV_value() == UV_ON){
      new_val = UV_OFF;
    }
    stepper_c->set_UV_value(new_val);
    print_current_position();
  }

}

void test_movement(StepperController *stepper_c)
{
    Serial.println("Test moving");
    stepper_c->set_steps_rate(500);
    stepper_c->set_enable(true);
    for(int i=0;i<(Y_MM_MAX_LIMIT * Y_STEPS_PER_MM);i++)
    {
        Serial.println(i);
        stepper_c->move_step(2, 1);
        Serial.println(stepper_c->get_steps_count()[Y_AXIS]);
        delayMicroseconds(500);
    }
    Serial.println("Done Test Moving");
    stepper_c->set_steps_rate(STEPS_RATE);    
}

void auto_homing(StepperController *stepper_c)
{
    Serial.println("Auto homing! ");
    stepper_c->set_steps_rate(AUTO_HOME_STEPS_RATE);
    stepper_c->set_enable(true);
    stepper_c->set_UV_value(UV_OFF);

    stepper_c->set_steps_count(mm_to_steps((X_MM_RAIL_LENGTH), X_STEPS_PER_MM), 0);

    // Move X to 0    
    while (stepper_c->get_steps_count()[X_AXIS] > 0)
    {
        stepper_c->move_step(1, 1);
    }
    Serial.println("Moved X axis to place.");

    // Move Y to 0 by using disable (gravity will drop the axis bottle to 0)
    const unsigned long current_time = millis();
    while(millis() - current_time < 1500 ){
      stepper_c->set_enable(false);
    }
    Serial.println("Moved Y axis to place.");
    stepper_c->set_steps_count(int(mm_to_steps(X_MM_HOMING_OFFSET, X_STEPS_PER_MM)), int(mm_to_steps(Y_MM_HOMING_OFFSET, Y_STEPS_PER_MM))); 
    
    // Move head to center of the board 
    stepper_c->set_enable(true);
    while (stepper_c->get_steps_count()[X_AXIS] < 0 )
    {
        stepper_c->move_step(1, 0);
    }
    while (stepper_c->get_steps_count()[Y_AXIS] < 0 )
    {
        stepper_c->move_step(2, 0);
    }

    stepper_c->set_steps_count(0, 0);
    stepper_c->set_limits(X_MM_MAX_LIMIT,Y_MM_MAX_LIMIT,X_MM_MIN_LIMIT, Y_MM_MIN_LIMIT);

    stepper_c->set_steps_rate(STEPS_RATE);
    Serial.println("Auto homing completed successfully! ");
    print_current_position();

    test_movement(stepper_c);
}

void print_current_position()
{
    Serial.println("Position: ");
    Serial.print(stepper_c.get_steps_count()[X_AXIS]);
    Serial.print(",");
    Serial.println(stepper_c.get_steps_count()[Y_AXIS]);

}

void setup()
{

    Serial.begin(115200);
    /** Init Joystick input pins **/
    /** AUTO HOME**/
    auto_homing(&stepper_c);
    state.sys_mode = IDLE;
    pinMode (UV_PIN, OUTPUT);
}

void loop()
{
    test_timer = micros();
    /** GET INPUT MASK **/
    current_steps_mask = 0;
    current_direction_mask = 0;
    UV_state = 0;
    
    getMovementMask(&current_steps_mask,&current_direction_mask, &UV_state, encoder_a,encoder_b);
    
    state_handler(current_steps_mask, UV_state, &stepper_c);

    switch (state.sys_mode)
    {
    case MOVE:
        toggle_UV_state(&stepper_c, UV_state);
        stepper_c.move_step(current_steps_mask, current_direction_mask);
        break;
    case IDLE:
        if(micros() - state.last_move_time_stamp > PEN_PENDING_TIME && stepper_c.get_UV_value() == UV_ON ){
            toggle_UV_state(&stepper_c, true);
        }
        break;
    default:
        break;
    }
    test_timer = micros() - test_timer;
    // Serial.println(test_timer);
}
