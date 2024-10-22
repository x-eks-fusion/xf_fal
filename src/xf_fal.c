/**
 * @file xf_fal.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-10-18
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_fal.h"

/* ==================== [Defines] =========================================== */

#define TAG "FAL"

/* ==================== [Typedefs] ========================================== */

typedef struct _xf_part_flash_info_t {
    const xf_fal_flash_dev_t *flash_dev;
} xf_part_flash_info_t;

typedef struct _xf_fal_flash_obj_t {
    const xf_fal_flash_dev_t const **table;
    const size_t table_len;
} xf_fal_flash_obj_t;

typedef struct _xf_fal_partition_obj_t {
    xf_part_flash_info_t *cache;
    const xf_fal_partition_t *table;
    size_t table_len;
} xf_fal_partition_obj_t;

typedef struct _xf_fal_obj_t {
    uint8_t is_init;
    xf_fal_flash_obj_t      flash;
    xf_fal_partition_obj_t partition;
} xf_fal_obj_t;

/* ==================== [Static Prototypes] ================================= */

static const xf_fal_flash_dev_t *flash_device_find_by_part(const xf_fal_partition_t *part);
static xf_err_t check_and_update_part_cache(const xf_fal_partition_t *table, size_t len);

/* ==================== [Static Variables] ================================== */

static const xf_fal_flash_dev_t const *device_table[] = XF_FAL_FLASH_TABLE;
static const xf_fal_partition_t partition_table[] = XF_FAL_PART_TABLE;
static xf_part_flash_info_t part_flash_cache[sizeof(partition_table) / sizeof(partition_table[0])];

static xf_fal_obj_t g_fal_obj = {
    .is_init = 0,
    .flash.table = device_table,
    .flash.table_len = sizeof(device_table) / sizeof(device_table[0]),
    .partition.cache = part_flash_cache,
    .partition.table = partition_table,
    .partition.table_len = sizeof(partition_table) / sizeof(partition_table[0])
};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_fal_init(void)
{
    xf_err_t result;

    if (g_fal_obj.is_init) {
        return XF_ERR_INITED;
    }

    for (size_t i = 0; i < g_fal_obj.flash.table_len; i++) {
        const xf_fal_flash_dev_t *device_table = g_fal_obj.flash.table[i];
        int (*init)(void) = device_table->ops.init;
        if (init == NULL) {
            continue;
        }
        init();
        XF_LOGD(TAG, "Flash device | %*.*s | addr: 0x%08x | len: 0x%08x | blk_size: 0x%08x |initialized finish.",
                XF_FAL_DEV_NAME_MAX, XF_FAL_DEV_NAME_MAX, device_table->name, device_table->addr, (int)device_table->len,
                (int)device_table->blk_size);
    }

    result = check_and_update_part_cache(&g_fal_obj.partition.table[0], g_fal_obj.partition.table_len);

    if (result != XF_OK) {
        XF_LOGE(TAG, "partition init failed.");
        return result;
    }

    g_fal_obj.is_init = 1;
    return XF_OK;
}

const xf_fal_flash_dev_t *xf_fal_flash_device_find(const char *name)
{
    if (name == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < g_fal_obj.flash.table_len; i++) {
        const xf_fal_flash_dev_t *device_table = g_fal_obj.flash.table[i];
        if (!strncmp(name, device_table->name, XF_FAL_DEV_NAME_MAX)) {
            return device_table;
        }
    }

    return NULL;
}

const xf_fal_partition_t *xf_fal_partition_find(const char *name)
{
    if (!g_fal_obj.is_init || !name) {
        return NULL;
    }

    for (size_t i = 0; i < g_fal_obj.partition.table_len; i++) {
        if (!strcmp(name, g_fal_obj.partition.table[i].name)) {
            return &g_fal_obj.partition.table[i];
        }
    }

    return NULL;
}

const xf_fal_partition_t *xf_fal_get_partition_table(size_t *len)
{
    if (!g_fal_obj.is_init || !len) {
        return NULL;
    }

    *len = g_fal_obj.partition.table_len;

    return g_fal_obj.partition.table;
}

xf_err_t xf_fal_set_partition_table_temp(xf_fal_partition_t *table, size_t len)
{
    if (!g_fal_obj.is_init || !table || !len) {
        return XF_ERR_INVALID_ARG;
    }

    check_and_update_part_cache(table, len);
    g_fal_obj.partition.table = table;
    g_fal_obj.partition.table_len = len;

    return XF_OK;

}

xf_err_t xf_fal_partition_read(const xf_fal_partition_t *part, uint32_t addr, uint8_t *buf, size_t size)
{
    xf_err_t result = XF_OK;
    const xf_fal_flash_dev_t *flash_dev = NULL;

    if (!g_fal_obj.is_init || !part || !buf || !size) {
        return XF_ERR_INVALID_ARG;
    }
    if (addr + size > part->len) {
        XF_LOGE(TAG, "Partition read error! Partition(%s) address(0x%08x) out of bound(0x%08x).", part->name,
                (int)(addr + size),
                (int)part->len);
        return XF_ERR_INVALID_ARG;
    }
    flash_dev = flash_device_find_by_part(part);
    if (flash_dev == NULL) {
        XF_LOGE(TAG, "Partition read error! Do NOT found the flash device(%s).", part->flash_name);
        return XF_ERR_INVALID_ARG;
    }

    result = flash_dev->ops.read(addr, buf, size);
    if (result != XF_OK) {
        XF_LOGE(TAG, "Partition read error! Flash device(%s) read failed.", part->flash_name);
    }

    return result;
}

