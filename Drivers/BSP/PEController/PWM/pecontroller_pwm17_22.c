/**
 ********************************************************************************
 * @file    	pecontroller_pwm17_22.c
 * @author 		Ahsan Waheed Abbasi
 * @date    	July 30, 2024
 *
 * @brief		Controls the PWM 17-22
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
#include "pecontroller_pwm17_22.h"
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
static bool pwm17_22_enabled = false;
/** Timer configurations
 */
static TIM_OC_InitTypeDef sConfigOC =
{
		.OCMode = TIM_OCMODE_PWM1,
		.Pulse = 0,
		.OCPolarity = TIM_OCPOLARITY_HIGH,
		.OCNPolarity = TIM_OCNPOLARITY_HIGH,
		.OCFastMode = TIM_OCFAST_DISABLE,
		.OCIdleState = TIM_OCIDLESTATE_RESET,
		.OCNIdleState = TIM_OCIDLESTATE_RESET,  // --TODO-- Confirm if it should be set/reset
};
static float dutyDeadTime = 0;
static bool isEdgeAligned;
static bool isDtEnabled;
/** keeps the callback function of all PWM module
 */
static PWMResetCallback resetCallback = NULL;
/********************************************************************************
 * Global Variables
 *******************************************************************************/
/** Timer 1 handle
 */
TIM_HandleTypeDef htim8;
/********************************************************************************
 * Function Prototypes
 *******************************************************************************/

/********************************************************************************
 * Code
 *******************************************************************************/
/**
 * @brief Initialize the relevant PWM modules (Timer8). Frequency is constant for the PWMs 17-22
 * @param *config Pointer to a structure that contains the configuration
 * 				   parameters for the PWM pair
 */
static void PWM17_22_Drivers_Init(pwm_config_t* config)
{
	if(pwm17_22_enabled)
		return;
	__HAL_RCC_TIM8_CLK_ENABLE();
	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 0;
	if(config->module->alignment == CENTER_ALIGNED)
	{
		htim8.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED2;
		htim8.Init.Period = (uint32_t)((TIM8_FREQ_Hz / 2.f) / config->module->f);
		htim8.Init.RepetitionCounter = 1;
		isEdgeAligned = false;
	}
	else
	{
		htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim8.Init.Period = (uint32_t)(TIM8_FREQ_Hz / config->module->f) - 1;
		htim8.Init.RepetitionCounter = 0;
		isEdgeAligned = true;
	}
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
		Error_Handler();
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
		Error_Handler();
	if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
		Error_Handler();

	// Trigger settings
	BSP_Timer_SetInputTrigger(&htim8, config->slaveOpts->src == TIM_TRG_SRC_TIM8 ? NULL : config->slaveOpts);
	BSP_Timer_SetOutputTrigger(&htim8, config->masterOpts);

	// Deadtime settings
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	if (IsDeadtimeEnabled(&config->module->deadtime))
	{
		int valBase = (uint32_t)((config->module->deadtime.nanoSec) * (TIM8_FREQ_Hz / 1000000000.f));
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
		dutyDeadTime = (config->module->deadtime.nanoSec * (config->module->f / 1000000000.f));
	}
	else
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = IsDeadtimeEnabled(&config->module->deadtime) ? TIM_AUTOMATICOUTPUT_ENABLE : TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
		Error_Handler();

	isDtEnabled = IsDeadtimeEnabled(&config->module->deadtime);

	pwm17_22_enabled = true;
}
/**
 * @brief Update the Duty Cycle of an Inverted Pair
 * @param pwmNo Channel no of reference channel is the PWM pair (Valid Values 17-22). <br>
 * 				<b>Pairs are classified as :</b>
 * 				-# CH1 = Reference channel available at pin pwmNo
 * 				-# CH2 = Inverted Channel from reference available at pin pwmNo + 1 if pwmNo is odd else pwmNo - 1
 * @param duty duty cycle to be applied to the pair (Range 0-1 or given in the config parameter)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM pair
 * @return float Duty cycle applied in this cycle. May differ from the duty variable if outside permitted limits
 */
