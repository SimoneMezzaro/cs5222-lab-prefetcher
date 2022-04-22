//
// Data Prefetching Championship Simulator 2
// Seth Pugsley, seth.h.pugsley@intel.com
//

/*

	GHB G/AC prefetcher

 */

#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include "../inc/prefetcher.h"
#define IT_SIZE 102400
#define BUF_SIZE (1024*1024)
#define MAX_POINTER (BUF_SIZE*16)
#define W_DEGREE 16

struct global_history_buffer {
	std::vector <std::pair <unsigned long long int, int>> buffer;
	int head;
};

std::vector <int> IT; // index table
std::vector <unsigned long long int> IT_tag; // index table
global_history_buffer GHB; // global history buffer
unsigned long long int pref = 0;

void prefetch(int pointer, unsigned long long int addr) {
	for(int i = 0; i < W_DEGREE; i++) {
		if(pointer == -1 || abs(pointer - GHB.head) > BUF_SIZE)
			return;
		int prefetch_pointer = (pointer + 1) % BUF_SIZE;
		if(prefetch_pointer != GHB.head % BUF_SIZE) {
			pref++;
			l2_prefetch_line(0, addr, GHB.buffer[prefetch_pointer].first, FILL_L2);
		}
		pointer = GHB.buffer[prefetch_pointer].second;
	}
}

void push(int pointer, unsigned long long int addr) {
	if(pointer == -1 || abs(pointer - GHB.head) > BUF_SIZE) {
		GHB.buffer[GHB.head % BUF_SIZE] = {addr, -1};
	} else {
		GHB.buffer[GHB.head % BUF_SIZE] = {addr, pointer};
	}
	GHB.head = (GHB.head + 1) % MAX_POINTER;
}

void l2_prefetcher_initialize(int cpu_num) {
	IT.resize(IT_SIZE, -1);
	IT_tag.resize(IT_SIZE, 0);
	GHB.buffer.resize(BUF_SIZE);
	GHB.head = 0;
}

void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit) {
	if(cache_hit) {
		return;
	}
	int index = addr % IT_SIZE;
	int pointer = IT[index];
	prefetch(pointer, addr);
	IT[index] = GHB.head;
	IT_tag[index] = addr;
	push(pointer, addr);
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
