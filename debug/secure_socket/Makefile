CXX = g++
BIN = client server
SOURCES = client.cpp server.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../../include/
CXXFLAGS += -Wall -Wextra
LDFLAGS += -L../../bin/ -ldm -lpthread -lcrypto
DEBUG_FLAG = 

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(@).o $(LDFLAGS) -o $(@)

.cpp.o:
	$(CXX) $(DEBUG_FLAG) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(OBJECTS)

run:
	./server 12345 &
	./client localhost 12345
	killall server

key_generate:
	openssl genrsa -out serverkey.priv 2048
	openssl rsa -in serverkey.priv -pubout -out serverkey.pub

key_check:
	openssl rsa -text -in serverkey.priv
