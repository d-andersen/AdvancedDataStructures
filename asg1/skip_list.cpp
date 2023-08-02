/**
 * @file skip_list.cpp
 * @author Dennis Andersen - deand17
 * @brief Implementation of Skip List data structure
 * @date 2022-03-17
 * 
 * DM803 Advanced Data Structures
 * 
 * Exam Project - Part 1 - Spring 2022
 * 
 * Implementation of Skip List data structure, based on the paper by William Pugh.
 */
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include <utility>

const int SENTINEL_KEY_VALUE{std::numeric_limits<int>::min()};
int LEVEL_CAP{32};

struct SkipListNode 
{	
	/**
	 * @brief Constructs a new Skip List Node object
	 * 
	 * @param level Number of forward pointers
	 * @param key Key
	 * @param value Value
	 */
	SkipListNode(const int level, const int key, const int value) 
		: level(level),
		  key(key),
		  value(value),
		  forward(LEVEL_CAP)
	{
	}

	/**
	 * @brief Destroys the Skip List Node object
	 * 
	 */
	~SkipListNode() 
	= default;

	// Number of forward pointers
	int level{};

	int key{};
	int value{};

	// Vector of forward pointers
	std::vector<SkipListNode *> forward;
};

struct SkipList 
{
	/**
	 * @brief Constructs a new Skip List object
	 * 
	 * @param p Constant between (0,1) defining number of elements that are level i or greater
	 * @param level_cap Upper bound for the number of possible forward pointers
	 */
	explicit SkipList(const double p=0.5, const int level_cap=LEVEL_CAP)
		: list_size(0), 
		  p(p),
		  level_cap(level_cap),
		  max_level(1),
		  sentinel(new SkipListNode(max_level, SENTINEL_KEY_VALUE, SENTINEL_KEY_VALUE)),
		  rng(rd()),
		  uniform_zero_one_distribution(std::uniform_real_distribution<>(0.0, 1.0))
	{
		sentinel->forward[0] = sentinel;
	}

	/**
	 * @brief Destroys the Skip List object
	 * 
	 */
	~SkipList() 
	{
		SkipListNode *current = sentinel->forward[0];
		SkipListNode *next;
		while (current != sentinel) {
			next = current->forward[0];
			delete current;
			current = next;
		}
		delete current; // The sentinel is last SkipListNode deleted
	}

	/**
	 * @brief Searches the Skip List for the given key
	 * 
	 * @param search_key Key to find
	 * @return std::pair<int, bool> first: number of comparisons
	 * 								second: true if key was found, false otherwise
	 */
	std::pair<int, bool> search(int search_key)
	{
		SkipListNode *node = sentinel;

		int comparisons{0};
		bool node_not_sentinel{false};
		bool key_less_than_search_key{false};
		for (size_t i = max_level; i > 0; i--) {
			while ((node_not_sentinel = node->forward[i-1] != sentinel) 
			      && (key_less_than_search_key = node->forward[i-1]->key < search_key)) {
				comparisons++;
				node = node->forward[i-1];
			}
			if (node_not_sentinel && !key_less_than_search_key) {
				comparisons++;
			}
		}
		node = node->forward[0];
		if (node != sentinel) {
			comparisons++;
			if (node->key == search_key) {
				return std::make_pair(comparisons, true);
			}
		}
		return std::make_pair(comparisons, false);
	}

	/**
	 * @brief Inserts key-value pair into the Skip List
	 * 
	 * @param search_key Key to insert
	 * @param new_value Value to insert
	 * @return std::pair<int, bool> first: number of comparisons
	 * 								second: true if key and value was inserted,
	 * 									    false otherwise (e.g. key already present)
	 */
	std::pair<int, bool> insert(const int search_key, const int new_value) 
	{
		std::vector<SkipListNode *> update(level_cap);

		SkipListNode *node = sentinel;

		int comparisons(traverse_list(search_key, node, update));

		if (node != sentinel) {
			comparisons++;
			if (node->key == search_key) {
				return std::make_pair(comparisons, false);
			}
			// node->value = new_value;
		}
		int lvl{random_level()};
		node = new SkipListNode(lvl, search_key, new_value);
		for (size_t i = 0; i < std::min(node->level, max_level); i++) {
			node->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = node;
		}
		list_size++;
		if (static_cast <int> (std::floor(L(list_size))) > max_level) {
			increase_max_level_of_list();
		}
		return std::make_pair(comparisons, true);
	}

