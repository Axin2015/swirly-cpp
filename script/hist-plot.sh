#!/bin/sh
set -eo pipefail

files=
for file in "$@"; do
    name=$(basename -s .hdr $file)
    files+="'$file' using 4:1 with lines title \"${name}\", "
done

gnuplot << EOF
set terminal pngcairo size 1280,300
set logscale x
set grid
set ylabel "Micros"
set xtics ("0%%" 1.0, "90%%" 10.0, "99%%" 100.0, "99.9%%" 1000.0, "99.99%%" 10000.0, "99.999%%" 100000.0, "99.9999%%" 1000000.0)
set xlabel "Latency"
set key top left
plot $files
EOF
