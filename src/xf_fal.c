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

#include "xf_utils.h"
#include "xf_fal.h"

/* ==================== [Defines] =========================================== */

#define TAG "xf_fal"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/**
 * @brief 更新分区表中的分区与关联的 flash 设备的缓存。
 *
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_INVALID_PORT   未注册 xf_fal
 */
static xf_err_t check_and_update_cache(void);

/* ==================== [Static Variables] ================================== */

static xf_fal_ctx_t     s_fal_ctx = {0};
static xf_fal_ctx_t    *sp_fal_ctx = &s_fal_ctx;
#define sp_fal()        (sp_fal_ctx)

/* ==================== [Macros] ============================================ */

#define XF_FAL_CTX_MUTEX_TRY_INIT() \
    do { \
        if (NULL == sp_fal()->mutex) { \
            xf_lock_init(&sp_fal()->mutex); \
        } \
    } while (0)

#define XF_FAL_CTX_MUTEX_TRY_DEINIT() \
    do { \
        if (NULL != sp_fal()->mutex) { \
            xf_lock_destroy(sp_fal()->mutex); \
            sp_fal()->mutex = NULL; \
        } \
    } while (0)

#define XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(_ret) \
    do { \
        if (sp_fal()->mutex) { \
            if (XF_LOCK_FAIL == xf_lock_trylock(sp_fal()->mutex)) { \
                return _ret; \
            } \
            break; \
        } \
        if (true == sp_fal()->is_lock) { \
            return _ret; \
        } \
        sp_fal()->is_lock = true; \
    } while (0)

#define XF_FAL_CTX_TRYLOCK__ANYWAY() \
    do { \
        if (sp_fal()->mutex) { \
            xf_lock_trylock(sp_fal()->mutex); \
            break; \
        } \
        sp_fal()->is_lock = true; \
    } while (0)

#define XF_FAL_CTX_UNLOCK() \
    do { \
        if (sp_fal()->mutex) { \
            xf_lock_unlock(sp_fal()->mutex); \
            break; \
        } \
        sp_fal()->is_lock = false; \
    } while (0);

#if XF_FAL_LOCK_IS_ENABLE == 0
#undef XF_FAL_CTX_MUTEX_TRY_INIT
#undef XF_FAL_CTX_MUTEX_TRY_DEINIT
#undef XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE
#undef XF_FAL_CTX_TRYLOCK__ANYWAY
#undef XF_FAL_CTX_UNLOCK
#define XF_FAL_CTX_MUTEX_TRY_INIT()
#define XF_FAL_CTX_MUTEX_TRY_DEINIT()
#define XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(_ret)
#define XF_FAL_CTX_TRYLOCK__ANYWAY()
#define XF_FAL_CTX_UNLOCK()
#endif

/* ==================== [Global Functions] ================================== */

xf_err_t xf_fal_register_flash_device(const xf_fal_flash_dev_t *p_dev)
{
    xf_err_t xf_ret = XF_OK;
    int idle_idx;

    if (NULL == p_dev) {
        return XF_ERR_INVALID_ARG;
    }
    if ((NULL == p_dev->ops.read)
            || (NULL == p_dev->ops.write)
            || (NULL == p_dev->ops.erase)
       ) {
        return XF_ERR_INVALID_PORT;
    }
    if (xf_fal_check_register_state()) {
        return XF_FAIL;
    }

    XF_FAL_CTX_MUTEX_TRY_INIT();

    XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(XF_ERR_BUSY);

    /* TODO 未检查 flash 设备名是否重复 */
    idle_idx = -1;
    for (size_t i = 0; i < XF_FAL_FLASH_DEVICE_NUM; i++) {
        if (p_dev == sp_fal()->flash_device_table[i]) {
            xf_ret = XF_ERR_INITED;
            goto l_unlock_ret;
        }
        if ((NULL == sp_fal()->flash_device_table[i])
                && (-1 == idle_idx)) {
            idle_idx = i;
        }
    }
    if (idle_idx == -1) {
        xf_ret = XF_ERR_RESOURCE;
        goto l_unlock_ret;
    }

    sp_fal()->flash_device_table[idle_idx] = p_dev;

l_unlock_ret:;
    XF_FAL_CTX_UNLOCK();

    return xf_ret;
}

