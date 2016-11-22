#include <sys/mount.h>

#include <stdio.h>
#include <stdlib.h>

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

panic:
	exit(EXIT_FAILURE);
}
