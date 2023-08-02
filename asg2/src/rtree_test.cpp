/**
 * @file rtree_test.cpp
 * @author Dennis Andersen - deand17
 * @brief Test file for randomized search tree.
 * @date 2022-05-06
 *
 * DM803 Advanced Data Structures
 *
 * Exam Project - Part 2 - Spring 2022
 *
 * Test file for randomized search tree.
 */

#include <algorithm>
#include <deque>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <string>

#include "RTree.hpp"


void display_help()
{
    std::cout << "DM803 Advanced Data Structures, Exam Project - Part 2 - Spring 2022, deand17\n"
              << "Test program for randomized binary search tree. Help text.\n\n"
              << "    C arg1 [arg2]\tCommands use the format shown to the left.\n"
              << "    \t\t\tC is the (case insensitive) command to execute, an unbracketed arg is required\n"
              << "    \t\t\tand a bracketed arg is optional. For example, to insert the key 42, enter I 42\n\n"
              << "    D k\t\t\tDelete the key k, if it exists, from the active tree.\n"
              << "    H\t\t\tShow this help text.\n"
              << "    I k [pr]\t\tInsert the key k into the active tree with optional priority pr.\n"
              << "    L\t\t\tList trees.\n"
              << "    M t1 t2\t\tMerge trees t1 and t2 into a new tree t, where t1 and t2 are integers\n"
              << "    \t\t\tindexing the list of trees, which can be shown by entering L. Note that as\n"
              << "    \t\t\tper the project description, all keys in one tree must be smaller than all\n"
              << "    \t\t\tkeys in the other tree. Failing to comply will result in undefined behaviour.\n"
              << "    \t\t\tAlso note that trees t1 and t2 are destroyed in the process.\n"
              << "    P\t\t\tPrint active tree. This is only recommended for small trees (e.g. with <10 primary_nodes).\n"
              << "    S k\t\t\tSearch active tree for key k.\n"
              << "    T k\t\t\tSplit active tree using key k.\n"
              << "    W i\t\t\tSwitch to ith tree in the list of trees.\n"
              << "    Q\t\t\tExit the program.\n"
              << std::endl;
}

void erase_from_active_tree(const std::shared_ptr<DM803::RTree> &ptr_t, const int key)
{
    std::pair<DM803::RTree::Node *, bool> removed(ptr_t->erase(key));
    if (removed.second) {
        std::cout << "S - deleted '" << key << "'. Size: " << ptr_t->size() << std::endl;
    } else {
        std::cout << "F - key '" << key << "' not present. Size: " << ptr_t->size() << std::endl;
    }
}

void insert_into_active_tree(const std::shared_ptr<DM803::RTree> &ptr_t, const int key, const int pr)
{
    std::pair<DM803::RTree::Node *, bool> inserted;
    if (pr == -1) {
        inserted = ptr_t->insert(key);
    } else {
        inserted = ptr_t->insert(key, static_cast<DM803::RTree::Pr_val>(pr));
    }
    if (inserted.second) {
        std::cout << "S - inserted '" << key << "'. Size: " << ptr_t->size() << std::endl;
    } else {
        std::cout << "F - key '" << key << "' already present. Size: " << ptr_t->size() << std::endl;
    }
}

void merge_trees(const int i, const int j, int active_tree_index, std::shared_ptr<DM803::RTree> &ptr_t,
                 std::deque<std::shared_ptr<DM803::RTree>> &d)
{
    if (i >= 0 && i < static_cast<int>(d.size()) &&
        j >= 0 && j < static_cast<int>(d.size())) {
        auto t1 = d.at(i);
        auto t2 = d.at(j);

        auto t{std::make_shared<DM803::RTree>(DM803::RTree::merge(*t1, *t2))};
        d.erase(d.begin() + i);
        d.erase(d.begin() + j);
        d.push_back(t);
        std::cout << "S - merged trees '" << i << "' and '" << j << "' into new tree: " << d.size() - 1 << std::endl;
        if (active_tree_index == i || active_tree_index == j) {
            ptr_t = d.front();
        }
    } else {
        std::cout << "F - Could not merge trees: An index was out of range." << std::endl;
    }
}

void list_trees(std::deque<std::shared_ptr<DM803::RTree>> &d)
{
    int i{0};
    for (const auto& it : d) {
        auto maybe_min{it->front()};
        auto maybe_max{it->back()};
        std::cout << i << ": ";
        if (maybe_min) {
            std::cout << "Key range: [" << maybe_min.value() << ", " << maybe_max.value() << "]. Size: "
                      << it->size() << std::endl;
        } else {
            std::cout << "empty" << std::endl;
        }
        ++i;
    }
}

void print_active_tree(const int active_tree_index, const std::shared_ptr<DM803::RTree> &ptr_t)
{
    std::cout << "Printing active tree " << active_tree_index << ". Size: " << ptr_t->size() << std::endl;
    std::cout << *ptr_t << std::endl;
}