xf_err_t xf_fal_register_partition_table(
    const xf_fal_partition_t *p_table, size_t table_len)
{
    xf_err_t xf_ret = XF_OK;
    int idle_idx;

    if ((NULL == p_table)
            || ((0 == table_len))) {
        return XF_ERR_INVALID_ARG;
    }
    if (xf_fal_check_register_state()) {
        return XF_FAIL;
    }

    XF_FAL_CTX_MUTEX_TRY_INIT();

    XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(XF_ERR_BUSY);

    /* TODO 未检查分区名是否重复 */
    idle_idx = -1;
    for (size_t i = 0; i < XF_FAL_PARTITION_TABLE_NUM; i++) {
        if (p_table == sp_fal()->partition_table[i]) {
            xf_ret = XF_ERR_INITED;
            goto l_unlock_ret;
        }
        if ((NULL == sp_fal()->partition_table[i])
                && (-1 == idle_idx)) {
            idle_idx = i;
        }
    }
    if (idle_idx == -1) {
        xf_ret = XF_ERR_RESOURCE;
        goto l_unlock_ret;
    }

    sp_fal()->partition_table[idle_idx]     = p_table;
    sp_fal()->partition_table_len[idle_idx] = table_len;

l_unlock_ret:;
    XF_FAL_CTX_UNLOCK();

    return xf_ret;
}

xf_err_t xf_fal_unregister_flash_device(const xf_fal_flash_dev_t *p_dev)
{
    xf_err_t xf_ret = XF_OK;
    int dev_idx;

    if (NULL == p_dev) {
        return XF_ERR_INVALID_ARG;
    }
    if (xf_fal_check_register_state()) {
        return XF_FAIL;
    }

    XF_FAL_CTX_MUTEX_TRY_INIT();

    XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(XF_ERR_BUSY);

    dev_idx = -1;
    for (size_t i = 0; i < XF_FAL_FLASH_DEVICE_NUM; i++) {
        if ((p_dev == sp_fal()->flash_device_table[i])) {
            dev_idx = i;
            break;
        }
    }
    if (dev_idx == -1) {
        xf_ret = XF_ERR_NOT_FOUND;
        goto l_unlock_ret;
    }

    sp_fal()->flash_device_table[dev_idx] = NULL;

l_unlock_ret:;
    XF_FAL_CTX_UNLOCK();

    return xf_ret;
}

xf_err_t xf_fal_unregister_partition_table(const xf_fal_partition_t *p_table)
{
    xf_err_t xf_ret = XF_OK;
    int table_idx;

    if (NULL == p_table) {
        return XF_ERR_INVALID_ARG;
    }
    if (xf_fal_check_register_state()) {
        return XF_FAIL;
    }

    XF_FAL_CTX_MUTEX_TRY_INIT();

    XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(XF_ERR_BUSY);

    table_idx = -1;
    for (size_t i = 0; i < XF_FAL_PARTITION_TABLE_NUM; i++) {
        if ((p_table == sp_fal()->partition_table[i])) {
            table_idx = i;
        }
    }
    if (table_idx == -1) {
        xf_ret = XF_ERR_NOT_FOUND;
        goto l_unlock_ret;
    }

    sp_fal()->partition_table[table_idx]        = NULL;
    sp_fal()->partition_table_len[table_idx]    = 0;

l_unlock_ret:;
    XF_FAL_CTX_UNLOCK();

    return xf_ret;
}

bool xf_fal_check_register_state(void)
{
    if (0 == sp_fal()->cached_num) {
        return false;
    }
    return true;
}

const xf_fal_ctx_t *xf_fal_get_ctx(void)
{
    return (const xf_fal_ctx_t *)sp_fal();
}

