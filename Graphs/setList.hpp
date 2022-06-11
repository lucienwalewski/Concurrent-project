#pragma once

#include <atomic>
#include <climits>
#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>

class Node {
public:
    std::mutex lock;
    long weight;
    long vertex;
    std::shared_ptr<Node> next;
    bool marked;

    Node(long weight, long vertex, std::shared_ptr<Node> n);
    Node(long weight, long vertex);
};


class SetList{
    static const long LOWEST_KEY;
    static const long LARGEST_KEY;
    std::shared_ptr<Node> head;

    static bool validate(const std::shared_ptr<const Node> pred, const std::shared_ptr<const Node> curr);

public:
    SetList();
    ~SetList(); // destructor
    bool add(std::pair<int, int> weight_vertex_pair);
    bool remove(std::pair<int, int> weight_vertex_pair);
    bool contains(std::pair<int, int> weight_vertex_pair);
    bool empty();
    void print();
    std::pair<int, int> pop();
};