CXX=g++ -std=c++20 -Wall -Werror

all: target/day1

target/day%: target/day%.o
	mkdir -p target
	$(CXX) $< -o $@

target/%.o: src/%.cc
	mkdir -p target
	$(CXX) -c $< -o $@

.PHONY: clean

clean:
	rm -rf target/
