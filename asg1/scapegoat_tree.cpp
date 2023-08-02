/**
 * @file scapegoat_tree.cpp
 * @author Dennis Andersen - deand17
 * @brief Implementation of Scapegoat Tree data structure
 * @date 2022-03-17
 * 
 * DM803 Advanced Data Structures
 * 
 * Exam Project - Part 1 - Spring 2022
 * 
 * Implementation of Scapegoat Tree data structure, based on the paper by Galperin and Rivest
 * 
 * Base binary tree implementation based on chapter 12 of CLRS
 */
#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <string>
#include <utility>


struct TreeNode 
{	
	/**
	 * @brief Constructs a new Tree Node object
	 * 
	 * @param key Key
	 */
	explicit TreeNode(const int key)
		: key(key),
          left(nullptr),
          right(nullptr)
	{
    }

	/**
	 * @brief Destroys the Tree Node object
	 * 
	 */
	~TreeNode() 
	= default;

	int key{};

    TreeNode *left;
    TreeNode *right;
};

struct ScapegoatTree 
{
    /**
     * @brief Constructs a new Scapegoat Tree object
     * 
     */
	explicit ScapegoatTree(const double alpha=0.55)
        : tree_size(0),
          max_tree_size(0),
          root(nullptr),
          alpha(alpha)
    {
    }

	/**
	 * @brief Destroys the Scapegoat Tree object
	 * 
	 */
	~ScapegoatTree() 
	{
        inorder_delete_tree(root);
	}

	/**
	 * @brief Searches the Scapegoat Tree for the given key
	 * 
	 * @param search_key Key to find
	 * @return std::pair<int, bool> first: number of comparisons
	 * 								second: true if key was found, false otherwise
	 */
	std::pair<int, bool> search(int search_key)
	{
		int comparisons{0};
		TreeNode *x(root);
		while (x != nullptr && search_key != x->key) {
			comparisons += 2;
			if (search_key < x->key) {
				x = x->left;
			} else {
				x = x->right;
			}
		}
		if (x != nullptr) {
			comparisons++;
			return std::make_pair(comparisons, true);
		}
        return std::make_pair(comparisons, false);
	}

	/**
	 * @brief Inserts key into the Scapegoat Tree. As in the paper, duplicate keys are allowed
	 * 
	 * @param search_key Key to insert
	 * @return std::pair<int, bool> first: number of comparisons
	 * 								second: true if key and value was inserted, 
	 * 									    false otherwise (e.g. key already present)
	 */
	std::pair<int, bool> insert(const int search_key) 
	{
		if (root != nullptr) {
			if (root->key == search_key) {
				return std::make_pair(1, false);
			}
		}

		TreeNode *x(root);
		TreeNode *y(nullptr);
        auto *z(new TreeNode(search_key));

		std::pair<int, int> comparisons_and_height = priv_insert(x, y, z);

		if (comparisons_and_height.second < 0) { // key was already present
			delete z;
			return std::make_pair(comparisons_and_height.first + 1, false);
		}
		return std::make_pair(comparisons_and_height.first + 1, true);
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
		return priv_remove(search_key);
	}

	/**
	 * @brief Returns the number of elements in the tree
	 * 
	 * @return size_t The number of elements in the tree
	 */
	size_t size() const
	{
		return tree_size;
	}

	/**
	 * @brief Allows printing of the Scapegoat Tree. Nodes are printed based on a BFS traversal
	 * 
	 * @param s Reference to output stream
	 * @param t Scapegoat tree to print
	 * @return std::ostream& Reference to output stream
	 */
	friend std::ostream &operator<<(std::ostream &s, const ScapegoatTree &t) 
	{
		std::queue<TreeNode *> q;
		q.push(t.root);
		while(!q.empty()) {
			TreeNode *node = q.front();
			q.pop();
			
			if (node != nullptr) {
				s << node->key << ": ";
				if (node->left != nullptr) {
					s << "L: " << node->left->key << " ";
				} else {
					s << "L: null ";
				}
				if (node->right != nullptr) {
					s << "R: " << node->right->key << " ";
				} else {
					s << "R: null ";
				}
				s << std::endl;
				q.push(node->left);
				q.push(node->right);
			}
		}
		return s;
	}

private:

