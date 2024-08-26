/**
 ********************************************************************************
 * @file    	phase_shifted_full_bridge.c
 * @author 		Waqas Ehsan Butt
 * @date    	Aug 13, 2024
 *
 * @brief   File used for the control of phase shifted full bridge (PSFB).
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
 * @brief Update PWM duty cycles of relevant inverter leg pairs.
 * @param pwmNo Channel no of the first PWM Channel in the pair (Valid Values 1,3,5,7,9,11,13).
 * 				 Channel1 = pwmNo
 * 				 Channel2 = pwmNo + 1
 * 				 Channel3 = pwmNo + 2
 * 				 Channel3 = pwmNo + 3
 * @param duty duty cycle to be applied to the pair (Range 0 - 1)
 * @param *config Pointer to a  pwm_pair_config_t structure that contains the configuration
 * 				   parameters for the PWM pair.
 */
static float Tnpc_PWM_UpdatePair(uint32_t pwmNo, float duty, pwm_config_t *config)
{
	float d1 = duty;
	duty = (fabsf(duty - 0.5f)) * 2;
	if(d1 < .5f)
	{
		BSP_PWM_UpdatePairDuty(pwmNo, 0, config);
		BSP_PWM_UpdatePairDuty(pwmNo + 2, 1 - duty, config);
	}
	else
	{
		BSP_PWM_UpdatePairDuty(pwmNo, duty, config);
		BSP_PWM_UpdatePairDuty(pwmNo + 2, 1, config);
	}
	return d1;
}
/**
 * @brief Configure the PWMs for a single leg of the power module.
 * @param *config Pointer to the power module configurations.
 * @param pwmNo Channel no of the first switch of the leg.
 * @return DutyCycleUpdateFnc Returns the function pointer of the type PWMPairUpdateCallback which needs to be called
 * 						  whenever the duty cycles of the leg needs to be updated
 */
DutyCycleUpdateFnc PM_ConfigLeg(pm_config_t* config, uint16_t pwmNo)
{
	DutyCycleUpdateFnc callback = BSP_PWM_ConfigInvertedPair(pwmNo, &config->pwmConfig);
	BSP_Dout_SetAsPWMPin(pwmNo);
	BSP_Dout_SetAsPWMPin(pwmNo + 1);
	/* for TNPC use four switches */
	if (config->legType == LEG_TNPC)
	{
		BSP_PWM_ConfigInvertedPair(pwmNo + 2, &config->pwmConfig);
		callback = Tnpc_PWM_UpdatePair; 				/* use this function to update all 4 switch duty cycles */
		BSP_Dout_SetAsPWMPin(pwmNo + 2);
		BSP_Dout_SetAsPWMPin(pwmNo + 3);
	}
	return callback;
}

/**
 * @brief Enable/Disable the PWMs for a single leg of the power module.
 * @param *config Pointer to the power module configurations.
 * @param pwmNo Channel no of the first switch of the leg.
 * @param en True if needs to be enabled else false
 */
void PM_EnableLeg(pm_config_t* config, uint16_t pwmNo, bool en)
{
	BSP_PWMOut_Enable(((config->legType == LEG_TNPC ? 15U : 3U) << (pwmNo - 1)) , en);
}

/* EOF */