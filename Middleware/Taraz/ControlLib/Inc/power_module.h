/**
 ********************************************************************************
 * @file 		power_module.h
 * @author 		Waqas Ehsan Butt
 * @date 		Aug 13, 2024
 *
 * @brief	Provides some basic configuration of the power modules control.
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
#ifndef POWER_MODULE_H
#define POWER_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Control_Library
 * @{
 */

/** @defgroup PowerModule Power Module
 * @brief Contains the declaration and procedures relating to a three phase inverter.
 * @details List of functions
 * 	-# <b>@ref PM_ConfigLeg() :</b> Configures a power module leg.
 * 	-# <b>@ref PM_EnableLeg() :</b> Enables a power module leg.
 * @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "general_header.h"
#include "pecontroller_pwm.h"
#include "pecontroller_digital_out.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Enums
 ******************************************************************************/
/** @defgroup PowerModule_Exported_Typedefs Type Definitions
  * @{
  */
/**
 * @brief Power module state
 */
typedef enum
{
	POWER_MODULE_INACTIVE,            /**< The power module state is inactive */
	POWER_MODULE_ACTIVE,              /**< The power module state is active */
} pm_state_t;
/**
 * @brief Definitions of Switch Legs
 */
typedef enum
{
	LEG_DEFAULT,            /**< Define the leg as a default leg */
	LEG_TNPC,               /**< Define the leg as a TNPC leg */
	LEG_RESERVED,			/**< Final Value Not Used */
} switch_leg_t;
/**
 * @}
 */
/*******************************************************************************
 * Structures
 ******************************************************************************/
/** @defgroup PowerModule_Exported_Structures Structures
  * @{
  */
/**
 * @brief Defines the power module configuration
 */
typedef struct
{
	pm_state_t state;							/**< @brief The power module state whether active or inactive */
	switch_leg_t legType;						/**< @brief Type of switch legs used for switching the power module */
	pwm_config_t pwmConfig;						/**< @brief The PWM configurations */
} pm_config_t;
/**
 * @}
 */
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/** @defgroup PowerModule_Exported_Functions Functions
  * @{
  */
/**
 * @brief Configure the PWMs for a single leg of the power module.
 * @param *config Pointer to the power module configurations.
 * @param pwmNo Channel no of the first switch of the leg.
 * @return DutyCycleUpdateFnc Returns the function pointer of the type PWMPairUpdateCallback which needs to be called
 * 						  whenever the duty cycles of the leg needs to be updated
 */
extern DutyCycleUpdateFnc PM_ConfigLeg(pm_config_t* config, uint16_t pwmNo);
/**
 * @brief Enable/Disable the PWMs for a single leg of the power module.
 * @param *config Pointer to the power module configurations.
 * @param pwmNo Channel no of the first switch of the leg.
 * @param en True if needs to be enabled else false
 */
extern void PM_EnableLeg(pm_config_t* config, uint16_t pwmNo, bool en);
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