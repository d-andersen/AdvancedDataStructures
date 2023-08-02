/**
 * @file RTree.hpp
 * @author Dennis Andersen - deand17
 * @brief Header file for randomized search tree.
 * @date 2022-05-03
 *
 * DM803 Advanced Data Structures
 *
 * Exam Project - Part 2 - Spring 2022
 *
 * Header file for randomized search tree.
 *
 * References:
 * [CLRS] Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest, Clifford Stein.
 *        Ch. 12 - Binary Search Trees, pp. 286--307.
 *        Ch. 13.2 - Red-Black Trees, Rotations, pp. 312--313.
 *        Introduction to Algorithms, 3rd ed.
 *        The MIT Press, 2009.
 */

#ifndef RTREE_HPP
#define RTREE_HPP

#include <iostream>
#include <limits>
#include <optional>
#include <random>

namespace DM803
{
class RTree
{
public:
    /**
     * Enum defining valid priority values.
     *
     * The range of valid priorities is [1, 2^64 - 1].
     *
     * Zero is reserved as a dummy priority to allow us to always insert a
     * node with a smaller priority than any other node in the tree. This
     * dummy value is used in the split method.
     */
    enum Pr_val : unsigned long long {
        pr_dummy = 0,
        pr_min = 1,
        pr_max = std::numeric_limits<unsigned long long>::max()
    };

    class Node;

    /**
     * Default constructor.
     */
    RTree();

    /**
     * Copy constructor.
     *
     * @param other
     */
    RTree(const RTree &other);

    /**
     * Destructor.
     *
     * Uses clear to delete all primary_nodes.
     */
    ~RTree();

    /**
     * Copy assignment operator.
     *
     * Uses the copy constructor and the copy-and-swap idiom.
     *
     * References:
     * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c60-make-copy-assignment-non-virtual-take-the-parameter-by-const-and-return-by-non-const
     * https://en.cppreference.com/w/cpp/language/operators
     * https://stackoverflow.com/a/3279550
     *
     * @param other Tree to copy.
     * @return A copy of other.
     */
    RTree &operator=(const RTree &other);

    /**
     * Returns the number of elements stored.
     *
     * @return the number of elements stored.
     */
    int size() const;

    /**
     * Returns true iff the container is empty, i.e. empty() == (size() == 0).
     *
     * @return true if the tree is empty and false otherwise.
     */
    bool empty() const;

    /**
     * Erases all elements.
     *
     * Uses tree_postorder_erase to recursively delete all primary_nodes in a post-order
     * traversal.
     */
    void clear();

    /**
     * Looks up the given key and returns a pointer to the node containing it,
     * or nullptr if no such node exists.
     *
     * Uses tree_iterative_search to iteratively search the tree for the given key.
     *
     * @param key The key to search for.
     * @return A pair where first is a pointer to the node with the given search key if
     *         it exists and nullptr otherwise, and second is the depth of the key in the
     *         tree.
     */
    std::pair<Node *, int> search(int key);

    /**
     * Inserts a new element in the tree or does nothing if the key already exists.
     * The new element is by default given a random priority in the range [1, 2^64 - 1],
     * however, this can be bypassed by providing a valid priority as an optional second
     * argument.
     *
     * Uses tree_insert to insert the key and returns a pointer to the newly inserted
     * node and a boolean being true if a new node was inserted, and nullptr and false
     * if the key  was already present.
     *
     * This is an O(h) operation, but since the tree is unbalanced, h = n in the
     * worst case and this becomes an O(n) operation.
     *
     * @param key Key to insert.
     * @param pr Optional user specified priority. Defaults to a random priority in the
     *           range [1, 2^64 - 1].
     * @return A pair (first, second).
     *         If the key did not already exist in the tree, then
     *         - first is a pointer to the newly inserted node, and
     *         - second is true, indicating that a new key was inserted.
     *         otherwise, if the key did already exist, then
     *         - first is nullptr, and
     *         - second is false indicating that no new key was inserted.
     */
    std::pair<Node *, bool> insert(int key, Pr_val pr=Pr_val::pr_dummy);

    /**
     * Splits this tree in two, with the original tree transformed to contain all keys
     * smaller than the given tree and the new tree returned containing all keys larger
     * than the given key. The given key is assumed to not be in the tree. If the given
     * tree is in the key, this does nothing and simply returns a new empty tree.
     *
     * The tree is split by the procedure given in the project description. That is, we
     * insert a new node x with the given key and the dummy priority of zero (which is
     * smaller than all other valid priorities), meaning x will be moved to be the new
     * root of the original tree. The tree is then split by making the root of the original
     * tree the left child of x and creating a new tree with its root being the right child
     * of x.
     *
     * This is in theory an O(h) operation, since the idea is to mainly rely on the insert
     * operation, which is an O(h) operation, and then return the two children of the new
     * root. However, because the tree is unbalanced, h = n in the worst case and this
     * becomes an O(n) operation. In addition, we have not implemented any functionality to
     * keep track of the size of subtrees, thus when splitting a tree, for the size() method
     * to be valid, we need to count the number of primary_nodes in each of the two trees, corresponding
     * to counting the n primary_nodes in the original tree, which is of course also an O(n) operation.
     *
     * @param key The key to split the tree on.
     * @return A new tree with all keys larger than all keys in the now modified original tree.
     */
    std::optional<RTree> split(int key);

