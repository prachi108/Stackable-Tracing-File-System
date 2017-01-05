all: treplay trctl

treplay: treplay.c
	gcc -Wall -Werror treplay.c -o treplay

trctl: trctl.c
	gcc -Wall -Werror trctl.c -o trctl
clean:
	rm -f treplay
	rm -f trctl
