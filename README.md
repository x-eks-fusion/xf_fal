# FLASH 抽象层

本仓库参考了RT-Thread的[FAL](https://github.com/RT-Thread-packages/fal/blob/master/README_ZH.md)

1. 支持静态可配置的分区表，并可关联多个 Flash 设备。
2. 仅依赖 xf_utils .支持c99，可以快速移植到任何平台。
3. 统一了对上的操作API ，为后续的OTA、FS等提供底层API支持。
4. 基于分区表进行分区操作。

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
│  └── xf_fal_config_internal.h # 内部默认配置
├── xmake.lua               # xmake工程构建脚本
└── README.md               # 说明文档
```

## 如何移植？

1. 添加`src/xf_fal.c`到工程，添加`src`到工程中
2. 对接`xf_fal_flash_dev_t`结构体对象
3. 添加`xf_fal_config.h`到工程中
4. 配置对接`xf_fal_flash_dev_t`结构体对象，给`XF_FAL_FLASH_TABLE`注册表
5. 配置分区表`XF_FAL_PART_TABLE`

## 如何运行例程？

1. 克隆 `xf_utils` 仓库
   ```shell
    git clone https://github.com/x-eks-fusion/xf_utils.git
    ```
2. 安装[xmake](https://xmake.io/#/zh-cn/getting_started)
3. 编译工程
    ```shell
    xmake b xf_fal   
    ```
4. 运行工程
    ```shell
    xmake r xf_fal  
    ```