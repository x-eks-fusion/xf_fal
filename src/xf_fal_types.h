/**
 * @file xf_fal_types.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief xf_fal 公共类型类型及定义。
 * @version 1.0
 * @date 2024-12-10
 *
 * Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_FAL_TYPES_H__
#define __XF_FAL_TYPES_H__

/* ==================== [Includes] ========================================== */

#include "xf_fal_config_internal.h"
#include "xf_utils.h"

/**
 * @cond (XFAPI_USER || XFAPI_PORT)
 * @addtogroup group_xf_fal
 * @endcond
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define XF_FAL_SW_VERSION                 "1.1.0"

/* ==================== [Typedefs] ========================================== */

/**
 * @brief flash 操作集。
 *
 * - 由对接层提供给 xf_fal 调用。
 * - 每个操作集合对应一个 flash 设备。
 * - 除 xf_fal_flash_ops_t.init 和 xf_fal_flash_ops_t.deinit 外，必须全部实现。
 *   对于内部 flash, 不一定需要 xf_fal_flash_ops_t.init 和 xf_fal_flash_ops_t.deinit.
 *
 * @cond XFAPI_PORT
 * @todo fal 操作实现时是否必须支持按字节读写？
 * @endcond
 */
typedef struct _xf_fal_flash_ops_t {
    /**
     * @brief 初始化 flash 设备。
     */
    xf_err_t (*init)(void);
    /**
     * @brief 反初始化 flash 设备。
     */
    xf_err_t (*deinit)(void);
    /**
     * @brief 从 flash 的指定偏移地址读取数据。
     *
     * @note xf_fal 只保证读取大小不超过分区长度。
     *       对于跨页的读取，接口需要自行处理。
     *
     * @param src_offset 要读取的数据的地址。
     *                   已在 xf_fal 内加上分区的偏移地址。
     * @param[out] dst   指向读取缓冲区。
     *                   已在 xf_fal 内判断是否为 NULL.
     * @param size       要读取的数据大小，单位：字节。
     *                   src_offset + size 不会大于分区长度。
     * @return xf_err_t
     *      - XF_OK                 成功，当成功从 src_offset 读取了 size 字节数到 dst
     *      - XF_FAIL               失败，其他情况
     */
    xf_err_t (*read)(size_t src_offset, void *dst, size_t size);
    /**
     * @brief 写数据到 flash 的指定偏移地址。
     *
     * @note xf_fal 只保证写入大小不超过分区长度。
     *       对于跨页的写入，接口需要自行处理。
     *
     * @param dst_offset 待写入数据的目标地址。
     *                   已在 xf_fal 内加上分区的偏移地址。
     * @param src        指向数据来源缓冲区。
     *                   已在 xf_fal 内判断是否为 NULL.
     * @param size       要写入的数据大小，单位：字节。
     *                   dst_offset + size 不会大于分区长度。
     * @return xf_err_t
     *      - XF_OK                 成功，当成功将 src 存的 size 字节写入到 dst_offset
     *      - XF_FAIL               失败，其他情况
     */
    xf_err_t (*write)(size_t dst_offset, const void *src, size_t size);
    /**
     * @brief 擦除 flash 的指定偏移地址指定长度。
     *
     * @param offset     待擦除的地址。
     *                   已在 xf_fal 内加上分区的偏移地址。
     * @param size       需要擦除的大小，单位：字节。
     *                   offset + size 不会大于分区长度。
     * @return xf_err_t
     *      - XF_OK                 成功，当擦除指定地址的指定字节数
     *      - XF_FAIL               失败，其他情况
     */
    xf_err_t (*erase)(size_t offset, size_t size);
} xf_fal_flash_ops_t;

/**
 * @brief flash 设备结构体。
 * @attention 结构体是可见的，但禁止用户修改其中内容。
 */
