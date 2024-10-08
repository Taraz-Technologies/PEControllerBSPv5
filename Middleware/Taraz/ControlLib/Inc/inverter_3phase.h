
/**
 ********************************************************************************
 * @file 		inverter_3phase.h
 * @author 		Waqas Ehsan Butt
 * @date 		November 18, 2021
 *
 * @brief	Controls the three phase inverter functionality for different leg types
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
#ifndef INVERTER_3PH_H
#define INVERTER_3PH_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Control_Library
 * @{
 */

/** @defgroup Inverter3Ph 3 Phase Inverter
 * @brief Contains the declaration and procedures relating to a three phase inverter.
 * @details List of functions
 * 	-# <b>@ref Inverter3Ph_Init() :</b> Initialize an inverter module.
 * 	-# <b>@ref Inverter3Ph_UpdateSPWM() :</b> Update the duty cycles of the inverter by using SPWM configuration.
 * 	-# <b>@ref Inverter3Ph_UpdateDuty() :</b> Update the duty cycles of the inverter.
 * 	-# <b>@ref Inverter3Ph_Activate() :</b> Activate/Deactive the 3-Phase inverter output
 * @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "general_header.h"
#include "pecontroller_pwm.h"
#include "pecontroller_digital_out.h"
#include "power_module.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structures
 ******************************************************************************/
/** @defgroup INVERTER_3PH_Exported_Structures Structures
  * @{
  */
/**
 * @brief Defines the 3 Phase Inverter parameters
 */
typedef struct
{
	pm_config_t pmConfig;						/**< @brief Power module configuration */
	uint16_t s1PinNos[3];						/**< @brief Collection of the Pin numbers of the first PWM switch
													in a leg of the 3-Phase System. The remaining PWMs are the
													consecutive pins after this pin */
	uint16_t dsblPinNo;							/**< @brief Pin no of the first disable pin if any. If more than one
	 	 	 	 	 	 	 	 	 	 	 	 	 the remaining disable pins should be consecutively after this pin */
	uint16_t dsblPinCount;						/**< @brief No of disable pins used by the system.
													These pins will be used to disable the relevant gate drivers */
	DutyCycleUpdateFnc updateCallbacks[3];		/**< @brief These call backs are used by the drivers to update
													the duty cycles of all switches according to the configuration */
	uint16_t s1PinDuplicate;					/**< @brief The inverter can have an optional 4th leg which will get the same signals of 3rd leg.
														This is done to fully test all modules in different PELab Configurations.
														This value represents the first switch of the 4th leg. To disable duplication set this to 0 */
	DutyCycleUpdateFnc updateCallbackDuplicate; /**< @brief These call backs are used by the drivers to update
													the duty cycles of the duplicate leg according to the configuration */

} inverter3Ph_config_t;
/**
 * @}
 */
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/** @defgroup INVETER_3PH_Exported_Functions Functions
  * @{
  */
/**
 * @brief Initialize the inverter module.
 * @note Sets up all Duty cycles to 0.5. Disables all PWM outputs.
 * @param *config Pointer to the Inverter Configurations.
 */
extern void Inverter3Ph_Init(inverter3Ph_config_t* config);
/**
 * @brief Update the duty cycles of the inverter.
 * @param *config Pointer to the Inverter Configurations.
 * @param *duties pointer to the three duty cycles of the inverter (Range 0-1)
 */
extern void Inverter3Ph_UpdateDuty(inverter3Ph_config_t* config, float* duties);
/**
 * @brief Update the duty cycles of the inverter by using SPWM configuration.
 * @param *config Pointer to the Inverter Configurations.
 * @param *theta angle of phase u in radians.
 * @param modulationIndex modulation index to be used for the generation.
 * @param dir Direction of the three phase signal.
 */
extern void Inverter3Ph_UpdateSPWM(inverter3Ph_config_t* config, float theta, float modulationIndex, bool dir);
/**
 * @brief Activate/Deactive the 3-Phase inverter output.
 * @param *config Pointer to the Inverter Configurations.
 * @param en <c>true</c> if needs to be enabled, else <c>false</c>.
 */
extern void Inverter3Ph_Activate(inverter3Ph_config_t* config, bool en);
/*******************************************************************************
 * Code
 ******************************************************************************/

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
