#include <sys/mount.h>
#include <sys/reboot.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum test_result { TEST_PASS, TEST_FAIL };

enum test_result do_test(void)
{
	return TEST_PASS;
}

int main(void)
{
	enum test_result ret;
	int err;

	err = mount("none", "/proc", "proc", 0, NULL);
	if (err)
		goto panic;
	err = mount("none", "/sys", "sysfs", 0, NULL);
	if (err)
		goto panic;
	err = mount("none", "/dev", "devtmpfs", 0, NULL);
	if (err)
		goto panic;

	ret = do_test();
	if (ret == TEST_PASS)
		printf("PASS\n");
	else
		printf("FAIL\n");

	umount("/dev");
	umount("/sys");
	umount("/proc");

	reboot(RB_POWER_OFF);

panic:
	exit(EXIT_FAILURE);
}
