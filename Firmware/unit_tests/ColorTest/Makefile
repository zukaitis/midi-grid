# Makefile for gtest examples

GOOGLE_TEST_LIB = gtest
GOOGLE_TEST_INCLUDE = /usr/src/googletest/googletest/include
SOURCE_DIR = ../../Src

G++ = g++
G++_FLAGS = -c -Wall -I $(GOOGLE_TEST_INCLUDE) -I $(SOURCE_DIR)
LD_FLAGS = -L /usr/src/googletest/googletest/lib/ -l $(GOOGLE_TEST_LIB) -l pthread

OBJECTS = ColorTest.o
TARGET = ColorTest

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

%.o : %.cpp
	$(G++) $(G++_FLAGS) $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
