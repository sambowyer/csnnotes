all: spy calibration 

spy: spy.c
	gcc -march=native -o $@ $@.c
	
calibration: calibration.c
	gcc -std=gnu11 -O2 -o $@ $@.c
	