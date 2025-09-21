# oklab-dithering-engine
![Repo Size](https://img.shields.io/github/repo-size/mynvs/oklab-dithering-engine)\
A series of command line utilities that manipulate images to look dithered


## installation

### 1. install development enviroments

**WINDOWS** - https://www.msys2.org/ \
**UNIX-LIKE** - install cmake and gcc

### 2. clone github repository
- run `git clone https://github.com/mynvs/oklab-dithering-engine`
- or download .zip file

### 3. actually build the utilities

**WINDOWS - MinGW**
```
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

**Unix-like**
```
mkdir build
cd build
cmake ..
make
```
