/**
 * @file xf_fal_port.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-10-21
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include <stdio.h>
#include <string.h>
#include "xf_fal.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

static uint8_t mock_flash_memory[1024 * 1024] = {0};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int mock_flash_init(void)
{
    printf("Mock flash initialized.\n");
    return 0;
}

int mock_flash_read(long addr, uint8_t *buf, size_t size)
{
    if (addr + size > sizeof(mock_flash_memory)) {
        return XF_FAIL;
    }
    memcpy(buf, &mock_flash_memory[addr], size);
    return XF_OK;
}

int mock_flash_write(long addr, const uint8_t *buf, size_t size)
{
    if (addr + size > sizeof(mock_flash_memory)) {
        return XF_FAIL;
    }
    memcpy(&mock_flash_memory[addr], buf, size);
    return XF_OK;
}

int mock_flash_erase(long addr, size_t size)
{
    if (addr + size > sizeof(mock_flash_memory)) {
        return XF_FAIL;
    }
    memset(&mock_flash_memory[addr], 0xFF, size);
    return XF_OK;
}

xf_fal_flash_dev_t mock_flash_dev = {
    .name = "mock_flash",
    .addr = 0x0,
    .len = 1024 * 1024,
    .blk_size = 512,
    .ops.init = &mock_flash_init,
    .ops.read = &mock_flash_read,
    .ops.write = &mock_flash_write,
    .ops.erase = &mock_flash_erase
};

/* ==================== [Static Functions] ================================== */
