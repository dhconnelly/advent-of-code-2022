CXX=g++ -std=c++20 -Wall -Werror

all: target/day1 target/day2 target/day3 target/day4

target/day%: target/day%.o target/util.o
	mkdir -p target
	$(CXX) $^ -o $@

target/day%.o: src/day%.cc
	mkdir -p target
	$(CXX) -c $< -o $@

target/util.o: src/util.cc src/util.h
	mkdir -p target
	$(CXX) -c $< -o $@

.PHONY: clean

clean:
	rm -rf target/
