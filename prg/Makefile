IDIR := src
SRCDIR := src
ODIR := obj
LDIR :=

BLACKTDIR = ../blackt
LIBMDDIR = ../libmd

AR=ar
CXX=g++
# Compile only
CDEFINES = -DBLACKT_ENABLE_LIBPNG
#CLIBS = -lpng
CFLAGS = -std=gnu++11 -O2 -Wall -L${LIBMDDIR} -lmd -L${BLACKTDIR} -lblackt -lpng
CINCLUDES = -I${BLACKTDIR}/src -I${LIBMDDIR}/src
CXXFLAGS=$(CFLAGS) $(CDEFINES) $(CINCLUDES) -I$(IDIR)

#SRC := $(wildcard $(SRCDIR)/*/*.cpp)
#OBJ := $(patsubst $(SRCDIR)/%,$(ODIR)/%,$(patsubst %.cpp,%.o,$(SRC)))
#DEP := $(patsubst %.o,%.d,$(OBJ))
#LIB := libmd.a

LIBDEPS := $(LIBMDDIR)/libmd.a $(BLACKTDIR)/libblackt.a

TOOLSRCS := $(wildcard $(SRCDIR)/*.cpp)
#TOOLS = test opbextr opbinsr opbfileextr opbfileinsr dsimgconv
TOOLSINDIR := $(patsubst %.cpp,%,$(TOOLSRCS))
TOOLS := $(notdir $(TOOLSINDIR))

all: blackt libmd $(TOOLS)

blackt:
	cd ${BLACKTDIR} && $(MAKE) && cd $(CURDIR)

libmd:
	cd ${LIBMDDIR} && $(MAKE) && cd $(CURDIR)
	
#$(SRCDIR)/%: blackt libmd
#	$(CXX) $@.cpp $(OBJ) -o $(notdir $@) $(CXXFLAGS)

test: $(SRCDIR)/test.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

grpdmp: $(SRCDIR)/grpdmp.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

smfontrip: $(SRCDIR)/smfontrip.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

smfontinsr: $(SRCDIR)/smfontinsr.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

srecpatch: $(SRCDIR)/srecpatch.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

grpunmap: $(SRCDIR)/grpunmap.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

tilemapper: $(SRCDIR)/tilemapper.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

datinjct: $(SRCDIR)/datinjct.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

datpatch: $(SRCDIR)/datpatch.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

wfmtrip: $(SRCDIR)/wfmtrip.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

logorender: $(SRCDIR)/logorender.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

logounrender: $(SRCDIR)/logounrender.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

grpundmp: $(SRCDIR)/grpundmp.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

smscriptrip: $(SRCDIR)/smscriptrip.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

scriptgen: $(SRCDIR)/scriptgen.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

smscriptinsr: $(SRCDIR)/smscriptinsr.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

romprep: $(SRCDIR)/romprep.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

smfontrender: $(SRCDIR)/smfontrender.cpp $(LIBDEPS)
	$(CXX) $(SRCDIR)/$@.cpp $(OBJ) -o $@ $(CXXFLAGS)

.PHONY: blackt libmd cleanme clean

cleanme:
	rm -f $(TOOLS)

clean: cleanme
#	rm -f $(LIB)
#	rm -rf $(ODIR)
	cd ${BLACKTDIR} && $(MAKE) clean && cd $(CURDIR)
	cd ${LIBMDDIR} && $(MAKE) clean && cd $(CURDIR)