void search_active_tree(const std::shared_ptr<DM803::RTree> &ptr_t, const int key)
{
    std::pair<DM803::RTree::Node *, int> key_found(ptr_t->search(key));
    if (key_found.first) {
        std::cout << "S - found '" << key << "'. Search depth: " << key_found.second;
        auto pred{key_found.first->prev()};
        std::cout << ". Pred: ";
        if (pred) {

            std::cout << '(' << std::setfill(' ') << std::setw( 5) << pred->key << ','
                      << std::setfill(' ') << std::setw(20) << pred->pr  << ')';
        } else {
            std::cout << "(---------- null ----------)";
        }
        auto succ{key_found.first->next()};
        std::cout << ". Succ: ";
        if (succ) {
            std::cout << '(' << std::setfill(' ') << std::setw( 5) << succ->key << ','
                      << std::setfill(' ') << std::setw(20) << succ->pr  << ')';
        } else {
            std::cout << "(---------- null ----------)";
        }
        std::cout << ". Size: " << ptr_t->size() << std::endl;
    } else {
        std::cout << "F - key '" << key << "' not present. Search depth: " << key_found.second;
        std::cout << ". Size: " << ptr_t->size() << std::endl;
    }
}

void split_active_tree(const std::shared_ptr<DM803::RTree> &ptr_t, std::deque<std::shared_ptr<DM803::RTree>> &d,
                       const int key)
{
    auto maybe_tree{ptr_t->split(key)};
    if (maybe_tree) {
        auto tree{std::make_shared<DM803::RTree>(maybe_tree.value())};
        d.push_back(tree);
        std::cout << "S - split tree at '" << key << "'. Size: " << ptr_t->size() << '\n' << *ptr_t << std::endl;
        auto ptr_t2 = d.back();
        std::cout << "New tree. Size: " << ptr_t2->size() << '\n' << *ptr_t2 << std::endl;
    } else {
        std::cout << "F - key '" << key << "' present. Size: " << ptr_t->size() << std::endl;
    }
}

void switch_active_tree(int &active_tree_index, std::shared_ptr<DM803::RTree> ptr_t,
                        std::deque<std::shared_ptr<DM803::RTree>> &d, const int key)
{
    if (key >= 0 && key < static_cast<int>(d.size())) {
        int i{0};
        ptr_t = d.at(key);
        active_tree_index = key;
        for (const auto& it : d) {
            auto maybe_min{it->front()};
            auto maybe_max{it->back()};
            std::cout << i << ": ";
            if (maybe_min) {
                std::cout << "Key range: [" << maybe_min.value() << ", " << maybe_max.value() << "]. Size: "
                          << it->size() << std::endl;
            } else {
                std::cout << "empty" << std::endl;
            }
            ++i;
        }
        std::cout << "S - switched active tree to '" << key << "'. Size: " << ptr_t->size() << std::endl;
    } else {
        std::cout << "F - Could not switch to another tree: Index out of range." << std::endl;
    }
}


int main()
{
    std::deque<std::shared_ptr<DM803::RTree>> d;
    auto t{std::make_shared<DM803::RTree>(DM803::RTree())};
    d.push_back(t);

    int active_tree_index{0};
    std::shared_ptr<DM803::RTree> ptr_t{d.at(active_tree_index)};

    std::string line{};
    std::string operation{};

    int segment;
    std::vector<int> args{-1, -1};

    while(std::getline(std::cin, line)) {
//        operation = line.substr(0, line.find(space_delimiter));
//        if (std::count(line.begin(), line.end(), ' ') == 1) {
//            try {
//                key = std::stoi(line.substr(line.find(space_delimiter) + space_delimiter.length()));
//            } catch (std::invalid_argument &e) {
//                ;
//            }
//        } else {
//            try {
//                key = std::stoi(line.substr(line.find(space_delimiter) + space_delimiter.length()));
//                pr = std::stoi(line.substr(line.find_last_of(space_delimiter) + space_delimiter.length()));
//                if (pr < 0) {
//                    pr = -1;
//                }
//            } catch (std::invalid_argument &e) {
//                ;
//            }
//        }
        std::stringstream s(line);
        s >> operation;
        int i{0};
        while(s >> segment) {
            if (i < static_cast<int>(args.size())) {
                args[i] = segment;
                ++i;
            }
        }
        if (operation == "D" || operation == "d") {
            erase_from_active_tree(ptr_t, args[0]);
        } else if (operation == "H" || operation == "h") {
            display_help();
        } else if (operation == "I" || operation == "i") {
            insert_into_active_tree(ptr_t, args[0], args[1]);
        } else if (operation == "M" || operation == "m") {
            merge_trees(args[0], args[1], active_tree_index, ptr_t, d);
        } else if (operation == "L" || operation == "l") {
            list_trees(d);
        } else if (operation == "P" || operation == "p") {
            print_active_tree(active_tree_index, ptr_t);
        } else if (operation == "Q" || operation == "q") {
            return 0;
        } else if (operation == "S" || operation == "s") {
            search_active_tree(ptr_t, args[0]);
        } else if (operation == "T" || operation == "t") {
            split_active_tree(ptr_t, d, args[0]);
        } else if (operation == "W" || operation == "w") {
            switch_active_tree(active_tree_index, ptr_t, d, args[0]);
        } else {
            std::cout << "F - " << operation << " command unknown, ignored" << std::endl;
        }
        args = {-1, -1};
    }
    return 0;
}
