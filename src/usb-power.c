/*
 *  TP-Link TL-WR703N USB power switch (GPIO) driver
 *
 *  Copyright (C) 2016 Jean-Christophe Rona <jc@rona.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

#define DRV_NAME		"usb-power"
#define DRV_DESC		"TP-Link TL-WR703N USB power switch (GPIO) driver"
#define DRV_VERSION		"0.1"

/* The USB power switch is on GPIO 8 for the TP-Link TL-WR703N router */
#define USB_POWER_GPIO_NR	8

struct usb_power_platform_data {
	int gpio;
};

struct platform_device *usb_power_pdev = NULL;


static ssize_t show_value(struct device *dev,
			struct device_attribute *devattr, char *buf)
{
	struct usb_power_platform_data *pdata = dev->platform_data;

	return 0;
}

static ssize_t store_value(struct device *dev,
			struct device_attribute *devattr,
			const char *buf, size_t count)
{
	struct usb_power_platform_data *pdata = dev->platform_data;
	unsigned int val;

	if (sscanf(buf, "%u", &val) != 1) {
		printk(KERN_ERR "%s: Unable to parse input !\n", __func__);
		return -1;
	}

	gpio_set_value(pdata->gpio, ((val > 0) ? 1 : 0));

	return count;
}

static DEVICE_ATTR(value, S_IWUSR | S_IRUGO, show_value,
			  store_value);

static struct attribute *usb_power_sysfs_attributes[] = {
	&dev_attr_value.attr,
	NULL
};

static const struct attribute_group usb_power_sysfs_group = {
	.attrs = usb_power_sysfs_attributes,
};

static int usb_power_remove(struct platform_device *pdev)
{
	struct usb_power_platform_data *pdata = pdev->dev.platform_data;

	sysfs_remove_group(&pdev->dev.kobj, &usb_power_sysfs_group);
	gpio_free(pdata->gpio);

	return 0;
}

static int usb_power_probe(struct platform_device *pdev)
{
	int ret;
	struct usb_power_platform_data *pdata;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		printk(KERN_ERR "%s: Missing platform data !\n", __func__);
		return -EINVAL;
	}

	/* Do not try to request the GPIO since it has been already requested by the system */
	//ret = gpio_request(pdata->gpio, "USB power GPIO");
	//if (ret) {
	//	printk(KERN_ERR "%s: Could not request GPIO %d !\n", __func__, pdata->gpio);
	//	goto error_gpio;
	//}

	ret = sysfs_create_group(&pdev->dev.kobj, &usb_power_sysfs_group);
	if (ret) {
		printk(KERN_ERR "%s: Could not create sysfs files !\n", __func__);
		goto error_create_sysfs;
	}

	printk(KERN_INFO "USB power switch registered on GPIO %d\n", pdata->gpio);

	return 0;

	sysfs_remove_group(&pdev->dev.kobj, &usb_power_sysfs_group);
error_create_sysfs:
	gpio_free(pdata->gpio);
error_gpio:

	return ret;
}

static struct platform_driver usb_power_driver = {
	.driver = {
		   .name = DRV_NAME,
		   .owner = THIS_MODULE,
		   },
	.probe		= usb_power_probe,
	.remove		= usb_power_remove,
};

static int __init usb_power_init(void)
{
	struct platform_device *pdev;
	struct usb_power_platform_data pdata;
	int ret;

	printk(KERN_INFO "%s version %s\n", DRV_DESC, DRV_VERSION);

	ret = platform_driver_register(&usb_power_driver);
	if (ret) {
		printk(KERN_ERR "%s: Could not register driver !\n", __func__);
		goto error_register;
	}

	pdev = platform_device_alloc(DRV_NAME, 0);
	if (!pdev) {
		printk(KERN_ERR "%s: Could not allocate device !\n", __func__);
		ret = -ENOMEM;
		goto error_alloc;
	}

	pdata.gpio = USB_POWER_GPIO_NR;

	ret = platform_device_add_data(pdev, &pdata, sizeof(pdata));
	if (ret) {
		printk(KERN_ERR "%s: Could not add platform data !\n", __func__);
		goto error_add_data;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR "%s: Could not add device !\n", __func__);
		goto error_add_device;
	}

	usb_power_pdev = pdev;

	return 0;

error_add_device:
error_add_data:
	platform_device_put(pdev);
error_alloc:
	platform_driver_unregister(&usb_power_driver);
error_register:

	return ret;;
}
module_init(usb_power_init);

static void __exit usb_power_exit(void)
{
	platform_device_del(usb_power_pdev);
	platform_device_put(usb_power_pdev);
	platform_driver_unregister(&usb_power_driver);
}
module_exit(usb_power_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Jean-Christophe Rona <jc@rona.fr>");
MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
