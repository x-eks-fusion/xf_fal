
typedef struct _xf_fal_flash_dev_t xf_fal_flash_dev_t;

extern xf_fal_flash_dev_t mock_flash_dev;

#define XF_FAL_FLASH_TABLE                                           \
{                                                                    \
    &mock_flash_dev,                                                 \
}

#define XF_FAL_PART_TABLE                                            \
{                                                                    \
    {       "bl",     "mock_flash",         0,   64},                \
    {      "app",     "mock_flash",         64,  1024*10},           \
    {"easyflash",     "mock_flash",         1024*10, 1024*20},       \
    { "download",     "mock_flash",         1024*20, 1024*40},       \
}
