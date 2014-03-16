TARGET = GPDD

CC = g++

CFLAGS = -g

LFLAGS = -O6 -lfl

OBJS = Analysis.o Ckt.o main.o GPDD.o Graph.o HashGraph.o HashGPDDNode.o HashReduction.o

SRCS = Analysis.cpp Ckt.cpp main.cpp GPDD.cpp Graph.cpp HashGraph.cpp HashGPDDNode.cpp HashReduction.cpp

HEADERS = Analysis.h Ckt.h GPDD.h GPDD_struct.h GPDDNode.h Graph.h HashGraph.h HashGPDDNode.h HashReduction.h

OBJP = SpScanner.o SpParser.o

HEADP = SpParser.hpp

FLEXSPAWN = SpScanner.cpp

BISONSPAWN = position.hh stack.hh location.hh SpParser.hpp SpParser.cpp


.cpp.o:
	$(CC) $(CFLAGS) -c $<
.cc.o:
	$(CC) $(CFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGET)

$(OBJS): $(HEADERS) $(HEADP) Makefile

$(OBJP): $(FLEXSPAWN) $(BISONSPAWN)

$(TARGET): $(OBJS) $(OBJP)
	$(CC) -o $@ $(OBJS) $(OBJP) $(LFLAGS)

$(BISONSPAWN) : SpParser.yy
	bison -d SpParser.yy

$(FLEXSPAWN) : SpParser.l
	flex SpParser.l

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f $(OBJP)
	rm -f $(HEADP)
	rm -f $(FLEXSPAWN)
	rm -f $(BISONSPAWN)
	rm -f gmon.out
	rm -f *.profile
	rm -f *.output *~
	rm -f ../test/*~
	rm -f *.res
