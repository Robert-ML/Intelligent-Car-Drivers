/*
 * motor_driver.h
 *
 *  Created on: Jul 11, 2020
 *      Author: Robert
 *
 * Warning: uses FTM0.
 * Usage: alocate the parameter "motors_instance" that is a pointer to the
 * 		  object of this class that should be singleton.
 * Requaierments: as stated in the configuration files (.txt for peripherals
 * 				  and .png for the clock speed).
 */

#ifndef MOTOR_INTERFACE_H_
#define MOTOR_INTERFACE_H_

#include <stdio.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_ftm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The Flextimer instance/channel used for board */
#define MD_FTM_BASEADDR FTM0
#define MA_IN1_FTM_CHANNEL kFTM_Chnl_0
#define MB_IN1_FTM_CHANNEL kFTM_Chnl_1
#define MB_IN2_FTM_CHANNEL kFTM_Chnl_2
#define MA_IN2_FTM_CHANNEL kFTM_Chnl_3

/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
#define mot_pwm_freq 4000U

/* Motor directions, if reversed set to 0 */
#define MA_direction 1
#define MB_direction 0

/* Constants */
#define SPEED_MAX 100
#define SPEED_MAX_REV -100 // the maximum reverse speed
#define ZERO_DC 0U // 0 dutycycle

/* Parameters */
#define SINGLETON 1 // 0 - not a  singleton class | 1 - singleton class
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
class Motors{
 private:
	static Motors *instance;

	// parameters for enabling and disabling the motors
	bool en_mot = false; // the enable pin is gneral for both motors
	bool d1_ma = true;
	bool d1_mb = true;

	int8_t speed = 0;

	ftm_config_t ftm_info;
	ftm_pwm_mode_t pwm_mode;
	ftm_pwm_level_select_t pwm_level;
	ftm_chnl_pwm_signal_param_t param_mx_inn[4];

	// parameters for MA_IN1
	uint8_t dutycycle_ma_in1;

	// parameters for MB_IN1
	uint8_t dutycycle_mb_in1;

	// parameters for MB_IN2
	uint8_t dutycycle_mb_in2;

	// parameters for MA_IN2
	uint8_t dutycycle_ma_in2;

	Motors();

 public:
/*!
 * FTM0 and channels 0, 1, 2 and 4 are enabled. Reinitialization of the FTM0 driver
 * will result in undefined behaviour.
 */
	static Motors* get_instance();

	void enable_motors(uint8_t en);
	void disable_outputs(uint8_t ma, uint8_t mb);
	void set_speed(int8_t speed);
	void stop_break();

 private:
	// some other helping functions
/*!
 *  in1 and in2 will be reversed inside the function if either macros
 * MA_direction or MB_direction are set to 0 in order to respect the
 * direction of the motors.
 */
	void set_dutycycle_from_speed(uint8_t ma_in1, uint8_t ma_in2, uint8_t mb_in1, uint8_t mb_in2);
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

#endif /* MOTOR_INTERFACE_H_ */