	/**
	 * @brief Performs an in-order tree walk of the tree rooted at x, deleting each node of the tree
	 * 
	 * @param x Pointer to root of tree to delete
	 */
    void inorder_delete_tree(TreeNode *&x)
    {
        if (x != nullptr) {
            inorder_delete_tree(x->left);
            inorder_delete_tree(x->right);
            delete x;
            x = nullptr;
        }
    }

	/**
	 * @brief Private recursive method to insert a node into the Scapegoat Tree
	 * 
	 * @param x Root of current subtree
	 * @param y Initially nullptr, but becomes parent to z
	 * @param z New node to insert
	 * @return std::pair<int, int> 
	 */
	std::pair<int, int> priv_insert(TreeNode *&x, TreeNode *&y, TreeNode *&z) 
	{
        // Implementation adapted from Tree-Insert from CLRS 12.3 p. 294
        int comparisons{0};
		std::pair<int, int> comparisons_and_height;

        if (x != nullptr) {
            y = x;
            comparisons++;
            if (z->key < x->key) {
				comparisons_and_height = priv_insert(x->left, y, z);
            } else {
				comparisons_and_height = priv_insert(x->right, y, z);
            }
        } else {
			comparisons++;      // We pay the key comparison forward
			if (y == nullptr) {
				root = z;       // No key comparison when tree is empty,
				comparisons--;  // so we subtract prepaid comparison
			} else if (z->key == y->key) {
				return std::make_pair(comparisons, -1);
			}  else if (z->key < y->key) {
				y->left = z;
			} else {
				y->right = z;
			}
			tree_size++;
        	max_tree_size = std::max(tree_size, max_tree_size);

			// We return 1 as the second member of the pair as this 
			// is the height from parent y to the new leaf node z
			return std::make_pair(comparisons, 1);
		}
		// End Tree-Insert CLRS
		comparisons += comparisons_and_height.first;
		if (comparisons_and_height.second == -1) {
			return std::make_pair(comparisons, -1);
		}

		int height_of_x{comparisons_and_height.second};
		int size_of_subtree_at_x(subtree_size(x));

		if (node_is_balanced(height_of_x, size_of_subtree_at_x)) {
			return std::make_pair(comparisons, ++height_of_x);
		}
		if (x == root) {
			std::cout << " >>> Rebuilding tree at root: " << x->key << ", subtree size: " << size_of_subtree_at_x;
            std::cout << ", tree size: " << tree_size << " Max tree size: " << max_tree_size << std::endl;
			max_tree_size = tree_size;
			root = rebuild_tree(size_of_subtree_at_x, x);
		} else {
			std::cout << " >>> Rebuilding tree at node: " << x->key << ", subtree size: " << size_of_subtree_at_x;
            std::cout << ", tree size: " << tree_size << " Max tree size: " << max_tree_size << std::endl;
			x = rebuild_tree(size_of_subtree_at_x, x);
		}
        return std::make_pair(comparisons, 0);
	}

	/**
	 * @brief Computes the size of the subtree rooted at x (i.e. number of nodes, including x)
	 * 
	 * @param x Subtree root
	 * @return int Number of nodes in subtree of x including x itself
	 */
	int subtree_size(TreeNode *&x)
	{
		if (x == nullptr) {
			return 0;
		}
		return subtree_size(x->left) + subtree_size(x->right) + 1;
	}

	/**
	 * @brief Computes if node is alpha-height-balanced according to equation (4.6) in the 
	 * 		  paper. That is, node x_i is alpha-height-balanced if the height, i, of x_i in 
	 * 		  the tree is less than or equal to h_alpha computed on the size of the subtree 
	 * 		  rooted at x_i
	 * 
	 * 		  (4.6)		i > h_alpha(size(x_i))
	 * 
	 * @param height_of_node The height of node x_i in the tree
	 * @param size_of_subtree The number of nodes in the subtree rooted at x_i
	 * @return true If node is alpha-balanced
	 * @return false If node is not alpha-balanced
	 */
	bool node_is_balanced(const int height_of_node, const int size_of_subtree)
	{
		if (height_of_node > h_alpha(size_of_subtree)) {
			return false;
		}
		return true;
	}

	/**
	 * @brief Computes the alpha-height-balance
	 * 
	 * @param size_of_subtree Number of nodes in the subtree
	 * @return int Alpha-height-balance value
	 */
	int h_alpha(const int size_of_subtree) const
	{
		return static_cast <int> (std::floor(std::log2(size_of_subtree) / (-std::log2(alpha))));
	}

