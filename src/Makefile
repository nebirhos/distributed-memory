CXX = g++
BIN = ../bin/dmserver
SOURCES = dmserver.cpp config.cpp server.cpp block.cpp message.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../lib/yaml-cpp/include/ -I../include/
CXXFLAGS += -Wall -Wextra
LDFLAGS += -L../lib/yaml-cpp/ -lyaml-cpp -lpthread

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(BIN)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(OBJECTS)
