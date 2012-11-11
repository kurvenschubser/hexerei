run: src/program.o
	$(LD) -o run src/program.o
	chmod +x run
	./run
	rm run

src/program.o: src/program.c
	$(CC) -c src/program.c
	