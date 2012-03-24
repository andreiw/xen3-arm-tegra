/*
 * fiqdb.h
 *
 * Copyright (C) 2012 Andrei Warkentin <andreiw@msalumni.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

struct platform_fiqdb {
	int (*init)(void);
	void (*putc)(char c);
	int (*getc)(void);
	void (*flush)(void);
};

#define FIQDB_NO_CHAR 0x00ff0000
#define FIQDB_BREAK   0x00ff0001

int fiqdb_init(void);
void fiqdb_register(struct platform_fiqdb *platform_fiqdb);
