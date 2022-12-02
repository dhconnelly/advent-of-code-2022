CXX=g++ -std=c++20 -Wall -Werror
DAYS=target/solutions/day1.o target/solutions/day0.o

all: target/aoc2022 target/aoc2022test

target/solutions/day%.o: src/solutions/day%.cc src/util.h src/aoc.h
	mkdir -p target/solutions
	$(CXX) -c $< -o $@

target/util.o: src/util.cc src/util.h
	mkdir -p target
	$(CXX) -c $< -o $@

target/aoc.o: src/aoc.cc src/aoc.h
	mkdir -p target
	$(CXX) -c $< -o $@

target/aoc2022.o: src/main.cc
	mkdir -p target
	$(CXX) -c $< -o $@

target/aoc2022test.o: src/test.cc
	mkdir -p target
	$(CXX) -c $< -o $@

target/%: target/%.o target/aoc.o target/util.o $(DAYS)
	mkdir -p target
	$(CXX) $^ -o $@

.PHONY: clean

clean:
	rm -rf target/
