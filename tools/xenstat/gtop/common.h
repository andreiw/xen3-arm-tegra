/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  Common header
 *
 *        Version:  1.0
 *        Created:  2008년 04월 16일 14시 32분 38초
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oh Inho (inho48.oh), inho48.oh@samsung.com
 *        Company:  SAMSUNG Electronics
 *
 * =====================================================================================
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <gtk/gtk.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include "mygen.h"

#define WINDOW_WIDTH	300
#define WINDOW_HEIGHT	270

#define	BTM_MAJOR_NUM  302	

#define debug(fmt, args ...) \
	g_print("<%s:%d> " fmt "\n", __FILE__, __LINE__, ##args);

#define XTOP_MAX_STEP	12

enum btm_cmd{
	SET_BATTERYLIFE,
	GET_BATTERYLIFE
};

typedef enum {
	BLACK,
	GRAY1,
	GRAY2,
	GRAY3,
	WHITE,
	GREEN1,
	GREEN2,
	YELLOW1,
	YELLOW2,
	BLUE1,
	BLUE2,
	RED,
	MAX_COLOR
} MYCOLOR;

#ifndef __MAIN
extern GtkWidget *main_window;
extern int btm_fd;
#endif

#ifndef __COMMON
extern GdkColor color[];
extern int dom0_cpu[XTOP_MAX_STEP];
extern int dom1_cpu[XTOP_MAX_STEP];
extern int dom0_mem[XTOP_MAX_STEP];
extern int dom1_mem[XTOP_MAX_STEP];
extern int num_domains;
#endif

/*
 * Prototype
 */
int start_gtop(GtkWidget *window);
void redraw_graph();
void send_redraw_event();

void array_shift_left(int array[], int size);

int open_dev_file(int major_num);

#endif
