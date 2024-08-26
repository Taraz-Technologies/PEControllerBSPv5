/**
 ********************************************************************************
 * @file 		pecontroller_pwm11_16.h
 * @author 		Ahsan Waheed Abbasi
 * @date 		July 30, 2024
 *
 * @brief	Controls the PWM 17-22
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
#ifndef PECONTROLLER_PWM17_22_H
#define PECONTROLLER_PWM17_22_H

#ifdef __cplusplus
extern "C" {
#endif
/** @addtogroup BSP
 * @{
 */

/** @addtogroup PWM
 * @{
 */

/** @defgroup PWM17-22 PWM 17-22
 * @brief Contains the declaration and procedures for the PWMs (17-22)
 * @details  This module has a single time base, so all PWMs under this module will operate on the same frequency.
 * 	This module can only be configured with three channel configurations. <br>The channels 18, 20 and 22 can
 * 	only be used as inverted PWM pairs of 17, 19 and 21 respectively and can 't be used separately
 * 		-# <b>PWM (17,18):</b> TIM8_CH1
 * 		-# <b>PWM (19,20):</b> TIM8_CH2
 * 		-# <b>PWM (21,22):</b> TIM8_CH3<br>
 *
 * The PWMs can either configured for individual or paired operation <br>
 * - <b>Individual Channels:</b><br>
 * For configuring the channels use @ref BSP_PWM17_22_ConfigChannels(),
 * whereas for updating the duty cycle of the output PWM use @ref BSP_PWM17_22_UpdateChannelDuty().<br>
 * - <b>Inverted Pair Channels:</b><br>
 * For configuring the channels use @ref BSP_PWM17_22_ConfigInvertedPairs(),
 * whereas for updating the duty cycle of the output PWM use @ref BSP_PWM17_22_UpdatePairDuty().<br>
 * @{
 */
/********************************************************************************
 * Includes
 *******************************************************************************/
#include "pecontroller_pwm_base.h"
/********************************************************************************
 * Defines
 *******************************************************************************/
#define TIM8_FREQ_Hz					(240000000)
/********************************************************************************
 * Typedefs
 *******************************************************************************/

/********************************************************************************
 * Structures
 *******************************************************************************/

/********************************************************************************
 * Exported Variables
 *******************************************************************************/
/** @defgroup PWM17_22_Exported_Variables Variables
  * @{
  */
/** Timer 8 handle
 */
extern TIM_HandleTypeDef htim8;
/**
 * @}
 */
/********************************************************************************
 * Global Function Prototypes
 *******************************************************************************/
/** @defgroup PWM17_22_Exported_Functions Functions
  * @{
  */
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
extern DutyCycleUpdateFnc BSP_PWM17_22_ConfigInvertedPairs(uint32_t pwmNo, pwm_config_t* config, int pairCount);
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
extern float BSP_PWM17_22_UpdatePairDuty(uint32_t pwmNo, float duty, pwm_config_t* config);
/**
 * @brief Configures consecutive PWM channels
 * @param pwmNo Channel no of the first PWM Channel in the pair (Valid Values 17-22)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channels
 * @param chCount No of channels to be configured with the setting. Max supported value is 3. The value should be counted while skipping even channels.
 * 					e.g. if pwmNo = 17 and chCount = 2 the PWMs 17 and 19 will be configured with the specified setting while PWM18 will be the inverted version of PWM17
 * @return DutyCycleUpdateFnc Returns the function pointer of the type DutyCycleUpdateFnc which needs to be called
 * 						  whenever the duty cycles of the pair need to be updated
 */
extern DutyCycleUpdateFnc BSP_PWM17_22_ConfigChannels(uint32_t pwmNo, pwm_config_t* config, int chCount);
/**
 * @brief Update the Duty Cycle of a channel
 * @param pwmNo PWM channel to be configured (Valid Values 17-22)
 * @param duty duty cycle to be applied to the channel (Range 0-1 or given in the config parameter)
 * @param *config Pointer to a  pwm_config_t structure that contains the configuration
 * 				   parameters for the PWM channel
 * @return float Duty cycle applied in this cycle. May differ from the duty variable if outside permitted limits
 */
extern float BSP_PWM17_22_UpdateChannelDuty(uint32_t pwmNo, float duty, pwm_config_t* config);
/**
 * @brief Enable / Disable interrupt for a PWM channel as per requirement
 * @param enable If enable interrupt set this parameter to <c>true</c>
 * @param callback Specifies the function to be called when the PWM is reset
 * @param priority Interrupt priority. Range (0-15). Here 0 is the highest priority
 */
extern void BSP_PWM17_22_Config_Interrupt(bool enable, PWMResetCallback callback, int priority);
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

#endif 
/* EOF */
