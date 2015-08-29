#!/bin/sh
g++ -c init.c
ar -rcs varibles.a init.o
g++ init.c
./a.out
g++ Ranger.c -o R
g++ Lion.c -o L
g++ Jackal.c -o J
