#////////////////////////////////////////////////////////////
#//
#// File:  Makefile
#// Authors:  G. Fotiades, A. Koutmos
#// Contributors:
#// Last modified: 2/2/11
#//
#// Description:  Makefile, targets:
#//               Linux Build:
#//                 - make
#//                 - make all
#//                 - make linux
#//               OS X Build:
#//                 - make mac
#//               Cleanup:
#//                 - make clean
#//                 - make realclean
#//
#////////////////////////////////////////////////////////////

LDLIBS = -lglut -lGLU -lGL
MACLDLIBS = -framework GLUT -framework OpenGL -framework Cocoa


SOURCE_C = *.cpp
OBJ_FILES = *.o
BINARY = out

CC =       gcc
CXX =      g++

COMPILER_FLAGS = -g

all: $(SOURCE_H) $(SOURCE_C)
	$(CXX) $(COMPILER_FLAGS) -c $(SOURCE_C)
	$(CXX) $(LDLIBS) -o $(BINARY) $(OBJ_FILES)

mac: $(SOURCE_H) $(SOURCE_C)
	$(CXX) $(COMPILER_FLAGS) -c $(SOURCE_C)
	$(CXX) $(MACLDLIBS) -o $(BINARY) $(OBJ_FILES)

clean:
	rm -rf $(OBJ_FILES) $(BINARY) ptrepository SunWS_cache .sb ii_files core 2> /dev/null

realclean:       clean