xf_err_t xf_fal_init(void)
{
    xf_err_t xf_ret;
    const xf_fal_flash_dev_t *device_table;

    if (sp_fal()->is_init) {
        return XF_ERR_INITED;
    }

    for (size_t i = 0; i < XF_FAL_FLASH_DEVICE_NUM; i++) {
        device_table = sp_fal()->flash_device_table[i];
        if ((!device_table) || (!device_table->ops.init)) {
            continue;
        }
        device_table->ops.init();
        XF_LOGD(TAG, "Flash device | %*.*s | "
                "addr: 0x%08x | len: 0x%08x | sector_size: 0x%08x | "
                "initialized finish.",
                XF_FAL_DEV_NAME_MAX, XF_FAL_DEV_NAME_MAX, device_table->name,
                device_table->addr, (int)device_table->len, (int)device_table->sector_size);
    }

    xf_ret = check_and_update_cache();
    if (xf_ret != XF_OK) {
        XF_LOGE(TAG, "partition init failed.");
        return xf_ret;
    }

    sp_fal()->is_init = true;

    return XF_OK;
}

xf_err_t xf_fal_deinit(void)
{
    const xf_fal_flash_dev_t *device_table;

    if (!xf_fal_check_register_state()) {
        return XF_ERR_INVALID_PORT;
    }
    if (!sp_fal()->is_init) {
        return XF_ERR_UNINIT;
    }

    for (size_t i = 0; i < XF_FAL_FLASH_DEVICE_NUM; i++) {
        device_table = sp_fal()->flash_device_table[i];
        if ((!device_table) || (!device_table->ops.init)) {
            continue;
        }
        device_table->ops.deinit();
    }

    sp_fal()->cached_num    = 0;
    sp_fal()->is_init       = false;

    return XF_OK;
}

const xf_fal_flash_dev_t *xf_fal_flash_device_find(const char *name)
{
    const xf_fal_flash_dev_t *flash_device;
    size_t i;

    if (!name) {
        return NULL;
    }

    /* 暂无递归互斥锁，此处不加锁没太大问题 */

    for (i = 0; i < XF_FAL_FLASH_DEVICE_NUM; i++) {
        flash_device = sp_fal()->flash_device_table[i];
        if (!flash_device) {
            continue;
        }
        if (0 == strncmp(name, flash_device->name, XF_FAL_DEV_NAME_MAX)) {
            return flash_device;
        }
    }

    return NULL;
}

const xf_fal_flash_dev_t *xf_fal_flash_device_find_by_part(
    const xf_fal_partition_t *part)
{
    const xf_fal_flash_dev_t *flash_dev = NULL;
    size_t i;

    if (!xf_fal_check_register_state()) {
        XF_LOGE(TAG, "Not registered.");
        return NULL;
    }
    if (!part) {
        return NULL;
    }

    XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(NULL);

    for (i = 0; i < sp_fal()->cached_num; i++) {
        if (sp_fal()->cache[i].partition == part) {
            flash_dev = sp_fal()->cache[i].flash_dev;
            goto l_unlock_ret;
        }
    }

l_unlock_ret:;
    XF_FAL_CTX_UNLOCK();

    return flash_dev;
}

const xf_fal_partition_t *xf_fal_partition_find(const char *name)
{
    const xf_fal_partition_t *p_table;
    const xf_fal_partition_t *part = NULL;
    size_t table_len;
    size_t i;
    size_t j;

    if (!name) {
        return NULL;
    }

    XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(NULL);

    for (i = 0; i < XF_FAL_PARTITION_TABLE_NUM; i++) {
        p_table     = sp_fal()->partition_table[i];
        table_len   = sp_fal()->partition_table_len[i];
        if ((NULL == p_table) || (0 == table_len)) {
            continue;
        }
        for (j = 0; j < table_len; j++) {
            if (0 == strncmp(name, p_table[j].name, XF_FAL_DEV_NAME_MAX)) {
                part = &p_table[j];
                goto l_unlock_ret;
            }
        }
    }

l_unlock_ret:;
    XF_FAL_CTX_UNLOCK();

    return part;
}

