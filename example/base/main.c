#include <stdio.h>
#include "xf_fal.h"
#include "mock_flash.h"

int main(void)
{
    /* 注册 xf_fal */
    mock_flash_register_to_xf_fal();

    uint8_t read_buf[16] = {0};
    uint8_t write_buf[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    xf_err_t xf_ret;

    /* 初始化 FAL */
    xf_ret = xf_fal_init();
    if (xf_ret != XF_OK) {
        printf("FAL init failed: %d\n", xf_ret);
        return -1;
    }

    /* 打印分区表 */
    xf_fal_show_part_table();

    /* 获取分区 */
    const xf_fal_partition_t *part = xf_fal_partition_find("easyflash");
    if (!part) {
        printf("Partition not found!\n");
        return -1;
    }

    /*
        擦除数据
        数据只能以扇区为单位擦除，此处先获取 flash 设备查询扇区大小
     */
    const xf_fal_flash_dev_t *flash_dev = xf_fal_flash_device_find_by_part(part);
    if (NULL == flash_dev) {
        printf("flash device not found.");
        return -1;
    }

    /* 写入之前要擦除数据 */
    xf_ret = xf_fal_partition_erase(part, 0, flash_dev->sector_size);
    if (xf_ret != XF_OK) {
        printf("Partition erase failed: %d\n", xf_ret);
        return -1;
    }

    /* 读取数据并打印 */
    xf_ret = xf_fal_partition_read(part, 0, read_buf, sizeof(read_buf));
    if (xf_ret == XF_OK) {
        printf("The partition was successfully. Data: ");
        for (size_t i = 0; i < sizeof(read_buf); i++) {
            printf("%02X ", read_buf[i]);
        }
        printf("\n");
    } else {
        printf("Partition read after erase failed: %d\n", xf_ret);
    }

    /* 写入数据到分区 */
    xf_ret = xf_fal_partition_write(part, 0, write_buf, sizeof(write_buf));
    if (xf_ret != XF_OK) {
        printf("Partition write failed: %d\n", xf_ret);
        return -1;
    }

    /* 读取数据并打印 */
    xf_ret = xf_fal_partition_read(part, 0, read_buf, sizeof(read_buf));
    if (xf_ret == XF_OK) {
        printf("Partition read successful! Data: ");
        for (size_t i = 0; i < sizeof(read_buf); i++) {
            printf("%02X ", read_buf[i]);
        }
        printf("\n");
    } else {
        printf("Partition read failed: %d\n", xf_ret);
    }

    /* 擦除数据 */
    xf_ret = xf_fal_partition_erase(part, 0, flash_dev->sector_size);
    if (xf_ret != XF_OK) {
        printf("Partition erase failed: %d\n", xf_ret);
        return -1;
    }

    /* 再次读取数据并打印 */
    xf_ret = xf_fal_partition_read(part, 0, read_buf, sizeof(read_buf));
    if (xf_ret == XF_OK) {
        printf("After erase, data: ");
        for (size_t i = 0; i < sizeof(read_buf); i++) {
            printf("%02X ", read_buf[i]);
        }
        printf("\n");
    } else {
        printf("Partition read after erase failed: %d\n", xf_ret);
    }

    return 0;
}
