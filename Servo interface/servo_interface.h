/*
 * servo_interface.h
 *
 *  Created on: Jul 30, 2020
 *      Author: Robert
 *
 * Warning: uses FTM2.
 * Requaierments: Activate PWM_SV1 and/or PWM_SV2 in the configuration files
 * 				  accordingly to the .txt file with configuration for
 * 				  peripherals (chose which one you want, or both) and set up
 * 				  the clock speed accordingly to the .png config file.
 */

#ifndef SERVO_INTERFACE_H_
#define SERVO_INTERFACE_H_

#include <stdio.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_ftm.h"
#include <cmath>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The Flextimer instance/channel used for board */
#define SV_FTM_BASEADDR FTM2
#define SV1_FTM_CHANNEL kFTM_Chnl_0
#define SV2_FTM_CHANNEL kFTM_Chnl_1

/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_McgFixedFreqClk)
#define sv_pwm_freq 50U

/* Constants */
#define SV_NOT_INIT 		  -100
#define SV_DEFAULT_POS 		  90
#define SV_MIN_ANG 			  0
#define SV_MID_ANG 			  SV_DEFAULT_POS
#define SV_MAX_ANG 			  180
#define SV_BASE_DC			  5      // 5% is the base dc in the 0 degree position
#define SV_DC_PROC_PER_DEGREE 0.0276 // 0.0276% of the dc is 1 degree of the angle
#define SV_ONE_PROC_IN_DEGREE 36.2   // 36.2 degrees per 1% of the dc

typedef enum _aviable_servos {
	noServo,
	fakeMG996R
} servo_model;
// fakeMG996R
#define fakeMG996R_min 23
#define fakeMG996R_max 117
#define fakeMG996R_unit_per_degree ((float) (fakeMG996R_max - fakeMG996R_min) / (SV_MAX_ANG - SV_MIN_ANG))

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
class Servo{

	static Servo *instance;

	servo_model sv1, sv2;
	uint8_t sv1_min, sv1_max;
	uint8_t sv2_min, sv2_max;
	float sv1_unit_per_degree, sv2_unit_per_degree;

	uint8_t sv1_angle = SV_NOT_INIT;
	uint8_t sv2_angle = SV_NOT_INIT;

	ftm_config_t ftm_info;
	ftm_pwm_mode_t pwm_mode;
	ftm_pwm_level_select_t pwm_level;
	ftm_chnl_pwm_signal_param_t param_svx_inn[2];

	Servo(servo_model sv1, servo_model sv2);

 public:
/*!
 *  Enables FTM2 and channels 0 and/or 1. Reinitialization of the FTM2 driver
 * will result in undefined behaviour.
 *
 *  The get_instance(bool sv1, bool sv2) function should be called first in order
 * to create the object and initialize the servo and the clocks.
 */
	static Servo* get_instance();
	static Servo* get_instance(servo_model sv1, servo_model sv2);

/*!
 * Angle should be from 0 to 180.
 */
	void set_angle(ftm_chnl_t sv, uint8_t angle);
	void set_dutycycle(ftm_chnl_t sv, uint8_t dc);
	void set_directly(ftm_chnl_t sv, uint8_t cnv);
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

#endif /* SERVO_INTERFACE_H_ */