xf_err_t xf_fal_partition_read(
    const xf_fal_partition_t *part,
    size_t src_offset, void *dst, size_t size)
{
    xf_err_t xf_ret = XF_OK;
    const xf_fal_flash_dev_t *flash_dev = NULL;

    if (!xf_fal_check_register_state()) {
        return XF_ERR_INVALID_PORT;
    }
    if (!sp_fal()->is_init) {
        return XF_ERR_UNINIT;
    }
    if (!part || !dst || !size) {
        return XF_ERR_INVALID_ARG;
    }
    if (src_offset + size > part->len) {
        XF_LOGE(TAG, "Partition read error! "
                "Partition(%s) address(0x%08x) out of bound(0x%08x).",
                part->name, (int)(src_offset + size), (int)part->len);
        return XF_ERR_INVALID_ARG;
    }

    flash_dev = xf_fal_flash_device_find_by_part(part);
    if (flash_dev == NULL) {
        XF_LOGE(TAG, "Partition read error! "
                "Do NOT found the flash device(%s).", part->flash_name);
        return XF_ERR_INVALID_ARG;
    }

    xf_ret = flash_dev->ops.read(part->offset + src_offset, dst, size);
    if (xf_ret != XF_OK) {
        XF_LOGE(TAG, "Partition read error! "
                "Flash device(%s) read failed.", part->flash_name);
    }

    return xf_ret;
}

xf_err_t xf_fal_partition_write(
    const xf_fal_partition_t *part,
    size_t dst_offset, const void *src, size_t size)
{
    xf_err_t xf_ret = XF_OK;
    const xf_fal_flash_dev_t *flash_dev = NULL;

    if (!xf_fal_check_register_state()) {
        return XF_ERR_INVALID_PORT;
    }
    if (!sp_fal()->is_init) {
        return XF_ERR_UNINIT;
    }
    if (!part || !src || !size) {
        return XF_ERR_INVALID_ARG;
    }
    if (dst_offset + size > part->len) {
        XF_LOGE(TAG, "Partition write error! "
                "Partition(%s) address(0x%08x) out of bound(0x%08x).",
                part->name, (int)(dst_offset + size), (int)part->len);
        return XF_ERR_INVALID_ARG;
    }

    flash_dev = xf_fal_flash_device_find_by_part(part);
    if (flash_dev == NULL) {
        XF_LOGE(TAG, "Partition write error! "
                "Do NOT found the flash device(%s).", part->flash_name);
        return XF_ERR_INVALID_ARG;
    }

    xf_ret = flash_dev->ops.write(part->offset + dst_offset, src, size);
    if (xf_ret != XF_OK) {
        XF_LOGE(TAG, "Partition write error! "
                "Flash device(%s) write failed.", part->flash_name);
    }

    return xf_ret;
}

xf_err_t xf_fal_partition_erase(
    const xf_fal_partition_t *part, size_t offset, size_t size)
{
    xf_err_t xf_ret = XF_OK;
    const xf_fal_flash_dev_t *flash_dev = NULL;

    if (!xf_fal_check_register_state()) {
        return XF_ERR_INVALID_PORT;
    }
    if (!sp_fal()->is_init) {
        return XF_ERR_UNINIT;
    }
    if (!part || !size) {
        return XF_ERR_INVALID_ARG;
    }
    if (offset + size > part->len) {
        XF_LOGE(TAG, "Partition write error! "
                "Partition(%s) address(0x%08x) out of bound(0x%08x).",
                part->name, (int)(offset + size), (int)part->len);
        return XF_ERR_INVALID_ARG;
    }

    flash_dev = xf_fal_flash_device_find_by_part(part);
    if (flash_dev == NULL) {
        XF_LOGE(TAG, "Partition write error! "
                "Do NOT found the flash device(%s).", part->flash_name);
        return XF_ERR_INVALID_ARG;
    }

    xf_ret = flash_dev->ops.erase(part->offset + offset, size);
    if (xf_ret != XF_OK) {
        XF_LOGE(TAG, "Partition write error! "
                "Flash device(%s) write failed.", part->flash_name);
    }

    return xf_ret;
}

