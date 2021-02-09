CXX = g++
CFLAGS = -std=c++17
CFILES = src/main.cc
COBJS = LibraryCell CellUnit CellGraph CellMap Net Coordinate SubPlacement CellPlacement
OUT = run
INCLUDE = include
OBJDIR = obj

.PHONY: build clean app

build: $(OBJDIR) $(OUT)

app_only: build clean

$(OUT): $(patsubst %,$(OBJDIR)/%.o, $(COBJS)) $(CFILES)
	$(CXX) $(CFLAGS) $^ -o $@ -I $(INCLUDE)

$(OBJDIR)/%.o: src/%.cc
	$(CXX) $(CFLAGS) -c $< -o $@ -I $(INCLUDE)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -r obj
