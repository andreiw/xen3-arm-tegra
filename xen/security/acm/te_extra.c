/*
 * te_extra.c 
 *
 * Copyright (C) 2008 Samsung Electronics 
 *          BokDeuk Jeong  <bd.jeong@samsung.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <security/acm/te.h>
#include <security/acm/te_extra.h>

/* Event Channel Label node operations */
int te_evtchn_equal(void *x, void *y)
{
	uint64_t val_x, val_y;
	uint64_t owner_index;

	ASSERT(x);
	owner_index = ((struct EVTCHN_Label *)x)->owner_index; 
	val_x = (owner_index << 32) + ((struct EVTCHN_Label *)x)->use;
	owner_index = ((struct EVTCHN_Label *)x)->owner_index; 

	ASSERT(y);
	val_y = (owner_index << 32) + ((struct EVTCHN_Label *)y)->use;

	return (val_x == val_y ? 1: 0);
}

int te_evtchn_less(void *x, void *y)
{
	uint64_t val_x, val_y;
	uint64_t owner_index;

	ASSERT(x);
	owner_index = ((struct EVTCHN_Label *)x)->owner_index; 
	val_x = (owner_index << 32) + ((struct EVTCHN_Label *)x)->use;
	owner_index = ((struct EVTCHN_Label *)x)->owner_index; 

	ASSERT(y);
	val_y = (owner_index << 32) + ((struct EVTCHN_Label *)y)->use;

	return (val_x < val_y ? 1: 0);
}

int te_evtchn_greater(void *x, void *y)
{
	uint64_t val_x, val_y;
	uint64_t owner_index;

	ASSERT(x);
	owner_index = ((struct EVTCHN_Label *)x)->owner_index; 
	val_x = (owner_index << 32) + ((struct EVTCHN_Label *)x)->use;
	owner_index = ((struct EVTCHN_Label *)x)->owner_index; 

	ASSERT(y);
	val_y = (owner_index << 32) + ((struct EVTCHN_Label *)y)->use;

	return (val_x > val_y ? 1: 0);
}

/* Grant Table Label node operations */
int te_gnttab_equal(void *x, void *y)
{
	uint64_t val_x, val_y;
	uint64_t owner_index, mem_space;

	ASSERT(x);
	owner_index = ((struct GNTTAB_Label *)x)->owner_index; 
	mem_space = ((struct GNTTAB_Label *)x)->mem_space;
	val_x = (owner_index << 48) + 
		( mem_space << 32) + 
		((struct GNTTAB_Label *)x)->use;

	ASSERT(y);
	owner_index = ((struct GNTTAB_Label *)y)->owner_index; 
	mem_space = ((struct GNTTAB_Label *)y)->mem_space;
	val_y = (owner_index << 48) + 
		(mem_space << 32) + 
		((struct GNTTAB_Label *)y)->use;

	return (val_x == val_y ? 1: 0);
}

int te_gnttab_less(void *x, void *y)
{
	uint64_t val_x, val_y;
	uint64_t owner_index, mem_space;

	ASSERT(x);
	owner_index = ((struct GNTTAB_Label *)x)->owner_index; 
	mem_space = ((struct GNTTAB_Label *)x)->mem_space;
	val_x = (owner_index << 48) + 
		( mem_space << 32) + 
		((struct GNTTAB_Label *)x)->use;

	ASSERT(y);
	owner_index = ((struct GNTTAB_Label *)y)->owner_index; 
	mem_space = ((struct GNTTAB_Label *)y)->mem_space;
	val_y = (owner_index << 48) + 
		(mem_space << 32) + 
		((struct GNTTAB_Label *)y)->use;

	return (val_x < val_y ? 1: 0);
}

int te_gnttab_greater(void *x, void *y)
{
	uint64_t val_x, val_y;
	uint64_t owner_index, mem_space;

	ASSERT(x);
	owner_index = ((struct GNTTAB_Label *)x)->owner_index; 
	mem_space = ((struct GNTTAB_Label *)x)->mem_space;
	val_x = (owner_index << 48) + 
		( mem_space << 32) + 
		((struct GNTTAB_Label *)x)->use;

	ASSERT(y);
	owner_index = ((struct GNTTAB_Label *)y)->owner_index; 
	mem_space = ((struct GNTTAB_Label *)y)->mem_space;
	val_y = (owner_index << 48) + 
		(mem_space << 32) + 
		((struct GNTTAB_Label *)y)->use;

	return (val_x > val_y ? 1: 0);
}

/* Device Label node operations */
int te_device_equal(void *x, void *y)
{
	uint16_t val_x, val_y;

	ASSERT(x);
	val_x = ((struct DEVICE_Label *)x)->device; 

	ASSERT(y);
	val_y = ((struct DEVICE_Label *)y)->device; 

	return (val_x == val_y ? 1: 0);
}

