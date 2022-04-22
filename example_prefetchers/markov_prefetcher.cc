//
// Data Prefetching Championship Simulator 2
// Seth Pugsley, seth.h.pugsley@intel.com
//

/*

	Markov prefetcher

 */

#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <list>
#include <iostream>
#include "../inc/prefetcher.h"
#define IT_SIZE 128000
#define W_DEGREE 4

std::vector <std::list <unsigned long long int> > IT; // index table
int last = -1;
unsigned long long int pref = 0;

void l2_prefetcher_initialize(int cpu_num) {
	IT.resize(IT_SIZE);
}

void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit) {
	if(cache_hit) {
		return;
	}
	int index = addr % IT_SIZE;
	for(std::list<unsigned long long int>::iterator i = IT[index].begin(); i != IT[index].end(); i++) {
		l2_prefetch_line(0, addr, *i, FILL_L2);
		pref++;
	}
	if(last != -1) {
		IT[last].push_back(addr);
		if(IT[last].size() > W_DEGREE)
			IT[last].pop_front();
	}
	last = index;
}

void l2_cache_fill(int cpu_num, unsigned long long int addr, int set, int way, int prefetch, unsigned long long int evicted_addr)
{
  // uncomment this line to see the information available to you when there is a cache fill event
  //printf("0x%llx %d %d %d 0x%llx\n", addr, set, way, prefetch, evicted_addr);
}

void l2_prefetcher_heartbeat_stats(int cpu_num)
{
  printf("Prefetcher heartbeat stats\n");
}

void l2_prefetcher_warmup_stats(int cpu_num)
{
  printf("Prefetcher warmup complete stats\n\n");
}

void l2_prefetcher_final_stats(int cpu_num)
{
	std::cout << pref << std::endl;
  printf("Prefetcher final stats\n");
}
