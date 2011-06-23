/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  GTop (Gtk Xentop)
 *
 *        Version:  1.0
 *        Created:  2008년 04월 16일 14시 33분 40초
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oh Inho (inho48.oh), inho48.oh@samsung.com
 *        Company:  SAMSUNG Electronics
 *
 * =====================================================================================
 */

#define __MAIN
#include "common.h"
#include <stdlib.h>
#include <string.h>

GtkWidget *main_window;
GtkWidget *form;
int TimerStatus;
int DEBUG;
int btm_fd;
;
void *timeout(gpointer data)
{
	mygen_result genres;

	TimerStatus = TRUE;
	while(TimerStatus) {
		sleep(2);

//		printf("TimerStatus = %d\n", TimerStatus);
#if 0
		// Update dom0_cpu, dom0_mem, dom1_cpu, dom1_mem
		array_shift_left(dom0_cpu, XTOP_MAX_STEP);
		array_shift_left(dom1_cpu, XTOP_MAX_STEP);

		dom0_cpu[XTOP_MAX_STEP-1] = rand() % 100;

		if (num_domains == 2)
			dom1_cpu[XTOP_MAX_STEP-1] = rand() % 100;
		else
			dom1_cpu[XTOP_MAX_STEP-1] = -1;
#endif
#if 1
		num_domains = mygen_numdomains();
		mygen(&genres);
		
		array_shift_left(dom0_cpu, XTOP_MAX_STEP);
		array_shift_left(dom1_cpu, XTOP_MAX_STEP);
		array_shift_left(dom0_mem, XTOP_MAX_STEP);
		array_shift_left(dom1_mem, XTOP_MAX_STEP);

		dom0_cpu[XTOP_MAX_STEP-1] = genres.cpu0;
		dom1_cpu[XTOP_MAX_STEP-1] = genres.cpu1;
		dom0_mem[XTOP_MAX_STEP-1] = genres.mem0;
		dom1_mem[XTOP_MAX_STEP-1] = genres.mem1;

		if (num_domains < 2) {
			dom1_cpu[XTOP_MAX_STEP-1] = -1;
			dom1_mem[XTOP_MAX_STEP-1] = -1;
		}
#endif

		send_redraw_event();
	}
	TimerStatus = 2;
	printf("Timer quited\n");
	return NULL;
}

int init_main_app(void)
{
	GtkWidget *mw;

	/*
	 * Main Window
	 */
	mw = gtk_main_window_new(GTK_WIN_STYLE_DEFAULT);
	main_window = mw;
	
	gtk_main_window_set_title_style(GTK_MAIN_WINDOW(mw), GTK_WIN_TITLE_STYLE_TEXT_ICON);
	gtk_window_set_title(GTK_WINDOW(mw), "GXenTop");

	/*
	 * Form
	 */
	form = gtk_form_new(FALSE);
	gtk_form_set_title(GTK_FORM(form), "GXenTop");
	gtk_main_window_add_form(GTK_MAIN_WINDOW(mw), GTK_FORM(form));
	gtk_main_window_set_current_form(GTK_MAIN_WINDOW(mw), GTK_FORM(form));

	g_print("start gtop!\n");
	start_gtop(form);
	
	gtk_widget_show_all(mw);
	gtk_window_set_focus(GTK_WINDOW(mw), GTK_BIN(form)->child);

	return 0;
}

