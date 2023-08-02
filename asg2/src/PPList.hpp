/**
 * @file PPList.hpp
 * @author Dennis Andersen - deand17
 * @brief Header file for partially persistent singly-linked list
 * @date 2022-04-25
 *
 * DM803 Advanced Data Structures
 *
 * Exam Project - Part 2 - Spring 2022
 *
 * Header file for partially persistent singly-linked list
 *
 * References:
 * [1] James R. Driscoll, Neil Sarnak, Daniel D. Sleator, and Robert E. Tarjan.
 *     Making Data Structures Persistent. Journal of Computer and System Sciences, 38:86–124, 1989.
 */

#ifndef PPLIST_HPP
#define PPLIST_HPP

#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace DM803
{
class PPList
{
// --------------------------------------------------------------------------------
// Public
// --------------------------------------------------------------------------------
public:
    /**
     * Enum to indicate type of extra pointer, where
     *   - next corresponds to an additional next pointer, and
     *   - assoc corresponds to an additional assoc pointer
     */
    enum class PtrType {next, assoc};

    /**
     * Enum to indicate PNode type, where
     *   - pnode is a node of the main list, and
     *   - anode is an associated node
     */
    enum class PNodeType {pnode, anode};

    /**
     * Enum defining non-existing key
     */
    enum : int {none = std::numeric_limits<int>::min()};

    class PNode;

    /**
     * Default constructor
     */
    explicit PPList();

    /**
     * Destructor
     */
    ~PPList();

    /**
     * Returns the number of main elements of current version
     *
     * @return Number of elements of current version
     */
    int size() const;

    /**
     * Returns the number of main elements of the vth version
     * @param v Version
     * @return Number of elements of version v
     */
    int size(int v) const;

    /**
     * Returns the current version
     *
     * @return Current version
     */
    int version() const;

    /**
     * Clears the contents
     */
    void clear();

    /**
     * Increases the version number
     */
    void newversion();

    /**
     * Returns the key of the ith element of the vth version, as well as the key from the
     * associated node, if it exists
     *
     * The keys are returned as a pair, where the enum none is used to indicate a missing value
     *
     * @param v Version to search
     * @param i Index i of element to search for
     * @return A pair of integers, with first being the key of the main node and second being
     *         the key of the associated node, if it exists, i.e.
     *         - (none, none): index i was out of range, nothing to return
     *         - (key, none): element i had no associated key, so only key is returned
     *         - (key, assoc key): element i has associated key, so both key and assoc key returned
     */
    std::pair<int, int> search(int v, int i);

    /**
     * Inserts the key k as the new ith element in the list, i.e., between the (i - 1)st and ith
     * element of the current version
     *
     * @param k Key to insert
     * @param i Index i to insert
     * @param k_prime Associated key, defaults to none
     * @return Boolean being true if key(s) was inserted and false otherwise
     */
    bool insert(int k, int i, int k_prime=none);

    /**
     * Updates the key in the ith element to the given key in the newest version,
     * and also the key in the associated item, if present
     *
     * @param k New key
     * @param i Index i to update
     * @param k_prime New associated key, defaults to none
     * @return Boolean being true if ith element was updated and false otherwise
     */
    bool update(int k, int i, int k_prime=none);

    /**
     * Housekeeping helper function to stringify how much memory is currently used by the data structure
     */
    void show_info();

    /**
     * Prints the list using the following format
     *
     * main list:         [  pred_v,  pred_k|  v,  k|  succ_v,  succ_k]    [ ... ]   ...
     * associated list: ( [a_pred_v,a_pred_k|a_v,a_k|a_succ_v,a_succ_k] )( [ ... ] ) ...
     *
     * where
     *   - pred_v   = predecessor version
     *   - pred_k   = predecessor key
     *   - v        = node version
     *   - k        = node key
     *   - succ_v   = successor version
     *   - succ_k   = successor key
     *   - a_pred_v = associated predecessor version
     *   - a_pred_v = associated predecessor key
     *   - a_v      = associated node version
     *   - a_k      = associated node key
     *   - a_succ_v = associated successor version
     *   - a_succ_k = associated successor key
     *
     * and the parentheses of the associated list indicating that an associated node may or may not
     * be present
     *
     * This is mainly intended as a debug function to verify correctness. As such, note that printing
     * a list with more than a few primary_nodes to the terminal will result in a very messy output. If you
     * want to see the contents of a list with 10-20 primary_nodes, directing the output to a file and
     * opening it with an editor that supports horizontal scrolling should give you a decent result.
     *
     * @param v Version to stringify.
     * @return The contents of the vth version of the list formatted as a string suitable for printing.
     */
    std::string stringify(int v) const;

    /**
     * Allows the list to be printed using the stringify method
     *
     * @param s Reference to output stream
     * @param l PPList object to stringify
     * @return Reference to output stream
     */
    friend std::ostream &operator<<(std::ostream &s, const PPList &l);

    /**
     * Encapsulates an integer version number v and an extra pointer type pt to enable
     * such a pair to be used as a key in an unordered map
     */
    struct EPKey
    {
        int v;
        PtrType pt;
    };

    struct Hash
    {
        /**
         * Enables hashing of EPKeys
         *
         * References:
         *   https://en.cppreference.com/w/cpp/utility/hash
         *   https://www.boost.org/doc/libs/1_64_0/boost/functional/hash/hash.hpp
         *
         * @param ep_key EPKey to hash
         * @return Hash of EPKey
         */
        std::size_t operator()(EPKey const &ep_key) const noexcept
        {
            std::size_t h1 = std::hash<int>{}(ep_key.v);
            std::size_t h2 = std::hash<PtrType>{}(ep_key.pt);
            h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2); // boost::hash_combine
            return h1;
        }
    };

    class PNode
    {
    public:
        /**
         * Default constructor
         *
         * @param version Version stamp
         * @param key Key to store
         * @param t Node type
         */
        PNode(int version, int key, PNodeType t);

        /**
         * Default destructor
         */
        ~PNode() = default;

        /**
         * Finds the live pointer of type pt of version v of this node
         *
         * @param v Version
         * @param pt Pointer type
         * @return A pair, where first is an EPKey containing the version number and pointer type and
         *         second is the actual pointer. Note that the version number of EPKey may be none,
         *         indicating that the version of the pointer is that of the node, and thus the pointer
         *         is stored in its respective field instead of the map of extra pointers
         */
        std::pair<EPKey, PNode *> live_ptr(int v, PtrType pt);

        int version;
        int key;
        PNodeType t;

        PNode *next;
        PNode *assoc;
        PNode *copy;
        PNode *pred;

        // Maximum number of allowed extra pointers
        const std::size_t max_eps;
        std::unordered_map<EPKey, PNode *, Hash> extra_pointers;
    };

// --------------------------------------------------------------------------------
// Private
// --------------------------------------------------------------------------------
private:
    int m_version;

    // Vector tracking number of elements of each version
    std::vector<int> n;

    // Access list for first main element of vth version
    std::vector<PNode *> head;

    // Access list for first associated node of vth version
    std::vector<PNode *> assoc_head;

    std::vector<PNode *> node_ptrs;

    // For keeping track of number of primary_nodes, extra pointers, and memory used
    int primary_nodes;
    int assoc_nodes;
    int eps;
    std::size_t eps_memory;
    std::size_t total_memory;

    /**
     * Checks whether index i is out of bounds of version v
     *
     * @param v Version
     * @param i Index
     * @return True if index i is out of bounds and false otherwise
     */
    bool index_out_of_range(int v, int i);

    /**
     * Traverses the list to find the ith position
     *
     * Returns a tuple of four node pointers:
     *
     *   - node_i   is the node in the ith position.
     *   - node_j   is the predecessor of node_i, i.e. the node in the (i - 1)st = jth position
     *   - node_k   is the node with largest index less than i which has an associated node
     *              Thus, if node_j has an associated node, node_k == node_j
     *   - assoc_k  is the associated node of node_k and thus the associated node with the largest
     *              index less than i
     *
     * @param i Index to find
     * @return A tuple of pointers to primary_nodes node_i, node_j, node_k, and assoc_k
     */
    std::tuple<PNode *, PNode *, PNode *, PNode *> find_index(int i);

// --------------------------------------------------------------------------------
// Insertion
// --------------------------------------------------------------------------------

    /**
     * Inserts a node, and if present an associated node, as the new head of the list
     *
     * Note that we in the context of insertion use head to mean the first element of
     * the list and tail to mean the *rest* of the list and thus NOT the last element
     *
     * @param new_node Node to insert
     */
    void insert_head(PNode *new_node);

    /**
     * Inserts a node, and if present an associated node, in the tail part of the list
     *
     * Note that we in the context of insertion use head to mean the first element of
     * the list and tail to mean the *rest* of the list and thus NOT the last element
     *
     * @param new_node Node to insert
     * @param i Index i indicating position to insert
     */
    void insert_tail(PNode *new_node, int i);

    /**
     * Handle the insertion of an associated node into the list, by updating pointers and
     * copying primary_nodes if necessary
     *
     * @param new_assoc New associated node to insert
     * @param assoc_k The associated node with the largest index less than the associated
     *                node to be inserted, i.e. the new predecessor of new_assoc
     * @param S Queue of pointers to copied primary_nodes
     * @return A pair, with first being a pointer to the now updated associated node assoc_k
     *         or its copy if assoc_k was copied, and second being true if assoc_k was copied
     *         and false otherwise
     */
    std::pair<PNode *, bool> insert_tail_handle_assoc_node(PNode *new_assoc, PNode *assoc_k, std::queue<PNode *> &S);

    /**
     * Handle the insertion of a node into the main part of the list, by updating pointers and
     * copying primary_nodes if necessary
     *
     * @param new_node New node to insert
     * @param node_j The node with the largest index less than new_node
     * @param node_k The node with the largest index less than new_node, which has an associated node, assoc_k
     * @param assoc_k The associated node of node_k, i.e. the associated node with the largest index
     *                less than new_node
     * @param assoc_k_copied A boolean indicating if assoc_k has been copied
     * @param S Queue of pointers to copied primary_nodes
     */
    void insert_tail_handle_main_node(PNode *new_node, PNode *node_j, PNode *node_k, PNode *assoc_k,
                                      bool assoc_k_copied, std::queue<PNode *> &S);

// --------------------------------------------------------------------------------
// Postprocessing
// --------------------------------------------------------------------------------

    /**
     * Postprocess copied primary_nodes according to [1, p. 95], that is, for each copied node y_bar,
     * find in y_bar's predecessor x_bar, the live pointer to y_bar and update the pointer in
     * x_bar to point to the copy of y_bar, copying x_bar if necessary
     *
     * @param S Queue of pointers to copied primary_nodes to postprocess
     */
    void postprocess_copied_nodes(std::queue<PNode *> &S);

    /**
     * Handle the update of x_bar in the postprocessing step, but either updating the appropriate
     * field, adding an extra pointer or copying x_bar if necessary
     *
     * @param x_bar Predecessor of y_bar
     * @param y_bar Node that was copied
     * @param S Queue of pointers to copied primary_nodes
     */
    void postprocessing_step_handle_x_bar(PNode *x_bar, PNode *y_bar, std::queue<PNode *> &S);

// --------------------------------------------------------------------------------
// Helper methods for linking primary_nodes
// --------------------------------------------------------------------------------

    /**
     * Link source src and target tgt primary_nodes by setting src->next to point to tgt and tgt->pred
     * to point to src
     *
     * @param src Source node
     * @param tgt Target node
     */
    void link_src_and_tgt(PNode *src, PNode *tgt);

    /**
     * Updates a pointer field in a node or copies it if there is no room for any additional extra pointers
     *
     * @param src Source node to update
     * @param pt Pointer type to update
     * @param tgt Target of pointer to update
     * @param S Queue of pointers to copied primary_nodes
     * @return A pair, with first being a pointer to the newly updated node or its copy, if the node was copied,
     *         and second being true if the node was copied and false otherwise
     */
    std::pair<PNode *, bool> update_or_copy_node_and_link_tgt(PNode *src, PtrType pt, PNode *tgt,
                                                              std::queue<PNode *> &S);

    /**
     * Updates source node src field of type pt to point to target tgt
     *
     * @param src Source node to update
     * @param pt Pointer type to update
     * @param tgt Target of pointer to update
     * @return Pointer to the updated node
     */
    PPList::PNode *update_ptr(PNode *src, PtrType pt, PNode *tgt);

    /**
     * Adds to source node src an extra pointer of type pt to target tgt
     *
     * @param src Source node to update
     * @param pt Pointer type to update
     * @param tgt Target of pointer to update
     * @return Pointer to the updated node
     */
    PPList::PNode *add_extra_ptr(PNode *src, PtrType pt, PNode *tgt);

    /**
     * Copies the source node src, initializing the copied node from the information in src and
     * also sets the pointer of type pt in the newly copied node to point to target tgt (for which
     * there was no room in src)
     *
     * Also sets the copy pointer of src to point to the newly copied node and adds src to the queue
     * S for post processing
     *
     * @param src Source node to update
     * @param pt Pointer type to update
     * @param tgt Target of pointer to update
     * @param S Queue of pointers to copied primary_nodes
     * @return A pair, with first being a pointer to the newly copied node and second being true
     */
    std::pair<PNode *, bool> copy_node(PNode *src, PtrType pt, PNode *tgt, std::queue<PNode *> &S);

// --------------------------------------------------------------------------------
// Helper method for printing
// --------------------------------------------------------------------------------

    /**
     * Static helper method to operator<< that "stringifies" a node and directing it to the given
     * ostream
     *
     * @param ostream Receiving stream
     * @param node Node to stringify
     * @param next Successor of node
     */
    static void node_to_ostringstream(std::ostringstream &ostream, PPList::PNode *node, PPList::PNode *next);
};

// --------------------------------------------------------------------------------
// Free functions
// --------------------------------------------------------------------------------
/**
 * Compares two EPKeys for equality
 *
 * @param lhs Left-hand side operand of equality operator.
 * @param rhs Right-hand side operand of equality operator.
 * @return True if both lhs and rhs have same version number and pointer type and false otherwise
 */
    bool operator==(const PPList::EPKey &lhs, const PPList::EPKey &rhs);

/**
 * Compares two EPKeys for inequality
 *
 * @param lhs Left-hand side operand of inequality operator.
 * @param rhs Right-hand side operand of inequality operator.
 * @return True if both lhs and rhs have same version number and pointer type and false otherwise
 */
    bool operator!=(const PPList::EPKey &lhs, const PPList::EPKey &rhs);

} // namespace DM803

#endif //PPLIST_HPP
