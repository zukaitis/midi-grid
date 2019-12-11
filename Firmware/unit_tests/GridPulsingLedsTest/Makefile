# Makefile for gtest examples

GOOGLE_TEST_LIB = gtest
GOOGLE_MOCK_LIB = gmock
GOOGLE_TEST_INCLUDE = /usr/src/googletest/googletest/include
GOOGLE_MOCK_INCLUDE = /usr/src/googletest/googlemock/include
SOURCE_DIR = ../../Src

G++ = g++
G++_FLAGS = -c -Wall -I $(GOOGLE_TEST_INCLUDE) -I $(GOOGLE_MOCK_INCLUDE) -I $(SOURCE_DIR) -I $(SOURCE_DIR)/configs -I ../../submodules/FreeRTOS-Kernel/include -I ../../submodules/etl/include -I ../ -I ../mocks -I /usr/src/googletest/googlemock
LD_FLAGS = -L /usr/lib/ -l $(GOOGLE_TEST_LIB) -l pthread -l $(GOOGLE_MOCK_LIB)

OBJECTS = GridPulsingLedsTest.o
OBJECTS += PulsingLeds.o
OBJECTS += Color.o
TARGET = GridPulsingLedsTest

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

%.o : %.cpp
	$(G++) $(G++_FLAGS) $<

PulsingLeds.o : $(SOURCE_DIR)/io/grid/PulsingLeds.cpp
	$(G++) $(G++_FLAGS) $<
	
Color.o : $(SOURCE_DIR)/types/Color.cpp
	$(G++) $(G++_FLAGS) $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
