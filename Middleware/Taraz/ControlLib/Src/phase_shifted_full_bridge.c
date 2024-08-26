/**
 ********************************************************************************
 * @file    	phase_shifted_full_bridge.c
 * @author 		Waqas Ehsan Butt
 * @date    	Aug 20, 2024
 *
 * @brief   
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

/********************************************************************************
 * Includes
 *******************************************************************************/
#include "phase_shifted_full_bridge.h"
/********************************************************************************
 * Defines
 *******************************************************************************/

/********************************************************************************
 * Typedefs
 *******************************************************************************/

/********************************************************************************
 * Structures
 *******************************************************************************/

/********************************************************************************
 * Static Variables
 *******************************************************************************/

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
 * @brief Initialize the phase shifted full bridge.
 * @note Sets up all Duty cycles to 0.5. Disables all PWM outputs.
 * @param *config Pointer to the phase shifted full bridge configurations.
 */
void PSFB_Init(psfb_config_t* config)
{
	pm_config_t* pmConfig = &config->pmConfig;

	/* --TODO-- whole system should support error handling */
	/* an even output pin should be used as inverted signals are required for the PWM */
	if((config->s1PinNos[0] % 2 == 0) || (config->s1PinNos[1] % 2 == 0))
		return;

	/* can't support tnpc leg */
	if (pmConfig->legType == LEG_TNPC)
		return;

	// configure PWM
	for (int i = 0; i < 2; i++)
	{
		DutyCycleUpdateFnc cb = PM_ConfigLeg(pmConfig, config->s1PinNos[i]);
		cb(config->s1PinNos[i], 0.5f, &pmConfig->pwmConfig);
	}

	// Deactivate the inverter at startup
	PSFB_Activate(config, false);
}

/**
 * @brief Update the duty cycles of the inverter.
 * @param *config Pointer to the Inverter Configurations.
 * @param *duties pointer to the three duty cycles of the inverter (Range 0-1)
 */
void PSFB_UpdateDuty(psfb_config_t* config, float duty)
{
	// because half cycle on represents full duty cycle
	duty *= 0.5f;
	BSP_PWM_UpdatePhaseShift(config->s1PinNos[1], duty);
}

/**
 * @brief Activate/De-active the Phase Shifted Full Bridge
 * @param *config Pointer to the configuration
 * @param en <c>true</c> if needs to be enabled, else <c>false</c>.
 */
void PSFB_Activate(psfb_config_t* config, bool en)
{
	pm_config_t* pmConfig = &config->pmConfig;
	PM_EnableLeg(pmConfig, config->s1PinNos[0], en);
	PM_EnableLeg(pmConfig, config->s1PinNos[1], en);
	pmConfig->state = en ? POWER_MODULE_ACTIVE : POWER_MODULE_INACTIVE;
}


/* EOF */
