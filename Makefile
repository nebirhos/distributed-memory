CC=g++
CFLAGS=-c -Wall
LDFLAGS=-Isrc/lib/yaml-cpp/include/ -Isrc/
SOURCES=src/dmserver.cpp src/config.cpp  src/lib/yaml-cpp/src/aliascontent.cpp src/lib/yaml-cpp/src/conversion.cpp src/lib/yaml-cpp/src/emitter.cpp src/lib/yaml-cpp/src/emitterstate.cpp src/lib/yaml-cpp/src/emitterutils.cpp src/lib/yaml-cpp/src/exp.cpp src/lib/yaml-cpp/src/iterator.cpp src/lib/yaml-cpp/src/map.cpp src/lib/yaml-cpp/src/node.cpp src/lib/yaml-cpp/src/null.cpp src/lib/yaml-cpp/src/ostream.cpp src/lib/yaml-cpp/src/parser.cpp src/lib/yaml-cpp/src/parserstate.cpp src/lib/yaml-cpp/src/regex.cpp src/lib/yaml-cpp/src/scalar.cpp src/lib/yaml-cpp/src/scanner.cpp src/lib/yaml-cpp/src/scanscalar.cpp src/lib/yaml-cpp/src/scantag.cpp src/lib/yaml-cpp/src/scantoken.cpp src/lib/yaml-cpp/src/sequence.cpp src/lib/yaml-cpp/src/simplekey.cpp src/lib/yaml-cpp/src/stream.cpp src/lib/yaml-cpp/src/tag.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/main

all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(LDFLAGS) $(CFLAGS) $< -o $@

run:
	bin/main

documentation:
	doxygen doc/doxygen/config
