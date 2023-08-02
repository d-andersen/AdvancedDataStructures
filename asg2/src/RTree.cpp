/**
 * @file RTree.cpp
 * @author Dennis Andersen - deand17
 * @brief Header file for randomized search tree.
 * @date 2022-05-03
 *
 * DM803 Advanced Data Structures
 *
 * Exam Project - Part 2 - Spring 2022
 *
 * Implementation of randomized search tree.
 */
#include "RTree.hpp"

#include <iomanip>
#include <queue>

namespace DM803
{
// --------------------------------------------------------------------------------
// Public
// --------------------------------------------------------------------------------

RTree::RTree() : n(0), m_root(nullptr), rng(rd()),
                 uni_int_dist(std::uniform_int_distribution<unsigned long long>(Pr_val::pr_min, Pr_val::pr_max))
{
}

RTree::RTree(const RTree &other) : n(0), m_root(nullptr), rng(rd()),
                                   uni_int_dist(std::uniform_int_distribution<unsigned long long>(Pr_val::pr_min, Pr_val::pr_max))
{
    auto q {std::queue<const Node *>()};
    q.push(other.root());
    while(!q.empty()) {
        auto node = q.front();
        q.pop();
        if (node) {
            tree_insert(node->key, node->pr);
            q.push(node->left);
            q.push(node->right);
        }
    }
}

RTree::~RTree()
{
    clear();
}

RTree &RTree::operator=(const RTree &other)
{
    auto tmp = other;
    swap(*this, tmp);
    return *this;
}

int RTree::size() const
{
    return n;
}

bool RTree::empty() const
{
    return size() == 0;
}

void RTree::clear()
{
    tree_postorder_erase(m_root);
    m_root = nullptr;
    n = 0;
}

std::pair<RTree::Node *, int> RTree::search(const int key)
{
    return tree_iterative_search(key);
}

std::pair<RTree::Node *, bool> RTree::insert(const int key, const Pr_val pr)
{
    if (pr != Pr_val::pr_dummy) {
        return tree_insert(key, pr);
    }
    return tree_insert(key, gen_pr());
}

std::optional<RTree> RTree::split(const int key)
{
    auto [x, inserted]{tree_insert(key, Pr_val::pr_dummy)};

    if (inserted) {
        auto all_larger{RTree()};
        std::swap(all_larger.m_root, x->right);
        tree_inorder_walk_count_nodes(all_larger.m_root, all_larger.n);
        if (all_larger.n > 0) {
            all_larger.m_root->parent = nullptr;
        }

        m_root = x->left;
        m_root->parent = nullptr;
        delete x;
        n = 0;
        tree_inorder_walk_count_nodes(m_root, n);
        return all_larger;
    }

    return {};
}

RTree RTree::merge(RTree &x, RTree &y)
{
    auto t{RTree()};
    t.n = x.size() + y.size();

    auto r{tree_merge(x.m_root, y.m_root)};
    std::swap(r, t.m_root);

    x.m_root = nullptr;
    y.m_root = nullptr;
    x.n = 0;
    y.n = 0;

    return t;
}

std::pair<RTree::Node *, bool> RTree::erase(const int key)
{
    auto [x, sd]{tree_iterative_search(key)};
    bool found{false};
    if (x) {
        found = true;
        auto r{tree_merge(x->left, x->right)};
        if (x->parent) {
            auto y{x->parent};
            if (y->left == x) {
                y->left = r;
            } else {
                y->right = r;
            }
            if (r) {
                r->parent = y;
            }
        } else {
            m_root = r;
            if (r) {
                r->parent = nullptr;
            }
        }
        delete x;
        --n;
        x = r;
    }
    return std::make_pair(x, found);
}

RTree::Node *RTree::root() const
{
    return m_root;
}

std::optional<int> RTree::front()
{
    auto node{tree_minimum(m_root)};
    if (node) {
        return node->key;
    }
    return {};
}

const std::optional<int> RTree::front() const
{
    auto node{tree_minimum(m_root)};
    if (node) {
        return node->key;
    }
    return {};
}

std::optional<int> RTree::back()
{
    auto node{tree_maximum(m_root)};
    if (node) {
        return node->key;
    }
    return {};
}

const std::optional<int> RTree::back() const
{
    auto node{tree_maximum(m_root)};
    if (node) {
        return node->key;
    }
    return {};
}

RTree::Node *RTree::begin()
{
    return m_root ? tree_minimum(m_root) : nullptr;
}

const RTree::Node *RTree::begin() const
{
    return m_root ? tree_minimum(m_root) : nullptr;
}

RTree::Node *RTree::end()
{
    return nullptr;
}

const RTree::Node *RTree::end() const
{
    return nullptr;
}

void swap(RTree &lhs, RTree &rhs) noexcept
{
    std::swap(lhs.n, rhs.n);
    std::swap(lhs.m_root, rhs.m_root);
}

std::ostream &operator<<(std::ostream &s, const RTree &t)
{
    auto q {std::queue<const RTree::Node *>()};
    q.push(t.m_root);
    while(!q.empty()) {
        auto *node = q.front();
        q.pop();
        if (node) {
            s << '(' << std::setfill(' ') << std::setw( 5) << node->key << ','
                     << std::setfill(' ') << std::setw(20) << node->pr  << "): ";
            if (node->left) {
                s << "L: (" << std::setfill(' ') << std::setw( 5) << node->left->key << ','
                            << std::setfill(' ') << std::setw(20) << node->left->pr << ')';
            } else {
                s << "L: (---------- null ----------)";
            }
            s << " | ";
            if (node->right) {
                s << "R: (" << std::setfill(' ') << std::setw(  5) << node->right->key << ','
                            << std::setfill(' ') << std::setw( 20) << node->right->pr  << ')';
            } else {
                s << "R: (---------- null ----------)";
            }
            s << '\n';
            q.push(node->left);
            q.push(node->right);
        }
    }
    return s;
}


// --------------------------------------------------------------------------------
// Private
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// Utility methods
// --------------------------------------------------------------------------------

unsigned long long RTree::gen_pr()
{
    return uni_int_dist(rng);
}

void RTree::left_rotate(RTree::Node *x)
{
    auto y{x->right};
    x->right = y->left;
    if (y->left) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (!x->parent) {
        m_root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void RTree::right_rotate(RTree::Node *y)
{
    auto x{y->left};
    y->left = x->right;
    if (x->right) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (!y->parent) {
        m_root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

void RTree::tree_inorder_walk_count_nodes(Node *x, int &c)
{
    if (x) {
        tree_inorder_walk_count_nodes(x->left, c);
        ++c;
        tree_inorder_walk_count_nodes(x->right, c);
    }
}

void RTree::tree_postorder_erase(Node *x)
{
    if (x) {
        tree_postorder_erase(x->left);
        tree_postorder_erase(x->right);
        delete x;
    }
}

std::pair<RTree::Node *, int> RTree::tree_iterative_search(const int key)
{
    int search_depth{0};
    auto x{m_root};
    while (x && key != x->key) {
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
        ++search_depth;
    }
    return std::make_pair(x, search_depth);
}

void RTree::tree_heapify(RTree::Node *x)
{
    while (x && x->parent && x->pr < x->parent->pr) {
        if (x == x->parent->left) {
            right_rotate(x->parent);
        } else {
            left_rotate(x->parent);
        }
    }
}

RTree::Node *RTree::tree_minimum(RTree::Node *x)
{
    while (x->left) {
        x = x->left;
    }
    return x;
}

RTree::Node *RTree::tree_maximum(RTree::Node *x)
{
    while (x->right) {
        x = x->right;
    }
    return x;
}

std::pair<RTree::Node *, bool> RTree::tree_insert(const int key, const unsigned long long pr)
{
    Node *y{nullptr};
    auto x{m_root};
    auto z{new Node(key, pr)};
    while (x) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent = y;
    if (!y) {
        m_root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else if (z->key == y->key) {
        delete z;
        return std::make_pair(nullptr, false);
    } else {
        y->right = z;
    }
    ++n;
    tree_heapify(z);
    return std::make_pair(z, true);
}

RTree::Node *RTree::tree_merge(Node *a, Node *b)
{
    // Base case
    if (!a && !b) {
        return nullptr;
    }
    if (!a) {
        return b;
    }
    if (!b) {
        return a;
    }

    Node *x{nullptr};
    Node *y{nullptr};
    if (a->pr < b->pr) {
        x = a;
        y = b;
    } else {
        x = b;
        y = a;
    }

    if (x->key < y->key) {
        x->right = tree_merge(x->right, y);
        x->right->parent = x;
    } else {
        x->left = tree_merge(x->left, y);
        x->left->parent = x;
    }
    return x;
}



RTree::Node::Node(const int key, const unsigned long long pr) :
    key(key), pr(pr), parent(nullptr), left(nullptr), right(nullptr)
{
}

RTree::Node::~Node() = default;

RTree::Node *RTree::Node::next()
{
    auto x {this};
    if (x->right) {
        return tree_minimum(x->right);
    }
    auto y {x->parent};
    while (y && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

const RTree::Node *RTree::Node::next() const
{
    auto x {this};
    if (x->right) {
        return tree_minimum(x->right);
    }
    auto y {x->parent};
    while (y && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

RTree::Node *RTree::Node::prev()
{
    auto x {this};
    if (x->left) {
        return tree_maximum(x->left);
    }
    auto y {x->parent};
    while (y && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

const RTree::Node *RTree::Node::prev() const
{
    auto x {this};
    if (x->left) {
        return tree_maximum(x->left);
    }
    auto y {x->parent};
    while (y && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

} // DM803
