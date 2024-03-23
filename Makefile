LDLIBS = -lglut -lGL -lGLU  -lm  -lX11 -lpng -lGLEW

CXXFLAGS= -O3  -std=c++11 -I/modules/cs324/glew-1.11.0/include
LDFLAGS= $(CXXFLAGS) $(LIBDIRS) -L/usr/X11R6/lib -L/modules/cs324/glew-1.11.0/lib -Wl,-rpath=/modules/cs324/glew-1.11.0/lib

TARGETS = main 

SRCS = main.cpp

OBJS =  $(SRCS:.cpp=.o)

CXX = g++

default: $(TARGETS)
