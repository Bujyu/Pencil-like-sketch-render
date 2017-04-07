OPENCV_PKG = ${shell pkg-config --cflags --libs opencv}
LOCAL_DIR = -I ~/local/include/ -L ~/local/lib/
#FLTK_LNK = -lfltk  -lXext -lX11 -lXft -lXinerama -lm
CPP_FILES = test.cpp

all:
	g++ ${LOCAL_DIR} $(CPP_FILES) -std=c++0x -o test  ${OPENCV_PKG}

clean:
	rm -rf test
