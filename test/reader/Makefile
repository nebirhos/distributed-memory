CXX = g++
BIN = reader1 reader2
SOURCES = reader1.cpp reader2.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../../include/
CXXFLAGS += -Wall -Wextra
LDFLAGS += -L../../bin/ -ldm -lpthread -lcrypto

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(@).o $(LDFLAGS) -o $(@)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(OBJECTS)
	rm -f *.log

run:
	./serverctl start
	./readertest
	./serverctl stop

