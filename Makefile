OPENCV_PKG = ${shell pkg-config --cflags --libs opencv}

MAIN_FILES = main.cpp
RENDER_FILES = renderPencil.cpp

CPP = g++

main: render.o main.cpp
	@${CPP} ${MAIN_FILES} render.o -std=c++0x -o pencil_render.out  ${OPENCV_PKG}

render.o: renderPencil.cpp renderPencil.hpp render.hpp
	@${CPP} ${RENDER_FILES} -std=c++0x -c -o render.o ${OPENCV_PKG}

test:
	./pencil_render.out ./data/image1.jpg ./texture/text1.jpg

clean:
	rm -f *.o
	rm -f pencil_render.out
