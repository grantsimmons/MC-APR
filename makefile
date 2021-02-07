CXX = g++
CFLAGS = -std=c++17
CFILES = src/main.cc
OUT = run
INCLUDE = include

$(OUT): $(CFILES)
	$(CXX) $(CFLAGS) $^ -o $@ -I $(INCLUDE)
