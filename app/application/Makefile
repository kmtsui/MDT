CXX=g++
LD=g++

CXXFLAGS += -Wall -std=c++11 -g $(shell root-config --cflags)\
			-I$(WCSIMDIR)/include\
			-I${MDTROOT}/cpp/include\
			-I${WCRDROOT}/include

LDFLAGS += $(shell root-config --ldflags) $(shell root-config --libs) -lTreePlayer\
		   -L$(WCSIMDIR) -lWCSimRoot\
		   -L${MDTROOT}/cpp -lMDT\
		   -L${WCRDROOT} -lWCRData

.PHONY: clean Execs

appWCTESingleEvent: appWCTESingleEvent.o PMTResponse3inchR12199_02.o
	$(RM) $@
	$(LD) $^ $(LDFLAGS) -o $@
	@echo "$@ done"


%o::%cc
	$(CXX) $(CXXFLAGS) -c $? -o $@

clean:
	$(RM) *.o
