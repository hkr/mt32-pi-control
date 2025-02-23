all: MT32PI.X

CARGS = -DUSE_CUSTOM_DELAY -DPROGRAM_NAME=\\\"MT32PI.X\\\" 

mcdrv_midi.o: mcdrv_midi.s
	run68 has060 mcdrv_midi.s

midi_dev.o: midi_dev.c ../common_src/*.h
	gcc -I ../common_src $(CARGS) -o midi_dev.o -c midi_dev.c

getopt.o: ../common_src/getopt.c ../common_src/*.h
	gcc -I ../common_src $(CARGS) -o getopt.o -c ../common_src/getopt.c

delay.o: delay.c ../common_src/*.h
	gcc -I ../common_src $(CARGS) -o delay.o -c delay.c

mt32-pi.o: ../common_src/mt32-pi.c ../common_src/*.h
	gcc -I ../common_src $(CARGS) -o mt32-pi.o -c ../common_src/mt32-pi.c

MT32PI.X: midi_dev.o getopt.o mt32-pi.o mcdrv_midi.o delay.o
	gcc -o MT32PI.X delay.o mt32-pi.o midi_dev.o getopt.o mcdrv_midi.o 

dist: MT32PI.X
	cp MT32PI.X ../x68000_bin/

clean:
	rm *.o
	rm MT32PI.X

