CXX = g++
CXXFLAGS = -O0 -g -Wall -std=c++17 -DDEBUG

SRC = a.cpp b.cpp c.cpp
OBJ = ${SRC:.cpp=.o}

all: a b c

.o: ${OBJ}
	${CXX} -c ${CXXFLAGS} $<

a: a.o
	${CXX} -o $@ a.o ${LDFLAGS}

b: b.o
	${CXX} -o $@ b.o ${LDFLAGS}

c: c.o
	${CXX} -o $@ c.o ${LDFLAGS}

clean:
	rm -f main ${OBJ}