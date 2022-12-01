CXX=clang++ -std=c++20 -stdlib=libc++ -Wall -Werror

all: target/day1

target/day%: target/day%.o
	mkdir -p target
	$(CXX) $< -o $@

target/%.o: src/%.cc src/util.h
	mkdir -p target
	$(CXX) -c $< -o $@

.PHONY: clean

clean:
	rm -rf target/