	/**
	 * @brief Returns a 'list' of the nodes in the subtree rooted at x, sorted in nondecreasing 
	 * 		  order
	 * 
	 * 		  Note that as described in the paper, this methods expects y to be a dummy node 
	 * 		  when first called
	 * 
	 * @param x Pointer to subtree root
	 * @param y Pointer to first node in a 'list' or nodes (linked using their *right* pointer fields)
	 * @return TreeNode* The 'list' resulting from flattening the tree
	 */
	TreeNode * flatten(TreeNode *&x, TreeNode *&y)
	{
		if (x == nullptr) {
			return y;
		}
		x->right = flatten(x->right, y);
		return flatten(x->left, x);
	}

	/**
	 * @brief Builds a 1/2-weight-balanced tree of size_of_subtree nodes from a list of nodes 
	 * 		  headed by x
	 * 
	 * @param size_of_subtree Number of nodes in subtree rooted at x
	 * @param x Root node of subtree
	 * @return TreeNode* Pointer to the n+1st node s in the 'list' modified such that s->left
	 * 		   points to the root of the size_of_subtree-node tree created
	 */
	TreeNode * build_tree(const int size_of_subtree, TreeNode *&x)
	{
		if (size_of_subtree == 0) {
			x->left = nullptr;
			return x;
		}
		int n{static_cast <int> (std::ceil((size_of_subtree - 1) / 2.0))};
		TreeNode *r = build_tree(n, x);
		n = static_cast <int> (std::floor((size_of_subtree - 1) / 2.0));
		TreeNode *s = build_tree(n, r->right);
		r->right = s->left;
		s->left = r;
		return s;
	}

	/**
	 * @brief Rebuilds the subtree rooted at the scapegoat
	 * 
	 * @param size_of_subtree Number of nodes in subtree of scapegoat
	 * @param scapegoat Root of subtree to rebuild
	 * @return TreeNode* Pointer to root of new subtree
	 */
	TreeNode * rebuild_tree(const int size_of_subtree, TreeNode *&scapegoat)
	{
		auto *w(new TreeNode(0));
		TreeNode *z(flatten(scapegoat, w));
		build_tree(size_of_subtree, z);
		z = w->left;
		delete w;
		return z;
	}

	/**
	 * @brief Private method to delete a key, if present, from the Scapegoat Tree
	 * 
	 * @param search_key Key to be deleted
	 * @return std::pair<int, bool> first: number of comparisons
	 * 								second: true if key was found and deleted, false otherwise
	 */
	std::pair<int, bool> priv_remove(const int search_key)
	{
		int comparisons{0};
		TreeNode *z(root);
		TreeNode *zp(nullptr);

		// Find the key to be deleted
		while (z != nullptr && search_key != z->key) {
			zp = z;
			comparisons += 2;
			if (search_key < z->key) {
				z = z->left;
			} else {
				z = z->right;
			}
		}
		if (z != nullptr) {
			comparisons++; 							   // found the key to be deleted
		} else {
			return std::make_pair(comparisons, false); // key not found so abort
		}

        // Implementation based on Tree-Delete from CLRS 12.3 p. 298
		if (z->left == nullptr) {
			transplant(z, zp, z->right, zp);
		} else if (z->right == nullptr) {
			transplant(z, zp, z->left, zp);
		} else {
			std::pair<TreeNode *, TreeNode *> subtree_min_and_parent = tree_minimum(z->right, z);
			TreeNode *y(subtree_min_and_parent.first);
			TreeNode *yp(subtree_min_and_parent.second);
			if (yp != z) {
				transplant(y, yp, y->right, yp);
				y->right = z->right;
			}
			transplant(z, zp, y, yp);
			y->left = z->left;
		}
		delete z;
		// End Tree-Delete CLRS
		tree_size--;

		// Check if tree needs to be rebuilt
		if (tree_size < alpha * max_tree_size) {
			// std::cout << " >>> Rebuilding tree. Size: " << tree_size << " Max tree size: " << max_tree_size << std::endl;
			max_tree_size = tree_size;
			root = rebuild_tree(tree_size, root);
		}
		return std::make_pair(comparisons, true);
	}

	/**
	 * @brief Helper method Transplant from CLRS p. 296 to replace subtrees
	 * 
	 * @param u Node to be removed
	 * @param up Parent of u, the node to be removed
	 * @param v Node to take u's place
	 * @param vp Parent of v
	 */
	void transplant(TreeNode *&u, TreeNode *&up, TreeNode *&v, TreeNode *&vp)
	{
		if (up == nullptr) {
			root = v;
		} else if (u == up->left) {
			up->left = v;
		} else {
			up->right = v;
		}
	}