xf_err_t xf_fal_partition_erase_all(const xf_fal_partition_t *part)
{
    return xf_fal_partition_erase(part, 0, part->len);
}

void xf_fal_show_part_table(void)
{
    const xf_fal_partition_t *p_table;
    size_t table_len;
    size_t i;
    size_t j;
    char *item1                 = "name";
    char *item2                 = "flash_dev";
    const xf_fal_partition_t *part;

    if (!xf_fal_check_register_state()) {
        return;
    }

    XF_LOGI(TAG, "==================== FAL partition table ===================");
    XF_LOGI(TAG, "| %-*.*s | %-*.*s |   offset   |    length  |",
            (int)XF_FAL_DEV_NAME_MAX, XF_FAL_DEV_NAME_MAX, item1,
            (int)XF_FAL_DEV_NAME_MAX, XF_FAL_DEV_NAME_MAX, item2);
    XF_LOGI(TAG, "-------------------------------------------------------------");
    for (i = 0; i < XF_FAL_PARTITION_TABLE_NUM; i++) {
        p_table     = sp_fal()->partition_table[i];
        table_len   = sp_fal()->partition_table_len[i];
        if ((NULL == p_table) || (0 == table_len)) {
            continue;
        }
        for (j = 0; j < table_len; j++) {
            part = &p_table[j];
            XF_LOGI(TAG, "| %-*.*s | %-*.*s | 0x%08lx | 0x%08lx |",
                    (int)XF_FAL_DEV_NAME_MAX, XF_FAL_DEV_NAME_MAX, part->name,
                    (int)XF_FAL_DEV_NAME_MAX, XF_FAL_DEV_NAME_MAX, part->flash_name,
                    part->offset, part->len);
        }
    }
    XF_LOGI(TAG, "=============================================================");
}

/* ==================== [Static Functions] ================================== */

static xf_err_t check_and_update_cache(void)
{
    xf_err_t xf_ret = XF_OK;
    const xf_fal_flash_dev_t *flash_dev;
    const xf_fal_partition_t *p_table;
    const xf_fal_partition_t *part;
    size_t table_len;
    size_t i;
    size_t j;

    XF_FAL_CTX_TRYLOCK__RETURN_ON_FAILURE(XF_ERR_BUSY);
    sp_fal()->cached_num = 0;
    for (i = 0; i < XF_FAL_PARTITION_TABLE_NUM; i++) {
        p_table     = sp_fal()->partition_table[i];
        table_len   = sp_fal()->partition_table_len[i];
        if ((NULL == p_table) || (0 == table_len)) {
            continue;
        }
        for (j = 0; j < table_len; j++) {
            part = &p_table[j];
            flash_dev = xf_fal_flash_device_find(part->flash_name);
            if (flash_dev == NULL) {
                XF_LOGD(TAG, "Warning: Do NOT found the flash device(%s).",
                        part->flash_name);
                continue;
            }

            if (part->offset >= (size_t)flash_dev->len) {
                XF_LOGE(TAG, "Initialize failed! "
                        "Partition(%s) offset address(%ld) out of flash bound(<%d).",
                        part->name, part->offset, (int)flash_dev->len);;
                xf_ret = XF_FAIL;
                goto l_unlock_ret;
            }

            if (sp_fal()->cached_num >= XF_FAL_CACHE_NUM) {
                XF_LOGW(TAG, "The cache is too small.");
                continue;
            }

            sp_fal()->cache[sp_fal()->cached_num].flash_dev = flash_dev;
            sp_fal()->cache[sp_fal()->cached_num].partition = part;
            ++sp_fal()->cached_num;
        }
    }

l_unlock_ret:;
    XF_FAL_CTX_UNLOCK();

    return xf_ret;
}
