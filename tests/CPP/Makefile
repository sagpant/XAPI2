CXX=clang++
C=clang
IFLAGS=-I../../ -I../../include/ -I../ -I./ 
CFLAGS=$(IFLAGS)-std=c++11 -O3 -fPIC -pthread -m64
DEPS=../../languages/CPP/XAPI_CPP/XApiImpl.o \
	 ../../languages/CPP/XAPI_CPP/XApiCpp.o \
	 ../../languages/CPP/XAPI_CPP/XApiC.o \
	 ../../include/toolkit.o

DEPS+=../../common/Queue/MsgQueue.o \
	  ../../common/Queue/RemoteQueue.o \
      ../../include/ApiProcess.o

LFLAGS=-Bsymbolic -Wl,-R\$$ORIGIN -ldl -L../../include/CTP/linux64 -lthosttraderapi -lthostmduserapi

all: CPP_TEST libQuantBox_CTP_Quote.so libQuantBox_CTP_Trade.so 

%.o: %.cpp 
	$(CXX) $(CFLAGS) -c $^ -o $@

libQuantBox_CTP_Quote.so libQuantBox_CTP_Trade.so:
	ln -s ../../apis/CTP/ctp_quote.so libQuantBox_CTP_Quote.so
	ln -s ../../apis/CTP/ctp_trade.so libQuantBox_CTP_Trade.so
	ln -s ../../include/CTP/linux64/libthosttraderapi.so
	ln -s ../../include/CTP/linux64/libthostmduserapi.so

CPP_TEST: CPP_TEST.o $(DEPS)
	$(CXX)  $(IFLAGS) $(CFLAGS) $(LFLAGS) -I../CTP_Trade -o $@ $^

clean:
	rm $(DEPS) *.o
