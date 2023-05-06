mkdir bin && cd src && g++ -pedantic -Wall -ffast-math -I../include color_space.cpp dither.cpp dithermain.cpp image_buffer.cpp palette.cpp ppm.cpp -o ../bin/dither -O3 && cd .. && bin/dither
