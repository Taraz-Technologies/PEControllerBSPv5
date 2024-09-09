/**
 ********************************************************************************
 * @file    	inverter_3phase.c
 * @author 		Waqas Ehsan Butt
 * @date    	Nov 25, 2021
 *
 * @brief   File for control of three phase inverter
 ********************************************************************************
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
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "inverter_3phase.h"
#include "spwm.h"
#include "power_module.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/**
 * @brief Initialize the inverter module.
 * @note Sets up all Duty cycles to 0.5. Disables all PWM outputs.
 * @param *config Pointer to the Inverter Configurations.
 */
void Inverter3Ph_Init(inverter3Ph_config_t* config)
{
	/* an even output pin should be used as inverted signals are required for the PWM */
	if((config->s1PinNos[0] % 2 == 0) || (config->s1PinNos[1] % 2 == 0) || (config->s1PinNos[2] % 2 == 0))
		return;

	// configure PWM
	for (int i = 0; i < 3; i++)
	{
		config->updateCallbacks[i] = PM_ConfigLeg(&config->pmConfig, config->s1PinNos[i]);
		config->updateCallbacks[i](config->s1PinNos[i], 0.5f, &config->pmConfig.pwmConfig);
	}

	// if the duplicate pin is defined also initialize it
	if (config->s1PinDuplicate)
	{
		config->updateCallbackDuplicate = PM_ConfigLeg(&config->pmConfig, config->s1PinDuplicate);
		config->updateCallbackDuplicate(config->s1PinDuplicate, 0.5f, &config->pmConfig.pwmConfig);
	}

	// enable the pwm signals by disabling any disable feature. Disable is by default active high
	for (int i = 0; i < config->dsblPinCount; i++)
		BSP_Dout_SetAsIOPin(config->dsblPinNo + i, GPIO_PIN_RESET);

	// Deactivate the inverter at startup
	Inverter3Ph_Activate(config, false);
}

/**
 * @brief Update the duty cycles of the inverter.
 * @param *config Pointer to the Inverter Configurations.
 * @param *duties pointer to the three duty cycles of the inverter (Range 0-1)
 */
void Inverter3Ph_UpdateDuty(inverter3Ph_config_t* config, float* duties)
{
	for (int i = 0; i < 3; i++)
		config->updateCallbacks[i](config->s1PinNos[i], duties[i], &config->pmConfig.pwmConfig);

	// if the duplicate pin is defined also process it
	if (config->s1PinDuplicate)
		config->updateCallbackDuplicate(config->s1PinDuplicate, duties[2], &config->pmConfig.pwmConfig);
}

/**
 * @brief Update the duty cycles of the inverter by using SPWM configuration.
 * @param *config Pointer to the Inverter Configurations.
 * @param *theta angle of phase u in radians.
 * @param modulationIndex modulation index to be used for the generation.
 * @param dir Direction of the three phase signal.
 */
void Inverter3Ph_UpdateSPWM(inverter3Ph_config_t* config, float theta, float modulationIndex, bool dir)
{
	float duties[3];
	ComputeDuty_SPWM(theta, modulationIndex, duties, dir);
	Inverter3Ph_UpdateDuty(config, duties);
}

/**
 * @brief Activate/Deactive the 3-Phase inverter output.
 * @param *config Pointer to the Inverter Configurations.
 * @param en <c>true</c> if needs to be enabled, else <c>false</c>.
 */
void Inverter3Ph_Activate(inverter3Ph_config_t* config, bool en)
{
	for (int i = 0; i < 3; i++)
		PM_EnableLeg(&config->pmConfig, config->s1PinNos[i], en);
	if (config->s1PinDuplicate)
		PM_EnableLeg(&config->pmConfig, config->s1PinDuplicate, en);
	config->pmConfig.state = en ? POWER_MODULE_ACTIVE : POWER_MODULE_INACTIVE;
}

/* EOF */
