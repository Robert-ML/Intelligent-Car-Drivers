/*
 * speed_sensor_interface.cpp
 *
 *  Created on: Aug 28, 2020
 *      Author: Robert
 */
#include "./speed_sensor_interface.h"

SpS* SpS::instance = NULL;

SpS* SpS::get_instance() {
  return instance;
}

SpS* SpS::get_instance(uint16_t second_divider) {
  if (instance == NULL) {
    instance = new SpS(second_divider);
  }
  return instance;
}

SpS::SpS(uint16_t second_divider) {
  this->cnt_mot_a = 0;
  this->rpc_a = 0;
  this->forward_a = false;
  
  this->cnt_mot_b = 0;
  this->rpc_b = 0;
  this->forward_b = false;

  // setup the timer interrupt and add the irq handler
  FTM1_Timer_int::get_instance(second_divider)->add_irq_handler(SpS::timer_overflowed);
}

//*****************************************************************************
// Called by the interrupts

// speed = false => Sensor A was triggered first
// speed = true => Sensor B was triggered first
// Like this we figure out the direction
#if SPS_MOT_A
void SpS::sensor_triggered_mot_a(bool speed, bool other_sensor) {
  ++cnt_mot_a;
  forward_a = SPS_MOT_A_FORWARD(speed ^ other_sensor);
}
#endif

#if SPS_MOT_B
void SpS::sensor_triggered_mot_b(bool speed, bool other_sensor) {
  ++cnt_mot_b;
  forward_b = SPS_MOT_B_FORWARD(speed ^ other_sensor);
}
#endif

void SpS::timer_overflowed() {
#if SPS_MOT_A
  instance->rpc_a = instance->cnt_mot_a;
  instance->cnt_mot_a = 0;
#endif

#if SPS_MOT_B
  instance->rpc_b = instance->cnt_mot_b;
  instance->cnt_mot_b = 0;
#endif
}

//*****************************************************************************
// The interrupt handlers
#if defined (__cplusplus)
extern "C" {
#endif

// default name
void GPIO_1_IRQHANDLER() {
#if SPS_MOT_A
  if (PORTA->ISFR & (1 << BOARD_INITPINS_MOT_A_SPEED_A_PIN)) {
    SpS::instance->sensor_triggered_mot_a(false, (GPIOA->PDIR & (1 << BOARD_INITPINS_MOT_A_SPEED_B_PIN)) > 0);
    PORTA->ISFR &= ~(1 << BOARD_INITPINS_MOT_A_SPEED_A_PIN);
  }
  if (PORTA->ISFR & (1 << BOARD_INITPINS_MOT_A_SPEED_B_PIN)) {
    SpS::instance->sensor_triggered_mot_a(true, (GPIOA->PDIR & (1 << BOARD_INITPINS_MOT_A_SPEED_A_PIN)) > 0);
    PORTA->ISFR &= ~(1 << BOARD_INITPINS_MOT_A_SPEED_B_PIN);
  }
#else
  PORTA->ISFR &= ~(1 << BOARD_INITPINS_MOT_A_SPEED_A_PIN);
  PORTA->ISFR &= ~(1 << BOARD_INITPINS_MOT_A_SPEED_B_PIN);
#endif

#if SPS_MOT_B
  if (PORTA->ISFR & (1 << BOARD_INITPINS_MOT_B_SPEED_A_PIN)) {
    SpS::instance->sensor_triggered_mot_b(false, (GPIOA->PDIR & (1 << BOARD_INITPINS_MOT_B_SPEED_B_PIN)) > 0);
    PORTA->ISFR &= ~(1 << BOARD_INITPINS_MOT_B_SPEED_A_PIN);
  }
  if (PORTA->ISFR & (1 << BOARD_INITPINS_MOT_B_SPEED_B_PIN)) {
    SpS::instance->sensor_triggered_mot_b(true, (GPIOA->PDIR & (1 << BOARD_INITPINS_MOT_B_SPEED_A_PIN)) > 0);
    PORTA->ISFR &= ~(1 << BOARD_INITPINS_MOT_B_SPEED_B_PIN);
  }
#else
  PORTA->ISFR &= ~(1 << BOARD_INITPINS_MOT_B_SPEED_A_PIN);
  PORTA->ISFR &= ~(1 << BOARD_INITPINS_MOT_B_SPEED_B_PIN);
#endif

}

#if defined (__cplusplus)
}
#endif
