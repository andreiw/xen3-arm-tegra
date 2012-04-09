/*
 * stacktrace.h
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

#ifndef ARM_STACKTRACE_H
#define ARM_STACKTRACE_H

struct stackframe {
	u32 fp;
	u32 sp;
	u32 pc;
};

int unwind_frame(struct stackframe *frame);
void walk_stackframe(struct stackframe *frame,
                     int (*fn)(struct stackframe *, void *), void *data);

#endif
