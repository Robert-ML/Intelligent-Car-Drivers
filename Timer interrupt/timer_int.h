/*
 * timer_int.h
 *
 *  Created on: Aug 28, 2020
 *      Author: Robert
 */

#ifndef TIMER_INT_H_
#define TIMER_INT_H_

#include <stdio.h>
#include <vector>
#include "fsl_debug_console.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_ftm.h"
#include "clock_config.h"
#include "MK64F12.h"

/* The Flextimer instance/channel used for board */
#define TIMER_INT_FTM_BASEADDR FTM1
/* Interrupt number and interrupt handler for the FTM instance used */
#define TIMER_INT_FTM_IRQ_NUM FTM1_IRQn
#define TIMER_INT_FTM_HANDLER FTM1_IRQHandler

#define TIMER_INT_FTM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_McgFixedFreqClk))

// THe timer starts automaticly after you create the first instance
class FTM1_Timer_int {
  std::vector<void (*)()> irq_handlers;
  uint16_t second_divider;
  volatile uint32_t cfs;

  FTM1_Timer_int(uint16_t second_divider);

 public:
  static FTM1_Timer_int *instance;

  // FTM1_Timer_int is not copyable
  FTM1_Timer_int(const FTM1_Timer_int&) = delete;
  FTM1_Timer_int& operator=(const FTM1_Timer_int&) = delete;

  static FTM1_Timer_int* get_instance();
  static FTM1_Timer_int* get_instance(uint16_t second_divider);

  void start_timer();
  void stop_timer();

  void add_irq_handler(void (*time_IRQn_handler)(void));
  void call_irq_handlers();

  uint32_t get_cfs() {return cfs; }
  uint32_t get_second_divider() { return second_divider; }
};



#endif /* TIMER_INT_H_ */
