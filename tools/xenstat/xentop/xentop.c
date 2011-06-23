/*
 *  Copyright (C) International Business Machines  Corp., 2005
 *  Author(s): Judy Fischbach <jfisch@us.ibm.com>
 *             David Hendricks <dhendrix@us.ibm.com>
 *             Josh Triplett <josht@us.ibm.com>
 *    based on code from Anthony Liguori <aliguori@us.ibm.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; under version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * Some codes in this file was modified
 * for supporting ARM processor by Samsung Electronics.
 */

#include "curses.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "xenstat.h"

#define XENTOP_VERSION "1.0"

#define XENTOP_DISCLAIMER \
"Copyright (C) 2005  International Business Machines  Corp\n"\
"This is free software; see the source for copying conditions.There is NO\n"\
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
#define XENTOP_BUGSTO "Report bugs to <dsteklof@us.ibm.com>.\n"

#define _GNU_SOURCE
#include <getopt.h>

#if !defined(__GNUC__) && !defined(__GNUG__)
#define __attribute__(arg) /* empty */
#endif

#define KEY_ESCAPE '\x1B'

/*
 * Function prototypes
 */
/* Utility functions */
static void usage(const char *);
static void version(void);
static void cleanup(void);
static void fail(const char *);
static int current_row(void);
static int lines(void);
static void print(const char *, ...) __attribute__((format(printf,1,2)));
static void attr_addstr(int attr, const char *str);
static void set_delay(char *value);
static void set_prompt(char *new_prompt, void (*func)(char *));
static int handle_key(int);
static int compare(unsigned long long, unsigned long long);
static int compare_domains(xenstat_domain **, xenstat_domain **);
static unsigned long long tot_net_bytes( xenstat_domain *, int);

