/**
 ********************************************************************************
 * @file    	pecontroller_pwm23_24.c
 * @author 		Ahsan Waheed Abbasi
 * @date    	September 05, 2024
 *
 * @brief		Controls the PWM 23-24
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
#include "pecontroller_pwm23_24.h"
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
/** <c>true</c> if module previously initialized
 */
static bool pwm23_24_enabled = false;
/** Timer configurations
 */
static TIM_OC_InitTypeDef sConfigOC_16 =
{
		.OCMode = TIM_OCMODE_PWM1,
		.Pulse = 0,
		.OCPolarity = TIM_OCPOLARITY_HIGH,
		.OCNPolarity = TIM_OCNPOLARITY_HIGH,
		.OCFastMode = TIM_OCFAST_DISABLE,
		.OCIdleState = TIM_OCIDLESTATE_RESET,
		.OCNIdleState = TIM_OCIDLESTATE_RESET,  // --TODO-- Confirm if it should be set/reset
};
static float dutyDeadTime_16 = 0;
static bool isEdgeAligned_16;
static bool isDtEnabled_16;
/** keeps the callback function of all PWM module
 */
static PWMResetCallback resetCallback_16 = NULL;
/********************************************************************************
 * Global Variables
 *******************************************************************************/
/** Timer 16 handle
 */
TIM_HandleTypeDef htim16;
/********************************************************************************
 * Function Prototypes
 *******************************************************************************/

/********************************************************************************
 * Code
 *******************************************************************************/
/**
 * @brief Initialize the relevant PWM modules (Timer16). Frequency is constant for the PWMs 23-24
 * @param *config Pointer to a structure that contains the configuration
 * 				   parameters for the PWM pair
 */
static void PWM23_24_Drivers_Init(pwm_config_t* config)
{
	if(pwm23_24_enabled)
		return;
	__HAL_RCC_TIM16_CLK_ENABLE();
	htim16.Instance = TIM16;
	htim16.Init.Prescaler = 0;
	htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim16.Init.Period = (uint32_t)(TIM16_FREQ_Hz / config->module->f) - 1;
	htim16.Init.RepetitionCounter = 0;
	isEdgeAligned_16 = true;

	htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
		Error_Handler();
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim16, &sClockSourceConfig) != HAL_OK)
		Error_Handler();
	if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
		Error_Handler();

	// Deadtime settings
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	if (IsDeadtimeEnabled(&config->module->deadtime))
	{
		int valBase = (uint32_t)((config->module->deadtime.nanoSec) * (TIM16_FREQ_Hz / 1000000000.f));
		if (valBase < 128)
			sBreakDeadTimeConfig.DeadTime = valBase;
		else
		{
			int val = (int)(valBase / 2 - 64);
			if (val < 64)
				sBreakDeadTimeConfig.DeadTime = val | 0x80;
			else
			{
				val = (int)(valBase / 8 - 32);
				if (val < 32)
					sBreakDeadTimeConfig.DeadTime = val | 0xC0;
				else
				{
					val = (int)(valBase / 16 - 32);
					if (val < 32)
						sBreakDeadTimeConfig.DeadTime = val | 0xE0;
				}
			}
		}
		sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
		dutyDeadTime_16 = (config->module->deadtime.nanoSec * (config->module->f / 1000000000.f));
	}
	else
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = IsDeadtimeEnabled(&config->module->deadtime) ? TIM_AUTOMATICOUTPUT_ENABLE : TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
		Error_Handler();

	isDtEnabled_16 = IsDeadtimeEnabled(&config->module->deadtime);

	pwm23_24_enabled = true;
}
/**
 * @brief Update the Duty Cycle of an Inverted Pair
 * @param pwmNo Channel no of reference channel is the PWM pair (Valid Values 23-24). <br>
 * 				<b>Pairs are classified as :</b>
 * 				-# CH1 = Reference channel available at pin pwmNo
 * 				-# CH2 = Inverted Channel from reference available at pin pwmNo + 1 if pwmNo is odd else pwmNo - 1
 * @param duty duty cycle to be applied to the pair (Range 0-1 or given in the config parameter)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM pair
 * @return float Duty cycle applied in this cycle. May differ from the duty variable if outside permitted limits
 */
