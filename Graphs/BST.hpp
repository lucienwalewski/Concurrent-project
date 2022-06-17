#pragma once

#include <algorithm>
#include <atomic>
#include <climits>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

//-----------------------------------------------------------------------------

class NodeBST {
public:
    std::pair<long, long> key;
    std::mutex lock;
    NodeBST* left;
    NodeBST* right;
    NodeBST* parent;
    NodeBST();
    NodeBST(std::pair<long, long> weight_vertex_pair);
};

class FineBST {
protected:
    NodeBST* root;
    static const long LARGEST_KEY = LONG_MAX;
    // returns the node that contains k or a node that would be
    // a parent of k if it will be inserted
    // Note: after the execution the returned node and its parent should be locked
    static NodeBST* search(NodeBST* root, std::pair<long, long> weight_vertex_pair);
    // removes the given node. Works under the assumption that the node and its parent
    // are locked
    static void remove_node(NodeBST* n);
public:
    FineBST();
    ~FineBST();
    bool add(std::pair<long, long> weight_vertex_pair);
    bool remove(std::pair<long, long> weight_vertex_pair);
    bool contains(std::pair<long, long> weight_vertex_pair);
    bool empty();
    std::pair<long, long> pop();
};

void DeleteTree(NodeBST* root);