float BSP_PWM17_22_UpdatePairDuty(uint32_t pwmNo, float duty, pwm_config_t* config)
{
	/* check for duty cycle limits */
	duty = (duty > config->lim.max) ? config->lim.max : (duty < config->lim.min ? config->lim.min : duty);

	uint32_t ch = (pwmNo - 17) / 2;
	volatile uint32_t* CCRx = &TIM8->CCR1 + ch;  // Cache pointer to CCRx register

	if (duty == 0)
	{
		*CCRx = isEdgeAligned ? 0 : TIM8->ARR;
	}
	else
	{
		float dutyUse = duty;
		if (isDtEnabled && config->dutyMode == OUTPUT_DUTY_AT_PWMH)
			dutyUse += dutyDeadTime;

		*CCRx = (isEdgeAligned ? dutyUse : (1 - dutyUse)) * TIM8->ARR;
	}

	return duty;
}

/**
 * @brief Configures a single inverted pair for PWM
 * @param pwmNo Channel no of reference channel is the PWM pair (Valid Values 17-22). <br>
 * 				<b>Pairs are classified as :</b>
 * 				-# CH1 = Reference channel available at pin pwmNo
 * 				-# CH2 = Inverted Channel from reference available at pin pwmNo + 1 if pwmNo is odd else pwmNo - 1
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM pair
 */
static void PWM17_22_ConfigInvertedPair(uint32_t pwmNo, pwm_config_t* config)
{
	uint32_t ch = (pwmNo - 17) / 2; -----TODO------
	uint32_t isCh2 = (pwmNo - 17) % 2;  -----TODO------

	ch = (ch == 0) ? TIM_CHANNEL_1 : (ch == 1) ? TIM_CHANNEL_2 : TIM_CHANNEL_3;

	TIM_OC_InitTypeDef sConfigOCLocal;
	memcpy((void*)&sConfigOCLocal, (void*)&sConfigOC, sizeof(TIM_OC_InitTypeDef));

	sConfigOCLocal.OCMode = (isCh2 ^ isEdgeAligned) ? TIM_OCMODE_PWM1 : TIM_OCMODE_PWM2;
	if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOCLocal, ch) != HAL_OK)
		Error_Handler();

	float oldMax = config->lim.max;
	config->lim.max = isDtEnabled ? 1 - dutyDeadTime : 1;
	if (oldMax < config->lim.max && oldMax != 0)
		config->lim.max = oldMax;

	if (config->lim.minMaxDutyCycleBalancing  && config->lim.max > .5f)
		config->lim.min = 1 - config->lim.max;
}
/**
 * @brief Configures consecutive inverted pairs for PWM
 * @param pwmNo Channel no of reference channel is the PWM pair (Valid Values 17-22). <br>
 * 				<b>Pairs are classified as :</b>
 * 				-# CH1 = Reference channel available at pin pwmNo
 * 				-# CH2 = Inverted Channel from reference available at pin pwmNo + 1 if pwmNo is odd else pwmNo - 1
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM pair
 * @param pairCount No of PWM pairs to be configured
 * @return DutyCycleUpdateFnc Returns the function pointer of the type DutyCycleUpdateFnc which needs to be called
 * 						  whenever the duty cycles of the pair need to be updated
 */
DutyCycleUpdateFnc BSP_PWM17_22_ConfigInvertedPairs(uint32_t pwmNo, pwm_config_t* config, int pairCount)
{
	if(!pwm17_22_enabled)
		PWM17_22_Drivers_Init(config);
	while (pairCount--)  
	{
		PWM17_22_ConfigInvertedPair(pwmNo, config);
		pwmNo += 2;
	}
	return BSP_PWM17_22_UpdatePairDuty;
}

/**
 * @brief Update the Duty Cycle of a channel
 * @param pwmNo PWM channel to be configured (Valid Values 17-22)
 * @param duty duty cycle to be applied to the channel (Range 0-1 or given in the config parameter)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channel
 * @return float Duty cycle applied in this cycle. May differ from the duty variable if outside permitted limits
 */
