#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>  // sensor_value 정의 포함
//#include <zephyr/drivers/display.h>
#include    "display_7seg.h"

int main(void)
{
    const struct device     *dev = DEVICE_DT_GET_ONE(st_vl53l0x);
    struct sensor_value     dist;

    if (!device_is_ready(dev)) {
        printk("VL53L0X device not found\n");
        return  -1;
    }

    printk("VL53L0X ready: %s\n", dev->name);

    while (1) {
        int ret = sensor_sample_fetch(dev);

        if (ret < 0) {
            printk("sensor_sample_fetch error: %d\n", ret);
        } else {
//            sensor_channel_get(dev, SENSOR_CHAN_DISTANCE, &dist);
            if (sensor_channel_get(dev, SENSOR_CHAN_DISTANCE, &dist) < 0) {
                printk("Channel get error\n");
                return -1;
            }

            printk("Distance: %d.%06d m\n", dist.val1, dist.val2);
//            printk("Distance: %d mm\n", dist.val1);
        }
        display_number(dist.val2 / 100, 10);   // 6 mm data into 4digit .

        k_sleep(K_MSEC(500));
//        display_number(5678, 10);   // number 5678 display.
    }

    return 0;
}
