CXX = g++
AR = ar
BIN = ../bin/dmserver
LIBRARY = ../bin/libdm.a
SOURCES = $(wildcard *.cpp) $(wildcard socket/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../lib/yaml-cpp/include/ -I../include/
CXXFLAGS += -Wall -Wextra
LDFLAGS += -L../lib/yaml-cpp/ -lyaml-cpp -lpthread

all: $(BIN) $(LIBRARY)

$(BIN): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(BIN)

$(LIBRARY): $(OBJECTS)
	$(AR) -cq $(LIBRARY) client.o \
                       config.o \
                       message.o \
                       base64.o \
                       logger.o \
                       socket/*.o \
                       block.o \
                       block_server.o \
                       block_local.o ../lib/yaml-cpp/src/*.o

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(LIBRARY) $(OBJECTS)
