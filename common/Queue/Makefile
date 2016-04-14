include ../../Makefile.global

LFLAGS=-ldl
ALL=libQueue.so 

all: $(ALL)

libQueue.so: main.o  MsgQueue.o  RemoteQueue.o  $(DEPS)
	$(CXX) --shared $(IFLAGS) $(CFLAGS) -I../CTP_Quote -o $@ $^