	/**
	 * @brief Deletes the key, if present, from the Skip List
	 * 
	 * @param search_key Key to be deleted
	 * @return std::pair<int, bool> first: number of comparisons
	 * 								second: true if key was found and deleted, false otherwise
	 */
	std::pair<int, bool> remove(const int search_key) 
	{
		std::vector<SkipListNode *> update(level_cap);

		SkipListNode *node = sentinel;

		int comparisons(traverse_list(search_key, node, update));

		if (node != sentinel) {
			comparisons++;
			if (node->key == search_key) {
				for (size_t i = 0; i < std::min(node->level, max_level); i++) {
					update[i]->forward[i] = node->forward[i];
				}
				list_size--;
				delete node;
				if (list_size > 0 && static_cast <int> (std::ceil(L(list_size))) < max_level) {
					if (max_level > 1) {
						max_level--;
					}
				}
				return std::make_pair(comparisons, true);
			}
		}
		return std::make_pair(comparisons, false);
	}

	/**
	 * @brief Returns the number of elements in the Skip List
	 * 
	 * @return size_t 
	 */
	size_t size() const
	{
		return list_size;
	}

	/**
	 * @brief Allows printing of the Skip List
	 * 
	 * @param s Reference to output stream
	 * @param l Skip List to print
	 * @return std::ostream& Reference to output stream
	 */
	friend std::ostream &operator<<(std::ostream &s, const SkipList &l) 
	{
		SkipListNode *node = l.sentinel->forward[0];
		for (size_t i = 0; i < l.size(); i++) {
			if (i > 0) {
				s << "->";
			}
			s << "[" << node->key << "]";
			node = node->forward[0];
		}
		return s;
	}

private:
	/**
	 * @brief Traverse the Skip List until an element with key >= search key is found
	 * 
	 * @param search_key Key to search for
	 * @param node Reference to the pointer for the header, which should always be the sentinel
	 * @param update Reference to local update vector of forward pointers
	 * @return int Number of comparisons made during traversal
	 */
	int traverse_list(const int search_key, SkipListNode *&node, std::vector<SkipListNode *> &update)
	{
		int comparisons{0};
		bool node_not_sentinel{false};
		bool key_less_than_search_key{false};
		for (size_t i = max_level; i > 0; i--) {
			while ((node_not_sentinel = node->forward[i-1] != sentinel) 
			      && (key_less_than_search_key = node->forward[i-1]->key < search_key)) {
				comparisons++;
				node = node->forward[i-1];
			}
			if (node_not_sentinel && !key_less_than_search_key) {
				comparisons++;
			}
			update[i-1] = node;
		}
		node = node->forward[0];
		return comparisons;
	}

	/**
	 * @brief Generates a random integer in the range [1,level cap] to use as the level for a
	 *        new SkipListNode
	 * 
	 * @return int Positive integer in range [1,level cap]
	 */
	int random_level()
	{
		int level{1};
		while (uniform_zero_one_distribution(rng) < p && level < level_cap) {
			level++;
		}
		return level;
	}

	/**
	 * @brief Computes the number of levels of pointers that should be used in a Skip List of
	 *        n elements
	 * 
	 * @param n Number of elements in the Skip List
	 * @return double The number of levels of pointers that should be used
	 */
	double L(const int n) const
	{
		// return static_cast <int> ();
		return std::log2(n) / (-std::log2(p));
	}

	/**
	 * @brief Increases the max level of the Skip List
	 * 
	 */
	void increase_max_level_of_list()
	{
		int level{max_level};
		SkipListNode *r = sentinel;
		SkipListNode *q = r->forward[level-1];
		while (q != sentinel) {
			if (q->level > level) {
				r->forward[level] = q;
				r = q;
			}
			q = q->forward[level-1];
		}
		r->forward[level] = sentinel;
		max_level++;
	}

	int list_size{};

	// Upper bound for the number of possible forward pointers
	const int level_cap{};
	
	// Number of forward pointers currently in use
	int max_level{};
	
	// Constant between (0,1) defining number of elements that are level i or greater
	const double p{};

	SkipListNode *sentinel;

