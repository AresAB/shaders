#!/bin/sh

x=1440
y=900

if [ $# -gt 1 ]; then
	x=$1
	y=$2
fi

scrn_x_scale=$((x * 2))
scrn_y_scale=$((y * 2))

image_info=$( python3 ./image_analyzer.py $( pwd ) )

./src/main.exe $x $y $scrn_x_scale $scrn_y_scale $image_info
