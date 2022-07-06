#include <stdio.h>
#include <numa.h>
#include <sched.h>
#include <sys/sysinfo.h>

void
set_bit(struct bitmask *bm, int idx)
{
	int offset = idx / sizeof(ulong);
	idx %= sizeof(ulong);
	*(bm->maskp + offset) |= ((ulong)0x1 << idx);
}

void
reset_bit(struct bitmask *bm, int idx)
{
	int offset = idx / sizeof(ulong);
	idx %= sizeof(ulong);
	*(bm->maskp + offset) &= ~((ulong)0x1 << idx);
}

int main(int argc, char* argv[])
{
    int i;
    volatile int n = numa_available();
    volatile int m = numa_max_node();
    volatile int p = numa_num_possible_nodes();
    volatile int c = numa_num_possible_cpus();

    volatile int a = numa_num_configured_cpus();
	    volatile int b = get_nprocs();


		struct bitmask **cpu_list; //[nodes][cpu]
		//int **dsa_wq_list; //[dsa][wq]

		// get the list of all cpu cores // remove core 0 from the list
		int nb_numa_nodes = numa_num_configured_nodes();
		int nb_cpus = numa_num_configured_cpus();
		cpu_list = calloc(nb_numa_nodes, sizeof(struct bitmask*));
		if (cpu_list == NULL) return -1;

		for (i = 0; i < nb_numa_nodes; i++) {
			char cpulist[128], filename[128];
			sprintf(filename, "/sys/devices/system/node/node%d/cpulist",i);
			FILE* fp = fopen(filename, "r");
			fgets(cpulist, 128, fp);
			fclose(fp);
			cpulist[strcspn(cpulist, "\r\n")] = 0;
			cpu_list[i] = numa_parse_cpustring(cpulist);
            //*cpu_list[i]->maskp &= ~((unsigned long)0x1);
    		reset_bit(cpu_list[0], 0);

            printf("just like that");
		}


    return 0;

}