float BSP_PWM23_24_UpdatePairDuty(uint32_t pwmNo, float duty, pwm_config_t* config)
{
	/* check for duty cycle limits */
	duty = (duty > config->lim.max) ? config->lim.max : (duty < config->lim.min ? config->lim.min : duty);
	volatile uint16_t* CCRx = &TIM16->CCR1 + 0;  // Cache pointer to CCRx register

	if (duty == 0)
	{
		*CCRx = 0;
	}
	else
	{
		float dutyUse = duty;
		if (isDtEnabled_16 && config->dutyMode == OUTPUT_DUTY_AT_PWMH)
			dutyUse += dutyDeadTime_16;

		*CCRx = (isEdgeAligned_16 ? dutyUse : (1 - dutyUse)) * TIM16->ARR;
	}

	return duty;
}

/**
 * @brief Configures a single inverted pair for PWM
 * @param pwmNo Channel no of reference channel is the PWM pair (Valid Values 23-24). <br>
 * 				<b>Pairs are classified as :</b>
 * 				-# CH1 = Reference channel available at pin pwmNo
 * 				-# CH2 = Inverted Channel from reference available at pin pwmNo + 1 if pwmNo is odd else pwmNo - 1
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM pair
 */
static void PWM23_24_ConfigInvertedPair(uint32_t pwmNo, pwm_config_t* config)
{
	uint32_t ch = TIM_CHANNEL_1;
	uint32_t isCh2 = (pwmNo - 23) % 2;

	TIM_OC_InitTypeDef sConfigOCLocal;
	memcpy((void*)&sConfigOCLocal, (void*)&sConfigOC_16, sizeof(TIM_OC_InitTypeDef));

	sConfigOCLocal.OCMode = isCh2 ? TIM_OCMODE_PWM2 : TIM_OCMODE_PWM1;
	if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOCLocal, ch) != HAL_OK)
		Error_Handler();

	float oldMax = config->lim.max;
	config->lim.max = isDtEnabled_16 ? 1 - dutyDeadTime_16 : 1;
	if (oldMax < config->lim.max && oldMax != 0)
		config->lim.max = oldMax;

	if (config->lim.minMaxDutyCycleBalancing  && config->lim.max > .5f)
		config->lim.min = 1 - config->lim.max;
}
/**
 * @brief Configures consecutive inverted pairs for PWM
 * @param pwmNo Channel no of reference channel is the PWM pair (Valid Values 23-24). <br>
 * 				<b>Pairs are classified as :</b>
 * 				-# CH1 = Reference channel available at pin pwmNo
 * 				-# CH2 = Inverted Channel from reference available at pin pwmNo + 1 if pwmNo is odd else pwmNo - 1
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM pair
 * @param pairCount No of PWM pairs to be configured
 * @return DutyCycleUpdateFnc Returns the function pointer of the type DutyCycleUpdateFnc which needs to be called
 * 						  whenever the duty cycles of the pair need to be updated
 */
DutyCycleUpdateFnc BSP_PWM23_24_ConfigInvertedPairs(uint32_t pwmNo, pwm_config_t* config, int pairCount)
{
	if(!pwm23_24_enabled)
		PWM23_24_Drivers_Init(config);
	while (pairCount--)
	{
		PWM23_24_ConfigInvertedPair(pwmNo, config);
		pwmNo += 2;
	}
	return BSP_PWM23_24_UpdatePairDuty;
}

/**
 * @brief Update the Duty Cycle of a channel
 * @param pwmNo PWM channel to be configured (Valid Values 23-24)
 * @param duty duty cycle to be applied to the channel (Range 0-1 or given in the config parameter)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channel
 * @return float Duty cycle applied in this cycle. May differ from the duty variable if outside permitted limits
 */
