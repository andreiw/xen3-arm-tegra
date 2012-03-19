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

struct cpio_header
{
	u8  c_magic[6];
	u64 c_ino;
	u64 c_mode;
	u64 c_uid;
	u64 c_gid;
	u64 c_nlink;
	u64 c_mtime;
	u64 c_filesize;
	u64 c_maj;
	u64 c_min;
	u64 c_rmaj;
	u64 c_rmin;
	u64 c_namesize;
	u64 c_chksum;
} __attribute__ ((packed));

struct cpio_file
{
	struct cpio_header *header;
	char *name;
	void *data;
	size_t size;
	size_t name_size;
	size_t record_size;
};

static int cpio_parse(struct cpio_header *header,
		      void *archive_end,
		      struct cpio_file *file)
{
	char buf[9];

	if (header + 1 > (struct cpio_header *) archive_end)
		return -EINVAL;

	if (memcmp(header->c_magic, "070701", 6) &&
	    memcmp(header->c_magic, "070702", 6))
		return -EINVAL;

	memcpy(buf, &header->c_filesize, 8);
	file->size = simple_strtoul(buf, NULL, 16);

	memcpy(buf, &header->c_namesize, 8);
	file->name_size = simple_strtoul(buf, NULL, 16);

	file->record_size =
		align_up(sizeof(*header) +
			 file->name_size, 4) +
		file->size;

	if (((u8 *) header) +
	    file->record_size > (u8 *) archive_end)
		return -EINVAL;

	file->header = header;
	file->name = (char *) (header + 1);

	file->data = ((u8 *) header) +
		(file->record_size - file->size);

	return 0;
}

int cpio_first(void *archive,
	       void *archive_end,
	       struct cpio_file *file)
{
	struct cpio_header *h = archive;
	return cpio_parse(h, archive_end, file);
}

int cpio_next(void *archive_end,
	      struct cpio_file *current)
{
	struct cpio_header *h = (struct cpio_header *)
		(((u8 *) current->header) +
		 align_up(current->record_size, 4));

	return cpio_parse(h, archive_end, current);
}

int bv_find(struct bv *bv,
	    char *name,
	    struct bv_file *file)
{
	struct cpio_file cpio_file;
	
	int ret = cpio_first(bv->start,
			     bv->end,
			     &cpio_file);
	while (!ret) {
		if (!strncmp(cpio_file.name, name,
			     cpio_file.name_size)) {
			file->start = cpio_file.data;
			file->end = (u8 *) cpio_file.data +
				cpio_file.size;
			return 0;
		}
		ret = cpio_next(bv->end, &cpio_file);
	}
	return ret;
}
