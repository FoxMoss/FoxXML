all:
	clang++ -Wall handler.cpp main.cpp -o foxlang -lexpat
gdb:
	clang++ -Wall handler.cpp main.cpp -o foxlang -lexpat -ggdb

