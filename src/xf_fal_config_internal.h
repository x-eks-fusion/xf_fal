/**
 * @file xf_fal_config_internal.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_fal 内部配置。
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_FAL_CONFIG_INTERNAL_H__
#define __XF_FAL_CONFIG_INTERNAL_H__

/* ==================== [Includes] ========================================== */

#include "xf_fal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* XF_FAL_ENABLE 无需对接 */

#if !defined(XF_FAL_LOCK_DISABLE) || (XF_FAL_LOCK_DISABLE)
#   define XF_FAL_LOCK_IS_ENABLE        0
#else
#   define XF_FAL_LOCK_IS_ENABLE        1
#endif

#ifndef XF_FAL_FLASH_DEVICE_NUM
#   define XF_FAL_FLASH_DEVICE_NUM      4
#endif

#ifndef XF_FAL_PARTITION_TABLE_NUM
#   define XF_FAL_PARTITION_TABLE_NUM   4
#endif

#ifndef XF_FAL_DEV_NAME_MAX
#   define XF_FAL_DEV_NAME_MAX          24
#endif

#ifndef XF_FAL_CACHE_NUM
#   define XF_FAL_CACHE_NUM             16
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_FAL_CONFIG_INTERNAL_H__
