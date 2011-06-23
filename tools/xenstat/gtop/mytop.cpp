#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include "mytop.h"

#define MAX_PROC_SHOW	7
#define MAX_PROC_NAME	255

PROCESS_SHOW proc_show[MAX_PROC_SHOW];

void clear_process_show_list()
{
	int i;

	for (i=0; i<MAX_PROC_SHOW; i++) {
		proc_show[i].pid = 0;
		memset(proc_show[i].pname, 0, MAX_PROC_NAME);
		proc_show[i].pct = 0;
	}
}

void scroll_down_from(int from)
{
	int i;

	for (i=MAX_PROC_SHOW-1; i>=from; i--) {
		memcpy(&proc_show[i], &proc_show[i-1], sizeof(PROCESS_SHOW));
	}
}

void insert_process_show_list(int pid, char *pname, float pct)
{
	int i;

	for (i=0; i<MAX_PROC_SHOW; i++) {
		if (pct > proc_show[i].pct) {
			scroll_down_from(i);
			proc_show[i].pid = pid;
			strcpy(proc_show[i].pname, pname);
			proc_show[i].pct = pct;
			break;
		}
	}
}

void show_process_list()
{
	int i;

	for (i=0; i<MAX_PROC_SHOW; i++) {
//		printf("%d [%4d] %%%3.2f %s\n", i, proc_show[i].pid, proc_show[i].pct, proc_show[i].pname); 
	}
}


void get_cpu_stat(struct cpudata *cpu)
{
	FILE *file;

	/* CPU */
	file = fopen("/proc/stat", "r");
	fscanf(file, "cpu  %lu %lu %lu %lu %lu %lu %lu %lu", 
			&cpu->user, &cpu->nice, &cpu->system, &cpu->idle,
			&cpu->iowait, &cpu->irq, &cpu->softirq, &cpu->steal);
	fclose(file);

	cpu->total = cpu->user + cpu->system + cpu->idle + cpu->nice
				+ cpu->iowait + cpu->irq + cpu->softirq + cpu->steal;
	cpu->busy = cpu->total - cpu->idle - cpu->iowait;

}

void get_proc_cpu_stat(struct proc_cpudata *proc, int pid)
{
	FILE *file;
	unsigned long itemp;
	char ctemp;
	char line[255];

	sprintf(line, "/proc/%d/stat", pid);
	file = fopen(line, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Error: process (pid %d) not found!\n", pid);
		return;
	}
	fgets(line, 255, file);
	fclose(file);

	memset(proc->pname, 0, 255);
	sscanf(line, "%d %c%s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu "
			"%ld %ld %ld %ld %ld %ld %lu %lu %ld",
			(int*)&itemp, &ctemp, proc->pname, &ctemp, (int*)&itemp, (int*)&itemp, (int*)&itemp, (int*)&itemp, (int*)&itemp,
			&itemp, &itemp, &itemp, &itemp, &itemp, &proc->userproc, &proc->systemproc,
			&itemp, &itemp, &itemp, &itemp, &itemp, &itemp, &itemp, &proc->vmem, &proc->rmem
		  );
	proc->pname[strlen(proc->pname)-1] = '\0';
}

int get_top_info(struct cpudata *cpu, struct cpudata *oldcpu, map<int, unsigned long> &pmap)
{
	DIR *dirp;
	struct dirent *dire;
	pid_t pid;
	struct proc_cpudata proc;
	unsigned long period;
	float percent;
	unsigned long lasttimes;

	dirp = opendir("/proc");
	if (dirp == NULL)
		return -1;

	clear_process_show_list();

	/*
	 * get cpu load information
	 * 	/proc/stat
	 */
	get_cpu_stat(cpu);
	period = (cpu->user + cpu->nice + cpu->system + cpu->idle) 
		- (oldcpu->user + oldcpu->nice + oldcpu->system + oldcpu->idle);

	/*
	 * scan all processes
	 * 	/proc/nnn
	 */
	while (1) {
		dire = readdir(dirp);
		if (dire == NULL)
			break;

		if (!isdigit(dire->d_name[0]))
			continue;

		/*
		 * get pid
		 * 	/proc/xxx
		 */
		pid = atoi(dire->d_name);

		/*
		 * get /proc/xxx/stat information
		 */
		get_proc_cpu_stat(&proc, pid);

		/*
		 * get process cpu load percentage
		 */
		lasttimes = pmap[pid];
		percent = (float)(proc.userproc + proc.systemproc - lasttimes);
		percent *= 100;
		percent /= (float)period;

		if (percent != 0) {
/*			
			printf("[%4d] %20s : tottick(%5lu), lasttimes(%5lu), tot-last(%5lu), period(%5lu), percent(%f)\n", pid, proc.pname,
					proc.userproc + proc.systemproc,
					lasttimes, 
					proc.userproc+proc.systemproc-lasttimes,
					period,
					percent);
*/
			insert_process_show_list(pid, proc.pname, percent);
		}

		lasttimes = (proc.userproc + proc.systemproc);
		pmap[pid] = lasttimes;
	}

	closedir(dirp);
	return 0;
}