float BSP_PWM23_24_UpdateChannelDuty(uint32_t pwmNo, float duty, pwm_config_t* config)
{
	/* check for duty cycle limits */
	// no need of minimum limit because it may cause confusion as timer is constant
	if (duty > config->lim.max)
		duty = config->lim.max;

	volatile uint16_t* CCR = &TIM16->CCR1 + 0;

	if (duty == 0)
		*CCR = 0;
	else
	{
		float dutyUse = duty;
		// always OUTPUT_DUTY_AT_PWMH MODE because dead time will be always added if due to common timer
		if (isDtEnabled_16)
		{
			dutyUse += dutyDeadTime_16;
		}
		*CCR = dutyUse * TIM16->ARR;
	}
	return duty;
}
/**
 * @brief Configures a single PWM channel
 * @param pwmNo Channel no of the PWM Channel in the pair (Valid Values 23-24)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channels
 */
static void PWM23_24_ConfigChannel(uint32_t pwmNo, pwm_config_t* config)
{
	uint32_t ch = TIM_CHANNEL_1;
	uint32_t isCh2 = (pwmNo - 23) % 2;

	TIM_OC_InitTypeDef sConfigOCLocal;
	memcpy((void*)&sConfigOCLocal, (void*)&sConfigOC_16, sizeof(TIM_OC_InitTypeDef));

	sConfigOCLocal.OCMode = isCh2 ? TIM_OCMODE_PWM2 : TIM_OCMODE_PWM1;  // Use PWM2 mode for specific pwmNo, otherwise PWM1

	if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOCLocal, ch) != HAL_OK)
		Error_Handler();
	float oldMax = config->lim.max;
	config->lim.max = isDtEnabled_16 ? (1 - dutyDeadTime_16) : 1;
	if (oldMax < config->lim.max && oldMax != 0)
		config->lim.max = oldMax;
}

/**
 * @brief Configures consecutive PWM channels
 * @param pwmNo Channel no of the first PWM Channel in the pair (Valid Values 23-24)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channels
 * @param chCount No of channels to be configured with the setting. Max supported value is 2. The value should be counted while skipping even channels.
 * 					e.g. if pwmNo = 23 and chCount = 2 the PWMs 23 and 24 will be configured with the specified setting while PWM24 will be the inverted version of PWM23
 * @return DutyCycleUpdateFnc Returns the function pointer of the type DutyCycleUpdateFnc which needs to be called
 * 						  whenever the duty cycles of the pair need to be updated
 */
DutyCycleUpdateFnc BSP_PWM23_24_ConfigChannels(uint32_t pwmNo, pwm_config_t* config, int chCount)
{
	if(!pwm23_24_enabled)
		PWM23_24_Drivers_Init(config);
	while (chCount--)
	{
		PWM23_24_ConfigChannel(pwmNo, config);
		pwmNo += 2;						// No need to configure the other
	}
	return BSP_PWM23_24_UpdateChannelDuty;
}

/**
 * @brief Enable / Disable interrupt for a PWM channel as per requirement
 * @param enable If enable interrupt set this parameter to <c>true</c>
 * @param callback Specifies the function to be called when the PWM is reset
 * @param priority Interrupt priority. Range (0-15). Here 0 is the highest priority
 */
void BSP_PWM23_24_Config_Interrupt(bool enable, PWMResetCallback callback, int priority)
{
	if (enable)
	{
		if (callback == NULL)
			return;
		resetCallback_16 = callback;
		__HAL_TIM_ENABLE_IT(&htim16, TIM_IT_UPDATE);
		HAL_NVIC_SetPriority(TIM16_IRQn, priority, 0);
		HAL_NVIC_EnableIRQ(TIM16_IRQn);
	}
	else
	{
		__HAL_TIM_DISABLE_IT(&htim16, TIM_IT_UPDATE);
		HAL_NVIC_DisableIRQ(TIM16_IRQn);
		resetCallback_16 = NULL;
	}
}

/**
 * @brief Timer16 update IRQ Handler to process Timer 16 interrupts
 */
void TIM16_IRQHandler(void)
{
	/* TIM Update event */
	if (__HAL_TIM_GET_FLAG(&htim16, TIM_FLAG_UPDATE) != RESET)
	{
		if (__HAL_TIM_GET_IT_SOURCE(&htim16, TIM_IT_UPDATE) != RESET)
		{
			resetCallback_16();
			__HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE);
		}
	}
}
/* EOF */
