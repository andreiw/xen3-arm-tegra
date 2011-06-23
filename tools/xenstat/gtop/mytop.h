#include <map>

using namespace std;

#ifndef __MYTOP_H__
#define __MYTOP_H__

typedef struct _PROCESS_SHOW {
	int	pid;
	char pname[255];
	float pct;
} PROCESS_SHOW;

struct cpudata
{
	unsigned long user, system, idle, nice;
	unsigned long iowait, irq, softirq, steal;
	unsigned long total, busy;
};

struct proc_cpudata
{
	unsigned long userproc, systemproc, vmem, rmem;
	char pname[255];
};

void get_cpu_stat(struct cpudata *cpu);
void get_proc_cpu_stat(struct proc_cpudata *proc, int pid);
long percentages(int cnt, int *out, long *newv, long *oldv, long *diffs);
int get_top_info(struct cpudata *cpu, struct cpudata *old_cpu, map<int, unsigned long> &pmap);

void show_process_list();
#endif
