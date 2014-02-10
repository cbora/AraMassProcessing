CXX =`root-config --cxx`
CXXFLAGS =`root-config --cflags`
ROOTCFLAGS = `root-config --cflags --libs`
ROOTLDFLAGS = `root-config --ldflags --libs --glibs`
# Linux 5, 64
#FFTLIBS=/net/user/hagar/ARA1/tools/include
#FFTLIBS=/home/cbora/Software/Install/FFTW/include/
#ARA_INC_DIR=/ARA/AraRoot/branches/TestBed1/AraEvent/
ARA_INC_DIR = /net/user/hagar/ARA1/tools/include
LDFLAGS += -L/net/user/hagar/ARA1/tools/lib -l AraEvent -l RootFftwWrapper -l fftw3 -l gsl -L . -lsqlite3
CXXFLAGS += -I ${ARA_INC_DIR} -I ${ARA_INC_DIR2} ${ROOTCFLAGS} -O2 -fPIC -pthread


#all: make01
make01: makeARAL0L1.icrr.cxx
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

