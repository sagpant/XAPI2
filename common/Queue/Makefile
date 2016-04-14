include ../../Makefile.global

LFLAGS=-Wl,-R\$$ORIGIN -ldl
ALL=libQueue.so 

all: $(ALL)

libQueue.so: main.o  MsgQueue.o  RemoteQueue.o  $(DEPS)
	$(CXX) --shared $(IFLAGS) $(CFLAGS) -I../CTP_Quote -o $@ $^


