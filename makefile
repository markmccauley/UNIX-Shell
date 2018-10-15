all: shell

Shell.o: Shell.cpp
	g++ -c -g Shell.cpp

Tokenizer.o: Tokenizer.cpp
	g++ -c -g Tokenizer.cpp

Main.o: Main.cpp
	g++ -c -g Main.cpp

shell: Main.o Shell.o Tokenizer.o
	g++ -o shell Main.o Shell.o Tokenizer.o

clean:
	rm *.o