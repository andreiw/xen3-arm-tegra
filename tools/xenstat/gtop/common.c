/*
 * =====================================================================================
 *
 *       Filename:  common.c
 *
 *    Description:  Common functions
 *
 *        Version:  1.0
 *        Created:  2008년 04월 16일 14시 33분 30초
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oh Inho (inho48.oh), inho48.oh@samsung.com
 *        Company:  SAMSUNG Electronics
 *
 * =====================================================================================
 */

#define __COMMON
#include "common.h"

GdkColor color[] = {
	//  Red     Green   Blue
	{0, 0x0000, 0x0000, 0x0000},	// BLACK
	{0, 0x4400, 0x4400, 0x4400},	// GRAY1
	{0, 0x8800, 0x8800, 0x8800},	// GRAY2
	{0, 0xC400, 0xC400, 0xC400},	// GRAY3
	{0, 0xff00, 0xff00, 0xff00},	// WHITE
	{0, 0x0000, 0x4000, 0x0000},	// GREEN1
	{0, 0x0000, 0xff00, 0x0000},	// GREEN2
	{0, 0x8800, 0x8800, 0x0000},	// YELLOW1
	{0, 0xff00, 0xff00, 0x0000},	// YELLOW2
	{0, 0x0000, 0x0000, 0x8800},	// BLUE1
	{0, 0x0000, 0x0000, 0xff00},	// BLUE2
	{0, 0xff00, 0x0000, 0x0000},	// RED
	{0, 0x0000, 0x0000, 0x0000},
};

int dom0_cpu[XTOP_MAX_STEP];
int dom1_cpu[XTOP_MAX_STEP];
int dom0_mem[XTOP_MAX_STEP];
int dom1_mem[XTOP_MAX_STEP];
int num_domains;

/**
 * Array shift left
 *
 *  a[0] a[1] a[2]   ==>  a[0] a[1] a[2]
 *    1    2    3           2    3    0
 *
 * @param[out]	array
 * @param[in]	array size
 */
void array_shift_left(int array[], int size)
{
	int i;

	for (i=0; i<size-1; i++)
		array[i] = array[i+1];

	if (size >= 1)
		array[size-1] = 0;
}

int open_dev_file(int major_num)
{
	int fd;

	fd = open("/dev/btm", O_RDWR, S_IRUSR);
	if(fd < 0){ 
		debug("/dev/btm open failed");
		return -1;
	}

	return fd;
}
