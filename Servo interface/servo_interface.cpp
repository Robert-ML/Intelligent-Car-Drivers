#include "./servo_interface.h"

Servo* Servo::instance = NULL;

Servo* Servo::get_instance() {
	return instance;
}

Servo* Servo::get_instance(servo_model sv1, servo_model sv2) {
	if (instance == NULL) {
		instance = new Servo(sv1, sv2);
	}
	return instance;
}

Servo::Servo(servo_model sv1, servo_model sv2) {
	this->sv1 = sv1;
	if (sv1 == fakeMG996R) {
		this->sv1_min = fakeMG996R_min;
		this->sv1_max = fakeMG996R_max;
		this->sv1_unit_per_degree = fakeMG996R_unit_per_degree;
	}
	this->sv2 = sv2;
	if (sv2 == fakeMG996R) {
		this->sv2_min = fakeMG996R_min;
		this->sv2_max = fakeMG996R_max;
		this->sv2_unit_per_degree = fakeMG996R_unit_per_degree;
	}

	pwm_level = kFTM_HighTrue;
	pwm_mode = kFTM_EdgeAlignedPwm;

	uint8_t iterator = 0;
	if (sv1 != noServo) {
		sv1_angle = SV_DEFAULT_POS;
		param_svx_inn[iterator].chnlNumber 			  = SV1_FTM_CHANNEL;
		param_svx_inn[iterator].level 				  = pwm_level;
		param_svx_inn[iterator].dutyCyclePercent 	  = 0U;
		param_svx_inn[iterator].firstEdgeDelayPercent = 0U;
		++iterator;
	}
	if (sv2 != noServo) {
		sv2_angle = SV_DEFAULT_POS;
			param_svx_inn[iterator].chnlNumber 			  = SV2_FTM_CHANNEL;
			param_svx_inn[iterator].level 				  = pwm_level;
			param_svx_inn[iterator].dutyCyclePercent 	  = 0U;
			param_svx_inn[iterator].firstEdgeDelayPercent = 0U;
			++iterator;
	}

	int ret;

	FTM_GetDefaultConfig(&ftm_info);
	/* Initialize FTM module */
	ret = FTM_Init(SV_FTM_BASEADDR, &ftm_info);
	if (ret != kStatus_Success) {
		printf("Error in Servo: failed the initialization\n");
	}

	ret = FTM_SetupPwm(SV_FTM_BASEADDR, param_svx_inn, iterator, pwm_mode, sv_pwm_freq, FTM_SOURCE_CLOCK);
	if (ret != kStatus_Success) {
		printf("Error in Servo: failed to set the pwm\n");
	}

	FTM_StartTimer(SV_FTM_BASEADDR, kFTM_FixedClock);

	if (sv1 != noServo) {
		this->set_angle(SV1_FTM_CHANNEL, SV_DEFAULT_POS);
	}
	if (sv2 != noServo) {
		this->set_angle(SV2_FTM_CHANNEL, SV_DEFAULT_POS);
	}
}

void Servo::set_angle(ftm_chnl_t sv, uint8_t angle) {
	if (sv == SV1_FTM_CHANNEL && sv1 == noServo) return;
	if (sv == SV2_FTM_CHANNEL && sv2 == noServo) return;
	if (sv != SV1_FTM_CHANNEL && sv != SV2_FTM_CHANNEL) return;

//	servo_model sv_m;
	uint8_t sv_min;
//	uint8_t sv_max
	float sv_unit_per_degree;
	if (sv == SV1_FTM_CHANNEL) {
//		sv_m = sv1;
		sv_min = sv1_min;
//		sv_max = sv1_max;
		sv_unit_per_degree = sv1_unit_per_degree;
	} else if (sv == SV2_FTM_CHANNEL){
//		sv_m = sv2;
		sv_min = sv2_min;
//		sv_max = sv2_max;
		sv_unit_per_degree = sv2_unit_per_degree;
	}

	if (angle > SV_MAX_ANG) {
		angle = SV_MAX_ANG;
	}

	uint16_t cnv = std::round(sv_unit_per_degree * angle) + sv_min;
	PRINTF("Cnv: %d\n", cnv);
	if (sv == SV1_FTM_CHANNEL) {
		sv1_angle = angle;
		this->set_directly(SV1_FTM_CHANNEL, cnv);
	} else {
		sv2_angle = angle;
		this->set_directly(SV2_FTM_CHANNEL, cnv);
	}
}

void Servo::set_dutycycle(ftm_chnl_t sv, uint8_t dc) {
	FTM_UpdateChnlEdgeLevelSelect(SV_FTM_BASEADDR, sv, kFTM_NoPwmSignal);

	FTM_UpdatePwmDutycycle(SV_FTM_BASEADDR, SV1_FTM_CHANNEL, pwm_mode, dc);

	FTM_SetSoftwareTrigger(SV_FTM_BASEADDR, true);

	FTM_UpdateChnlEdgeLevelSelect(SV_FTM_BASEADDR, sv, pwm_level);
}

void Servo::set_directly(ftm_chnl_t sv, uint8_t cnv) {
	FTM_UpdateChnlEdgeLevelSelect(SV_FTM_BASEADDR, sv, kFTM_NoPwmSignal);

	SV_FTM_BASEADDR->CONTROLS[sv].CnV = cnv;

	FTM_SetSoftwareTrigger(SV_FTM_BASEADDR, true);

	FTM_UpdateChnlEdgeLevelSelect(SV_FTM_BASEADDR, sv, pwm_level);
}



