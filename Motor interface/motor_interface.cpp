#include "./motor_interface.h"

Motors::Motors() {
	if (motors_instance != NULL) {
		return;
	}

	pwm_level = kFTM_HighTrue;

	// parameters for MA_IN1
	dutycycle_ma_in1 = 0U;
	param_mx_inn[0].chnlNumber 	          = MA_IN1_FTM_CHANNEL;
	param_mx_inn[0].level 				  = pwm_level;
	param_mx_inn[0].dutyCyclePercent      = dutycycle_ma_in1;
	param_mx_inn[0].firstEdgeDelayPercent = 0U;

	// parameters for MB_IN1
	dutycycle_mb_in1 = 0U;
	param_mx_inn[1].chnlNumber 		   	  = MB_IN1_FTM_CHANNEL;
	param_mx_inn[1].level 			      = pwm_level;
	param_mx_inn[1].dutyCyclePercent      = dutycycle_mb_in1;
	param_mx_inn[1].firstEdgeDelayPercent = 0U;

	// parameters for MB_IN2
	dutycycle_mb_in2 = 0U;
	param_mx_inn[2].chnlNumber   	      = MB_IN2_FTM_CHANNEL;
	param_mx_inn[2].level 				  = pwm_level;
	param_mx_inn[2].dutyCyclePercent      = dutycycle_mb_in2;
	param_mx_inn[2].firstEdgeDelayPercent = 0U;

	// parameters for MA_IN2
	dutycycle_ma_in2 = 0U;
	param_mx_inn[3].chnlNumber 		      = MA_IN2_FTM_CHANNEL;
	param_mx_inn[3].level 			      = pwm_level;
	param_mx_inn[3].dutyCyclePercent      = dutycycle_ma_in2;
	param_mx_inn[3].firstEdgeDelayPercent = 0U;

	FTM_GetDefaultConfig(&ftm_info);
	/* Initialize FTM module */
	FTM_Init(MD_FTM_BASEADDR, &ftm_info);

	FTM_SetupPwm(MD_FTM_BASEADDR, param_mx_inn, 4U, kFTM_CenterAlignedPwm, pwm_freq, FTM_SOURCE_CLOCK);

	FTM_StartTimer(MD_FTM_BASEADDR, kFTM_SystemClock);
}

/*!
 * param en = 1 - enable the drivers (the enable pin is common)
 * 			  0 - disable the drivers (sleep mode)
 */
void Motors::enable_motors(uint8_t en) {
	GPIO_PinWrite(BOARD_INITPINS_EN_MOT_GPIO, BOARD_INITPINS_EN_MOT_PIN, en);
}

/*!
 * param ma = 1 - disables the output of driver A and leaves the motor unconnected
 * 			  0 - connect the output of driver A
 * param mb = 1 - disables the output of driver B and leaves the motor unconnected
 * 			  0 - connect the output of driver B
 */
void Motors::disable_outputs(uint8_t ma, uint8_t mb) {
	GPIO_PinWrite(BOARD_INITPINS_D1_MA_GPIO, BOARD_INITPINS_D1_MA_PIN, ma);
	GPIO_PinWrite(BOARD_INITPINS_D1_MB_GPIO, BOARD_INITPINS_D1_MB_PIN, mb);
}

/*!
 * We won't use phase-lock motor driving dutycycle.
 *
 * param speed = the speed of the car (from -100 to 100), a negative value
 * 				 means the car is moving backwards
 *
 * The default direction is regarding to the skematik: for speed 100 we set
 * the in1 to high (100% dc) and in2 to low (0% dc) and for speed -100 in
 * reverse.
 */
void Motors::set_speed(int8_t speed) {
	if (speed < SPEED_MAX_REV) {
		speed = SPEED_MAX_REV;
	} else if (speed > SPEED_MAX) {
		speed = SPEED_MAX;
	}

	/* Disable channel output before updating the dutycycle */
	FTM_UpdateChnlEdgeLevelSelect(MD_FTM_BASEADDR, MA_IN1_FTM_CHANNEL, kFTM_NoPwmSignal);
	FTM_UpdateChnlEdgeLevelSelect(MD_FTM_BASEADDR, MB_IN1_FTM_CHANNEL, kFTM_NoPwmSignal);
	FTM_UpdateChnlEdgeLevelSelect(MD_FTM_BASEADDR, MB_IN2_FTM_CHANNEL, kFTM_NoPwmSignal);
	FTM_UpdateChnlEdgeLevelSelect(MD_FTM_BASEADDR, MA_IN2_FTM_CHANNEL, kFTM_NoPwmSignal);

	/* Update PWM duty cycle */
 #if MA_direction
	FTM_UpdatePwmDutycycle(MD_FTM_BASEADDR, MA_IN1_FTM_CHANNEL, kFTM_CenterAlignedPwm, speed);
	FTM_UpdatePwmDutycycle(MD_FTM_BASEADDR, MA_IN2_FTM_CHANNEL, kFTM_CenterAlignedPwm, ZERO_DC);
 #else
	FTM_UpdatePwmDutycycle(MD_FTM_BASEADDR, MA_IN1_FTM_CHANNEL, kFTM_CenterAlignedPwm, ZERO_DC);
	FTM_UpdatePwmDutycycle(MD_FTM_BASEADDR, MA_IN2_FTM_CHANNEL, kFTM_CenterAlignedPwm, speed);
 #endif

 #if MB_direction
	FTM_UpdatePwmDutycycle(MD_FTM_BASEADDR, MB_IN1_FTM_CHANNEL, kFTM_CenterAlignedPwm, speed);
	FTM_UpdatePwmDutycycle(MD_FTM_BASEADDR, MB_IN2_FTM_CHANNEL, kFTM_CenterAlignedPwm, ZERO_DC);
 #else
	FTM_UpdatePwmDutycycle(MD_FTM_BASEADDR, MB_IN1_FTM_CHANNEL, kFTM_CenterAlignedPwm, ZERO_DC);
	FTM_UpdatePwmDutycycle(MD_FTM_BASEADDR, MB_IN2_FTM_CHANNEL, kFTM_CenterAlignedPwm, speed);
 #endif

	/* Software trigger to update registers */
	FTM_SetSoftwareTrigger(MD_FTM_BASEADDR, true);

	/* Start channel output with updated dutycycle */
	FTM_UpdateChnlEdgeLevelSelect(MD_FTM_BASEADDR, MA_IN1_FTM_CHANNEL, pwm_level);
	FTM_UpdateChnlEdgeLevelSelect(MD_FTM_BASEADDR, MB_IN1_FTM_CHANNEL, pwm_level);
	FTM_UpdateChnlEdgeLevelSelect(MD_FTM_BASEADDR, MB_IN2_FTM_CHANNEL, pwm_level);
	FTM_UpdateChnlEdgeLevelSelect(MD_FTM_BASEADDR, MA_IN2_FTM_CHANNEL, pwm_level);
}






