APPLICATION = main
CXXFLAGS = -O3 -Wall -I. -DVART_OGL -DIL_LIB -std=c++11
CXX = mpic++
LDFLAGS = 
LDLIBS = 

OBJECTS = main.o grafo.o solucao.o buscaLocal.o

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

$(APPLICATION): $(OBJECTS) main.o grafo.o solucao.o buscaLocal.o
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	rm -f *.o *~ $(APPLICATION)
