CXX=g++ -std=c++20 -Wall -Ofast

all: target/day1 target/day2 target/day3 target/day4 target/day5 \
	target/day6 target/day7 target/day8 target/day9 target/day10 \
	target/day11 target/day12 target/day13 target/day14 target/day15 \
	target/day16 target/day17 target/day18 target/day19 target/day20 \
	target/day21 target/day22

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
