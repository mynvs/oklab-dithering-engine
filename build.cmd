cd src
g++ -pedantic -Wall -ffast-math -I../include color_space.cpp dither.cpp dithermain.cpp image_buffer.cpp palette.cpp ppm.cpp -o ../bin/dither.exe -O3
g++ -pedantic -Wall -ffast-math -I../include subpixeler.cpp -o ../bin/subpixeler -O3
g++ -pedantic -Wall -ffast-math -I../include make_palette.cpp -o ../bin/make_palette -O3
g++ -pedantic -Wall -ffast-math -I../include make_rainbow.cpp -o ../bin/make_rainbow -O3