    /**
     * Returns a new tree that is the merge of the two given trees.
     *
     * The two trees given as arguments are both emptied in the process, leaving them in a
     * state equivalent to calling the clear() method.
     *
     * The new tree is created using the tree_merge method and std::swap.
     *
     * @param x Reference to first tree.
     * @param y Reference to second tree.
     * @return A new tree which is the merge of trees x and y.
     */
    static RTree merge(RTree &x, RTree &y);

    /**
     * If a node z with the given key exists in the tree, z is erased and its position in
     * the tree replaced by the merge of its two children; otherwise this does nothing.
     *
     * @param key Key to erase.
     * @return A pair where first is a pointer to the root of the merge of z's two children
     *         if a node z with the given key existed in the tree and nullptr otherwise, and
     *         second is a boolean being true if z was erased and false otherwise.
     */
    std::pair<Node *, bool> erase(int key);

    /**
     * Accesses the root node.
     *
     * @return A pointer to the root of the tree.
     */
    Node *root() const;

    /**
     * Accesses the first element.
     *
     * Uses tree_minimum to find and return smallest key.
     *
     * @return A std::optional containing the smallest key if the tree is non-empty,
     *         and nothing otherwise.
     */
    std::optional<int> front();

    /**
     * Accesses the first element.
     *
     * Uses tree_minimum to find and return smallest key.
     *
     * @return A std::optional containing the smallest key if the tree is non-empty,
     *         and nothing otherwise.
     */
    const std::optional<int> front() const;

    /**
     * Accesses the last element.
     *
     * Uses tree_maximum to find and return largest key.
     *
     * @return A std::optional containing the largest key if the tree is non-empty,
     *         and nothing otherwise.
     */
    std::optional<int> back();

    /**
     * Accesses the last element.
     *
     * Uses tree_maximum to find and return largest key.
     *
     * @return A std::optional containing the largest key if the tree is non-empty,
     *         and nothing otherwise.
     */
    const std::optional<int> back() const;

    /**
     * Accesses the node storing the first element.
     *
     * Uses tree_minimum to find and return a pointer to the node with the
     * smallest key or nullptr if the tree is empty.
     *
     * @return A pointer to the node with the smallest key if the tree is
     *         non-empty and nullptr otherwise.
     */
    Node *begin();

    /**
     * Accesses the node storing the first element.
     *
     * Uses tree_minimum to find and return a pointer to the node with the
     * smallest key or nullptr if the tree is empty.
     *
     * @return A pointer to the node with the smallest key if the tree is
     *         non-empty and nullptr otherwise.
     */
    const Node *begin() const;

    /**
     * Returns a pointer to the node past the last element, meaning this function
     * always returns nullptr.
     *
     * @return nullptr.
     */
    Node *end();

    /**
     * Returns a pointer to the node past the last element, meaning this function
     * always returns nullptr.
     *
     * @return nullptr.
     */
    const Node *end() const;

    /**
     * Swap function for applying the copy-and-swap idiom.
     *
     * References:
     * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c83-for-value-like-types-consider-providing-a-noexcept-swap-function
     * https://stackoverflow.com/a/3279550
     *
     * @param lhs Left-hand side operand to be swapped.
     * @param rhs Right-hand side operand to be swapped.
     */
    friend void swap(RTree &lhs, RTree &rhs) noexcept;

    /**
     * Enables printing of the tree. Uses a queue to stringify the tree in a BFS manner.
     * This has mainly been used as a convenience function for debugging.
     */
    friend std::ostream &operator<<(std::ostream &s, const RTree &t);

    class Node
    {
    public:
        /**
         * Default constructor.
         *
         * @param key Key of this node.
         * @param pr Priority of this node.
         */
        Node(int key, unsigned long long pr);

        /**
         * Destructor.
         */
        ~Node();

        /**
         * Returns the node with the smallest key larger than the key in
         * this node if it exists and nullptr otherwise.
         *
         * Based on Tree-Successor from CLRS ch. 12, p. 291.
         *
         * @return A pointer to the successor of this node if exists and
         *         nullptr otherwise.
         */
        Node *next();

        /**
         * Returns the node with the smallest key larger than the key in
         * this node if it exists and nullptr otherwise.
         *
         * Based on Tree-Successor from CLRS ch. 12, p. 291.
         *
         * @return A const pointer to the successor of this node if exists and
         *         nullptr otherwise.
         */
        const Node *next() const;

