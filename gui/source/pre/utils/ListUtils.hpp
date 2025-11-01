#pragma once
#include <list>
#include <stdexcept>

// Swaps two elementy (given by their indices) of a std::list by reordering the underlying nodes,
// so that references to the values stay valid.
template<typename T>
void swapListNodes(std::list<T>& list, size_t i, size_t j) {
    if(i >= list.size() || j >= list.size()) {
        throw std::invalid_argument("Invalid indices for swapping");
    }

    // Nothing to do if source = target
    if(i == j) {
        return;
    }

    // Indices must be in ascending order
    if(i > j) {
        std::swap(i, j);
    }

    // Iterators of the nodes to be swapped + one after the second node
    auto it1 = std::next(list.begin(), i);
    auto it2 = std::next(list.begin(), j);
    auto after2 = std::next(it2);

    // Move it2 before it1
    list.splice(it1, list, it2);

    // Move it1 (now after it2) to after2
    list.splice(after2, list, it1);
}
