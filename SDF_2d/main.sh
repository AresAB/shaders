#!/bin/sh

texture="textures/awesomeface.png"

if [ $# -gt 0 ]; then
	texture=$1
fi

image_info=$( python3 ./image_analyzer.py $( file $texture ) $( pwd ) )

./src/main.exe $image_info $texture