void battery_handler(int signo, siginfo_t *info, void *context)
{
	int b_ret = FALSE;
	char domain_no[10];
	pid_t pid;
	GtkWidget *dlg;

	memset(domain_no, 0x00, sizeof(domain_no) );
	
	if(info) {
		// Show a wanring message
		//b_ret = pDlg->openCheckWindow( info->si_errno );
		dlg = gtk_message_dialog_new(
					GTK_WINDOW(main_window),
					GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"Shut down %u ?",
					info->si_errno
				);

		b_ret = gtk_dialog_run(GTK_DIALOG(dlg));
		gtk_widget_destroy(dlg);

		// if a user agree the message destroy domain ID(info->si_errno) by using "/xen_tools/dom0_util"
		//if(b_ret == TRUE) {
		if(b_ret == GTK_RESPONSE_YES) {
			char *envp[] = {
				"LD_LIBRARY_PATH=/opt/xocean/usr/local/arm-python/lib:/opt/xocean/usr/local/xen-tools/lib:/opt/xocean/lib", 
				"PATH=/usr/sbin:/bin:/usr/bin:/sbin:/usr/X11R6/bin:/sbin:/usr/xocean/bin:/opt/xocean/usr/local/xen-tools/bin:/opt/xocean/usr/local/xen-tools/sbin:/opt/xocean/usr/local/arm-python/bin"};
			sprintf(domain_no, "%u", info->si_errno);
			printf("Received domain shutdown signal. Shut down %u\n", info->si_errno);

			switch(pid = fork() ) {
				case -1: printf("Can't fork\n");
						 exit (-1);
				case 0 :
						//execl("/xen_tools/dom0_util", "dom0_util", "destroy", domain_no, (char*)0 );
						execle("/opt/xocean/usr/local/arm-python/bin/python", "python", "/opt/xocean/usr/local/xen-tools/sbin/xm", "destroy", domain_no, (char*)0, envp);
						break;
			}
		} else {
			printf("User doesn't agree this policy\n");
		}

	}
	else
		printf("Received signal without siginfo.\n");

}

int main(int argc, char *argv[])
{
	int ret;
	pthread_t thread_t;
	struct sigaction sigbt;

	// Open device file 
	btm_fd= open_dev_file( BTM_MAJOR_NUM );
	if(btm_fd < 0) {
		debug("btm device file open failed");
		//return -1;
	}

	// register a sig handler 
	sigemptyset(&sigbt.sa_mask);
	sigbt.sa_flags = SA_SIGINFO;
	sigbt.sa_sigaction = battery_handler;
	
	if(sigaction(SIGUSR1, &sigbt, NULL) < 0){
		perror("SIGACTION FAILED\n");
		exit(1);
	}

	if(sigaction(SIGUSR2, &sigbt, NULL) < 0){
		perror("SIGACTION FAILED\n");
		exit(1);
	}

	ret = mygeninit();
	if (ret != 0) {
		debug("mygeninit failed");
		return -1;
	}

	g_thread_init(NULL);

	gdk_threads_init();
	gdk_threads_enter();

	gtk_init(&argc, &argv);

#if 0
	dom0_cpu[0] = 0;
	dom0_cpu[1] = 10;
	dom0_cpu[2] = 20;
	dom0_cpu[3] = 30;
	dom0_cpu[4] = 20;
	dom0_cpu[5] = 25;
	dom0_cpu[6] = 50;
	dom0_cpu[7] = 25;
	dom0_cpu[8] = 75;
	dom0_cpu[9] = 100;

	dom1_cpu[0] = 0;
	dom1_cpu[1] = 20;
	dom1_cpu[2] = 30;
	dom1_cpu[3] = 10;
	dom1_cpu[4] = 100;
	dom1_cpu[5] = 55;
	dom1_cpu[6] = 40;
	dom1_cpu[7] = 75;
	dom1_cpu[8] = 25;
	dom1_cpu[9] = 60;
#endif

	num_domains = 2;

	debug("init_main_app");

	// GUI 
	init_main_app();

	// Timer (milliseconds)
//	gdk_threads_add_timeout(1000, timeout, NULL);

	if (pthread_create(&thread_t, NULL, timeout, NULL) < 0) {
		perror("thread create error:");
		exit(0);
	}

	gtk_main();

	pthread_join(thread_t, NULL);
	gdk_threads_leave();

	mygenclose();

	return ret;
}
