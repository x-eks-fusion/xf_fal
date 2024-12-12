/**
 * @file mock_flash.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2024-12-10
 *
 * Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __MOCK_FLASH_H__
#define __MOCK_FLASH_H__

/* ==================== [Includes] ========================================== */

#include "xf_fal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define MOCK_FLASH_NAME                 "mock_flash"
#define MOCK_FLASH_START_ADDR           (0)
#define MOCK_FLASH_LEN                  (8 * 1024 * 1024)
#define MOCK_FLASH_SECTOR_SIZE          (4 * 1024)
#define MOCK_FLASH_IO_SIZE              (1)

#define MOCK_FLASH_PART_TABLE                                           \
    {                                                                   \
        {"bl",          MOCK_FLASH_NAME,   0,          64        },     \
        {"app",         MOCK_FLASH_NAME,   64,         1024 * 10 },     \
        {"easyflash",   MOCK_FLASH_NAME,   1024 * 10,  1024 * 20 },     \
        {"download",    MOCK_FLASH_NAME,   1024 * 20,  1024 * 40 },     \
    }

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

void mock_flash_register_to_xf_fal(void);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __MOCK_FLASH_H__
