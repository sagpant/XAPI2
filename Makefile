all:
	$(MAKE) -C common/Queue all
	$(MAKE) -C apis/CTP all                                
	$(MAKE) -C tests/CPP all                                                       

clean:
	$(MAKE) -C common/Queue clean                                                      	
	$(MAKE) -C apis/CTP clean                                                       
	$(MAKE) -C tests/CPP clean                                                      
	
