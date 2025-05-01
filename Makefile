CXX=g++
CXXFLAGS=-g -Wall -Wextra -pedantic -std=c++17 -O3
SOURCES:=$(wildcard ./*.cpp)
HEADERS:=$(wildcard ./*.hpp)
OBJECTS:=$(SOURCES:./%.cpp=./%.o)

EXE=bms
ZIP=xwagne10.zip

.PHONY: clean pack zip tar

############################################
# PROGRAM

$(EXE): $(OBJECTS)
	$(CXX) $^ -o $@


# TODO every file has its needs (ungeneralize)
./%.o: ./%.cpp ./%.hpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

############################################
# MISC

tar: pack
zip: pack
pack: 
	rm -f $(ZIP)
	zip -r $(ZIP) $(SOURCES) $(HEADERS)	makefile

clean:
	rm -f $(OBJECTS) $(ZIP) $(EXE)