	/**
	 * @brief Returns a pair of pointers, with first being a pointer to the node with the
	 *        minimum key of the subtree rooted at x and second being the parent of first
	 * 
	 * @param x Subtree root
	 * @param xp Parent of subtree root. Will be nullptr if x is the tree root
	 * @return std::pair<TreeNode *, TreeNode *> first: node in subtree with minimum key
	 * 											 second: parent of node with minimum key
	 */
	static std::pair<TreeNode *, TreeNode *> tree_minimum(TreeNode *x, TreeNode *xp)
	{
		while (x->left != nullptr) {
			xp = x;
			x = x->left;
		}
		return std::make_pair(x, xp);
	}

	/**
	 * @brief Returns a pair of pointers, with first being a pointer to the node with the
	 *        maximum key of the subtree rooted at x and second being the parent of first
	 * 
	 * @param x Subtree root
	 * @param xp Parent of subtree root. Will be nullptr if x is the tree root
	 * @return std::pair<TreeNode *, TreeNode *> first: node in subtree with maximum key
	 * 											 second: parent of node with maximum key
	 */
	static std::pair<TreeNode *, TreeNode *> tree_maximum(TreeNode *x, TreeNode *xp)
	{
		while (x->right != nullptr) {
			xp = x;
			x = x->right;
		}
		return std::make_pair(x, xp);
	}

    // The number of nodes in the tree
	int tree_size{};

    // The maximal value of tree_size since the last time the tree was completely rebuilt
    int max_tree_size{};

    TreeNode *root;
	
	// Constant between (0.5,1) used to determine balance of tree
	const double alpha{};
};


/**
 * @brief Prints a helper message to stdout for how to use this program
 * 
 * @param program First argument from the command line, i.e. argv[0]
 */
static void show_usage(const std::string& program)
{
    std::cout << "Usage: " << program << " [<alpha>]\n"
              << "Arguments:\n"
              << "\talpha \t\tOptional: Floating point constant between (0.5,1) defining\n"
			  <<   "\t\t\tthe alpha-weight-balance of the scapegoat tree. Default value is 0.55.\n"
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
	double alpha{0.55};
	double eps{0.0001};
	bool fail{false};

	if (argc > 1) {
		try {
			alpha = std::stod(argv[1]);
		} catch (std::invalid_argument &e) {
			fail = true;
		} catch (std::out_of_range &e) {
			fail = true;
		}
		if ((alpha < 0.5 + eps || alpha > 1.0 - eps) || fail) {
			show_usage(argv[0]);
			return exit_with_error_msg("Error: the value of alpha must be in the range (0.5,1).\n");
		}
	} 

	ScapegoatTree t(alpha);

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
			std::pair<int, bool> inserted(t.insert(key));
			if (inserted.second) {
				std::cout << "S - inserted '" << key << "'. Comparisons: " << inserted.first;
				std::cout << ". Tree size: " << t.size() << std::endl;
			} else {
				std::cout << "F - key '" << key << "' already present. Comparisons: " << inserted.first;
				std::cout << ". Tree size: " << t.size() << std::endl;
			}
		} else if (operation == "S" || operation == "s") {
			std::pair<int, bool> key_found(t.search(key));
			if (key_found.second) {
				std::cout << "S - found '" << key << "'. Comparisons: " << key_found.first;
				std::cout << ". Tree size: " << t.size() << std::endl;
			} else {
				std::cout << "F - key '" << key << "' not present. Comparisons: " << key_found.first;
				std::cout << ". Tree size: " << t.size() << std::endl;
			}
		} else if (operation == "D" || operation == "d") {
			std::pair<int, bool> removed(t.remove(key));
			if (removed.second) {
				std::cout << "S - deleted '" << key << "'. Comparisons: " << removed.first;
				std::cout << ". Tree size: " << t.size() << std::endl;
			} else {
				std::cout << "F - key '" << key << "' not present. Comparisons: " << removed.first;
				std::cout << ". Tree size: " << t.size() << std::endl;
			}
		} else if (operation == "Q" || operation == "q") {
			return 0;
		} else {
			std::cout << "F - " << operation << " command unknown, ignored" << std::endl;
		}
	}
	return 0;
}
