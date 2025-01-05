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

#define MOCK_FLASH1_NAME                "mock_flash1"
#define MOCK_FLASH1_START_ADDR          (0)
#define MOCK_FLASH1_LEN                 (4 * 1024 * 1024)
#define MOCK_FLASH1_SECTOR_SIZE         (4 * 1024)
#define MOCK_FLASH1_PAGE_SIZE           (256)
#define MOCK_FLASH1_IO_SIZE             (1)

#define MOCK_FLASH2_NAME                "mock_flash2"
#define MOCK_FLASH2_START_ADDR          (4 * 1024)
#define MOCK_FLASH2_LEN                 (8 * 1024 * 1024)
#define MOCK_FLASH2_SECTOR_SIZE         (4 * 1024)
#define MOCK_FLASH2_PAGE_SIZE           (256)
#define MOCK_FLASH2_IO_SIZE             (1)

#define MOCK_FLASH1_PART_TABLE1                                         \
    {                                                                   \
        {"bl1",         MOCK_FLASH1_NAME,   0,          64        },    \
        {"app1",        MOCK_FLASH1_NAME,   64,         1024 * 10 },    \
        {"easyflash1",  MOCK_FLASH1_NAME,   1024 * 10,  1024 * 20 },    \
        {"download1",   MOCK_FLASH1_NAME,   1024 * 20,  1024 * 40 },    \
    }

#define MOCK_FLASH1_PART_TABLE2                                         \
    {                                                                   \
        {"storage1",    MOCK_FLASH1_NAME,   1024 * 40,  1024 * 40 },    \
    }

#define MOCK_FLASH2_PART_TABLE                                          \
    {                                                                   \
        {"bl2",         MOCK_FLASH2_NAME,   0,          64        },   \
        {"app2",        MOCK_FLASH2_NAME,   64,         1024 * 10 },   \
        {"easyflash2",  MOCK_FLASH2_NAME,   1024 * 10,  1024 * 20 },   \
        {"download2",   MOCK_FLASH2_NAME,   1024 * 20,  1024 * 40 },   \
    }

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

void mock_flash_register_to_xf_fal(void);

void mock_flash1_register_to_xf_fal(void);
void mock_flash2_register_to_xf_fal(void);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __MOCK_FLASH_H__
