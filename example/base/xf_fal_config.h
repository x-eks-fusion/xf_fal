/**
 * @file xf_fal_config.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2024-12-10
 *
 * Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_FAL_CONFIG_H__
#define __XF_FAL_CONFIG_H__

/* ==================== [Includes] ========================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_FAL_LOCK_DISABLE 0
#define XF_FAL_FLASH_DEVICE_NUM 4
#define XF_FAL_PARTITION_TABLE_NUM 4
#define XF_FAL_DEV_NAME_MAX 24
#define XF_FAL_CACHE_NUM 16
#define XF_FAL_DEFAULT_FLASH_DEVICE_NAME    "chip_flash"
#define XF_FAL_DEFAULT_PARTITION_NAME       "storage"
#define XF_FAL_DEFAULT_PARTITION_OFFSET     0
#define XF_FAL_DEFAULT_PARTITION_LENGTH     4096

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_FAL_CONFIG_H__
