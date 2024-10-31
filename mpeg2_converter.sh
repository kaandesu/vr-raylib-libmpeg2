#!/bin/bash

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 input_file output_file [fps]"
  exit 1
fi

input="$1"
output="$2"
fps="${3:-29.97}"

ffmpeg -i "$input" -c:v mpeg2video -pix_fmt yuv420p -an -r "$fps" -f mpeg2video "$output" -q:v 2
