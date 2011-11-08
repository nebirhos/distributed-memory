CXX = g++
BIN = client server
SOURCES = client.cpp server.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../../include/
CXXFLAGS += -Wall -Wextra
LDFLAGS += -L../../bin/ -ldm -lpthread

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(@).o $(LDFLAGS) -o $(@)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(OBJECTS)

run:
	./server 12345 &
	./client localhost 12345
	killall server