xf_err_t xf_fal_partition_write(const xf_fal_partition_t *part, uint32_t addr, const uint8_t *buf, size_t size)
{
    xf_err_t result = XF_OK;
    const xf_fal_flash_dev_t *flash_dev = NULL;

    if (!g_fal_obj.is_init || !part || !buf || !size) {
        return XF_ERR_INVALID_ARG;
    }
    if (addr + size > part->len) {
        XF_LOGE(TAG, "Partition write error! Partition(%s) address(0x%08x) out of bound(0x%08x).", part->name,
                (int)(addr + size),
                (int)part->len);
        return XF_ERR_INVALID_ARG;
    }
    flash_dev = flash_device_find_by_part(part);
    if (flash_dev == NULL) {
        XF_LOGE(TAG, "Partition write error! Do NOT found the flash device(%s).", part->flash_name);
        return XF_ERR_INVALID_ARG;
    }

    result = flash_dev->ops.write(addr, buf, size);
    if (result != XF_OK) {
        XF_LOGE(TAG, "Partition write error! Flash device(%s) write failed.", part->flash_name);
    }

    return result;
}

xf_err_t xf_fal_partition_erase(const xf_fal_partition_t *part, uint32_t addr, size_t size)
{
    xf_err_t result = XF_OK;
    const xf_fal_flash_dev_t *flash_dev = NULL;

    if (!g_fal_obj.is_init || !part || !size) {
        return XF_ERR_INVALID_ARG;
    }
    if (addr + size > part->len) {
        XF_LOGE(TAG, "Partition write error! Partition(%s) address(0x%08x) out of bound(0x%08x).", part->name,
                (int)(addr + size),
                (int)part->len);
        return XF_ERR_INVALID_ARG;
    }
    flash_dev = flash_device_find_by_part(part);
    if (flash_dev == NULL) {
        XF_LOGE(TAG, "Partition write error! Do NOT found the flash device(%s).", part->flash_name);
        return XF_ERR_INVALID_ARG;
    }

    result = flash_dev->ops.erase(addr, size);
    if (result != XF_OK) {
        XF_LOGE(TAG, "Partition write error! Flash device(%s) write failed.", part->flash_name);
    }

    return result;
}

xf_err_t xf_fal_partition_erase_all(const xf_fal_partition_t *part)
{
    return xf_fal_partition_erase(part, 0, part->len);
}

void xf_fal_show_part_table(void)
{
    char *item1 = "name", *item2 = "flash_dev";
    size_t i, part_name_max = strlen(item1), flash_dev_name_max = strlen(item2);

    const xf_fal_partition_t *part;
    if (g_fal_obj.partition.table_len == 0) {
        return;
    }

    for (i = 0; i < g_fal_obj.partition.table_len; i++) {
        part = &g_fal_obj.partition.table[i];
        if (part_name_max < strlen(part->name)) {
            part_name_max = strlen(part->name);
        }
        if (flash_dev_name_max < strlen(part->flash_name)) {
            flash_dev_name_max = strlen(part->flash_name);
        }
    }
    XF_LOGI(TAG, "==================== FAL partition table ===================");
    XF_LOGI(TAG, "| %-*.*s | %-*.*s |   offset   |    length  |", (int)part_name_max, XF_FAL_DEV_NAME_MAX, item1,
            (int)flash_dev_name_max, XF_FAL_DEV_NAME_MAX, item2);
    XF_LOGI(TAG, "-------------------------------------------------------------");
    for (i = 0; i < g_fal_obj.partition.table_len; i++) {
        part = &partition_table[i];
        XF_LOGI(TAG, "| %-*.*s | %-*.*s | 0x%08lx | 0x%08lx |", (int)part_name_max, XF_FAL_DEV_NAME_MAX, part->name,
                (int)flash_dev_name_max, XF_FAL_DEV_NAME_MAX, part->flash_name, part->offset, part->len);
    }
    XF_LOGI(TAG, "=============================================================");
}

/* ==================== [Static Functions] ================================== */

static const xf_fal_flash_dev_t *flash_device_find_by_part(const xf_fal_partition_t *part)
{
    if (part < g_fal_obj.partition.table || part > &g_fal_obj.partition.table[g_fal_obj.partition.table_len - 1]) {
        XF_LOGE(TAG, "Partition not found!");
        return NULL;
    };

    return g_fal_obj.partition.cache[part - g_fal_obj.partition.table].flash_dev;
}


static xf_err_t check_and_update_part_cache(const xf_fal_partition_t *table, size_t len)
{
    const xf_fal_flash_dev_t *flash_dev = NULL;

    for (size_t i = 0; i < len; i++) {
        flash_dev = xf_fal_flash_device_find(table[i].flash_name);
        if (flash_dev == NULL) {
            XF_LOGD(TAG, "Warning: Do NOT found the flash device(%s).", table[i].flash_name);
            continue;
        }

        if (table[i].offset >= (long)flash_dev->len) {
            XF_LOGE(TAG, "Initialize failed! Partition(%s) offset address(%ld) out of flash bound(<%d).",
                    table[i].name, table[i].offset, (int)flash_dev->len);;

            return XF_FAIL;
        }

        g_fal_obj.partition.cache[i].flash_dev = flash_dev;
    }

    return XF_OK;
}
