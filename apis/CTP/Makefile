CXX=clang
IFLAGS=-I../../ -I../../include/ -I../ -I./ 
CFLAGS=$(IFLAGS)-std=c++11 -O3 -fPIC -m64

DEPS=../../../XAPI2/languages/CPP/XAPI_CPP/XApiImpl.o \
	 ../../../XAPI2/languages/CPP/XAPI_CPP/XApiCpp.o \
	 ../../../XAPI2/languages/CPP/XAPI_CPP/XApiC.o \
	 ../../include/toolkit.o

DEPS+=../../common/Queue/MsgQueue.o \
	  ../../common/Queue/RemoteQueue.o \
      ../../include/ApiProcess.o

LFLAGS=-Bsymbolic -Wl,-R\$$ORIGIN -ldl -L../../include/CTP/linux64 -lthosttraderapi -lthostmduserapi 
all: ctp_quote.so ctp_trade.so 

%.o: %.cpp 
	$(CXX) $(CFLAGS) -c $^ -o $@

ctp_quote.so: Quote/main.cpp Quote/MdUserApi.cpp TypeConvert.cpp $(DEPS)
	$(CXX) --shared $(IFLAGS) $(CFLAGS) -I../CTP_Quote -o $@ $^

ctp_trade.so: Trade/main.cpp Trade/TraderApi.cpp TypeConvert.cpp $(DEPS)
	$(CXX) --shared $(IFLAGS) $(CFLAGS) -I../CTP_Trade -o $@ $^