	// Get a seed for the random number engine
	std::random_device rd;

	// Standard mersenne_twister_engine seeded with rd()
	std::mt19937 rng;

	// Used to get random number between [0,1)
	std::uniform_real_distribution<> uniform_zero_one_distribution;
};


/**
 * @brief Prints a helper message to stdout for how to use this program
 * 
 * @param program First argument from the command line, i.e. argv[0]
 */
static void show_usage(const std::string& program)
{
    std::cout << "Usage: " << program << " [<p>|<level cap>]\n"
              << "Arguments:\n"
              << "\tp \t\tOptional: Floating point constant between (0,1) defining number of\n"
			  <<   "\t\t\telements that are level i or greater. Default value is 1/e = 0.36788.\n"
              << "\tlevel cap\tOptional: Positive integer constant between [1,64] defining an\n" 
			  <<   "\t\t\tupper bound on the number of levels a skip list element can have.\n"
              <<   "\t\t\tDefault value is 32.\n"
              << std::endl;
}

/**
 * @brief Error handling helper function
 * 
 * @param error_msg Error message to print
 * @return int Error code
 */
int exit_with_error_msg(const std::string& error_msg)
{
	std::cerr << error_msg;
	return 1;
}

int main(int argc, char *argv[]) 
{
	double arg{};
	double p{0.36788};
	double eps{0.0001};
	bool fail{false};
	int relevant_args{1};

	if (argc > 1 && argc <= 2) {
		relevant_args = 2;
	} else if (argc > 1 && argc <= 3 || argc > 3) {
		relevant_args = 3;
	}

    for (size_t i = 1; i < relevant_args; i++) {
        try {
            arg = std::stod(argv[i]);
        } catch (std::invalid_argument &e) {
            fail = true;
        } catch (std::out_of_range &e) {
            fail = true;
        }
        if (fail || arg < 0.0 + eps) {
            show_usage(argv[0]);
            return exit_with_error_msg("Error: the value of p must be in the range (0,1).\n");
        } else if (arg > 64.0 + eps) {
            show_usage(argv[0]);
            return exit_with_error_msg("Error: the value of level cap must be in the range [1,64].\n");
        } else if (arg > 1.0 - eps && arg < 64.0 + eps) {
            LEVEL_CAP = static_cast <int> (arg);
        } else {
            p = arg;
        }
    }

	SkipList l(p, LEVEL_CAP);

	std::string line{};
	std::string space_delimiter{" "};
	std::string operation{};
	int key{};

	while(std::getline(std::cin, line)) {
		operation = line.substr(0, line.find(space_delimiter));
		try {
			key = std::stoi(line.substr(line.find(space_delimiter) + space_delimiter.length()));
		} catch (std::invalid_argument &e) {
			key = -1;
		}
		if (operation == "I" || operation == "i") {
			std::pair<int, bool> inserted(l.insert(key, key));
			if (inserted.second) {
				std::cout << "S - inserted '" << key << "'. Comparisons: " << inserted.first;
				std::cout << ". List size: " << l.size() << std::endl;
			} else {
				std::cout << "F - key '" << key << "' already present. Comparisons: " << inserted.first;
				std::cout << ". List size: " << l.size() << std::endl;
			}
		} else if (operation == "S" || operation == "s") {
			std::pair<int, bool> key_found(l.search(key));
			if (key_found.second) {
				std::cout << "S - found '" << key << "'. Comparisons: " << key_found.first;
				std::cout << ". List size: " << l.size() << std::endl;
			} else {
				std::cout << "F - key '" << key << "' not present. Comparisons: " << key_found.first;
				std::cout << ". List size: " << l.size() << std::endl;
			}
		} else if (operation == "D" || operation == "d") {
			std::pair<int, bool> removed(l.remove(key));
			if (removed.second) {
				std::cout << "S - deleted '" << key << "'. Comparisons: " << removed.first;
				std::cout << ". List size: " << l.size() << std::endl;
			} else {
				std::cout << "F - key '" << key << "' not present. Comparisons: " << removed.first;
				std::cout << ". List size: " << l.size() << std::endl;
			}
		} else if (operation == "Q" || operation == "q") {
			return 0;
		} else {
			std::cout << "F - " << operation << " command unknown, ignored" << std::endl;
		}
	}
	return 0;
}
