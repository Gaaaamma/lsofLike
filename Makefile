cc= g++ -std=c++11 
filename = hw1.cpp
outputFile = hw1

all: hw1.cpp
	$(cc) $(filename) -o $(outputFile)

clean:
	rm -f $(outputFile)
