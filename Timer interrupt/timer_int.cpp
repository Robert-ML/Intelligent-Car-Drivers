/*
 * timer_int.cpp
 *
 *  Created on: Aug 28, 2020
 *      Author: Robert
 */

#include "./timer_int.h"
FTM1_Timer_int* FTM1_Timer_int::instance = NULL;

FTM1_Timer_int* FTM1_Timer_int::get_instance() {
  if (instance == NULL) {
    instance = new FTM1_Timer_int(4); // default second divider is 4
  }
  return instance;
}

FTM1_Timer_int* FTM1_Timer_int::get_instance(uint16_t second_divider) {
  if (instance == NULL) {
    instance = new FTM1_Timer_int(second_divider);
  }
  return instance;
}

FTM1_Timer_int::FTM1_Timer_int(uint16_t second_divider) {
  ftm_config_t ftmInfo;
  FTM_GetDefaultConfig(&ftmInfo);
  ftmInfo.prescale = kFTM_Prescale_Divide_32; // the closest I could find to one second for a base frequency of 1.5 MHz
  /* Initialize FTM module */
  FTM_Init(TIMER_INT_FTM_BASEADDR, &ftmInfo);
  FTM_SetTimerPeriod(TIMER_INT_FTM_BASEADDR, (uint32_t) (TIMER_INT_FTM_SOURCE_CLOCK / 32) / second_divider);
  FTM_EnableInterrupts(TIMER_INT_FTM_BASEADDR, kFTM_TimeOverflowInterruptEnable);
  EnableIRQ(TIMER_INT_FTM_IRQ_NUM);
  this->start_timer();
}

void FTM1_Timer_int::start_timer() {
  FTM_StartTimer(TIMER_INT_FTM_BASEADDR, kFTM_FixedClock);
}

void FTM1_Timer_int::stop_timer() {
  FTM_StopTimer(TIMER_INT_FTM_BASEADDR);
}

void FTM1_Timer_int::add_irq_handler(void (*time_IRQn_handler)(void)) {
  irq_handlers.push_back(time_IRQn_handler);
}

void FTM1_Timer_int::call_irq_handlers() {
  ++cfs;
  for (auto& x : irq_handlers) {
    x();
  }
}

//*****************************************************************************
// The interrupt handler
#if defined (__cplusplus)
extern "C" {
#endif

void TIMER_INT_FTM_HANDLER() {
  FTM1_Timer_int::instance->call_irq_handlers();
  FTM_ClearStatusFlags(TIMER_INT_FTM_BASEADDR, kFTM_TimeOverflowFlag);
}

#if defined (__cplusplus)
}
#endif
