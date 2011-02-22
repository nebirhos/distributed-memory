CXX = g++
BIN = ../bin/dmserver
SOURCES = dmserver.cpp config.cpp server.cpp block.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../lib/yaml-cpp/include/
CXXFLAGS += -Wall
LDFLAGS += -L../lib/yaml-cpp/ -lyaml-cpp -lpthread

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(BIN)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(OBJECTS)
