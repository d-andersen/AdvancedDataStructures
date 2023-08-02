/**
 * @file PPList.cpp
 * @author Dennis Andersen - deand17
 * @brief Implementation of partially persistent singly-linked list
 * @date 2022-04-25
 *
 * DM803 Advanced Data Structures
 *
 * Exam Project - Part 2 - Spring 2022
 *
 * Implementation of partially persistent singly-linked list
 */
#include <iomanip>
#include <sstream>

#include "PPList.hpp"

namespace DM803
{
// --------------------------------------------------------------------------------
// Public
// --------------------------------------------------------------------------------

PPList::PPList() : m_version(0), primary_nodes(0), assoc_nodes(0), eps(0), eps_memory(0), total_memory(0)
{
    n.push_back(0);
    head.push_back(nullptr);
    assoc_head.push_back(nullptr);
    total_memory += sizeof(*this) + sizeof(int) + (2 * sizeof(nullptr));
}

PPList::~PPList()
{
    clear();
}

int PPList::size() const
{
    return n[m_version];
}

int PPList::size(int v) const
{
    return n[v];
}

int PPList::version() const
{
    return m_version;
}

void PPList::clear()
{
    for (auto ptr : node_ptrs) {
        delete ptr;
    }
    primary_nodes = 0;
    assoc_nodes = 0;
    eps = 0;
    eps_memory = 0;
    total_memory = sizeof(*this) + sizeof(int) + (2 * sizeof(nullptr));
}

void PPList::newversion()
{
    ++m_version;
    n.push_back(n[m_version - 1]);
    head.push_back(head[m_version - 1]);
    assoc_head.push_back(assoc_head[m_version - 1]);
    total_memory += sizeof(int) + (2 * sizeof(PNode *));
}

std::pair<int, int> PPList::search(const int v, const int i)
{
    std::cout << v << " " << i << '\n';
    if (i < 0 || i >= n[v] || n[v] == 0) {
        return std::make_pair(none, none);
    }
    auto node{head[v]};
    for (int j = 0; j < i; ++j) {
        node = node->live_ptr(v, PtrType::next).second;
    }
    int assoc_key{none};
    if (node->assoc) {
        assoc_key = node->live_ptr(v, PtrType::assoc).second->key;
    }
    return std::make_pair(node->key, assoc_key);
}

bool PPList::insert(const int k, const int i, const int k_prime)
{
    if (index_out_of_range(m_version, i) || k == none) {
        return false;
    }
    newversion();
    auto new_node{new PNode(m_version, k, PNodeType::pnode)};
    node_ptrs.push_back(new_node);
    ++primary_nodes;
    total_memory += sizeof(*new_node) + sizeof(new_node);
    if (k_prime != none) {
        new_node->assoc = new PNode(m_version, k_prime, PNodeType::anode);
        node_ptrs.push_back(new_node->assoc);
        ++assoc_nodes;
        total_memory += sizeof(*new_node->assoc) + sizeof(new_node->assoc);
    }
    if (i == 0) {
        insert_head(new_node);
    } else {
        insert_tail(new_node, i);
    }
    ++n[m_version];
    return true;
}

bool PPList::update(const int k, const int i, const int k_prime)
{
    if (i == n[m_version] || index_out_of_range(m_version, i) || k == none) {
        return false;
    }
    auto [node_i, node_j, node_k, assoc_k] = find_index(i);

    newversion();
    auto node_i_copy{new PNode(m_version, k, node_i->t)};
    node_ptrs.push_back(node_i_copy);
    ++primary_nodes;
    total_memory += sizeof(*node_i_copy) + sizeof(node_i_copy);
    node_i->copy = node_i_copy;

    auto node_i_next{node_i->live_ptr(m_version, PtrType::next).second};
    link_src_and_tgt(node_i_copy, node_i_next);

    auto S{std::queue<PNode *>()};
    if (node_j) {
        update_or_copy_node_and_link_tgt(node_j, PtrType::next, node_i_copy, S);
    }
    if (i == 0) {
        head[m_version] = node_i_copy;
    }

    auto assoc_i{node_i->live_ptr(m_version, PtrType::assoc).second};
    if (k_prime != none) {
        auto new_assoc_i{new PNode(m_version, k_prime, PNodeType::anode)};
        node_ptrs.push_back(node_i_copy);
        ++assoc_nodes;
        total_memory += sizeof(*new_assoc_i) + sizeof(new_assoc_i);

        PNode *assoc_i_next{nullptr};
        if (assoc_i) {
            assoc_i_next = assoc_i->live_ptr(m_version, PtrType::next).second;
            assoc_i->copy = new_assoc_i;
        } else if (assoc_k) {
            assoc_i_next = assoc_k->live_ptr(m_version, PtrType::next).second;
            update_or_copy_node_and_link_tgt(assoc_k, PtrType::next, new_assoc_i, S);
        } else {
            assoc_i_next = assoc_head[m_version];
        }
        link_src_and_tgt(new_assoc_i, assoc_i_next);
        node_i_copy->assoc = new_assoc_i;
        if (i == 0) {
            assoc_head[m_version] = new_assoc_i;
        }
    } else {
        node_i_copy->assoc = assoc_i;
        if (i == 0) {
            assoc_head[m_version] = assoc_i;
        }
    }
    postprocess_copied_nodes(S);
    return true;
}

void PPList::show_info()
{
    auto node_size = sizeof(PNode(m_version, 0, PNodeType::pnode));
    std::cout << std::setfill('=') << std::setw(40) << "" << '\n';

    std::cout << "Version:"          << std::setfill(' ') << std::setw(26) << std::right << m_version     << '\n';
    std::cout << "Primary nodes:"    << std::setfill(' ') << std::setw(20) << std::right << primary_nodes << '\n';
    std::cout << "Associated nodes:" << std::setfill(' ') << std::setw(17) << std::right << assoc_nodes   << '\n';
    std::cout << "Total nodes:"      << std::setfill(' ') << std::setw(22) << std::right << primary_nodes
                                                                                                + assoc_nodes   << '\n';
    std::cout << "Extra pointers:"   << std::setfill(' ') << std::setw(19) << std::right << eps           << '\n';
    std::cout << std::setfill('-') << std::setw(30) << "" << '\n';

    std::size_t primary_node_memory{(primary_nodes * node_size)};
    std::size_t assoc_node_memory{(assoc_nodes * node_size)};
    std::size_t list_obj_memory{total_memory - primary_node_memory - assoc_node_memory - eps_memory};
    std::cout << "Memory usage\n";
    std::cout << "PPList:"           << std::setfill(' ') << std::setw(27) << std::right << list_obj_memory     << " bytes\n";
    std::cout << "Primary nodes:"    << std::setfill(' ') << std::setw(20) << std::right << primary_node_memory << " bytes\n";
    std::cout << "Associated nodes:" << std::setfill(' ') << std::setw(17) << std::right << assoc_node_memory   << " bytes\n";
    std::cout << "Extra pointers:"   << std::setfill(' ') << std::setw(19) << std::right << eps_memory          << " bytes\n";
    std::cout << "In total:"         << std::setfill(' ') << std::setw(25) << std::right << total_memory        << " bytes\n";

    std::cout << std::setfill('=') << std::setw(40) << "" << '\n';
}

PPList::PNode::PNode(const int version, const int key, const PNodeType t) :
        version(version), key(key), t(t), next(nullptr), assoc(nullptr), copy(nullptr), pred(nullptr), max_eps(16)
{
}

std::pair<PPList::EPKey, PPList::PNode *> PPList::PNode::live_ptr(int v, PtrType pt)
{
    PNode *succ_p{pt == PtrType::assoc ? assoc : next};
    EPKey ep_key{none, pt};
    if (version == v) {
        return std::make_pair(ep_key, succ_p);
    }
    int ep_v{none};
    for (auto& ep: extra_pointers) {
        if (ep_v < ep.first.v && ep.first.v <= v && pt == ep.first.pt) {
            ep_v = ep.first.v;
            succ_p = ep.second;
        }
    }
    ep_key.v = ep_v;
    return std::make_pair(ep_key, succ_p);
}

// --------------------------------------------------------------------------------
// Private
// --------------------------------------------------------------------------------

bool PPList::index_out_of_range(const int v, const int i)
{
    if (i < 0 || i > n[v]) {
        std::cerr << "Error: index i out of range\n";
        return true;
    }
    return false;
}

std::tuple<PPList::PNode *, PPList::PNode *, PPList::PNode *, PPList::PNode *> PPList::find_index(int i)
{
    auto node_i{head[m_version]};
    PNode *node_j{nullptr};
    PNode *node_k{nullptr};
    PNode *assoc_k{nullptr};

    for (int j = 0; j < i; ++j) {
        node_j = node_i;
        node_i = node_i->live_ptr(m_version, PtrType::next).second;
        if (node_j->assoc) {
            node_k = node_j;
            assoc_k = node_j->live_ptr(m_version, PtrType::assoc).second;
        }
    }
    return std::make_tuple(node_i, node_j, node_k, assoc_k);
}

// --------------------------------------------------------------------------------
// Insertion
// --------------------------------------------------------------------------------

void PPList::insert_head(PPList::PNode *new_node)
{
    link_src_and_tgt(new_node, head[m_version - 1]);
    head[m_version] = new_node;
    if (new_node->assoc) {
        link_src_and_tgt(new_node->assoc, assoc_head[m_version - 1]);
        assoc_head[m_version] = new_node->assoc;
    }
}

void PPList::insert_tail(PNode *new_node, int i)
{
    auto [node_i, node_j, node_k, assoc_k] = find_index(i);
    auto S{std::queue<PNode *>()};
    bool assoc_k_copied{false};

    if (new_node->assoc) {
        std::tie(assoc_k, assoc_k_copied) = insert_tail_handle_assoc_node(new_node->assoc, assoc_k, S);
    }
    insert_tail_handle_main_node(new_node, node_j, node_k, assoc_k, assoc_k_copied, S);

    postprocess_copied_nodes(S);
}

std::pair<PPList::PNode *, bool> PPList::insert_tail_handle_assoc_node(PNode *new_assoc, PNode *assoc_k,
                                                                       std::queue<PNode *> &S)
{
    bool assoc_k_copied{false};
    if (assoc_k) { // there is a pred assoc node
        link_src_and_tgt(new_assoc, assoc_k->live_ptr(m_version, PtrType::next).second);
        std::tie(assoc_k, assoc_k_copied) = update_or_copy_node_and_link_tgt(assoc_k, PtrType::next,
                                                                                    new_assoc, S);
    } else {       // there is NO pred assoc node, meaning new_node->assoc will be new assoc head
        link_src_and_tgt(new_assoc, assoc_head[m_version - 1]);
        assoc_head[m_version] = new_assoc;
    }
    return std::make_pair(assoc_k, assoc_k_copied);
}

void PPList::insert_tail_handle_main_node(PNode *new_node, PNode *node_j, PNode *node_k, PNode *assoc_k,
                                          bool assoc_k_copied, std::queue<PNode *> &S)
{
    link_src_and_tgt(new_node, node_j->live_ptr(m_version, PtrType::next).second);
    if (assoc_k_copied) {
        auto [updated_node_k, node_k_copied] = update_or_copy_node_and_link_tgt(
                node_k,PtrType::assoc, assoc_k, S);
        if (node_k == node_j) {
            update_or_copy_node_and_link_tgt(updated_node_k, PtrType::next, new_node, S);
        } else {
            update_or_copy_node_and_link_tgt(node_j, PtrType::next, new_node, S);
        }
    } else {
        update_or_copy_node_and_link_tgt(node_j, PtrType::next, new_node, S);
    }
}

// --------------------------------------------------------------------------------
// Postprocessing
// --------------------------------------------------------------------------------

void PPList::postprocess_copied_nodes(std::queue<PNode *> &S)
{
    while (!S.empty()) {
        auto y_bar = S.front(); // node that was copied
        S.pop();
        auto x_bar = y_bar->pred;
        if (x_bar) {
            postprocessing_step_handle_x_bar(x_bar, y_bar, S);
        } else {
            if (y_bar->t == PNodeType::pnode) {
                head[m_version] = y_bar->copy;
            } else {
                assoc_head[m_version] = y_bar->copy;
            }
        }
        y_bar->copy->next->pred = y_bar->copy;
    }
}

void PPList::postprocessing_step_handle_x_bar(PNode *x_bar, PNode *y_bar, std::queue<PNode *> &S)
{
    while (y_bar->copy) {
        y_bar = y_bar->copy;
    }
    if (x_bar->version == m_version && x_bar->next == y_bar) {
        x_bar->next = y_bar->copy;
        y_bar->copy->pred = x_bar;
    } else {
        int live_ptr_v{0};
        for (auto& ep: x_bar->extra_pointers) {
            if (ep.second == y_bar) {
                live_ptr_v = ep.first.v;
            }
        }
        if (live_ptr_v == m_version) {
            x_bar->extra_pointers[{live_ptr_v, PtrType::next}] = y_bar->copy;
            y_bar->copy->pred = x_bar;
        } else {
            update_or_copy_node_and_link_tgt(x_bar, PtrType::next, y_bar, S);
        }
    }
}

// --------------------------------------------------------------------------------
// Helper methods for linking primary_nodes
// --------------------------------------------------------------------------------

void PPList::link_src_and_tgt(PNode *src, PNode *tgt)
{
    src->next = tgt;
    if (tgt) {
        tgt->pred = src;
    }
}

std::pair<PPList::PNode *, bool> PPList::update_or_copy_node_and_link_tgt(PNode *src, PtrType pt, PNode *tgt,
                                                                          std::queue<PNode *> &S)
{
    PNode *live_src{nullptr};
    bool src_copied{false};
    if (src->version == m_version) {
        live_src = update_ptr(src, pt, tgt);
    } else if (src->extra_pointers.size() < src->max_eps) {
        live_src= add_extra_ptr(src, pt, tgt);
    } else {
        std::tie(live_src, src_copied) = copy_node(src, pt, tgt, S);
    }
    if (pt == PtrType::next) {
        tgt->pred = live_src;
    }
    return std::make_pair(live_src, src_copied);
}

PPList::PNode *PPList::update_ptr(PNode *src, PtrType pt, PNode *tgt)
{
    if (pt == PtrType::assoc) {
        src->assoc = tgt;
    } else {
        src->next = tgt;
    }
    return src;
}

PPList::PNode *PPList::add_extra_ptr(PNode *src, PtrType pt, PNode *tgt)
{
    EPKey ep_key{m_version, pt};
    src->extra_pointers.insert({ep_key, tgt});
    ++eps;
    auto ep_mem_size = sizeof(EPKey) + sizeof(tgt);
    eps_memory += ep_mem_size;
    total_memory += ep_mem_size;
    return src;
}

std::pair<PPList::PNode *, bool> PPList::copy_node(PNode *src, PtrType pt, PNode *tgt, std::queue<PNode *> &S)
{
    auto src_copy {new PNode(m_version, src->key, src->t)};
    node_ptrs.push_back(src_copy);
    if (src->t == PNodeType::pnode) {
        ++primary_nodes;
    } else {
        ++assoc_nodes;
    }
    total_memory += sizeof(*src_copy) + sizeof(src_copy);
    if (pt == PtrType::assoc) {
        src_copy->assoc = tgt;
        src_copy->next = src->live_ptr(m_version, PtrType::next).second;
    } else {
        src_copy->assoc = src->live_ptr(m_version, PtrType::assoc).second;
        src_copy->next = tgt;
    }
    src_copy->pred = src->pred;
    src->copy = src_copy;
    S.push(src);
    return std::make_pair(src_copy, true);
}

// --------------------------------------------------------------------------------
// Helper method for printing
// --------------------------------------------------------------------------------

void PPList::node_to_ostringstream(std::ostringstream &ostream, PPList::PNode *node, PPList::PNode *next)
{
    ostream << "[";
    if (node->pred) {
        ostream << std::setfill(' ') << std::setw(4) << node->pred->version << ",";
        ostream << std::setfill(' ') << std::setw(4) << node->pred->key << "|";
    } else {
        ostream << "     null|";
    }
    ostream << std::setfill(' ') << std::setw(4) << node->version << ",";
    ostream << std::setfill(' ') << std::setw(4) << node->key << "|";
    if (next) {
        ostream << std::setfill(' ') << std::setw(4) << next->version << ",";
        ostream << std::setfill(' ') << std::setw(4) << next->key << "]";
    } else {
        ostream << "     null]";
    }
}

std::string PPList::stringify(const int v) const
{
    std::string s{};
    if (size(v) == 0) {
        return s;
    }
    PPList::PNode *node{head[v]};
    PPList::PNode *assoc;
    std::ostringstream main_list;
    std::ostringstream assoc_list;
    for (int i = 0; i < size(v); ++i) {
        if (i > 0) {
            main_list << "    ";
            assoc_list << "    ";
        }
        auto next = node->live_ptr(v, PPList::PtrType::next).second;
        PPList::node_to_ostringstream(main_list, node, next);
        assoc = node->live_ptr(v, PPList::PtrType::assoc).second;

        if (assoc) {
            auto assoc_next = assoc->live_ptr(v, PPList::PtrType::next).second;
            PPList::node_to_ostringstream(assoc_list, assoc, assoc_next);
        } else {
            assoc_list << std::setfill(' ') << std::setw(31) << "";
        }
        node = next;
    }
    s = main_list.str() + '\n' + assoc_list.str();
    return s;
}

// --------------------------------------------------------------------------------
// Free functions
// --------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &s, const PPList &l)
{
    if (l.size() == 0) {
        return s;
    }
    s << l.stringify(l.m_version);
    return s;
}

bool operator==(const PPList::EPKey &lhs, const PPList::EPKey &rhs)
{
    return lhs.v == rhs.v && lhs.pt == rhs.pt;
}

bool operator!=(const PPList::EPKey &lhs, const PPList::EPKey &rhs)
{
    return !(lhs == rhs);
}

} // namespace DM803
