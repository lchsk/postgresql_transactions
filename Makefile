CC = g++
BIN = txn

CFLAGS += -std=c++17 -pedantic -O0 -g
LIBS = `pkg-config libpqxx --cflags --libs` -pthread -lpqxx -lpq

SRCDIR = src
SRC = \
	${SRCDIR}/main.cpp \
	${SRCDIR}/args.cpp \
	${SRCDIR}/task1.cpp \
	${SRCDIR}/pool.cpp \
	${SRCDIR}/timer.cpp \

$(BIN):
	@mkdir -p bin
	rm -f bin/$(BIN) $(OBJS)
	$(CC) $(SRC) $(CFLAGS) -o bin/$(BIN) $(LIBS)
