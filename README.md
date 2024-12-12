# FLASH 抽象层

本仓库参考了 RT-Thread 的[FAL](https://github.com/RT-Thread-packages/fal/blob/master/README_ZH.md)

1. 支持注册静态可配置的分区表，并可关联多个 flash 设备。
1. 支持多次注册不同的分区表和不同的 flash 设备。
1. 仅依赖 xf_utils .支持 c99，可以快速移植到任何平台。
1. 统一了对上的操作 API ，为后续的 OTA、FS 等提供底层 API 支持。
1. 基于分区表进行分区操作。

## 仓库目录

```shell
.
├── example                 # linux上模拟的例程
│  ├── main.c               # 例程主函数
│  ├── xf_fal_config.h      # xf_fal的配置文件，其中有分区表的注册
│  └── xf_fal_port.c        # xf_fal对接部分
├── src                     # xf_fal源码
│  ├── xf_fal.c             # xf_fal源文件
│  ├── xf_fal.h             # xf_fal头文件
│  ├── xf_fal_types.h       # xf_fal公共类型类型及定义头文件
│  └── xf_fal_config_internal.h # 内部默认配置
├── xmake.lua               # xmake工程构建脚本
└── README.md               # 说明文档
```

## 如何移植？

1. 添加 `src/xf_fal.c` 到工程，添加 `src` 到工程中。
1. 实现 `xf_fal_flash_dev_t` 结构体对象（flash 设备）。
1. 实现 `xf_fal_partition_t` 结构体对象（分区表）。
1. 调用 `xf_fal_register_flash_device()` 和 `xf_fal_register_partition_table()` 注册 flash 设备和分区表。

## 如何运行例程？

1. 克隆 `xf_utils` 仓库
   ```shell
    git clone https://github.com/x-eks-fusion/xf_utils.git
   ```
2. 安装[xmake](https://xmake.io/#/zh-cn/getting_started)

3. 编译工程

   ```shell
   xmake b base
   ```

4. 运行工程
   ```shell
   xmake r base
   ```

## xf_fal 提供的示例

目前提供了两个示例：

1.  base

基础示例，只有一个 flash 设备和一个分区表。

演示了 xf_fal 注册、初始化和擦除、写入、读取指定分区。

1.  multi_flash_device

多 flash 设备和多分区表示例。

分区表 `MOCK_FLASH1_PART_TABLE1` 和 `MOCK_FLASH1_PART_TABLE2` 都属于 flash 设备 1；
分区表 `MOCK_FLASH2_PART_TABLE` 属于 flash 设备 2。

演示了对同一 flash 的跨分区表（两个分区表）访问，和跨 flash 的访问。
