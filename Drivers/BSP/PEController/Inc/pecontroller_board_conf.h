/**
 ********************************************************************************
 * @file 		pecontroller_board_conf.h
 * @author 		Waqas Ehsan Butt
 * @date 		Jan 08, 2026
 *
 * @brief    Header file for the PEController Board Configuration and Selection
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

#ifndef PEONTROLLER_BOARD_CONF_H
#define PEONTROLLER_BOARD_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup BSP
 * @{
 */
/********************************************************************************
 * Includes
 *******************************************************************************/

/********************************************************************************
 * Defines
 *******************************************************************************/
/** @defgroup PEDISPLAY_Exported_Typedefs Typedefs
  * @{
  */
#define PECTRL_HW_REV            1   // or 2
#define PECTRL_HAS_QSPI_RAM      (PECTRL_HW_REV >= 2)

#define PECTRL_LCD_W             800
#define PECTRL_LCD_H             480
#define PECTRL_FB_BPP            16  // RGB565

#define PECTRL_EXT_RAM_BYTES     (1024*1024)  // v2 with IS66WVS1M8 = 1MB
#define PECTRL_FB_BYTES          (PECTRL_LCD_W*PECTRL_LCD_H*(PECTRL_FB_BPP/8))

#define PECTRL_USE_DOUBLE_FB     (PECTRL_HAS_QSPI_RAM && (PECTRL_EXT_RAM_BYTES >= (2*PECTRL_FB_BYTES)))
#define PECTRL_USE_DMA2D         1
/**
  * @}
  */
/********************************************************************************
 * Typedefs
 *******************************************************************************/

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

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif 
/* EOF */
