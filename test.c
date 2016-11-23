#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <scsi/sg.h>

#include "boot-test.h"

#define DEVICE "/dev/sg0"
#define SB_LEN 96

static int iterations = 1500;

enum test_result do_test(void)
{
	struct sg_io_hdr hdr;
	unsigned char cdb[16];
	unsigned char scsi_sense[SB_LEN];
	unsigned char expect[64];
	int fd;
	void *addr;
	size_t lba = 0;
	size_t block_count = 1;
	int i, ret;

	fd = open(DEVICE, O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	addr = valloc(8192);
	if (!addr) {
		perror("valloc");
		exit(EXIT_FAILURE);
	}

	memset(&hdr, 0, sizeof(hdr));
	memset(cdb, 0, sizeof(cdb));
	memset(scsi_sense, 0, sizeof(scsi_sense));
	memset(expect, 0, sizeof(expect));

	cdb[0] = (unsigned char)(0x88);
	cdb[2] = (unsigned char) ((lba >> 56) & 0xff);
	cdb[3] = (unsigned char)((lba >> 48) & 0xff);
	cdb[4] = (unsigned char)((lba >> 40) & 0xff);
	cdb[5] = (unsigned char)((lba >> 32) & 0xff);
	cdb[6] = (unsigned char)((lba >> 24) & 0xff);
	cdb[7] = (unsigned char)((lba >> 16) & 0xff);
	cdb[8] = (unsigned char)((lba >> 8) & 0xff);
	cdb[9] = (unsigned char)(lba & 0xff);
	cdb[10] = (unsigned char)((block_count >> 24) & 0xff);
	cdb[11] = (unsigned char)((block_count >> 16) & 0xff);
	cdb[12] = (unsigned char)((block_count >> 8) & 0xff);
	cdb[13] = (unsigned char)(block_count & 0xff);

	hdr.interface_id = 'S';
	hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	hdr.dxfer_len = 512;
	hdr.iovec_count = 0;
	hdr.mx_sb_len = SB_LEN;
	hdr.sbp = scsi_sense;
	hdr.timeout = 30000;
	hdr.pack_id = 0;
	hdr.usr_ptr = 0;
	hdr.cmdp = cdb;
	hdr.cmd_len = sizeof(cdb);
	hdr.dxferp = addr;

	for (i = 0; i < iterations; i ++) {
		ret = ioctl(fd, SG_IO, &hdr);
		if (ret && errno == EINTR)
			continue;
		else if (ret && errno != EINTR) {
			perror("ioctl()");
			exit(EXIT_FAILURE); /* Panic on error */
		}
		if (i == 0)
			memcpy(expect, addr, sizeof(expect));

		if (memcmp(addr, expect, sizeof(expect))) {
			printf("Fail on iteration %d\n", i);
			return TEST_FAIL;
		}
	}

	free(addr);
	close(fd);

	return TEST_PASS;
}

