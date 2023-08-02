/**
 * @file pplist_test.cpp
 * @author Dennis Andersen - deand17
 * @brief Test file for partially persistent list.
 * @date 2022-05-06
 *
 * DM803 Advanced Data Structures
 *
 * Exam Project - Part 2 - Spring 2022
 *
 * Test file for partially persistent list.
 */

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <string>

#include "PPList.hpp"


void display_help()
{
    std::cout << "DM803 Advanced Data Structures, Exam Project - Part 2 - Spring 2022, deand17\n"
              << "Test program for partially persistent list. Help text.\n\n"
              << "    C arg1 [arg2]\tCommands use the format shown to the left.\n"
              << "    \t\t\tC is the (case insensitive) command to execute, an unbracketed arg is required\n"
              << "    \t\t\tand a bracketed arg is optional. For example, to insert the key 42 at the head\n\n"
              << "    \t\t\tof the list, enter I 42 0\n\n"
              << "    F\t\t\tShow internal list info (#primary_nodes, #extra pointers, and memory usage).\n"
              << "    H\t\t\tShow this help text.\n"
              << "    I k i [k']\t\tInsert the key k at index i in the list. If k' is given, also insert an\n"
              << "    \t\t\tassociated key.\n"
              << "    P [v]\t\tPrint current version of the list or the list with version v if provided.\n"
              << "    S v i\t\tFind the ith element of the vth version of the list.\n"
              << "    U k i [k']\tUpdate key in ith element to k. If k' is given, also update the associated\n"
              << "    \t\t\tkey. If no associated key exists in the current version, one will be created.\n"
              << "    Q\t\t\tExit the program.\n"
              << std::endl;
}

void insert(std::shared_ptr<DM803::PPList> &l, const int k, const int i, const int k_prime=DM803::PPList::none)
{
    auto success{l->insert(k, i, k_prime)};
    if (success) {
        std::cout << "S - inserted '" << k << "'";
        if (k_prime != DM803::PPList::none) {
            std::cout << " and associated key '" << k_prime << "'";
        }
        std::cout << " at index " << i << ". Size: " << l->size() << std::endl;
    } else {
        std::cout << "F - index " << i << " out of range." << std::endl;
    }
}

void print(std::shared_ptr<DM803::PPList> &l, const int v)
{
    if (v == -1) {
        std::cout << "Printing list version " << l->version() << ". Size: " << l->size() << std::endl;
        std::cout << *l << std::endl;
    } else if (v >= 0 && v <= l->version()) {
        std::cout << "Printing list version " << v << ". Size: " << l->size(v) << std::endl;
        std::cout << l->stringify(v) << std::endl;
    } else {
        std::cout << "No list with version " << v << '.' << std::endl;
    }
}

void search(std::shared_ptr<DM803::PPList> &l, const int v, const int i)
{
    auto [k, k_prime] = l->search(v, i);
    if (k != DM803::PPList::none) {
        std::cout << "S - found key '" << k << "'";
        if (k_prime != DM803::PPList::none) {
            std::cout << " and associated key '" << k_prime << "'";
        }
        std::cout << " at index " << i << ". Size: " << l->size() << std::endl;
    } else {
        std::cout << "F - index " << i << " out of range." << std::endl;
    }
}

void show_list_info(std::shared_ptr<DM803::PPList> &l)
{
    l->show_info();
}

void update(std::shared_ptr<DM803::PPList> &l, const int k, const int i, const int k_prime=DM803::PPList::none)
{
    auto success{l->update(k, i, k_prime)};
    if (success) {
        std::cout << "S - updated '" << k << "'";
        if (k_prime != DM803::PPList::none) {
            std::cout << " and associated key '" << k_prime << "'";
        }
        std::cout << " at index " << i << ". Size: " << l->size() << std::endl;
    } else {
        std::cout << "F - index " << i << " out of range." << std::endl;
    }
}

int main()
{
    auto l{std::make_shared<DM803::PPList>(DM803::PPList())};

    std::string line{};
    std::string operation{};

    int segment;
    std::vector<int> args{-1, -1, DM803::PPList::none};

    while(std::getline(std::cin, line)) {
        std::stringstream s(line);
        s >> operation;
        int j{0};
        while(s >> segment) {
            if (j < static_cast<int>(args.size())) {
                args[j] = segment;
                ++j;
            }
        }
        if (operation == "F" || operation == "f") {
            show_list_info(l);
        } else if (operation == "H" || operation == "h") {
            display_help();
        } else if (operation == "I" || operation == "i") {
            insert(l, args[0], args[1], args[2]);
        } else if (operation == "P" || operation == "p") {
            print(l, args[0]);
        } else if (operation == "Q" || operation == "q") {
            return 0;
        } else if (operation == "S" || operation == "s") {
            search(l, args[0], args[1]);
        } else if (operation == "U" || operation == "u") {
            update(l, args[0], args[1], args[2]);
        } else {
            std::cout << "F - " << operation << " command unknown, ignored" << std::endl;
        }
        args = {-1, -1, DM803::PPList::none};
    }
    return 0;
}
