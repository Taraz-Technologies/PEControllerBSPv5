/**
 ********************************************************************************
 * @file    	open_loop_vf_controller.c
 * @author 		Waqas Ehsan Butt
 * @date    	December 7, 2021
 *
 * @brief   
 ********************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 Taraz Technologies Pvt. Ltd.</center></h2>
 * <h3><center>All rights reserved.</center></h3>
 *
 * <center>This software component is licensed by Taraz Technologies under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the License. You may obtain 
 * a copy of the License at:
 *                        www.opensource.org/licenses/BSD-3-Clause</center>
 *
 ********************************************************************************
 */
#pragma GCC push_options
#pragma GCC optimize ("-Ofast")
/********************************************************************************
 * Includes
 *******************************************************************************/
#include "user_config.h"
#include "open_loop_vf_controller.h"
#include "shared_memory.h"
#include "pecontroller_timers.h"
/********************************************************************************
 * Defines
 *******************************************************************************/
#define MIN_MAX_BALANCING_INVERTER		(false)
#define INVERTER_DUTY_MODE				OUTPUT_DUTY_MINUS_DEADTIME_AT_PWMH//OUTPUT_DUTY_AT_PWMH//OUTPUT_DUTY_MINUS_DEADTIME_AT_PWMH
/********************************************************************************
 * Typedefs
 *******************************************************************************/

/********************************************************************************
 * Structures
 *******************************************************************************/

/********************************************************************************
 * Static Variables
 *******************************************************************************/
static pwm_module_config_t inverterPWMModuleConfig =
{
		.alignment = CENTER_ALIGNED,
		.f = PWM_FREQ_Hz,
		.deadtime = {
				.on = true,
				.nanoSec = INVERTER_DEADTIME_ns,
		},
};
static tim_in_trigger_config_t timerTriggerIn =
{
		.src = TIM_TRG_SRC_TIM1,
		.type = TIM_TRGI_TYPE_RESET_AND_START
};
static tim_out_trigger_config_t timerTriggerOut =
{
		.type = TIM_TRGO_OUT_UPDATE
};
/********************************************************************************
 * Global Variables
 *******************************************************************************/

/********************************************************************************
 * Function Prototypes
 *******************************************************************************/

/********************************************************************************
 * Code
 *******************************************************************************/
/**
 * @brief Initialize the grid tie controller
 * @param config Pointer to the configuration structure
 * @param pwmResetCallback Function callback issued after each PWM completion
 */
void OpenLoopVfControl_Init(openloopvf_config_t* config, PWMResetCallback pwmResetCallback)
{
	/***************** Configure Inverter *********************/
	inverter3Ph_config_t* inverterConfig = &config->inverterConfig;
	pm_config_t* pmConfig = &inverterConfig->pmConfig;
#if PECONTROLLER_CONFIG == PLB_TNPC
	pmConfig->legType = LEG_TNPC;
#else
	pmConfig->legType = LEG_DEFAULT;
#endif
	pmConfig->pwmConfig.lim.min = 0;
	pmConfig->pwmConfig.lim.max = 1;
	pmConfig->pwmConfig.lim.minMaxDutyCycleBalancing = MIN_MAX_BALANCING_INVERTER;
	pmConfig->pwmConfig.dutyMode = INVERTER_DUTY_MODE;
	pmConfig->pwmConfig.slaveOpts = &timerTriggerIn;
	pmConfig->pwmConfig.masterOpts = &timerTriggerOut;
	pmConfig->pwmConfig.module = &inverterPWMModuleConfig;
	Inverter3Ph_Init(inverterConfig);
	/***************** Configure Inverter *********************/

	/***************** Configure Control *********************/
	config->pwmFreq = PWM_FREQ_Hz;
	config->wt = 0;
	config->currentFreq = 0;
	/***************** Configure Control *********************/

	if(pwmResetCallback != NULL)
		BSP_PWM_Config_Interrupt(inverterConfig->s1PinNos[0], true, pwmResetCallback, 1);

	Inverter3Ph_Activate(inverterConfig, false);
}

static bool EnableDisableInverter_IfRequired(openloopvf_config_t* config, float a)
{
	// if inverter state change request in process
	if (config->requestedState != config->inverterConfig.pmConfig.state)
	{
		float f = config->currentFreq - a;
		// if frequency negligible update state
		if (f <= 0)
		{
			config->currentFreq = 0;
			Inverter3Ph_Activate(&config->inverterConfig, config->requestedState == POWER_MODULE_INACTIVE ? false : true);
		}
		else
			config->currentFreq = f;
		return true;
	}
	return false;
}

static bool ChangeInverterDirection_IfRequired(openloopvf_config_t* config, float a)
{
	// if direction changed
	if (config->currentDir != config->dir)
	{
		float f = config->currentFreq - a;
		// if frequency becomes smaller than 0 acknowledge direction change
		if (f <= 0)
		{
			config->currentFreq = 0;
			config->currentDir = config->dir;
		}
		else
			config->currentFreq = f;
		return true;
	}
	return false;
}

static void UpdateCurrentFrequency(openloopvf_config_t* config, float a)
{
	if (config->currentFreq > config->outputFreq)
	{
		config->currentFreq -= a;
		if(config->currentFreq < config->outputFreq)
			config->currentFreq = config->outputFreq;
	}
	else if (config->currentFreq < config->outputFreq)
	{
		config->currentFreq += a;
		if(config->currentFreq > config->outputFreq)
			config->currentFreq = config->outputFreq;
	}
}

/**
 * @brief This function computes new duty cycles for the inverter in each cycle
 * @param config Pointer to the inverter structure
 */
void OpenLoopVfControl_Loop(openloopvf_config_t* config)
{
	float a = config->acceleration / config->pwmFreq;

	EnableDisableInverter_IfRequired(config, a);

	if (config->inverterConfig.pmConfig.state == POWER_MODULE_INACTIVE)
		return;

	if (config->requestedState == config->inverterConfig.pmConfig.state)
	{
		if(ChangeInverterDirection_IfRequired(config, a) == false)
			UpdateCurrentFrequency(config, a);
	}

	// compute the current modulation index
	config->currentModulationIndex = (config->nominalModulationIndex / config->nominalFreq) * config->currentFreq;
	float stepSize = (TWO_PI * config->currentFreq) / config->pwmFreq;
	config->wt += stepSize;
	if(config->wt > TWO_PI)
		config->wt -= TWO_PI;

	// generate and apply SPWM according to the theta and modulation index
	Inverter3Ph_UpdateSPWM(&config->inverterConfig, config->wt, config->currentModulationIndex, config->currentDir);
}
/**
 * @brief Activate/Deactivate the inverter
 * @param config Pointer to the inverter structure
 * @param activate en <c>true</c> if needs to be enabled else <c>false</c>
 */
void OpenLoopVfControl_Activate(openloopvf_config_t* config, bool activate)
{
	Inverter3Ph_Activate(&config->inverterConfig, activate);
}

#pragma GCC pop_options
/* EOF */
