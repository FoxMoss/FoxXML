COMPILER = g++
all:
	$(COMPILER) -Wall handler.cpp main.cpp -o foxlang -lexpat
gdb:
	$(COMPILER) -Wall handler.cpp main.cpp -o foxlang -lexpat -ggdb
