/*
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <xen/kernel.h>
#include <xen/config.h>
#include <xen/string.h>
#include <xen/errno.h>
#include <xen/lib.h>
#include <xen/bv.h>

#define C_MAGIC    (0)
#define C_INO      (6)
#define C_MODE     (14)
#define C_UID      (22)
#define C_GID      (30)
#define C_NLINK    (38)
#define C_MTIME    (46)
#define C_FILESIZE (54)
#define C_MAJ      (62)
#define C_MIN      (70)
#define C_RMAJ     (78)
#define C_RMIN     (86)
#define C_NAMESIZE (94)
#define C_CHKSUM   (102)
#define CPIO_HSZ   (110)

struct cpio_file
{
	void *header;
	char *name;
	void *data;
	size_t size;
	size_t name_size;
	size_t record_size;
};

static int cpio_parse(u8 *header,
		      u8 *archive_end,
		      struct cpio_file *file)
{
	char buf[9];

	if (header + CPIO_HSZ > archive_end)
		return -EINVAL;

	if (memcmp(header + C_MAGIC, "070701", 6) &&
	    memcmp(header + C_MAGIC, "070702", 6))
		return -EINVAL;

	memcpy(buf, header + C_FILESIZE, 8);
	buf[8] = '\0';
	file->size = simple_strtoul(buf, NULL, 16);

	memcpy(buf, header + C_NAMESIZE, 8);
	buf[8] = '\0';
	file->name_size = simple_strtoul(buf, NULL, 16);

	file->record_size =
		ROUND_UP(CPIO_HSZ +
			 file->name_size, 4) +
		file->size;

	if (header +
	    file->record_size > archive_end) {
		printk(header);
		return -EINVAL;
        }

	file->header = header;
	file->name = header + CPIO_HSZ;

	file->data = header +
		file->record_size -
		file->size;

	return 0;
}

int cpio_first(u8 *archive,
	       u8 *archive_end,
	       struct cpio_file *file)
{
	return cpio_parse(archive, archive_end, file);
}

int cpio_next(u8 *archive_end,
	      struct cpio_file *current)
{
	u8 *h = current->header +
		ROUND_UP(current->record_size, 4);
	return cpio_parse(h, archive_end, current);
}

int bv_find(struct bv *bv,
	    char *name,
	    struct bv_file *file)
{
	struct cpio_file cpio_file;
	
	int ret = cpio_first((u8 *) bv->start,
			     (u8 *) bv->end,
			     &cpio_file);
	while (!ret) {
		if (!strncmp(cpio_file.name, name,
			     cpio_file.name_size)) {
			file->start = cpio_file.data;
			file->end = (u8 *) cpio_file.data +
				cpio_file.size;
			return 0;
		}
		ret = cpio_next((u8 *) bv->end, &cpio_file);
	}
	return ret;
}
