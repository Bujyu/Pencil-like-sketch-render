OPENCV_PKG = ${shell pkg-config --cflags --libs opencv}
CPP_FILES = test.cpp

all:
	g++ $(CPP_FILES) -std=c++0x -o test.out  ${OPENCV_PKG}

clean:
	rm -rf test.out