int te_device_less(void *x, void *y)
{
	uint16_t val_x, val_y;

	ASSERT(x);
	val_x = ((struct DEVICE_Label *)x)->device; 

	ASSERT(y);
	val_y = ((struct DEVICE_Label *)y)->device; 

	return (val_x < val_y ? 1: 0);
}

int te_device_greater(void *x, void *y)
{
	uint16_t val_x, val_y;

	ASSERT(x);
	val_x = ((struct DEVICE_Label *)x)->device; 

	ASSERT(y);
	val_y = ((struct DEVICE_Label *)y)->device; 

	return (val_x > val_y ? 1: 0);
}

/* Simple Resource Label node operations */
int te_smpres_equal(void *x, void *y)
{
	uint32_t val_x, val_y;

	ASSERT(x);
	val_x = ((struct SIMPLE_RESOURCE_Label *)x)->value; 

	ASSERT(y);
	val_y = ((struct SIMPLE_RESOURCE_Label *)y)->value; 

	return (val_x == val_y ? 1: 0);
}

int te_smpres_less(void *x, void *y)
{
	uint32_t val_x, val_y;

	ASSERT(x);
	val_x = ((struct SIMPLE_RESOURCE_Label *)x)->value; 

	ASSERT(y);
	val_y = ((struct SIMPLE_RESOURCE_Label *)y)->value; 

	return (val_x < val_y ? 1: 0);
}

int te_smpres_greater(void *x, void *y)
{
	uint32_t val_x, val_y;

	ASSERT(x);
	val_x = ((struct SIMPLE_RESOURCE_Label *)x)->value; 

	ASSERT(y);
	val_y = ((struct SIMPLE_RESOURCE_Label *)y)->value; 

	return (val_x > val_y ? 1: 0);
}

int bits_shift[ACM_ETC] =
{ 	0,		// ACM_DOMAIN:  [7:0] bits out of 32 request bits in domain resource matrix
	8,		// ACM_VCPU:    [8:8] in domain resource matrix
	0, 	// ACM_SCHEDULER:      [3:0] in simple resource matrix
	4,		// ACM_PERFCOUNTER:    [4:4] in simple resource matrix
	9,		// ACM_HANDLE:  [9:9] in domain resource matrix
	10,	// ACM_DEBUG:   [10:10] in domain resource matrix
	5, 	// ACM_CONSOLEIO:      [6:5] in simple reource matrix
	7, 	// ACM_TRACEBUFFER:    [7:7] in simple resource matrix
	8,		// ACM_TIME:           [8:8] in simple resource matrix
	-1,	// ACM_PIRQ: 
			// PIRQ_CONTROL [11:11] in domain resource matrix)
			//		   PIRQ_SET_TYPE [9:9] in simple resource matrix
	-1,	// ACM_VIRQ: UNDEF
	12,	// ACM_EVTCHN:  [13:12] in domain resource matrix
			//             EVTCHN_CONTROL, EVTCHN_GET_STATE
	14,	// ACM_GRANTTAB:[16:14] in domain resource matrix
			//               GNTTAB_SETUP, GNTTAB_DUMP_TABLE
	17, 	// ACM_IOMEM:   [17:17] in domain resource matrix
	18, 	// ACM_MEMORY:  [21:18] in domain resource matrix
	10,	// ACM_POLICY:         [11:10] in simple resource matrix
	12,	// ACM_SECUSTORAGE:    [23:12] in simple resource matrix
	-1,	// ACM_DMACHN
	-1,	// UNDEF OBJECT TYPE 0x13
	-1,	// UNDEF OBJECT TYPE 0x14
	-1,	// UNDEF OBJECT TYPE 0x15
	-1,	// UNDEF OBJECT TYPE 0x16
	-1,	// UNDEF OBJECT TYPE 0x17
	-1,	// UNDEF OBJECT TYPE 0x18
	-1,	// UNDEF OBJECT TYPE 0x19
	-1,	// UNDEF OBJECT TYPE 0x1a
	-1,	// UNDEF OBJECT TYPE 0x1b
	-1,	// UNDEF OBJECT TYPE 0x1c
	-1,	// UNDEF OBJECT TYPE 0x1d
	-1,	// UNDEF OBJECT TYPE 0x1e
	-1,	// UNDEF OBJECT TYPE 0x1f
	24,	// ACM_ETC:             [ :24] in simple resource matrix
};

unsigned long shift_request(unsigned long request, unsigned int object_group)
{
	int shift_bits = 0;

	if(object_group > 0 && object_group <= ACM_ETC)
		shift_bits = bits_shift[object_group - 1];

	if(object_group == ACM_PIRQ){
		if(request == PIRQ_CONTROL) 
			shift_bits = 11;	/* [11:11] in domain resource matrix */
		else if(request == PIRQ_SET_TYPE) 
			shift_bits = 3;	/* [9:9] in simple resource matrix */
	}
	return request << shift_bits; 
}


