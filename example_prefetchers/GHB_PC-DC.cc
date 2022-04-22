//
// Data Prefetching Championship Simulator 2
// Seth Pugsley, seth.h.pugsley@intel.com
//

/*

	GHB PC/DC prefetcher

 */

#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <list>
#include "../inc/prefetcher.h"
#define IT_SIZE 512
#define BUF_SIZE 512
#define MAX_POINTER (BUF_SIZE*16)
#define DEGREE 4

struct global_history_buffer {
	std::vector <std::pair <unsigned long long int, int>> buffer;
	int head;
};

std::vector <int> IT; // index table
global_history_buffer GHB; // global history buffer

/**
Manage the prefetching caused by a cache miss.
pointer is the pointer retrieved from the index table;
addr is the address causing the cache miss
**/
void prefetch(int pointer, unsigned long long int addr) {
	std::list <long long> delta_sequence; // stores the sequence of deltas used to compute the prefetch addresses
	std::pair <long long, long long> pattern; // delta pair to match in the linked list
	unsigned long long int prev_address; // auxiliar variable storing the previous address encountered in the list
	bool match = false; // auxiliar variable which is turned to true when a matching fro the pattern is found
	
	// compute the second delta of the pair
	if(pointer == -1 || abs(pointer - GHB.head) > BUF_SIZE) // checks if the pointer is valid (i.e. the pointed element hasn't been overwritten)
		return;
	pattern.second = GHB.buffer[pointer % BUF_SIZE].first - addr;
	prev_address = GHB.buffer[pointer % BUF_SIZE].first;
	pointer = GHB.buffer[pointer % BUF_SIZE].second;
	
	// compute the first delta of the pair
	if(pointer == -1 || abs(pointer - GHB.head) > BUF_SIZE)
		return;
	pattern.first = GHB.buffer[pointer % BUF_SIZE].first - prev_address;
	prev_address = GHB.buffer[pointer % BUF_SIZE].first;
	pointer = GHB.buffer[pointer % BUF_SIZE].second;
	
	// push the two deltas in the sequence
	delta_sequence.push_front(pattern.second);
	delta_sequence.push_front(pattern.first);
	
	// scan the linked list to find two consecutive deltas matching the pattern
	for(int i = 0; i < BUF_SIZE && !match; i++) {
		if(pointer == -1 || abs(pointer - GHB.head) > BUF_SIZE)
			return;
		long long new_delta = GHB.buffer[pointer % BUF_SIZE].first - prev_address;
		prev_address = GHB.buffer[pointer % BUF_SIZE].first;
		if(new_delta == pattern.first && delta_sequence.front() == pattern.second) { // the pattern is matched
			match = true;
			delta_sequence.pop_front(); // pops the front element of the sequence beacuse it belongs to the matched pattern
		} else { // the pattern is not matched
			delta_sequence.push_front(new_delta); // push the new delta on the sequence
			if(delta_sequence.size() > DEGREE + 1) // no more than degree + 1 deltas are stored in the sequence. In this way when a
				delta_sequence.pop_back();		   // match is found an element is removed and at most degree elements are in the sequence
		}
		pointer = GHB.buffer[pointer % BUF_SIZE].second;
		
	}
	
	// prefecth data based on the deltas in the sequence
	if(!match)
		return;
	for(int i = 0; i < DEGREE && !delta_sequence.empty(); i++) {
		l2_prefetch_line(0, addr, addr - delta_sequence.front(), FILL_L2);
		delta_sequence.pop_front();
	}

}

/**
Update the GHB after a cache miss.
pointer is the pointer retrieved from the index table;
addr is the address causing the cache miss
**/
void push(int pointer, unsigned long long int addr) {
	if(pointer == -1 || abs(pointer - GHB.head) > BUF_SIZE) { // if the pointer is invalid a new element pointing to nothing
		GHB.buffer[GHB.head % BUF_SIZE] = {addr, -1};		  // is added
	} else {
		GHB.buffer[GHB.head % BUF_SIZE] = {addr, pointer};
	}
	GHB.head = (GHB.head + 1) % MAX_POINTER; // increment head pointer
}

void l2_prefetcher_initialize(int cpu_num) {
	IT.resize(IT_SIZE, -1);
	GHB.buffer.resize(BUF_SIZE);
	GHB.head = 0;
}

void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit) {
	if(cache_hit) {
		return;
	}
	int index = ip % IT_SIZE;
	int pointer = IT[index];
	prefetch(pointer, addr);
	IT[index] = GHB.head; // update index table
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
  printf("Prefetcher final stats\n");
}
