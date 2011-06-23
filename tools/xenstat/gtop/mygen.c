/**
 *  QTop  (Qt-TOP)
 *
 *	2007.07.31. WebISpy
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "xen-interface.h"
#include "xenstat.h"
#include "mygen.h"

/**
 * Global variables
 */
xenstat_handle *xhandle = NULL;
xenstat_domain **domains;
xenstat_node *prev_node = NULL;
xenstat_node *cur_node = NULL;
struct timeval curtime, oldtime;
extern int DEBUG;
extern int DELAY;

#define debug(fmt, args...) \
	if (DEBUG) printf("[%s]" fmt "\n", __FILE__, ##args);

/**
 * @brief		Computes the CPU percentage used for a specified domain
 * 				copied from xentop.c
 * @param[in]	domain : Xen-domain
 * @return		percentage
 */
static double get_cpu_pct(xenstat_domain *domain)
{
	xenstat_domain *old_domain;
	double us_elapsed;

	/* Can't calculate CPU percentage without a previous sample. */
	if(prev_node == NULL)
		return 0.0;

	old_domain = xenstat_node_domain(prev_node, xenstat_domain_id(domain));
	if(old_domain == NULL)
		return 0.0;

	/* Calculate the time elapsed in microseconds */
	us_elapsed = ((curtime.tv_sec-oldtime.tv_sec)*1000000.0
		      +(curtime.tv_usec - oldtime.tv_usec));

	/* In the following, nanoseconds must be multiplied by 1000.0 to
	 * convert to microseconds, then divided by 100.0 to get a percentage,
	 * resulting in a multiplication by 10.0 */
	return ((xenstat_domain_cpu_ns(domain)
		 -xenstat_domain_cpu_ns(old_domain))/10.0)/us_elapsed;
}

/**
 * @brief		xenstat initialize
 * @return		0(Success), -1(Failure)
 */
int mygeninit()
{
	debug("try xenstat_init()");
	xhandle = xenstat_init();
	if (xhandle == NULL) {
		debug("Failed to initialize xenstat library\n");
		return -1;
	}
	debug("xenstat_init() success\n");

	debug("page_size=%d\n", xhandle->page_size);
	return 0;
}

/**
 * @brief		xenstat un-initialize
 * @return		none
 */
void mygenclose()
{
	debug("try xenstat_uninit()");
	xenstat_uninit(xhandle);
}

int mygen_numdomains()
{
	return xenstat_node_num_domains(cur_node);
}

/**
 * @brief		Calculate Domain0,1 CPU/MEM Usages
 * @param[in]	res : usages result values
 * @return		0(Success), -1(Failure)
 */
int mygen(mygen_result *res)
{
	unsigned int i, num_domains=0;

	if (prev_node != NULL)
		xenstat_free_node(prev_node);

	prev_node = cur_node;
	debug("Try xenstat_get_node(xhandle, XENSTAT_NETWORK | XENSTAT_VCPU)");
	cur_node = xenstat_get_node(xhandle, XENSTAT_NETWORK | XENSTAT_VCPU);
	if (cur_node == NULL) {
		debug("MYGEN: Failed to retrieve statistics from libxenstat");
		return -1;
	}

	/* Count the number of domains for which to report data */
	debug("Try xenstat_node_num_domains(cur_node)");
	num_domains = xenstat_node_num_domains(cur_node);
	domains = malloc(num_domains*sizeof(xenstat_domain *));
	if(domains == NULL) {
		debug("Failed to allocate memory");
		return -1;
	}

	debug("num_domains: %d", num_domains);

	gettimeofday(&curtime, NULL);
	debug("Delay: %lu", curtime.tv_sec - oldtime.tv_sec);

	// Get domains information
	for (i=0; i<num_domains; i++) {
		debug("Try xenstat_node_domain_by_index(cur_node, %d)", i);
		domains[i] = xenstat_node_domain_by_index(cur_node, i);

		debug("Domain-%d", i);
		debug("CPU(%%): %6.1f", get_cpu_pct(domains[i]));
		debug("MEM(%%): %6.1f (cur_mem/tot_mem*100)", 
				(double)xenstat_domain_cur_mem(domains[i]) /
				(double)xenstat_node_tot_mem(cur_node) * 100);

		debug("CPU-sec: %llu", xenstat_domain_cpu_ns(domains[i])/1000000000);
		debug("MEM-cur: %llu k", xenstat_domain_cur_mem(domains[i])/1024);
		debug("MEM-tot: %llu k", xenstat_node_tot_mem(cur_node)/1024);
		debug("MEM-max: %llu k", xenstat_domain_max_mem(domains[i])/1024);

		switch(i) {
			case 0:
				// Domain-0 info
				res->cpu0 = get_cpu_pct(domains[i]);
				res->mem0 = (double)xenstat_domain_cur_mem(domains[i]) /
					(double)xenstat_node_tot_mem(cur_node) * 100;
				break;

			case 1:
				// Domain-1 info
				res->cpu1 = get_cpu_pct(domains[i]);
				res->mem1 = (double)xenstat_domain_cur_mem(domains[i]) /
					(double)xenstat_node_tot_mem(cur_node) * 100;
				break;
		}
	}
	oldtime = curtime;

	return 0;
}
