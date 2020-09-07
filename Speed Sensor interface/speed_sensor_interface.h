/*
 * speed_sensor_interface.h
 *
 *  Created on: Aug 28, 2020
 *      Author: Robert
 */

#ifndef SPEED_SENSOR_INTERFACE_H_
#define SPEED_SENSOR_INTERFACE_H_

#include <stdio.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_ftm.h"
#include "clock_config.h"
#include "MK64F12.h"

#include "./timer_int.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SPS_MOT_A 1
#define SPS_MOT_B 0


#define SPS_MOT_A_FORWARD(x) (!x)
#define SPS_MOT_B_FORWARD(x) (x)


// Speed Sensor class for a 2 sensor encoder with A and B
// For optimization purposses, to enable the sensors, change the macros SPS_MOT_x to 1 to enable the
class SpS {
  volatile uint32_t cnt_mot_a;
  volatile uint32_t rpc_a;
  volatile bool forward_a;

  volatile uint32_t cnt_mot_b;
  volatile uint32_t rpc_b;
  volatile bool forward_b;

  SpS(uint16_t second_divider);
 
 public:
  static SpS *instance;
  
  // SpS is not copyable
  SpS(const SpS&) = delete;
  SpS& operator=(const SpS&) = delete;

  static SpS* get_instance();
  static SpS* get_instance(uint16_t second_divider);

  uint32_t get_rpc_a() { return rpc_a; }
  bool get_forward_a() { return forward_a; }

  uint32_t get_rpc_b() { return rpc_b; }
  bool get_forward_b() { return forward_b; }

// only called on the rising edge, if called on the falling edge, the direction will be negated
#if SPS_MOT_A
  void sensor_triggered_mot_a(bool speed, bool other_sensor);
#endif

#if SPS_MOT_B
  void sensor_triggered_mot_b(bool speed, bool other_sensor);
#endif

  static void timer_overflowed();
};

#endif /* SPEED_SENSOR_INTERFACE_H_ */
