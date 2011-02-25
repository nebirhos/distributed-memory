YAML_DIR = ../../lib/yaml-cpp
# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = ../../lib/gtest

# Where to find user code.
USER_DIR = ../../src

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include -I$(USER_DIR) -I$(YAML_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = message_test

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

LDFLAGS = -L$(GTEST_DIR) -lgtest_main -lpthread -L$(YAML_DIR) -lyaml-cpp

# House-keeping build targets.

all : $(TESTS)

clean :
	rm -f $(TESTS) *.o

run :
	for i in *_test; do echo "\nRunning $$i"; ./$$i; done;

message_test.o: message_test.cpp \
                     $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c message_test.cpp

message_test : message_test.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(USER_DIR)/message.o $^ $(LDFLAGS) -o $@
