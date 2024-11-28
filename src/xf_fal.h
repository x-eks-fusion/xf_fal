/**
 * @file xf_fal.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief Flash 抽象层 FAL (Flash Abstraction Layer).
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

/**
 * @brief flash 设备结构体。
 */
typedef struct _xf_fal_flash_dev_t {
    char name[XF_FAL_DEV_NAME_MAX];     /*!< flash 设备名 */
    uint32_t addr;                      /*!< flash 设备起始地址  */
    size_t len;                         /*!< flash 设备长度  */
    size_t blk_size;                    /*!< flash 中最小擦除粒度的块大小 */
    struct {
        int (*init)(void);
        int (*read)(long offset, uint8_t *buf, size_t size);
        int (*write)(long offset, const uint8_t *buf, size_t size);
        int (*erase)(long offset, size_t size);
    } ops;
    /**
     * @brief  最小写入粒度(write minimum granularity)，单位: bit.
     * 1（非闪存）/8（stm32f2/f4）/32（stm32f1）/64（stm32l4）
     * 0 不生效。
     */
    size_t write_gran;
} xf_fal_flash_dev_t;

/**
 * @brief flash 分区表结构体。
 */
typedef struct _xf_fal_partition_t {
    char name[XF_FAL_DEV_NAME_MAX];         /*!< 分区名 */
    char flash_name[XF_FAL_DEV_NAME_MAX];   /*!< 分区对应的 flash 设备的名称 */
    long offset;                            /*!< flash 设备上的该分区偏移地址 */
    size_t len;                             /*!< flash 设备上的该分区长度 */
} xf_fal_partition_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 初始化 FAL.
 *
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_INITED         已初始化
 */
xf_err_t xf_fal_init(void);

/**
 * @brief 根据 flash 名称查找 flash 设备。
 *
 * @param name flash 名称。
 * @return const xf_fal_flash_dev_t*
 *      - NULL                  未找到或参数错误
 *      - (OTHER)               flash 设备句柄
 */
const xf_fal_flash_dev_t *xf_fal_flash_device_find(const char *name);

/**
 * @brief 根据分区表名称查找分区表句柄。
 *
 * @param name 分区表名称。
 * @return const xf_fal_partition_t*
 *      - NULL                  未找到或参数错误
 *      - (OTHER)               分区表句柄
 */
const xf_fal_partition_t *xf_fal_partition_find(const char *name);

/**
 * @brief 获取分区表句柄及长度。
 *
 * @param[out] len 传出分区表长度。
 * @return const xf_fal_partition_t*
 *      - NULL                  未初始化或参数错误
 *      - (OTHER)               分区表句柄
 */
const xf_fal_partition_t *xf_fal_get_partition_table(size_t *len);

/**
 * @brief 临时设置分区表。
 *
 * 此设置会暂时修改分区表，重启后设置会丢失。
 *
 * @param table 分区表数组。
 * @param len 分区表数组的长度（个数）。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_fal_set_partition_table_temp(xf_fal_partition_t *table, size_t len);

/**
 * @brief 从指定分区读取数据。
 *
 * @param part 分区表的指定分区。
 * @param addr 分区的相对地址。
 * @param[out] buf 读取缓冲区。
 * @param size 读取大小。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_fal_partition_read(
    const xf_fal_partition_t *part, uint32_t addr, uint8_t *buf, size_t size);

/**
 * @brief 将数据写入指定分区。
 *
 * @param part 分区表的指定分区。
 * @param addr 分区的相对地址。
 * @param buf 写入缓冲区。
 * @param size 写入大小。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_fal_partition_write(
    const xf_fal_partition_t *part, uint32_t addr, const uint8_t *buf, size_t size);

/**
 * @brief 擦除指定分区数据。
 *
 * @param part 分区表的指定分区。
 * @param addr 分区的相对地址。
 * @param size 写入大小。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_fal_partition_erase(
    const xf_fal_partition_t *part, uint32_t addr, size_t size);

/**
 * @brief 擦除指定分区所有数据。
 *
 * @param part 分区表的指定分区。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_fal_partition_erase_all(const xf_fal_partition_t *part);

/**
 * @brief 打印分区表信息。
 */
void xf_fal_show_part_table(void);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_FAL_H__
