CPPFLAGS=
CXXFLAGS=-g -O2 $(SANFLAGS)
LDFLAGS=$(SANFLAGS)
LIBS=

SANFLAGS=-fsanitize=undefined -fsanitize=address -fsanitize=leak

.PHONY: all
all: skip_list scapegoat_tree

skip_list: skip_list.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

scapegoat_tree: scapegoat_tree.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

test: all
	./skip_list < example_input
	./scapegoat_tree < example_input

.PHONY: clean
clean:
	rm -f *.o skip_list scapegoat_tree

.PHONY: clean_test
clean_test:
	rm -f out* test_*
