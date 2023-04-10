INCLUDE_DIRS = -I/usr/include/opencv4
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video -lrt

PRODUCT= adas 
HFILES= ped-detect.h follow.h lanedetect.h traffic.h
CFILES= adas.cpp  ped-detect.cpp follow.cpp lanedetect.cpp traffic.cpp

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.cpp=.o}

all: ${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}


adas: adas.o ped-detect.o follow.o lanedetect.o traffic.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o ped-detect.o follow.o lanedetect.o traffic.o -lpthread `pkg-config --libs opencv4` $(LIBS)

depend:

.cpp.o: $(SRCS)
	$(CC) $(CFLAGS) -c $<