        /**
         * Returns the node with the largest key smaller than the key in
         * this node if it exists and nullptr otherwise.
         *
         * Based on Tree-Predecessor from CLRS ch. 12, p. 291.
         *
         * @return A pointer to the predecessor of this node if exists and
         *         nullptr otherwise.
         */
        Node *prev();

        /**
         * Returns the node with the largest key smaller than the key in
         * this node if it exists and nullptr otherwise.
         *
         * Based on Tree-Predecessor from CLRS ch. 12, p. 291.
         *
         * @return A const pointer to the predecessor of this node if exists and
         *         nullptr otherwise.
         */
        const Node *prev() const;

        int key;
        unsigned long long pr;

        Node *parent;
        Node *left;
        Node *right;
    };

private:
    int n;

    Node *m_root;

    std::random_device rd;
    std::mt19937_64 rng;
    std::uniform_int_distribution<unsigned long long> uni_int_dist;

    /**
     * Generates a positive integer in the range [1, 2^64 - 1] uniformly at random.
     *
     * @return Uniformly random positive integer in the range [1, 2^64 - 1]
     */
    unsigned long long int gen_pr();

    /**
     * Performs a left rotation on the tree rooted at node x.
     *
     * Based on Left-Rotate from CLRS ch. 13, p. 313.
     *
     * @param x Node to rotate.
     */
    void left_rotate(Node *x);

    /**
     * Performs a right rotation on the tree rooted at node x.
     *
     * Symmetric to Left-Rotate from CLRS ch. 13, p. 313.
     *
     * @param x Node to rotate.
     */
    void right_rotate(Node *y);

    /**
     * Counts the number of primary_nodes in the subtree rooted at x via an inorder tree walk.
     *
     * @param x Pointer to root of subtree to count the number of primary_nodes in.
     * @param c Reference to integer to use as counter.
     */
    void tree_inorder_walk_count_nodes(Node *x, int &c);

    /**
     * Recursive function performing a post-order traversal of the tree, deleting
     * each visited node in the process.
     *
     * @param x Root of the subtree to be deleted.
     */
    void tree_postorder_erase(Node *x);

    /**
     * Searches the tree iteratively for the given key.
     *
     * Based on Iterative-Tree-Search from CLRS ch. 12, p. 291.
     *
     * @param key The key to search for.
     * @return A pair where first is a pointer to the node with the given search key if
     *         it exists and nullptr otherwise, and second is the depth of the key in the
     *         tree.
     */
    std::pair<RTree::Node *, int> tree_iterative_search(int key);

    /**
     * Inserts a new element in the tree or does nothing if the key already exists.
     *
     * Once the new key has been inserted in the correct place in the tree, a call
     * is made to tree_heapify to make the tree heap-ordered again.
     *
     * Returns a pointer to the newly inserted node and a boolean being true if a
     * new node was inserted and false if the key was already present.
     *
     * Based on Tree-Insert from CLRS ch. 12, p. 294.
     *
     * @param key The key to insert.
     * @return A pair (first, second).
     *         If the key did not already exist in the tree, then
     *         - first is a pointer to the newly inserted node, and
     *         - second is true, indicating that a new key was added.
     *         otherwise, if the key did already exist, then
     *         - first is nullptr, and
     *         - second is false indicating that no new key was added.
     */
    std::pair<Node *, bool> tree_insert(int key, unsigned long long int pr);

    /**
     * Performs single (left or right) rotations until the tree is heap-ordered.
     *
     * @param x Node to heapify.
     */
    void tree_heapify(Node *x);

    /**
     * Finds the smallest key in the tree by following the leftmost path.
     * It is the responsibility of the caller to to verify that the tree is non-empty.
     *
     * Based on Tree-Minimum from CLRS ch. 12, p. 291.
     *
     * @return A pointer to the node with the smallest key.
     */
    static Node *tree_minimum(Node *x);

    /**
     * Finds the largest key in the tree by following the rightmost path.
     * It is the responsibility of the caller to to verify that the tree is non-empty.
     *
     * Based on Tree-Maximum from CLRS ch. 12, p. 291.
     *
     * @return A pointer to the node with the largest key.
     */
    static Node *tree_maximum(Node *x);

    /**
     * Private method to merge two trees based on the recursive approach described
     * in the project description.
     *
     *  - x is the root node with the smallest priority of root primary_nodes in trees a and b, and
     *  - y is the other root node, i.e. the root node with the largest priority,
     *  - z is the left or right child of x which contains the middle range of keys, and
     *  - z_prime is the other child of x.
     *
     * @param a Pointer to root node of first subtree.
     * @param b Pointer to root node of second subtree.
     * @return The merge of trees z and y.
     */
    static RTree::Node *tree_merge(Node *a, Node *b);
};

} // DM803

#endif //RTREE_HPP
