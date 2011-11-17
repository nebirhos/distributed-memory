CXX = g++
BIN = client server
SOURCES = client.cpp server.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CPPFLAGS += -I../../include/
CXXFLAGS += -Wall -Wextra -g
LDFLAGS += -L../../bin/ -ldm -lpthread -lcrypto

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(@).o $(LDFLAGS) -o $(@)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(BIN) $(OBJECTS)

key_generate:
	openssl genrsa -out serverkey.priv 2048
	openssl rsa -in serverkey.priv -pubout -out serverkey.pub

key_check:
	openssl rsa -text -in serverkey.priv
