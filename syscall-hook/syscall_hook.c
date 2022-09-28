#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm-generic/unistd.h>
#include "resolve_kallsyms.h"
#include "set_page_flags.h"
#include "direct_syscall_hook.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sizzle");
MODULE_DESCRIPTION("syscall table hook on arm64, no ftrace");
MODULE_VERSION("1.0");

static asmlinkage int (*orig_ioctl) (const struct pt_regs *);

asmlinkage int new_ioctl(const struct pt_regs *regs) {
    unsigned int fd = (unsigned int)regs->regs[0];
    pr_info("debug: ioctl called fd: %d\n", fd);
    return orig_ioctl(regs);
}

struct direct_syscall_hook hook = {__NR_ioctl, new_ioctl, &orig_ioctl};

static int __init hook_test_mod_init(void) {
    pr_info("debug: module loaded\n");
    hook_syscall(&hook);
    return 0;
}

static void __exit hook_test_mod_exit(void) {
    pr_info("debug: module unloaded\n");
}


module_init(hook_test_mod_init);
module_exit(hook_test_mod_exit);
