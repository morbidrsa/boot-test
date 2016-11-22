#include <sys/mount.h>
#include <sys/reboot.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
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

	umount("/dev");
	umount("/sys");
	umount("/proc");


	reboot(RB_POWER_OFF);

panic:
	exit(EXIT_FAILURE);
}
