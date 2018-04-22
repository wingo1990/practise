NAME = zbase
CFILES = plist.c 
CPPFILES = 
TESTFILES = zbase_test.cc

CC = gcc
CXX = g++
CFLAGS = -g -Werror -Wall -fPIC 
CXXFLAGS = $(CFLAGS)

OBJECTS = $(CFILES:.c=.o) $(CPPFILES:.cc=.o)
TESTOBJECTS = $(TESTFILES:.cc=.o)

TARGET = lib$(NAME).so
TEST_TARGET = $(NAME)_test

#TEST files install in /usr/local/include and /usr/local/lib, compiler can find it
TEST_LIBS = -lgtest_main -lgtest -lpthread

.PHONY: all clean test

$(TARGET) : $(OBJECTS) 
	$(CC) $(CFLAGS) -shared -o $(TARGET) $(OBJECTS)

$(TEST_TARGET): $(TARGET) $(TESTOBJECTS)	
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TESTOBJECTS) $(TEST_LIBS) -L. -l$(NAME)

test: $(TEST_TARGET) 
	@echo "#!/bin/bash" > lib_test.sh
	@echo "export LD_LIBRARY_PATH=./" >> lib_test.sh
	@echo "./$(TEST_TARGET)" >> lib_test.sh
	@sh lib_test.sh
	
clean:
	@rm -f $(OBJECTS) $(TESTOBJECTS) $(TARGET) $(TEST_TARGET) lib_test.sh 

install:
	@cp -f *.so /usr/local/lib64/
