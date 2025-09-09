#include <Arduino.h>
#include <AccelStepper.h>
#include "JoystickInterface.h"
#include "Settings.h"
// DEFINITIONS:
void print_current_position();

AccelStepper stepper_x(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepper_y(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
int current_x_pos = 0;
int current_y_pos = 0;

// USER INTERFACE OBJECTS

Encoder encoder_x = Encoder(ENCODER_A_BIT_0, ENCODER_A_BIT_1, ENCODER_A_BUTTON,'A', X_MAX_LIMIT, X_MIN_LIMIT);
Encoder encoder_y = Encoder(ENCODER_B_BIT_0, ENCODER_B_BIT_1, ENCODER_B_BUTTON,'B', Y_MAX_LIMIT, Y_MIN_LIMIT);
unsigned long time_last_action = 0;
bool uv_state = LOW;

void cartesian_auto_homing()
{
    Serial.println("homing");
    Serial.print("stepper_X loc");
    Serial.println(stepper_x.currentPosition());
    // pin to left
    stepper_x.moveTo(stepper_x.currentPosition() - (X_MAX_LIMIT + 100 - X_MIN_LIMIT) * STEPPER_X_STEPSIZE);
    float acc = stepper_x.acceleration();
    float max_speed = stepper_x.maxSpeed();
    stepper_x.setAcceleration(100);
    stepper_x.setMaxSpeed(1000);
    stepper_x.runToPosition();
    // move to middle
    stepper_x.setAcceleration(acc);
    stepper_x.setMaxSpeed(max_speed);
    stepper_x.moveTo(stepper_x.currentPosition() + X_HOMING_OFFSET * STEPPER_X_STEPSIZE);
    stepper_x.runToPosition();
    Serial.println("moved x");
    // move to bottom
    stepper_y.disableOutputs();
    Serial.println("disabled y");
    delay(2000);
    stepper_y.enableOutputs();
    Serial.println("enabled y");
    // move to middle
    stepper_y.moveTo(stepper_y.currentPosition() + Y_HOMING_OFFSET * STEPPER_Y_STEPSIZE);
    stepper_y.runToPosition();
    stepper_x.setCurrentPosition(0);
    stepper_y.setCurrentPosition(0);
}

void move_to_switch (AccelStepper as, int pin_no)
{
    float acc = as.acceleration();
    float max_speed = as.maxSpeed();
    as.setAcceleration(500);
    as.setMaxSpeed(500);
    as.moveTo(-5000);
    while(digitalRead(pin_no))
    {
        as.run();
    }
    as.setCurrentPosition(0);
    as.moveTo(0);
    as.setAcceleration(acc);
    as.setMaxSpeed(max_speed);

}
void polar_extra_setup_and_auto_homing()
{
    pinMode(X_LIMIT_SW_PIN, INPUT_PULLUP);
    pinMode(Y_LIMIT_SW_PIN, INPUT_PULLUP);
    stepper_x.moveTo(3000);
    stepper_x.runToPosition();
    stepper_y.moveTo(3000);
    stepper_y.runToPosition();
    move_to_switch(stepper_x, X_LIMIT_SW_PIN);
    move_to_switch(stepper_y, Y_LIMIT_SW_PIN);
    stepper_x.setCurrentPosition(0);
    stepper_y.setCurrentPosition(0);
    
}

void print_current_position()
{
    Serial.println("Position: ");
    Serial.print(stepper_x.currentPosition());
    Serial.print(",");
    Serial.println(stepper_y.currentPosition());

}

void setup()
{

    Serial.begin(115200);
    /** Init Joystick input pins **/
    /** AUTO HOME**/
    // auto_homing(&stepper_c);
    pinMode (UV_PIN, OUTPUT);
    
    stepper_x.setMaxSpeed(X_MAX_SPEED * STEPPER_X_STEPSIZE);
    stepper_x.setAcceleration(X_ACCELERATION * STEPPER_X_STEPSIZE);

    stepper_y.setMaxSpeed(Y_MAX_SPEED * STEPPER_Y_STEPSIZE);
    stepper_y.setAcceleration(Y_ACCELERATION * STEPPER_Y_STEPSIZE);
    stepper_y.setEnablePin(EN_PIN);
    stepper_y.setPinsInverted(false,false,true);
    stepper_y.enableOutputs();
    if(SYSTEM_TYPE == "cartesian")
        cartesian_auto_homing();
    else if(SYSTEM_TYPE == "polar")
        polar_extra_setup_and_auto_homing();

}
// void update_UV(Encoder enc_1, Encoder enc_2)
// {
//     static bool was_pressed = false;

//     if(enc_1.is_pressed() || enc_2.is_pressed())
//     {
//         if(!was_pressed)
//         {
//             uv_state = !uv_state;
//             digitalWrite (UV_PIN, uv_state); 
//             time_last_action = micros();
//         }
//         was_pressed = true;
//     }
//     else
//         was_pressed = false;
// }

void print_when_press(Encoder enc_1, Encoder enc_2)
{
    static bool is_pressed = false;
    if(enc_1.is_pressed() || enc_2.is_pressed())
    {
        if(!is_pressed)
        {
            Serial.print("Encoder X Count: ");
            Serial.println(current_x_pos);
            Serial.print(", Encoder Y Count: ");
            Serial.println(current_y_pos);
        }
        is_pressed = true;
    }
    else
    {
        is_pressed = false;
    }
}
void update_uv_movement()
{
    time_last_action = micros();
    if(!uv_state)
    {
        uv_state = HIGH;
        digitalWrite (UV_PIN, uv_state);
    }
}

void check_idle()
{
    if (uv_state && ((micros() - time_last_action) > PEN_PENDING_TIME))
    {
        uv_state = LOW;
        digitalWrite (UV_PIN, uv_state);
        time_last_action = micros();
    }
}
void loop()
{
    int new_x_pos = encoder_x.read_encoder();
    int new_y_pos = encoder_y.read_encoder();

    // update_UV(encoder_x, encoder_y);
    print_when_press(encoder_x, encoder_y);

    // calculate move X
    if(current_x_pos != new_x_pos && new_x_pos > X_MIN_LIMIT + 5 && new_x_pos < X_MAX_LIMIT - 5)
    {   
        stepper_x.moveTo(new_x_pos * STEPPER_X_STEPSIZE);
        current_x_pos = new_x_pos;
        update_uv_movement();
    }
    
    // calculate move Y
    if(current_y_pos != new_y_pos && new_y_pos > Y_MIN_LIMIT + 5 && new_y_pos < Y_MAX_LIMIT - 5)
    {   
        stepper_y.moveTo(new_y_pos * STEPPER_X_STEPSIZE);
        current_y_pos = new_y_pos;
        update_uv_movement();
    }
    
    // turn off LED if idle.
    check_idle();

    // update steppers.
    stepper_x.run();
    stepper_y.run();
}
