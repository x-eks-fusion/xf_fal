/**
 * @file mock_flash.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 
 * @version 1.0
 * @date 2024-12-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/* ==================== [Includes] ========================================== */

#include <stdio.h>
#include <string.h>

#include "xf_fal.h"
#include "mock_flash.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

void mock_flash_register_to_xf_fal(void)
{
    mock_flash1_register_to_xf_fal();
    mock_flash2_register_to_xf_fal();
}

/* ==================== [Static Functions] ================================== */
