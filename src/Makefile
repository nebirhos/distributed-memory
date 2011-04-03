CXX = g++
BIN = ../bin/dmserver
SOURCES = dmserver.cpp config.cpp server.cpp block.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CXXFLAGS = -Wall
LDFLAGS = -Llib/yaml-cpp/ -lyaml-cpp -Ilib/yaml-cpp/include/

all: $(BIN)

$(BIN): $(OBJECTS) extlibraries
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(BIN)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(LDFLAGS) $< -o $@

extlibraries:
	@echo Compiling external libraries...
	$(MAKE) -C lib/

clean:
	rm -f $(BIN) $(OBJECTS)
	$(MAKE) -C lib/ clean
