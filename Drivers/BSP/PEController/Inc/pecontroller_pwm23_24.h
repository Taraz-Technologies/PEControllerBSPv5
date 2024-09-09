/**
 ********************************************************************************
 * @file 		pecontroller_pwm11_16.h
 * @author 		Ahsan Waheed Abbasi
 * @date 		September 05, 2024
 *
 * @brief	Controls the PWM 23-24
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
#ifndef PWM_PECONTROLLER_PWM23_24_H_
#define PWM_PECONTROLLER_PWM23_24_H_

#ifdef __cplusplus
extern "C" {
#endif
/** @addtogroup BSP
 * @{
 */

/** @addtogroup PWM
 * @{
 */

/** @defgroup PWM23-24 PWM 23-24
 * @brief Contains the declaration and procedures for the PWMs (23-24)
 * @details  This module has a single time base, so all PWMs under this module will operate on the same frequency.
 * 	This module can only be configured with one channel configurations. <br>The channel 24 can
 * 	only be used as inverted PWM pair of 23 and can 't be used separately
 * 		-# <b>PWM (23,24):</b> TIM16_CH1<br>
 *
 * The PWMs can either configured for individual or paired operation <br>
 * - <b>Individual Channels:</b><br>
 * For configuring the channels use @ref BSP_PWM23_24_ConfigChannels(),
 * whereas for updating the duty cycle of the output PWM use @ref BSP_PWM23_24_UpdateChannelDuty().<br>
 * - <b>Inverted Pair Channels:</b><br>
 * For configuring the channels use @ref BSP_PWM23_24_ConfigInvertedPairs(),
 * whereas for updating the duty cycle of the output PWM use @ref BSP_PWM23_24_UpdatePairDuty().<br>
 * @{
 */
/********************************************************************************
 * Includes
 *******************************************************************************/
#include "pecontroller_pwm_base.h"
/********************************************************************************
 * Defines
 *******************************************************************************/
#define TIM16_FREQ_Hz					(240000000)
/********************************************************************************
 * Typedefs
 *******************************************************************************/

/********************************************************************************
 * Structures
 *******************************************************************************/

/********************************************************************************
 * Exported Variables
 *******************************************************************************/
/** @defgroup PWM23_24_Exported_Variables Variables
  * @{
  */
/** Timer 16 handle
 */
extern TIM_HandleTypeDef htim16;
/**
 * @}
 */
/********************************************************************************
 * Global Function Prototypes
 *******************************************************************************/
/** @defgroup PWM23_24_Exported_Functions Functions
  * @{
  */
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
extern DutyCycleUpdateFnc BSP_PWM23_24_ConfigInvertedPairs(uint32_t pwmNo, pwm_config_t* config, int pairCount);
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
extern float BSP_PWM23_24_UpdatePairDuty(uint32_t pwmNo, float duty, pwm_config_t* config);
/**
 * @brief Configures consecutive PWM channels
 * @param pwmNo Channel no of the first PWM Channel in the pair (Valid Values 23-24)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channels
 * @param chCount No of channels to be configured with the setting. Max supported value is 2. The value should be counted while skipping even channels.
 * 					e.g. if pwmNo = 23 and chCount = 2 the PWMs 23 and PWM24 will be configured with the specified setting while PWM24 will be the inverted version of PWM23
 * @return DutyCycleUpdateFnc Returns the function pointer of the type DutyCycleUpdateFnc which needs to be called
 * 						  whenever the duty cycles of the pair need to be updated
 */
extern DutyCycleUpdateFnc BSP_PWM23_24_ConfigChannels(uint32_t pwmNo, pwm_config_t* config, int chCount);
/**
 * @brief Update the Duty Cycle of a channel
 * @param pwmNo PWM channel to be configured (Valid Values 23-24)
 * @param duty duty cycle to be applied to the channel (Range 0-1 or given in the config parameter)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channel
 * @return float Duty cycle applied in this cycle. May differ from the duty variable if outside permitted limits
 */
extern float BSP_PWM23_24_UpdateChannelDuty(uint32_t pwmNo, float duty, pwm_config_t* config);
/**
 * @brief Enable / Disable interrupt for a PWM channel as per requirement
 * @param enable If enable interrupt set this parameter to <c>true</c>
 * @param callback Specifies the function to be called when the PWM is reset
 * @param priority Interrupt priority. Range (0-15). Here 0 is the highest priority
 */
extern void BSP_PWM23_24_Config_Interrupt(bool enable, PWMResetCallback callback, int priority);
/********************************************************************************
 * Code
 *******************************************************************************/



/**
 * @}
 */
#ifdef __cplusplus
}
#endif
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#endif /* PWM_PECONTROLLER_PWM23_24_H_ */
/* EOF */