/* Field functions */
static int compare_state(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_state(xenstat_domain *domain);
static int compare_cpu(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_cpu(xenstat_domain *domain);
static int compare_cpu_pct(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_cpu_pct(xenstat_domain *domain);
static int compare_mem(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_mem(xenstat_domain *domain);
static void print_mem_pct(xenstat_domain *domain);
static int compare_maxmem(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_maxmem(xenstat_domain *domain);
static void print_max_pct(xenstat_domain *domain);
static int compare_vcpus(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_vcpus(xenstat_domain *domain);
static int compare_nets(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_nets(xenstat_domain *domain);
static int compare_net_tx(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_net_tx(xenstat_domain *domain);
static int compare_net_rx(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_net_rx(xenstat_domain *domain);
static int compare_ssid(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_ssid(xenstat_domain *domain);
static int compare_name(xenstat_domain *domain1, xenstat_domain *domain2);
static void print_name(xenstat_domain *domain);

/* Section printing functions */
static void do_summary(void);
static void do_header(void);
static void do_bottom_line(void);
static void do_domain(xenstat_domain *);
static void do_vcpu(xenstat_domain *);
static void do_network(xenstat_domain *);
static void top(void);

/* Field types */
typedef enum field_id {
	FIELD_DOMID,
	FIELD_NAME,
	FIELD_STATE,
	FIELD_CPU,
	FIELD_CPU_PCT,
	FIELD_MEM,
	FIELD_MEM_PCT,
	FIELD_MAXMEM,
	FIELD_MAX_PCT,
	FIELD_VCPUS,
	FIELD_NETS,
	FIELD_NET_TX,
	FIELD_NET_RX,
	FIELD_SSID
} field_id;

typedef struct field {
	field_id num;
	const char *header;
	unsigned int default_width;
	int (*compare)(xenstat_domain *domain1, xenstat_domain *domain2);
	void (*print)(xenstat_domain *domain);
} field;

field fields[] = {
	{ FIELD_NAME,    "NAME",      10, compare_name,    print_name    },
	{ FIELD_STATE,   "STATE",      6, compare_state,   print_state   },
	{ FIELD_CPU,     "CPU(sec)",  10, compare_cpu,     print_cpu     },
	{ FIELD_CPU_PCT, "CPU(%)",     6, compare_cpu_pct, print_cpu_pct },
	{ FIELD_MEM,     "MEM(k)",    10, compare_mem,     print_mem     },
	{ FIELD_MEM_PCT, "MEM(%)",     6, compare_mem,     print_mem_pct },
	{ FIELD_MAXMEM,  "MAXMEM(k)", 10, compare_maxmem,  print_maxmem  },
	{ FIELD_MAX_PCT, "MAXMEM(%)",  9, compare_maxmem,  print_max_pct },
	{ FIELD_VCPUS,   "VCPUS",      5, compare_vcpus,   print_vcpus   },
	{ FIELD_NETS,    "NETS",       4, compare_nets,    print_nets    },
	{ FIELD_NET_TX,  "NETTX(k)",   8, compare_net_tx,  print_net_tx  },
	{ FIELD_NET_RX,  "NETRX(k)",   8, compare_net_rx,  print_net_rx  },
	{ FIELD_SSID,    "SSID",       4, compare_ssid,    print_ssid    }
};

const unsigned int NUM_FIELDS = sizeof(fields)/sizeof(field);

/* Globals */
struct timeval curtime, oldtime;
xenstat_handle *xhandle = NULL;
xenstat_node *prev_node = NULL;
xenstat_node *cur_node = NULL;
field_id sort_field = FIELD_DOMID;
unsigned int first_domain_index = 0;
unsigned int delay = 3;
int show_vcpus = 0;
int show_networks = 0;
int repeat_header = 0;
#define PROMPT_VAL_LEN 80
char *prompt = NULL;
char prompt_val[PROMPT_VAL_LEN];
int prompt_val_len = 0;
void (*prompt_complete_func)(char *);

/*
 * Function definitions
 */

/* Utility functions */

/* Print usage message, using given program name */
static void usage(const char *program)
{
	printf("Usage: %s [OPTION]\n"
	       "Displays ongoing information about xen vm resources \n\n"
	       "-h, --help           display this help and exit\n"
	       "-V, --version        output version information and exit\n"
	       "-d, --delay=SECONDS  seconds between updates (default 3)\n"
	       "-n, --networks       output vif network data\n"
	       "-r, --repeat-header  repeat table header before each domain\n"
	       "-v, --vcpus          output vcpu data\n"
	       "\n" XENTOP_BUGSTO,
	       program);
	return;
}

/* Print program version information */
static void version(void)
{
	printf("xentop " XENTOP_VERSION "\n"
	       "Written by Judy Fischbach, David Hendricks, Josh Triplett\n"
	       "\n" XENTOP_DISCLAIMER);
}

/* Clean up any open resources */
static void cleanup(void)
{
	if(!isendwin())
		endwin();
	if(prev_node != NULL)
		xenstat_free_node(prev_node);
	if(cur_node != NULL)
		xenstat_free_node(cur_node);
	if(xhandle != NULL)
		xenstat_uninit(xhandle);
}

/* Display the given message and gracefully exit */
static void fail(const char *str)
{
	if(!isendwin())
		endwin();
	fprintf(stderr, str);
	exit(1);
}

/* Return the row containing the cursor. */
static int current_row(void)
{
	int y, x;
	getyx(stdscr, y, x);
	return y;
}

/* Return the number of lines on the screen. */
static int lines(void)
{
	int y, x;
	getmaxyx(stdscr, y, x);
	return y;
}

/* printf-style print function which calls printw, but only if the cursor is
 * not on the last line. */
static void print(const char *fmt, ...)
{
	va_list args;

	if(current_row() < lines()-1) {
		va_start(args, fmt);
		vw_printw(stdscr, fmt, args);
		va_end(args);
	}
}

/* Print a string with the given attributes set. */
static void attr_addstr(int attr, const char *str)
{
	attron(attr);
	addstr(str);
	attroff(attr);
}

/* Handle setting the delay from the user-supplied value in prompt_val */
static void set_delay(char *value)
{
	int new_delay;
	new_delay = atoi(value);
	if(new_delay > 0)
		delay = new_delay;
}

/* Enable prompting mode with the given prompt string; call the given function
 * when a value is available. */
static void set_prompt(char *new_prompt, void (*func)(char *))
{
	prompt = new_prompt;
	prompt_val[0] = '\0';
	prompt_val_len = 0;
	prompt_complete_func = func;
}

/* Handle user input, return 0 if the program should quit, or 1 if not */
static int handle_key(int ch)
{
	if(prompt == NULL) {
		/* Not prompting for input; handle interactive commands */
		switch(ch) {
		case 'n': case 'N':
			show_networks ^= 1;
			break;
		case 'r': case 'R':
			repeat_header ^= 1;
			break;
		case 's': case 'S':
			sort_field = (sort_field + 1) % NUM_FIELDS;
			break;
		case 'v': case 'V':
			show_vcpus ^= 1;
			break;
		case KEY_DOWN:
			first_domain_index++;
			break;
		case KEY_UP:
			if(first_domain_index > 0)
				first_domain_index--;
			break;
		case 'd': case 'D':
			set_prompt("Delay(sec)", set_delay);
			break;
		case 'q': case 'Q': case KEY_ESCAPE:
			return 0;
		}
	} else {
		/* Prompting for input; handle line editing */
		switch(ch) {
		case '\r':
			prompt_complete_func(prompt_val);
			set_prompt(NULL, NULL);
			break;
		case KEY_ESCAPE:
			set_prompt(NULL, NULL);
			break;
		case KEY_BACKSPACE:
			if(prompt_val_len > 0)
				prompt_val[--prompt_val_len] = '\0';
		default:
			if((prompt_val_len+1) < PROMPT_VAL_LEN
			   && isprint(ch)) {
				prompt_val[prompt_val_len++] = (char)ch;
				prompt_val[prompt_val_len] = '\0';
			}
		}
	}

	return 1;
}

/* Compares two integers, returning -1,0,1 for <,=,> */
static int compare(unsigned long long i1, unsigned long long i2)
{
	if(i1 < i2)
		return -1;
	if(i1 > i2)
		return 1;
	return 0;
}

/* Comparison function for use with qsort.  Compares two domains using the
 * current sort field. */
static int compare_domains(xenstat_domain **domain1, xenstat_domain **domain2)
{
	return fields[sort_field].compare(*domain1, *domain2);
}

/* Field functions */

/* Compare domain names, returning -1,0,1 for <,=,> */
int compare_name(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return strcasecmp(xenstat_domain_name(domain1), xenstat_domain_name(domain2));
}

/* Prints domain name */
void print_name(xenstat_domain *domain)
{
	print("%10s", xenstat_domain_name(domain));
}

struct {
	unsigned int (*get)(xenstat_domain *);
	char ch;
} state_funcs[] = {
	{ xenstat_domain_dying,    'd' },
	{ xenstat_domain_shutdown, 's' },
	{ xenstat_domain_blocked,  'b' },
	{ xenstat_domain_crashed,  'c' },
	{ xenstat_domain_paused,   'p' },
	{ xenstat_domain_running,  'r' }
};
const unsigned int NUM_STATES = sizeof(state_funcs)/sizeof(*state_funcs);

/* Compare states of two domains, returning -1,0,1 for <,=,> */
static int compare_state(xenstat_domain *domain1, xenstat_domain *domain2)
{
	unsigned int i, d1s, d2s;
	for(i = 0; i < NUM_STATES; i++) {
		d1s = state_funcs[i].get(domain1);
		d2s = state_funcs[i].get(domain2);
		if(d1s && !d2s)
			return -1;
		if(d2s && !d1s)
			return 1;
	}
	return 0;
}

/* Prints domain state in abbreviated letter format */
static void print_state(xenstat_domain *domain)
{
	unsigned int i;
	for(i = 0; i < NUM_STATES; i++)
		print("%c", state_funcs[i].get(domain) ? state_funcs[i].ch
		                                       : '-');
}

/* Compares cpu usage of two domains, returning -1,0,1 for <,=,> */
static int compare_cpu(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return -compare(xenstat_domain_cpu_ns(domain1),
			xenstat_domain_cpu_ns(domain2));
}

/* Prints domain cpu usage in seconds */
static void print_cpu(xenstat_domain *domain)
{
	print("%10llu", xenstat_domain_cpu_ns(domain)/1000000000);
}

/* Computes the CPU percentage used for a specified domain */
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

static int compare_cpu_pct(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return -compare(get_cpu_pct(domain1), get_cpu_pct(domain2));
}

/* Prints cpu percentage statistic */
static void print_cpu_pct(xenstat_domain *domain)
{
	print("%6.1f", get_cpu_pct(domain));
}

/* Compares current memory of two domains, returning -1,0,1 for <,=,> */
static int compare_mem(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return -compare(xenstat_domain_cur_mem(domain1),
	                xenstat_domain_cur_mem(domain2));
}

/* Prints current memory statistic */
static void print_mem(xenstat_domain *domain)
{
	print("%10llu", xenstat_domain_cur_mem(domain)/1024);
}

/* Prints memory percentage statistic, ratio of current domain memory to total
 * node memory */
static void print_mem_pct(xenstat_domain *domain)
{
	print("%6.1f", (double)xenstat_domain_cur_mem(domain) /
	               (double)xenstat_node_tot_mem(cur_node) * 100);
}

/* Compares maximum memory of two domains, returning -1,0,1 for <,=,> */
static int compare_maxmem(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return -compare(xenstat_domain_max_mem(domain1),
	                xenstat_domain_max_mem(domain2));
}

/* Prints maximum domain memory statistic in KB */
static void print_maxmem(xenstat_domain *domain)
{
	unsigned long long max_mem = xenstat_domain_max_mem(domain);
	if(max_mem == ((unsigned long long)-1))
		print("%10s", "no limit");
	else
		print("%10llu", max_mem/1024);
}

/* Prints memory percentage statistic, ratio of current domain memory to total
 * node memory */
static void print_max_pct(xenstat_domain *domain)
{
	if (xenstat_domain_max_mem(domain) == (unsigned long long)-1)
		print("%9s", "n/a");
	else
		print("%9.1f", (double)xenstat_domain_max_mem(domain) /
		               (double)xenstat_node_tot_mem(cur_node) * 100);
}

/* Compares number of virtual CPUs of two domains, returning -1,0,1 for
 * <,=,> */
static int compare_vcpus(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return -compare(xenstat_domain_num_vcpus(domain1),
	                xenstat_domain_num_vcpus(domain2));
}

/* Prints number of virtual CPUs statistic */
static void print_vcpus(xenstat_domain *domain)
{
	print("%5u", xenstat_domain_num_vcpus(domain));
}

/* Compares number of virtual networks of two domains, returning -1,0,1 for
 * <,=,> */
static int compare_nets(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return -compare(xenstat_domain_num_networks(domain1),
	                xenstat_domain_num_networks(domain2));
}

/* Prints number of virtual networks statistic */
static void print_nets(xenstat_domain *domain)
{
	print("%4u", xenstat_domain_num_networks(domain));
}

/* Compares number of total network tx bytes of two domains, returning -1,0,1
 * for <,=,> */
static int compare_net_tx(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return -compare(tot_net_bytes(domain1, FALSE),
	                tot_net_bytes(domain2, FALSE));
}

/* Prints number of total network tx bytes statistic */
static void print_net_tx(xenstat_domain *domain)
{
	print("%8llu", tot_net_bytes(domain, FALSE)/1024);
}

/* Compares number of total network rx bytes of two domains, returning -1,0,1
 * for <,=,> */
static int compare_net_rx(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return -compare(tot_net_bytes(domain1, TRUE),
	                tot_net_bytes(domain2, TRUE));
}

/* Prints number of total network rx bytes statistic */
static void print_net_rx(xenstat_domain *domain)
{
	print("%8llu", tot_net_bytes(domain, TRUE)/1024);
}

/* Gets number of total network bytes statistic, if rx true, then rx bytes
 * otherwise tx bytes
 */
static unsigned long long tot_net_bytes(xenstat_domain *domain, int rx_flag)
{
	int i = 0;
	xenstat_network *network;
	unsigned num_networks = 0;
	unsigned long long total = 0;

	/* How many networks? */
	num_networks = xenstat_domain_num_networks(domain);

	/* Dump information for each network */
	for (i=0; i < num_networks; i++) {
		/* Next get the network information */
		network = xenstat_domain_network(domain,i);
		if (rx_flag)
			total += xenstat_network_rbytes(network);
		else
			total += xenstat_network_tbytes(network);
	}

	return total;
}

/* Compares security id (ssid) of two domains, returning -1,0,1 for <,=,> */
static int compare_ssid(xenstat_domain *domain1, xenstat_domain *domain2)
{
	return compare(xenstat_domain_ssid(domain1),
		       xenstat_domain_ssid(domain2));
}

/* Prints ssid statistic */
static void print_ssid(xenstat_domain *domain)
{
	print("%4u", xenstat_domain_ssid(domain));
}

/* Section printing functions */
/* Prints the top summary, above the domain table */
void do_summary(void)
{
#define TIME_STR_LEN 9
	const char *TIME_STR_FORMAT = "%H:%M:%S";
	char time_str[TIME_STR_LEN];
	const char *ver_str;
	unsigned run = 0, block = 0, pause = 0,
	         crash = 0, dying = 0, shutdown = 0;
	unsigned i, num_domains = 0;
	unsigned long long used = 0;
	xenstat_domain *domain;

	/* Print program name, current time, and number of domains */
	strftime(time_str, TIME_STR_LEN, TIME_STR_FORMAT,
	         localtime(&curtime.tv_sec));
	num_domains = xenstat_node_num_domains(cur_node);
	ver_str = xenstat_node_xen_version(cur_node);
	print("xentop - %s   Xen %s\n", time_str, ver_str);

	/* Tabulate what states domains are in for summary */
	for (i=0; i < num_domains; i++) {
		domain = xenstat_node_domain_by_index(cur_node,i);
		if (xenstat_domain_running(domain)) run++;
		else if (xenstat_domain_blocked(domain)) block++;
		else if (xenstat_domain_paused(domain)) pause++;
		else if (xenstat_domain_shutdown(domain)) shutdown++;
		else if (xenstat_domain_crashed(domain)) crash++;
		else if (xenstat_domain_dying(domain)) dying++;
	}

	print("%u domains: %u running, %u blocked, %u paused, "
	      "%u crashed, %u dying, %u shutdown \n",
	      num_domains, run, block, pause, crash, dying, shutdown);

	used = xenstat_node_tot_mem(cur_node)-xenstat_node_free_mem(cur_node);

	/* Dump node memory and cpu information */
	print("Mem: %lluk total, %lluk used, %lluk free    "
	      "CPUs: %u @ %lluMHz\n",
	      xenstat_node_tot_mem(cur_node)/1024, used/1024,
	      xenstat_node_free_mem(cur_node)/1024,
	      xenstat_node_num_cpus(cur_node),
	      xenstat_node_cpu_hz(cur_node)/1000000);
}

/* Display the top header for the domain table */
void do_header(void)
{
	field_id i;

	/* Turn on REVERSE highlight attribute for headings */
	attron(A_REVERSE);
	for(i = 0; i < NUM_FIELDS; i++) {
		if(i != 0)
			print(" ");
		/* The BOLD attribute is turned on for the sort column */
		if(i == sort_field)
			attron(A_BOLD);
		print("%*s", fields[i].default_width, fields[i].header);
		if(i == sort_field)
			attroff(A_BOLD);
	}
	attroff(A_REVERSE);
	print("\n");
}

/* Displays bottom status line or current prompt */
void do_bottom_line(void)
{
	move(lines()-1, 2);

	if (prompt != NULL) {
		printw("%s: %s", prompt, prompt_val);
	} else {
		addch(A_REVERSE | 'D'); addstr("elay  ");

		/* network */
		addch(A_REVERSE | 'N');
		attr_addstr(show_networks ? COLOR_PAIR(1) : 0, "etworks");
		addstr("  ");

		/* vcpus */
		addch(A_REVERSE | 'V');
		attr_addstr(show_vcpus ? COLOR_PAIR(1) : 0, "CPUs");
		addstr("  ");

		/* repeat */
		addch(A_REVERSE | 'R');
		attr_addstr(repeat_header ? COLOR_PAIR(1) : 0, "epeat header");
		addstr("  ");

		/* sort order */
		addch(A_REVERSE | 'S'); addstr("ort order  ");

		addch(A_REVERSE | 'Q'); addstr("uit  ");
	}
}

/* Prints Domain information */
void do_domain(xenstat_domain *domain)
{
	unsigned int i;
	for(i = 0; i < NUM_FIELDS; i++) {
		if(i != 0)
			print(" ");
		if(i == sort_field)
			attron(A_BOLD);
		fields[i].print(domain);
		if(i == sort_field)
			attroff(A_BOLD);
	}
	print("\n");
}

/* Output all vcpu information */
void do_vcpu(xenstat_domain *domain)
{
	int i = 0;
	unsigned num_vcpus = 0;
	xenstat_vcpu *vcpu;

	print("VCPUs(sec): ");

	num_vcpus = xenstat_domain_num_vcpus(domain);

	/* for all online vcpus dump out values */
	for (i=0; i< num_vcpus; i++) {
		vcpu = xenstat_domain_vcpu(domain,i);

		if (xenstat_vcpu_online(vcpu) > 0) {
			if (i != 0 && (i%5)==0)
				print("\n        ");
			print(" %2u: %10llus", i, 
					xenstat_vcpu_ns(vcpu)/1000000000);
		}
	}
	print("\n");
}

/* Output all network information */
void do_network(xenstat_domain *domain)
{
	int i = 0;
	xenstat_network *network;
	unsigned num_networks = 0;

	/* How many networks? */
	num_networks = xenstat_domain_num_networks(domain);

	/* Dump information for each network */
	for (i=0; i < num_networks; i++) {
		/* Next get the network information */
		network = xenstat_domain_network(domain,i);

		print("Net%d RX: %8llubytes %8llupkts %8lluerr %8lludrop  ",
		      i,
		      xenstat_network_rbytes(network),
		      xenstat_network_rpackets(network),
		      xenstat_network_rerrs(network),
		      xenstat_network_rdrop(network));

		print("TX: %8llubytes %8llupkts %8lluerr %8lludrop\n",
		      xenstat_network_tbytes(network),
		      xenstat_network_tpackets(network),
		      xenstat_network_terrs(network),
		      xenstat_network_tdrop(network));
	}
}

static void top(void)
{
	xenstat_domain **domains;
	unsigned int i, num_domains = 0;

	/* Now get the node information */
	if (prev_node != NULL)
		xenstat_free_node(prev_node);
	prev_node = cur_node;
	cur_node = xenstat_get_node(xhandle, XENSTAT_ALL);
	if (cur_node == NULL)
		fail("Failed to retrieve statistics from libxenstat\n");

	/* dump summary top information */
	do_summary();

	/* Count the number of domains for which to report data */
	num_domains = xenstat_node_num_domains(cur_node);

	domains = malloc(num_domains*sizeof(xenstat_domain *));
	if(domains == NULL)
		fail("Failed to allocate memory\n");

	for (i=0; i < num_domains; i++)
		domains[i] = xenstat_node_domain_by_index(cur_node, i);

	/* Sort */
	qsort(domains, num_domains, sizeof(xenstat_domain *),
	      (int(*)(const void *, const void *))compare_domains);

	if(first_domain_index >= num_domains)
		first_domain_index = num_domains-1;

	for (i = first_domain_index; i < num_domains; i++) {
		if(current_row() == lines()-1)
			break;
		if (i == first_domain_index || repeat_header)
			do_header();
		do_domain(domains[i]);
		if (show_vcpus)
			do_vcpu(domains[i]);
		if (show_networks)
			do_network(domains[i]);
	}

	do_bottom_line();
}

int main(int argc, char **argv)
{
	int opt, optind = 0;
	int ch = ERR;

	struct option lopts[] = {
		{ "help",          no_argument,       NULL, 'h' },
		{ "version",       no_argument,       NULL, 'V' },
		{ "networks",      no_argument,       NULL, 'n' },
		{ "repeat-header", no_argument,       NULL, 'r' },
		{ "vcpus",         no_argument,       NULL, 'v' },
		{ "delay",         required_argument, NULL, 'd' },
		{ 0, 0, 0, 0 },
	};
	const char *sopts = "hVbnvd:";

	if (atexit(cleanup) != 0)
		fail("Failed to install cleanup handler.\n");

	while ((opt = getopt_long(argc, argv, sopts, lopts, &optind)) != -1) {
		switch (opt) {
		case 'h':
		case '?':
		default:
			usage(argv[0]);
			exit(0);
		case 'V':
			version();
			exit(0);
		case 'n':
			show_networks = 1;
			break;
		case 'r':
			repeat_header = 1;
			break;
		case 'v':
			show_vcpus = 1;
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		}
	}

	/* Get xenstat handle */
	xhandle = xenstat_init();
	if (xhandle == NULL)
		fail("Failed to initialize xenstat library\n");

	/* Begin curses stuff */
	initscr();
	start_color();
	cbreak();
	noecho();
	nonl();
	keypad(stdscr, TRUE);
	halfdelay(5);
	use_default_colors();
	init_pair(1, -1, COLOR_YELLOW);

	do {
		gettimeofday(&curtime, NULL);
		if(ch != ERR || (curtime.tv_sec - oldtime.tv_sec) >= delay) {
			clear();
			top();
			oldtime = curtime;
			refresh();
		}
		ch = getch();
	} while (handle_key(ch));

	/* Cleanup occurs in cleanup(), so no work to do here. */

	return 0;
}
