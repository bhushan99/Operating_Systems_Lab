#!/bin/bash 
gcc page.c
./a.out
gnuplot << EOF 
reset
set term postscript
set output "FIFO.ps"
set title "FIFO"
set xlabel "Frames"
set ylabel "Pagefaults"
set autoscale
plot "FIFO.txt" with linespoints
EOF
gnuplot << EOF 
reset
set term postscript
set output "LRU.ps"
set title "LRU"
set xlabel "Frames"
set ylabel "Pagefaults"
set autoscale
plot "LRU.txt" with linespoints
EOF
gnuplot << EOF 
reset
set term postscript
set output "LFU.ps"
set title "LFU"
set xlabel "Frames"
set ylabel "Pagefaults"
set autoscale
plot "LFU.txt" with linespoints
EOF
gnuplot << EOF 
reset
set term postscript
set output "IICHANCE.ps"
set title "IICHANCE"
set xlabel "Frames"
set ylabel "Pagefaults"
set autoscale
plot "IICHANCE.txt" with linespoints
EOF