
# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O0


# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wno-ignored-attributes

CCFLAGS = $(CCOPT)


all: BORWin


BORWin: BORWin.o Graph.o
	$(CCC) $(CCOPT) bin/BORWin.o bin/Graph.o -o BORWin

BORWin.o: BORWin.cpp BORWin.h
	$(CCC) -c $(CCOPT) BORWin.cpp -o bin/BORWin.o

Graph.o: Graph.cpp Graph.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) Graph.cpp -o bin/Graph.o
