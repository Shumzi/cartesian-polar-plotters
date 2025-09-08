/* 1 Sep 2025 Add IO D7 for UV/LED
 * based on Shalev_Shield_Tester_21_JAN_2024
 * 12 Nov 2023 Test code for Shalev Shield , XY motors , 2 encoders, + switch, 2 sensors, Servo 
 * See  J:\Amir Design\PCB_Design\EASYEDA JLBPCB\projects\SHIELD_V4\Shield_Amir_Shalev_V_0_0 
 * Amirb@mada.org.il.  
 * Nano, ATmega328T oldbootloader
 * pin out - see CONST_N_Global.h
 
 *serial print and/or (acording to flags - see/set in const_N_Global) display (OLED SH106) encoders counting 
 * 
 * (1) Rotary 2bit encoder: like https://www.aliexpress.com/item/1005005239756119.html  or https://www.aliexpress.com/item/1005004796884931.html
 */

#include "CONST_N_GLOBAL.h"
#include "Serial_Routins.h"
#include "Display_Routins.h"
#include "Encoders_Routins.h"
#include "Steppers_Routins.h"
#include <AccelStepper.h>
// The X Stepper pins
#define STEPPER1_DIR_PIN 2
#define STEPPER1_STEP_PIN 5
#define STEPPER1_U_STEP 4
// The Y stepper pins
#define STEPPER2_DIR_PIN 3
#define STEPPER2_STEP_PIN 6
#define STEPPER2_U_STEP 4

#include <Servo.h>
Servo myservo;  // create servo object to control a servo
const int max_speed = 2000;
const int acceleration = 1000;
bool uv_state = false;
AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);
//==================================-
void setup() {
  // make encoder IO pulsedup inputs:
  pinMode(ENCODER_A_BIT_0, INPUT_PULLUP);  
  pinMode(ENCODER_A_BIT_1, INPUT_PULLUP);  
  pinMode(ENCODER_B_BIT_0, INPUT_PULLUP);  
  pinMode(ENCODER_B_BIT_1, INPUT_PULLUP);  
  pinMode(ENCODER_A_SW, INPUT_PULLUP);  
  pinMode(ENCODER_B_SW, INPUT_PULLUP);  
  // pinMode(STEPPER_EN_PIN, OUTPUT);// invert !
  pinMode(UV_LED, OUTPUT);// 
//   digitalWrite(STEPPER_EN_PIN, HIGH);// disable motors, 
//   for(uint8_t i=0; i< NUMBER_OF_MOTORS; ++i){
//     pinMode(DIR_PIN[i],OUTPUT);
//     digitalWrite(DIR_PIN[i],LOW);// initialy set direction to low/0
//     pinMode(STEP_PIN[i],OUTPUT);
//     digitalWrite(STEP_PIN[i],LOW);// pulse low 
//   }
//  digitalWrite(STEPPER_EN_PIN, LOW);// enable motors, in this code motors consume current all time.     

  stepper1.setMaxSpeed(max_speed * STEPPER1_U_STEP);
  stepper1.setAcceleration(2000.0 * STEPPER1_U_STEP);
    
  stepper2.setMaxSpeed(max_speed * STEPPER2_U_STEP);
  stepper2.setAcceleration(2000.0 * STEPPER2_U_STEP);

//  update_rates();
  Current_Time = millis();
  if(Use_Serial) {Start_Serial();}
  if(Use_Display) {Start_Display();}
//   Homming();
  set_steps(0,0);// set (define) current (steps) position 
  //set_position(0, 0, 0);
 
 myservo.attach(SERVO_CONTROL_IO);  // attaches the servo on pin SERVO_CONTROL_IO to the servo object 
}

void loop() {
  Update_Encoder_A_Count();
  if(Old_Encoder_A_Count != Current_Encoder_A_Count)
  {   
    // print_step();
    stepper1.moveTo(Current_Encoder_A_Count * STEPPER1_U_STEP);
    Old_Encoder_A_Count = Current_Encoder_A_Count;
  }
  // if (Current_Encoder_A_Count > Old_Encoder_A_Count){stepper1.moveTo();}
  // if (Current_Encoder_A_Count < Old_Encoder_A_Count){one_step(0,true);}
  
  Update_Encoder_B_Count();
  if(Old_Encoder_B_Count != Current_Encoder_B_Count)
  {   
    // print_step();
    stepper2.moveTo(Current_Encoder_B_Count);
    Old_Encoder_B_Count = Current_Encoder_B_Count;
  }
  read_SW();
  if (OLD_SW_B != SW_B)
  {
    Serial.println(uv_state);
    Serial.println("SW_B");
    Serial.println(SW_B);
    if (SW_B == 0)
    {
      Serial.println(uv_state);
      digitalWrite(UV_LED, !uv_state); //change (On/off uv led)
      uv_state = !uv_state;
    }
    Print_Encoders_Count();
//    myservo.attach(SERVO_CONTROL_IO);
    myservo.write(SW_B==0 ? UP_SERVO : DN_SERVO);// up/dn pen acording to switch preased
//    delay(1000);
//    myservo.detach();
    OLD_SW_B = SW_B;
    }
//  if (SW_B==0) {myservo.write(UP_SERVO);}// up pen if B switch preased
//  if (SW_B==1) {myservo.write(DN_SERVO);}// up down  
  if (SW_A==0) {// if A  switch is preased = serial print and/or display count and steps 
   if (millis() >= Current_Time+Update_Display_Time) { 
    if(Use_Serial) {Print_Encoders_Count();}
    if(Use_Display) {display_step();}
    Current_Time = millis();     
    }
  }    
  stepper1.run();
  stepper2.run();
//  delay(Main_Loop_Delay_Time);
//    one_step(j,true);
}
