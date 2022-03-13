cc= g++ -std=c++11 -Wall
filename = hw1.cpp
outputFile = hw1

all: hw1.cpp
	$(cc) $(filename) -o $(outputFile)

clean:
	rm -f $(outputFile)