float BSP_PWM17_22_UpdateChannelDuty(uint32_t pwmNo, float duty, pwm_config_t* config)
{
	/* check for duty cycle limits */
	// no need of minimum limit because it may cause confusion as timer is constant
	if (duty > config->lim.max)
		duty = config->lim.max;

	uint32_t ch = (pwmNo - 17) / 2;
	volatile uint32_t* CCRx = &TIM8->CCR1 + ch;
	if (duty == 0)
		*CCRx = isEdgeAligned ? 0 : TIM1->ARR;
	else
	{
		float dutyUse = duty;
		// always OUTPUT_DUTY_AT_PWMH MODE because dead time will be always added if due to common timer
		if (isDtEnabled)
			dutyUse += dutyDeadTime;

		*CCRx = (isEdgeAligned ? dutyUse : (1 - dutyUse)) * TIM1->ARR;
	}
	return duty;
}
/**
 * @brief Configures a single PWM channel
 * @param pwmNo Channel no of the PWM Channel in the pair (Valid Values 17-22)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channels
 */
static void PWM17_22_ConfigChannel(uint32_t pwmNo, pwm_config_t* config)
{
	uint32_t ch = (pwmNo - 17) / 2;
	uint32_t isCh2 = (pwmNo - 17) % 2;

	ch = ch == 0 ? TIM_CHANNEL_1 : (ch == 1 ? TIM_CHANNEL_2 : TIM_CHANNEL_3);

	TIM_OC_InitTypeDef sConfigOCLocal;
	memcpy((void*)&sConfigOCLocal, (void*)&sConfigOC, sizeof(TIM_OC_InitTypeDef));

	sConfigOCLocal.OCMode = (isCh2 ^ isEdgeAligned) ? TIM_OCMODE_PWM1 : TIM_OCMODE_PWM2;
	if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOCLocal, ch) != HAL_OK)
		Error_Handler();

	float oldMax = config->lim.max;
	config->lim.max = isDtEnabled ? 1 - dutyDeadTime : 1;
	if (oldMax < config->lim.max && oldMax != 0)
		config->lim.max = oldMax;
}

/**
 * @brief Configures consecutive PWM channels
 * @param pwmNo Channel no of the first PWM Channel in the pair (Valid Values 17-22)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channels
 * @param chCount No of channels to be configured with the setting. Max supported value is 3. The value should be counted while skipping even channels.
 * 					e.g. if pwmNo = 17 and chCount = 2 the PWMs 17 and 18 will be configured with the specified setting while PWM18 will be the inverted version of PWM17
 * @return DutyCycleUpdateFnc Returns the function pointer of the type DutyCycleUpdateFnc which needs to be called
 * 						  whenever the duty cycles of the pair need to be updated
 */
DutyCycleUpdateFnc BSP_PWM17_22_ConfigChannels(uint32_t pwmNo, pwm_config_t* config, int chCount)
{
	if(!pwm17_22_enabled)
		PWM17_22_Drivers_Init(config);
	while (chCount--)
	{
		PWM17_22_ConfigChannel(pwmNo, config);
		pwmNo += 2;						// No need to configure the other
	}
	return BSP_PWM17_22_UpdateChannelDuty;
}

/**
 * @brief Enable / Disable interrupt for a PWM channel as per requirement
 * @param enable If enable interrupt set this parameter to <c>true</c>
 * @param callback Specifies the function to be called when the PWM is reset
 * @param priority Interrupt priority. Range (0-15). Here 0 is the highest priority
 */
void BSP_PWM17_22_Config_Interrupt(bool enable, PWMResetCallback callback, int priority)
{
	if (enable)
	{
		if (callback == NULL)
			return;
		resetCallback = callback;
		__HAL_TIM_ENABLE_IT(&htim8, TIM_IT_UPDATE);
		HAL_NVIC_SetPriority(TIM8_UP_IRQn, priority, 0);
		HAL_NVIC_EnableIRQ(TIM8_UP_IRQn);
	}
	else
	{
		__HAL_TIM_DISABLE_IT(&htim8, TIM_IT_UPDATE);
		HAL_NVIC_DisableIRQ(TIM8_UP_IRQn);
		resetCallback = NULL;
	}
}

/**
 * @brief Timer8 update IRQ Handler to process Timer 8 interrupts
 */
void TIM8_UP_IRQHandler(void)
{
	/* TIM Update event */
	if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_UPDATE) != RESET)
	{
		if (__HAL_TIM_GET_IT_SOURCE(&htim8, TIM_IT_UPDATE) != RESET)
		{
			resetCallback();
			__HAL_TIM_CLEAR_IT(&htim8, TIM_IT_UPDATE);
		}
	}
}
/* EOF */
