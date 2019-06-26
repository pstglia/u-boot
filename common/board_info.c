/*
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <linux/libfdt.h>
#include <linux/compiler.h>
#include <asm/gpio.h>

int __weak checkboard(void)
{
	return 0;
}



static void pcie_sata_switch(void)
{
	unsigned int det_pcie_sata_gpio = 10;
	unsigned int switch_ctrl_gpio = 150;
	unsigned int gms561_rst_gpio = 152;
	int ret;

	ret = gpio_request(gms561_rst_gpio, "gms561_rst_gpio");
	if (ret && ret != -EBUSY) {
		debug("gpio: requesting pin %u failed\n", gms561_rst_gpio);
	}

	ret = gpio_request(det_pcie_sata_gpio, "det_pcie_sata_gpio");
	if (ret && ret != -EBUSY) {
		debug("gpio: requesting pin %u failed\n", det_pcie_sata_gpio);
	}

	ret = gpio_request(switch_ctrl_gpio, "switch_ctrl_gpio");
	if (ret && ret != -EBUSY) {
		debug("gpio: requesting pin %u failed\n", switch_ctrl_gpio);
	}

	gpio_direction_output(gms561_rst_gpio, 0);
	gpio_direction_input(det_pcie_sata_gpio);
	gpio_direction_output(switch_ctrl_gpio, 0);

	mdelay(10);//must delay for io detect
	if(gpio_get_value(det_pcie_sata_gpio)) {
		gpio_set_value(switch_ctrl_gpio, 0);
		mdelay(300);//must delay for nvme detect
	} else {
		gpio_set_value(switch_ctrl_gpio, 1);
		gpio_set_value(gms561_rst_gpio, 1);
	}
}


/*
 * If the root node of the DTB has a "model" property, show it.
 * Then call checkboard().
 */
int __weak show_board_info(void)
{
#ifdef CONFIG_OF_CONTROL
	DECLARE_GLOBAL_DATA_PTR;
	const char *model;

	model = fdt_getprop(gd->fdt_blob, 0, "model", NULL);

	if (model)
		printf("Model: %s\n", model);
#endif

	pcie_sata_switch();

	return checkboard();
}
