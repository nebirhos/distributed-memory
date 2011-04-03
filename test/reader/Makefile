CXX = g++
BIN = reader
SOURCES = reader.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../../include/
CXXFLAGS += -Wall -Wextra
LDFLAGS += -L../../bin/ -ldm -lpthread

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(BIN)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(OBJECTS)
