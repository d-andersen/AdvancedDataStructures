CXX=g++
CPPFLAGS=-I$(SRC_DIR)
CXXFLAGS=-std=c++17 -g -O2 -Wall -Wextra $(SANFLAGS)
LDFLAGS=$(SANFLAGS)
SANFLAGS=-fsanitize=undefined -fsanitize=address -fsanitize=leak

RTREE=rtree_test
PPLIST=pplist_test

SRC_DIR=src/
OBJ_DIR=obj/
RTREE_SRC=src/rtree_test.cpp src/RTree.cpp
RTREE_OBJ=obj/rtree_test.o obj/RTree.o

PPLIST_SRC=src/pplist_test.cpp src/PPList.cpp
PPLIST_OBJ=obj/pplist_test.o obj/PPList.o

all: $(RTREE) $(PPLIST)

clean:
	rm -rf $(RTREE) $(PPLIST) $(OBJ_DIR)

.PHONY: all

$(RTREE): $(RTREE_OBJ)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

$(PPLIST): $(PPLIST_OBJ)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

$(RTREE_OBJ): obj/%.o: src/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(PPLIST_OBJ): obj/%.o: src/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
