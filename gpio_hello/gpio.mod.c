#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x17634745, "module_layout" },
	{ 0x77ecd5e7, "cdev_del" },
	{ 0x5f6174e2, "device_destroy" },
	{ 0x21e4a50b, "cdev_add" },
	{ 0x39320ce4, "cdev_init" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x2f729c13, "class_destroy" },
	{ 0xb4356af, "device_create" },
	{ 0x95d52825, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xa8f59416, "gpio_direction_output" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x91715312, "sprintf" },
	{ 0x6c8d5ae8, "__gpio_get_value" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0x27e1a049, "printk" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "4E9A2F341663B12820E2A25");
