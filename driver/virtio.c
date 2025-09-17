/**
 * @FilePath: /vboot/virtio.c
 * @Description:  
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-05-21 14:21:01
 * @LastEditTime: 2025-09-17 21:05:28
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
*/
#include "virtio.h"

volatile struct virtio_mmio_regs *virtio = (volatile struct virtio_mmio_regs *)VIRTIO_MMIO_BASE;

int virtio_blk_init()
{
    if(virtio->magic != VIRTIO_MMIO_MAGIC_VALUE ||
       virtio->version != VIRTIO_MMIO_VERSION ||
       virtio->vendor_id != VIRTIO_MMIO_VENDOR_ID)
    {
        return -1;
    }

    u32 status = 0; 
    // 1. Reset the device.
    virtio->status = status;
    __sync_synchronize();

    // 2. Set the ACKNOWLEDGE status bit: the guest OS has noticed the device
    status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
    virtio->status = status;
    __sync_synchronize();

    // 3. Set the DRIVER status bit: the guest OS knows how to drive the device.
    status |= VIRTIO_CONFIG_S_DRIVER;
    virtio->status = status;

    __sync_synchronize();

    // 4. Read device feature bits, and write the subset of feature bits understood by the OS and driver to the device.
    virtio->device_features_sel = 0;
    __sync_synchronize();
    u32 device_features = virtio->device_features;
    u32 features = device_features;
    // 这里设置你想要启用的功能位
    features &= ~(1 << VIRTIO_BLK_F_RO);
    features &= ~(1 << VIRTIO_BLK_F_SCSI);
    features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
    features &= ~(1 << VIRTIO_BLK_F_MQ);
    features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
    features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
    features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);

    // 写入驱动支持的特性
    virtio->driver_features_sel = 0;
    __sync_synchronize();
    virtio->driver_features = features;
    __sync_synchronize();

    // 5. Set the FEATURES_OK status bit. The driver MUST NOT accept new feature bits after this step. 
    status |= VIRTIO_CONFIG_S_FEATURES_OK;
    virtio->status = status;

    __sync_synchronize();
    // 6. Re-read device status to ensure the FEATURES_OK bit is still set: otherwise, the device does not
    //  support our subset of features and the device is unusable
    if(!(virtio->status & VIRTIO_CONFIG_S_FEATURES_OK))
    {
        return -1;
    }

    return 0;

}


