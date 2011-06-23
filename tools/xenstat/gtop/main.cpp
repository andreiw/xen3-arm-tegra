#include <qapplication.h>
#include "BatteryDLG.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <limits.h>
#include <linux/fs.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <qwidget.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qslider.h>

class QSlider;

#define	MAJOR_NUM 253

BatteryDlg *pDlg;
// End of the definition for the Battery Manager

/*
extern void *te_generate_policy(char *policy_file, int *size);
extern void *prop_generate_policy(char *policy_file, int *size);

enum btm_cmd{
	SET_BATTERYLIFE,
	GET_BATTERYLIFE
};
*/

int open_dev_file(int major_num)
{
	int fd;

READ_FILE:
	fd = open("/dev/btm", O_RDWR, S_IRUSR);
	if(fd < 0){ 
		printf("major_num:%d\n", major_num);   

		if(mknod("/dev/btm", S_IFCHR, MKDEV(major_num,0))<0){
			printf("\t unable to create device file /dev/btm\n");
			return -1;
		}else
			goto READ_FILE;
	}

	return fd;
}

void repaint()
{
	pDlg->hide();
	pDlg->show();
}

void test()
{
	bool b_ret = false;
	char domain_no[10];
	unsigned int errno22 = 2;
	int status, died;
	pid_t pid;
	
	memset(domain_no, 0x00, sizeof(domain_no) );
	
	// Show a wanring message
	b_ret = pDlg->openCheckWindow( errno22 );

	// if a user agree the message destroy domain ID(info->si_errno) by using "/xen_tools/dom0_util"
	if(b_ret == true) {
		sprintf(domain_no, "%u", errno22);
		printf("Received domain shutdown signal. Shut down %u\n", errno22);

		switch(pid = fork() ) {
			case -1: printf("Can't fork\n");
					 exit (-1);
			case 0 :
					execl("/xen_tools/dom0_util", "dom0_util", "destroy", domain_no, (char*)0 );
					break;
		}
	} else {
		printf("User doesn't agree this policy\n");
	}

	repaint();
}
/**
Show a warning message to a user and confirm shutdown cpu rate or destroy domain
Now(070815) its policy is destroy a domain.
info->si_errno is a domain ID.
So it will destroy info->si_errno domain ID when a user agree the message
*/

void battery_handler(int signo, siginfo_t *info, void *context)
{
	bool b_ret = false;
	char domain_no[10];
	int status, died;
	pid_t pid;
	
	memset(domain_no, 0x00, sizeof(domain_no) );
	
	if(info) {
		// Show a wanring message
		b_ret = pDlg->openCheckWindow( info->si_errno );

		// if a user agree the message destroy domain ID(info->si_errno) by using "/xen_tools/dom0_util"
		if(b_ret == true) {
			sprintf(domain_no, "%u", info->si_errno);
			printf("Received domain shutdown signal. Shut down %u\n", info->si_errno);

			switch(pid = fork() ) {
				case -1: printf("Can't fork\n");
						 exit (-1);
				case 0 :
						execl("/xen_tools/dom0_util", "dom0_util", "destroy", domain_no, (char*)0 );
						break;
			}
		} else {
			printf("User doesn't agree this policy\n");
		}

		repaint();
	}
	else
		printf("Received signal without siginfo.\n");

}

int main( int argc, char ** argv )
{
	QApplication a( argc, argv );
	BatteryDlg *w = new BatteryDlg(0, "Battery", TRUE, Qt::WStyle_Customize | Qt::WStyle_NoBorder);
	int fd;
	struct sigaction sigbt;
	unsigned int batterylife = 100;

	// set the globalized variable to Battery dialog
	pDlg = new BatteryDlg(0, "Battery", TRUE, Qt::WStyle_Customize | Qt::WStyle_NoBorder);
	pDlg = w;
	
	// SetMask to Widget
	w->setBackgroundPixmap(QPixmap("/opt/Qtopia/img/battery.png"));

	//w->setAutoMask(TRUE);
	a.setMainWidget( w );

	// Open device file 
	fd= open_dev_file( MAJOR_NUM );
	if(fd < 0)
		return -1;
	
	// register a sig handler 
	sigemptyset(&sigbt.sa_mask);
	sigbt.sa_flags = SA_SIGINFO;
	
	// below code is not complied by g++ or arm-linux-g++ 
	//sigbt.sa_handler = battery_handler;
	
	// test code in arm-linux-g++, compile success 
	sigbt.sa_sigaction = battery_handler;
	
	if(sigaction(SIGUSR1, &sigbt, NULL) < 0){
		perror("SIGACTION FAILED\n");
		exit(1);
	}

	w->slider1->setValue(100);
	w->show();
	a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

	/* set file descriptor to Battery Manager */
	
	w->setFileDescriptor( fd ) ;
	
	// Show a wanring message
	//test();

	return a.exec();
}
