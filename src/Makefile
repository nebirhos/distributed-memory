CXX = g++
AR = ar
BIN = ../bin/dmserver
LIBRARY = ../bin/libdm.a
SOURCES = dmserver.cpp \
          config.cpp \
          server.cpp \
          block.cpp \
          block_server.cpp \
          message.cpp \
          base64.cpp \
          client.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../lib/yaml-cpp/include/ -I../include/
CXXFLAGS += -Wall -Wextra
LDFLAGS += -L../lib/yaml-cpp/ -lyaml-cpp -lpthread

all: $(BIN) $(LIBRARY)

$(BIN): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(BIN)

$(LIBRARY): $(OBJECTS)
	$(AR) -cq $(LIBRARY) client.o config.o ../lib/yaml-cpp/src/*.o

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(LIBRARY) $(OBJECTS)
