CXX = g++
AR = ar
LIBRARY = yaml-cpp/libyaml-cpp.a
SOURCES = yaml-cpp/src/aliascontent.cpp yaml-cpp/src/conversion.cpp yaml-cpp/src/emitter.cpp yaml-cpp/src/emitterstate.cpp yaml-cpp/src/emitterutils.cpp yaml-cpp/src/exp.cpp yaml-cpp/src/iterator.cpp yaml-cpp/src/map.cpp yaml-cpp/src/node.cpp yaml-cpp/src/null.cpp yaml-cpp/src/ostream.cpp yaml-cpp/src/parser.cpp yaml-cpp/src/parserstate.cpp yaml-cpp/src/regex.cpp yaml-cpp/src/scalar.cpp yaml-cpp/src/scanner.cpp yaml-cpp/src/scanscalar.cpp yaml-cpp/src/scantag.cpp yaml-cpp/src/scantoken.cpp yaml-cpp/src/sequence.cpp yaml-cpp/src/simplekey.cpp yaml-cpp/src/stream.cpp yaml-cpp/src/tag.cpp
OBJECTS = $(SOURCES:.cpp=.o)
LDFLAGS = -Iyaml-cpp/include/
CXXFLAGS = -Wall

all: $(LIBRARY)

.cpp.o:
	$(CXX) -c $(CFLAGS) $(LDFLAGS) $< -o $@

$(LIBRARY): $(OBJECTS)
	ar -cq $(LIBRARY) $(OBJECTS)

clean:
	rm -f $(LIBRARY) $(OBJECTS)
