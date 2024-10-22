/**
 * @file xf_fal.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-18
 * 
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 * 
 */

#ifndef __XF_FAL_H__
#define __XF_FAL_H__

/* ==================== [Includes] ========================================== */

#include "xf_fal_config_internal.h"
#include "xf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_FAL_SW_VERSION                 "1.0.0"

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_fal_flash_dev_t
{
    char name[XF_FAL_DEV_NAME_MAX];
    uint32_t addr;
    size_t len;
    size_t blk_size;
    struct
    {
        int (*init)(void);
        int (*read)(long offset, uint8_t *buf, size_t size);
        int (*write)(long offset, const uint8_t *buf, size_t size);
        int (*erase)(long offset, size_t size);
    } ops;
    size_t write_gran;
}xf_fal_flash_dev_t;

typedef struct _xf_fal_partition_t
{
    char name[XF_FAL_DEV_NAME_MAX];
    char flash_name[XF_FAL_DEV_NAME_MAX];
    long offset;
    size_t len;
}xf_fal_partition_t;

/* ==================== [Global Prototypes] ================================= */

xf_err_t xf_fal_init(void);
const xf_fal_flash_dev_t *xf_fal_flash_device_find(const char *name);
const xf_fal_partition_t *xf_fal_partition_find(const char *name);
const xf_fal_partition_t *xf_fal_get_partition_table(size_t *len);
xf_err_t xf_fal_set_partition_table_temp(xf_fal_partition_t *table, size_t len);
xf_err_t xf_fal_partition_read(const xf_fal_partition_t *part, uint32_t addr, uint8_t *buf, size_t size);
xf_err_t xf_fal_partition_write(const xf_fal_partition_t *part, uint32_t addr, const uint8_t *buf, size_t size);
xf_err_t xf_fal_partition_erase(const xf_fal_partition_t *part, uint32_t addr, size_t size);
xf_err_t xf_fal_partition_erase_all(const xf_fal_partition_t *part);
void xf_fal_show_part_table(void);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_FAL_H__
