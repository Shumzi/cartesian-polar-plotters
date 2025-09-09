#include "JoystickInterface.h"

Encoder::Encoder(int bit_0_pin,int bit_1_pin,int push_button_pin, char encoder_char, int max_val, int min_val)
:bit_0_pin_(bit_0_pin),bit_1_pin_(bit_1_pin),push_button_pin_(push_button_pin), encoder_char_(encoder_char),max_val(max_val),min_val(min_val),time_stamp_(0),
prev_encoder_read_(0), new_encoder_read_(0),counter_(0),prev_counter_(0),direction_(0), steps_to_move_(0)
{
  initialize_encoder();
 }

void Encoder::initialize_encoder(){
  pinMode(bit_0_pin_, INPUT_PULLUP);  
  pinMode(bit_1_pin_, INPUT_PULLUP);  
  pinMode(push_button_pin_, INPUT_PULLUP);
  time_stamp_ = millis();
  Serial.print("Initialized encoder ");
  Serial.println(encoder_char_);
  }

bool Encoder::is_pressed_ = 0;
  
int Encoder::read_encoder(){
  prev_encoder_read_ = new_encoder_read_ ;
  new_encoder_read_ = (((digitalRead(bit_1_pin_)) << 1) + (digitalRead(bit_0_pin_))) ;
  byte check_direction  = (prev_encoder_read_ << 2) + new_encoder_read_  ; // x4 = 2 rotate left 
  switch (check_direction) // see https://commons.wikimedia.org/wiki/File:Incremental_directional_encoder.gif
  {
    case 1: case 7: case 8: case 14:
    counter_ = max(counter_-1, min_val);
    break;
    case 2: case 4: case 11: case 13:
    counter_ = min(counter_+1, max_val);
    break;
  }
  return counter_; 
}

void Encoder::set_direction(){
    direction_ = sgn(counter_ - prev_counter_);
    if(direction_ && direction_ != last_direction_  ){
      last_direction_ = direction_;
    }
  }

void Encoder::print_current_counter(){
  Serial.print("Encoder ");
  Serial.print(encoder_char_);
//  Serial.print(", time:");
//  Serial.print(millis()-time_stamp_);
//  time_stamp_ = micros();
  Serial.print(", is pressed:");
  Serial.print(!digitalRead(push_button_pin_));
  Serial.print(", counter: ");
  Serial.println(counter_);
  }

bool Encoder::is_pressed(){
  return !digitalRead(push_button_pin_);
  }
  
 void read_encoder_toggle_press(Encoder &encoder_a, Encoder &encoder_b, int *UV_state){
    if(!Encoder::is_pressed_ && (encoder_a.is_pressed() || encoder_b.is_pressed())){
      Encoder::is_pressed_ = true;
   }
   else if(Encoder::is_pressed_ && (!encoder_a.is_pressed() && !encoder_b.is_pressed())){
      Encoder::is_pressed_ = false;
      Serial.println("pressed! ");
      *UV_state = 1;
   }
 }

  
void read_encoder_long_press(Encoder &encoder_a, Encoder &encoder_b, int *UV_state){
  if(!Encoder::is_pressed_ && (encoder_a.is_pressed() || encoder_b.is_pressed())){
      Encoder::is_pressed_ = true;
      *UV_state = 1;
   }
   else if(Encoder::is_pressed_ && (encoder_a.is_pressed() || encoder_b.is_pressed())){
      // Serial.println("pressed! ");
      *UV_state = 0;
      
   }
    else if(Encoder::is_pressed_ && (!encoder_a.is_pressed() && !encoder_b.is_pressed())){
      Encoder::is_pressed_ = false;
      Serial.println("pressed! ");
      *UV_state = 1;
   }
   

}
