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

/**
 * @cond (XFAPI_USER || XFAPI_PORT || XFAPI_INTERNAL)
 * @defgroup group_xf_fal xf_fal
 * @brief xf_fal Flash 抽象层 FAL (Flash Abstraction Layer).
 * @endcond
 */

#ifndef __XF_FAL_H__
#define __XF_FAL_H__

/* ==================== [Includes] ========================================== */

#include "xf_fal_types.h"

#include "xf_fal_config_internal.h"
#include "xf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/**
 * @cond XFAPI_PORT
 * @addtogroup group_xf_fal
 * @endcond
 * @{
 */

/**
 * @brief 获取 xf_fal 上下文。
 *
 * @note 通过此接口可获取当前 xf_fal 注册状态、注册的 flash 设备和注册的分区表。
 *       可用于动态扩展分区表。
 * @attention 禁止直接修改其中内容。
 *
 * @return xf_fal_ctx_t*        xf_fal 上下文
 */
const xf_fal_ctx_t *xf_fal_get_ctx(void);

/**
 * @brief 注册一个 flash 设备到 xf_fal 中。
 *
 * @attention xf_fal 内仅保存 flash 设备指针，
 *            用户必须保证在使用 xf_fal 的整个过程中 flash 设备可访问。
 * @attention 用户应保证 flash 设备名不重复。否则只会找到第一个。
 * @attention 注册完毕后需要调用 xf_fal_init() 初始化 xf_fal .
 * @attention 已初始化后禁止注册或注销，需调用  xf_fal_deinit() 反初始化.
 *
 * @param p_dev flash 设备。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               xf_fal 已初始化，禁止注册注销
 *      - XF_ERR_BUSY           xf_fal 被别处占用
 *      - XF_ERR_INVALID_ARG    无效参数
 *      - XF_ERR_INVALID_PORT   无效对接, p_dev->ops 的 read, write, erase 存在 NULL
 *      - XF_ERR_INITED         p_dev 已注册
 *      - XF_ERR_RESOURCE       xf_fal 的设备表数组已满，需增大 XF_FAL_FLASH_DEVICE_NUM
 */
xf_err_t xf_fal_register_flash_device(const xf_fal_flash_dev_t *p_dev);

/**
 * @brief 注册一个分区表到 xf_fal 中。
 *
 * @attention xf_fal 内仅保存分区表指针，
 *            用户必须保证在使用 xf_fal 的整个过程中分区表可访问。
 * @attention 用户应保证分区表内的分区不发生交叠，且分区不重名，否则只会找到第一个。
 * @attention 注册完毕后需要调用 xf_fal_init() 初始化 xf_fal .
 * @attention 已初始化后禁止注册或注销，需调用  xf_fal_deinit() 反初始化.
 *
 * @param p_table   分区表数组。注意不是分区。
 * @param table_len 分区表表长。单位：分区个数。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               xf_fal 已初始化，禁止注册注销
 *      - XF_ERR_BUSY           xf_fal 被别处占用
 *      - XF_ERR_INVALID_ARG    无效参数
 *      - XF_ERR_INITED         p_table 已注册
 *      - XF_ERR_RESOURCE       xf_fal 的分区表数组已满，需增大 XF_FAL_PARTITION_TABLE_NUM
 */
xf_err_t xf_fal_register_partition_table(
    const xf_fal_partition_t *p_table, size_t table_len);

/**
 * @brief 从 xf_fal 中注销一个 flash 设备。
 *
 * @attention 已初始化后禁止注册或注销，需调用  xf_fal_deinit() 反初始化.
 *
 * @param p_dev 要注销的 flash 设备。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               xf_fal 已初始化，禁止注册注销
 *      - XF_ERR_BUSY           xf_fal 被别处占用
 *      - XF_ERR_INVALID_ARG    无效参数
 *      - XF_ERR_NOT_FOUND      未找到
 */
xf_err_t xf_fal_unregister_flash_device(const xf_fal_flash_dev_t *p_dev);

/**
 * @brief 从 xf_fal 中注销一个分区表。
 *
 * @attention 已初始化后禁止注册或注销，需调用  xf_fal_deinit() 反初始化.
 *
 * @param p_table   分区表数组。注意不是分区。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               xf_fal 已初始化，禁止注册注销
 *      - XF_ERR_BUSY           xf_fal 被别处占用
 *      - XF_ERR_INVALID_ARG    无效参数
 *      - XF_ERR_NOT_FOUND      未找到
 */
xf_err_t xf_fal_unregister_partition_table(const xf_fal_partition_t *p_table);

/**
 * End of addtogroup group_xf_fal
 * @}
 */

/**
 * @cond (XFAPI_USER || XFAPI_PORT)
 * @addtogroup group_xf_fal
 * @endcond
 * @{
 */

/**
 * @brief 检查 xf_fal 注册状态。
 *
 * @return true                 已注册
 * @return false                未注册
 */
bool xf_fal_check_register_state(void);

/**
 * @brief 初始化 FAL.
 *
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_INITED         已初始化
 *      - XF_ERR_INVALID_PORT   未注册 xf_fal
 */
xf_err_t xf_fal_init(void);

/**
 * @brief 反初始化 FAL.
 *
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_PORT   未注册 xf_fal
 */
xf_err_t xf_fal_deinit(void);

/**
 * @brief 根据 flash 名称查找 flash 设备。
 *
 * @param name flash 名称。
 * @return const xf_fal_flash_dev_t*
 *      - NULL                  未找到或参数错误或未注册 xf_fal
 *      - (OTHER)               flash 设备句柄
 */
const xf_fal_flash_dev_t *xf_fal_flash_device_find(const char *name);

/**
 * @brief 通过给定分区查找 flash 设备。
 *
 * @note 此函数通常用于获取 xf_fal_flash_dev_t.sector_size
 *       和 xf_fal_flash_dev_t.io_size .
 *       使用户确定擦除大小。
 *
 * @param part 分区表中的指定分区。
 *             可以通过 xf_fal_partition_find() 获取。
 * @return xf_err_t
 *      - NULL                  未找到或参数错误或未注册 xf_fal
 *      - (OTHER)               flash 设备句柄
 */
const xf_fal_flash_dev_t *xf_fal_flash_device_find_by_part(
    const xf_fal_partition_t *part);

/**
 * @brief 根据分区名称查找分区句柄。
 *
 * @param name 分区表名称。
 * @return const xf_fal_partition_t*
 *      - NULL                  未找到或参数错误或未注册 xf_fal
 *      - (OTHER)               分区表句柄
 */
const xf_fal_partition_t *xf_fal_partition_find(const char *name);

/**
 * @brief 从指定分区读取数据。
 *
 * @note 读写数据的大小需要是 flash 最小读写颗粒大小的整数倍。
 *       见 @ref xf_fal_flash_dev_t.io_size .
 *
 * @param part       分区表中的指定分区。
 *                   可以通过 xf_fal_partition_find() 获取。
 * @param src_offset 要读取的数据的地址。相对当前分区起始地址的偏移地址。
 * @param[out] buf   指向读取缓冲区。
 * @param size       要读取的数据大小，单位：字节。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_fal_partition_read(
    const xf_fal_partition_t *part,
    size_t src_offset, void *dst, size_t size);

/**
 * @brief 将数据写入指定分区。
 *
 * @note 读写数据的大小需要是 flash 最小读写颗粒大小的整数倍。
 *       见 @ref xf_fal_flash_dev_t.io_size .
 * @note 写入前一定要确认目标地址已经被擦除。
 *
 * @param part       分区表中的指定分区。
 *                   可以通过 xf_fal_partition_find() 获取。
 * @param dst_offset 要写入的数据的地址。相对当前分区起始地址的偏移地址。
 * @param src        指向数据来源缓冲区。
 * @param size       要写入的数据大小，单位：字节。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_fal_partition_write(
    const xf_fal_partition_t *part,
    size_t dst_offset, const void *src, size_t size);

/**
 * @brief 擦除指定分区数据。
 *
 * @note 擦除数据的大小推荐对齐到 flash 扇区大小的整数倍。
 *       扇区大小见 @ref xf_fal_flash_dev_t.sector_size .
 *       可以通过 xf_fal_flash_device_find_by_part() 获取。
 *
 * @param part       分区表中的指定分区。
 *                   可以通过 xf_fal_partition_find() 获取。
 * @param offset     待擦除的地址。相对当前分区起始地址的偏移地址。
 * @param size       要擦除的数据大小，单位：字节。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_FAIL               失败
 *      - XF_ERR_UNINIT         未初始化
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_fal_partition_erase(
    const xf_fal_partition_t *part, size_t offset, size_t size);

/**
 * @brief 擦除指定分区所有数据。
 *
 * @param part 分区表的指定分区。
 *             可以通过 xf_fal_partition_find() 获取。
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

/**
 * End of addtogroup group_xf_fal
 * @}
 */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_FAL_H__
