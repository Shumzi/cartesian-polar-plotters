// 
/*==========Arduino Nano pinout====== 
 * IO map see J:\Amir Design\PCB_Design\EASYEDA JLBPCB\projects\SHIELD_V4\Shield_Amir_Shalev_V_0_0
 * Encoder A - Left, B-right 
 *                      _______
 *                 TXD-|       |-Vin 
 *                 RXD-|       |-Gnd to Display  
 *                 RST-|       |-RST
 *  To Encoders    GND-|       |-+5V To Display
 *                  D2-|       |-A7
 *                  D3-|       |-A6
 *                  D4-|       |-A5 SCL (to Display) 
 *                  D5-|       |-A4 SDA (to Display)
 *                  D6-|       |-A3 (D17) Encoder B Switch 
 *                  D7-|       |-A2 (D16) Encoder A Switch
 *                  D8-|       |-A1 (D15) Encoder B bit 0
 *  Encoder A bit 1 D9-|       |-A0 (D14) Encoder A bit 0 , mark Abort 
 *                 D10-|       |-Ref
 *                 D11-|       |-3.3V   
 * Encoder B bit 1 D12-|       |-D13
 *                      --USB--          
 *                     
 * ! Nano can use only D2, D3 as interupt -  encoder B must read by pulling                      
 * Mechanical encoder like https://www.aliexpress.com/item/1005005239756119.html 
 */ 

#ifndef SETTINGS_H
#define SETTINGS_H

// Instructions indices
#define X_AXIS 0
#define Y_AXIS 1
#define SERVO_ANGLE 2

#define N_AXIS 2
#define N_INSTRUCTIONS 3
#define bit_istrue(x, mask) ((x & mask) != 0)
#define bit_to_sign(x, mask) (((x & mask) != 0) ? -1 : 1)
#define DIRECTION_INVERT_MASK 3

// STEPERS CONFIGURATIONS
#define X_STEP_PIN 5
#define Y_STEP_PIN 6
#define X_DIR_PIN 2
#define Y_DIR_PIN 3
#define EN_PIN 8

// LIMIT SWITCHES
#define X_LIMIT_SW_PIN 9
#define Y_LIMIT_SW_PIN 10

// ENCODERS PINS
#define ENCODER_A_BIT_1 (A0) // input IO for gray code bit 0 
#define ENCODER_A_BIT_0 (9) // input IO for gray code bit 1
#define ENCODER_B_BIT_1 (A1) // input IO for gray code bit 0
#define ENCODER_B_BIT_0 (12) // input IO for gray code bit 1
#define ENCODER_A_BUTTON (A2)
#define ENCODER_B_BUTTON (A3)
#define ENCODER_READ_INTERVAL 200 //microseconds
#define STEPS_PER_CLICK 300
#define DIRECTION_CHANGE_DELAY 40
#define ENCODER_BEHAVIOR 1 // if 0: PEN_ON applyed as long as the user press, if 1: each press will toggle the pen state

// SERVO
#define SERVO_COMMAND_PIN (13)
#define PEN_ON 0
#define PEN_OFF 30

//UV
#define UV_PIN 7
#define UV_ON 1
#define UV_OFF 0






// SYSTEM CONFIGURATIONS
#define STEP_PULSE_LENGTH 20
#define STEPS_RATE 1500
#define X_STEPS_PER_MM (10)
#define Y_STEPS_PER_MM (7.6)

// SOFT LIMITS SETTINGS
// #define ENABLE_SOFT_LIMIT 1 // uncomment to disable soft limits
#define AUTO_HOME_STEPS_RATE 1200

#define X_MM_HOMING_OFFSET (-330)
#define Y_MM_HOMING_OFFSET (-330)

#define Y_MM_MIN_LIMIT (-300)
#define X_MM_MIN_LIMIT (-290)
#define Y_MM_MAX_LIMIT 230
#define X_MM_MAX_LIMIT 310

#define X_MM_RAIL_LENGTH (-X_MM_MIN_LIMIT + X_MM_MAX_LIMIT)
#define Y_MM_RAIL_LENGTH (-Y_MM_MIN_LIMIT + Y_MM_MAX_LIMIT)

// AUTO PRINTING
#define PENDING_TIME (1000000 * 180)
#define PEN_PENDING_TIME (1000000 * 3)

#define steps_to_mm(steps, ratio) (steps / ratio)
#define mm_to_steps(mm, ratio) (mm * ratio)

template <typename T>
int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}

enum State
{
  IDLE,
  MOVE,
  PRINT
};

struct sys_state
{
  State sys_mode;
  long unsigned last_move_time_stamp;
};

#endif
