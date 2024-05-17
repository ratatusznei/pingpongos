a.out: ppos-core-aux.c pingpong-scheduler-srtf.c
	gcc ppos-core-aux.c pingpong-scheduler-srtf.c libppos_static.a