typedef struct _xf_fal_flash_dev_t {
    char       *name;                   /*!< flash 设备名 */
    uint32_t    addr;                   /*!< flash 设备起始地址 */
    size_t      len;                    /*!< flash 设备起始地址之后的长度 */
    /**
     * @brief 扇区大小。扇区是最小擦除大小。单位: byte.
     */
    size_t      sector_size;
    /**
     * @brief  最小读写单元大小，单位: byte.
     * 可能的值：
     * - 1      stm32f2/f4，或者一些 flash(W25Q128)
     * - 2
     * - 4      stm32f1
     * - 8      stm32l4
     * - 更大   可能是 nandflash
     * @note 注意不是页大小。
     */
    size_t      io_size;
    /**
     * @brief flash 操作集，见 @ref xf_fal_flash_ops_t .
     */
    xf_fal_flash_ops_t  ops;
} xf_fal_flash_dev_t;

/**
 * @brief flash 分区结构体。
 * 描述了各分区的分区名、分区在哪个 flash、分区在 flash 上的偏移地址和长度。
 * @attention 结构体是可见的，但禁止用户修改其中内容。
 */
typedef struct _xf_fal_partition_t {
    char   *name;                       /*!< 当前分区的分区名 */
    char   *flash_name;                 /*!< 与当前分区关联的 flash 设备名
                                         *   见 @ref xf_fal_flash_dev_t.name .
                                         */
    /**
     * @brief flash 设备上的该分区偏移地址。必须对齐到扇区大小。
     */
    size_t  offset;
    /**
     * @brief flash 设备上的该分区长度。必须对齐到扇区大小。
     */
    size_t  len;
} xf_fal_partition_t;

/**
 * @brief xf_fal 分区缓存。
 */
typedef struct _xf_fal_cache_t {
    /**
     * @brief 指向分区信息。
     */
    const xf_fal_partition_t   *partition;
    /**
     * @brief 分区对应的 flash 设备对象。
     */
    const xf_fal_flash_dev_t   *flash_dev;
} xf_fal_cache_t;

/**
 * @brief xf_fal 对象上下文结构体。
 */
typedef struct _xf_fal_ctx_t {
    /**
     * @brief 用于判断 xf_fal 是否已初始化。
     */
    volatile uint8_t            is_init;

    /**
     * @brief flash 设备表（数组）指针。
     *
     * 可以改变 flash_device_table 指向；
     * 不可通过 flash_device_table 修改：
     * - flash_device_table[0] (xf_fal_flash_dev_t *xxx)
     * - 和 flash_device_table[0].xxx .
     */
    const xf_fal_flash_dev_t   *flash_device_table[XF_FAL_FLASH_DEVICE_NUM];

    /**
     * @brief 分区表（数组）的数组。
     * @code
     * xf_fal_ctx_t.partition_table[IDX]     --> xf_fal_partition_t partition_table[N]
     * @endcode
     */
    const xf_fal_partition_t   *partition_table[XF_FAL_PARTITION_TABLE_NUM];
    /**
     * @brief 分区表（数组）表长（分区表内所有分区的总个数）数组。
     * @code
     * xf_fal_ctx_t.partition_table[IDX]     --> xf_fal_partition_t partition_table[N]
     * xf_fal_ctx_t.partition_table_len[IDX] --> N
     * @endcode
     */
    size_t                      partition_table_len[XF_FAL_PARTITION_TABLE_NUM];

    /**
     * @brief 缓存"分区和 flash 设备关系"的缓存表（数组）指针。
     * 
     * 用于从分区快速找到关联的 flash 设备。
     * 
     * @attention 缓存数组个数至少和分区的个数一样多。
     */
    xf_fal_cache_t              cache[XF_FAL_CACHE_NUM];
    /**
     * @brief 已缓存的个数。
     */
    size_t                      cached_num;
} xf_fal_ctx_t;

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * End of addtogroup group_xf_fal
 * @}
 */

#endif // __XF_FAL_TYPES_H__
