PREFETCHER = GHB_PC-DC
HIDE = "true"

all: run

run:	
	$(CXX) -Wall -no-pie -o dpc2sim-stream example_prefetchers/${PREFETCHER}.cc lib/dpc2sim.a
	@if [ ${HIDE} = "true" ]; then\
		zcat traces/gcc_trace2.dpc.gz | ./dpc2sim-stream -hide_heartbeat;\
		zcat traces/GemsFDTD_trace2.dpc.gz | ./dpc2sim-stream -hide_heartbeat;\
		zcat traces/lbm_trace2.dpc.gz | ./dpc2sim-stream -hide_heartbeat;\
		zcat traces/leslie3d_trace2.dpc.gz | ./dpc2sim-stream -hide_heartbeat;\
		zcat traces/libquantum_trace2.dpc.gz | ./dpc2sim-stream -hide_heartbeat;\
		zcat traces/mcf_trace2.dpc.gz | ./dpc2sim-stream -hide_heartbeat;\
		zcat traces/milc_trace2.dpc.gz | ./dpc2sim-stream -hide_heartbeat;\
		zcat traces/omnetpp_trace2.dpc.gz | ./dpc2sim-stream -hide_heartbeat;\
	fi
	@if [ ${HIDE} = "false" ]; then\
		zcat traces/gcc_trace2.dpc.gz | ./dpc2sim-stream;\
		zcat traces/GemsFDTD_trace2.dpc.gz | ./dpc2sim-stream;\
		zcat traces/lbm_trace2.dpc.gz | ./dpc2sim-stream;\
		zcat traces/leslie3d_trace2.dpc.gz | ./dpc2sim-stream;\
		zcat traces/libquantum_trace2.dpc.gz | ./dpc2sim-stream;\
		zcat traces/mcf_trace2.dpc.gz | ./dpc2sim-stream;\
		zcat traces/milc_trace2.dpc.gz | ./dpc2sim-stream;\
		zcat traces/omnetpp_trace2.dpc.gz | ./dpc2sim-stream;\
	fi
	
clean:
	rm -rf dpc2sim-stream

.PHONY: all run clean
