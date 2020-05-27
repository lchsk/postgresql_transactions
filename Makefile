CC = g++
BIN = txn

CFLAGS += -std=c++17 -pedantic -O0 -g
LIBS = `pkg-config libpqxx --cflags` -pthread -lpqxx -lpq

SRCDIR = src
SRC = \
	${SRCDIR}/main.cpp \
	${SRCDIR}/args.cpp \
	${SRCDIR}/tasks.cpp \
	${SRCDIR}/pool.cpp \
	${SRCDIR}/timer.cpp \

OBJ = main.o args.o tasks.o pool.o timer.o

all: $(OBJ) $(BIN)

%.o: src/%.cpp
	${CC} -o $@ -c $< ${CFLAGS}

clean:
	rm -f $(OBJ)
	rm -f bin/$(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin
	$(CC) $(OBJ) $(CFLAGS) -o bin/$(BIN) $(LIBS)

.PHONY: clean all
