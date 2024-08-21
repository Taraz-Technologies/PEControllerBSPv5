/**
 ********************************************************************************
 * @file 		phase_shifted_full_bridge.h
 * @author 		Waqas Ehsan Butt
 * @date 		Aug 20, 2024
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

#ifndef TARAZ_CONTROLLIB_INC_PHASE_SHIFTED_FULL_BRIDGE_H_
#define TARAZ_CONTROLLIB_INC_PHASE_SHIFTED_FULL_BRIDGE_H_

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************
 * Includes
 *******************************************************************************/
#include "power_module.h"
/********************************************************************************
 * Defines
 *******************************************************************************/
/**
 *
 */

/********************************************************************************
 * Typedefs
 *******************************************************************************/
/**
 * @brief Defines the Phase-Shifted Full Bridge's parameters.
 */
typedef struct
{
	pm_config_t pmConfig;		/**< @brief Module configuration for the PSFB. */
	uint16_t s1PinNos[2];		/**< @brief Pin numbers for the first pin of each leg. S1 and S3 pins for the bridge. */
} psfb_config_t;
/********************************************************************************
 * Structures
 *******************************************************************************/

/********************************************************************************
 * Exported Variables
 *******************************************************************************/

/********************************************************************************
 * Global Function Prototypes
 *******************************************************************************/

/********************************************************************************
 * Code
 *******************************************************************************/
/**
 * @brief Initialize the phase shifted full bridge.
 * @note Sets up all Duty cycles to 0.5. Disables all PWM outputs.
 * @param *config Pointer to the phase shifted full bridge configurations.
 */
extern void PSFB_Init(psfb_config_t* config);

/**
 * @brief Update the duty cycles of the inverter.
 * @param *config Pointer to the Inverter Configurations.
 * @param *duties pointer to the three duty cycles of the inverter (Range 0-1)
 */
extern void PSFB_UpdateDuty(psfb_config_t* config, float duty);

/**
 * @brief Activate/De-active the Phase Shifted Full Bridge
 * @param *config Pointer to the configuration
 * @param en <c>true</c> if needs to be enabled, else <c>false</c>.
 */
extern void PSFB_Activate(psfb_config_t* config, bool en);

#ifdef __cplusplus
}
#endif

#endif 
/* EOF */
