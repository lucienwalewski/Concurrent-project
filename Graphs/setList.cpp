#include <atomic>
#include <climits>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

//--------- Slightly modified lazy version of SetList from the lecture --------

class Node {
public:
    std::mutex lock;
    long weight;
    long vertex;
    std::shared_ptr<Node> next;
    bool marked;
    Node(long weight, long vertex, std::shared_ptr<Node> n) {
        this->weight = weight;
        this->vertex = vertex;
        this->next = n;
        this->marked = false;
    }
    Node(long weight, long vertex) {
        this->weight = weight;
        this->vertex = vertex;
        this->next = NULL; 
        this->marked = false;
    }
};

class SetList{
    static const long LOWEST_KEY;
    static const long LARGEST_KEY;
    std::shared_ptr<Node> head;

    static bool validate(const std::shared_ptr<const Node> pred, const std::shared_ptr<const Node> curr);

public:
    SetList() { // constructor
        head = std::make_shared<Node>(SetList::LOWEST_KEY, SetList::LOWEST_KEY);
        head->next = std::make_shared<Node>(SetList::LARGEST_KEY, SetList::LARGEST_KEY);
    }
    ~SetList() {}; // destructor
    bool add(std::pair<int, int> weight_vertex_pair);
    bool remove(std::pair<int, int> weight_vertex_pair);
    bool contains(std::pair<int, int> weight_vertex_pair);
    bool empty();
    void print();
    std::pair<int, int> pop();
};

const long SetList::LOWEST_KEY = -1;
const long SetList::LARGEST_KEY = LONG_MAX;
 
bool SetList::validate(std::shared_ptr<const Node> pred, std::shared_ptr<const Node> curr) {
  return (!pred->marked) && (!curr->marked) && (pred->next == curr);
}

bool SetList::empty() {
    return head->next->weight == LARGEST_KEY;
}

bool SetList::add(std::pair<int, int> weight_vertex_pair) {
    auto[weight, vertex] = weight_vertex_pair;
    while (true) {
        std::shared_ptr<Node> pred = this->head;
        std::shared_ptr<Node> curr = pred->next;
        while (curr->weight < weight) {
            pred = curr;
            curr = curr->next;
        }
        while (curr->vertex < vertex && curr->weight == weight) {
            pred = curr;
            curr = curr->next;
        }
        std::lock_guard<std::mutex> pred_lk(pred->lock);
        std::lock_guard<std::mutex> curr_lk(curr->lock);
        if (SetList::validate(pred, curr)) {
            if (weight == curr->weight && vertex == curr->vertex) {
                return false;
            } else {
                pred->next = std::make_shared<Node>(weight, vertex, curr);
                return true;
            }
        }
    }
}
    
bool SetList::remove(std::pair<int, int> weight_vertex_pair) {
    auto[weight, vertex] = weight_vertex_pair;
    while (true) {
        std::shared_ptr<Node> pred = this->head;
        std::shared_ptr<Node> curr = pred->next;
        while (curr->weight < weight) {
            pred = curr;
            curr = curr->next;
        }
        while (curr->vertex < vertex && curr->weight == weight) {
            pred = curr;
            curr = curr->next;
        }
        std::lock_guard<std::mutex> pred_lk(pred->lock);
        std::lock_guard<std::mutex> curr_lk(curr->lock);
        if (SetList::validate(pred, curr)) {
            if (weight == curr->weight && vertex == curr->vertex) {
                curr->marked = true;
            	pred->next = curr->next;
	            return true;
            } else {
                return false;
            }
        }
    }
}

bool SetList::contains(std::pair<int, int> weight_vertex_pair) {
    auto[weight, vertex] = weight_vertex_pair;
    std::shared_ptr<Node> curr = this->head;
    while (curr->weight < weight) {
        curr = curr->next;
    }
    while (curr->vertex < vertex && curr->weight == weight)  {
        curr = curr->next;
    }
    return (curr->weight == weight) && (curr->vertex == vertex) && (!curr->marked);
}

// Assume that the list is not empty, don't need to be thread safe because of the way we use it
std::pair<int, int> SetList::pop() {
    std::shared_ptr<Node> first_pair = this->head->next;
    this->head->next = first_pair->next;
    return std::make_pair(first_pair->weight, first_pair->vertex);
}

void SetList::print() {
    std::shared_ptr<Node> curr = this->head;
    std::cout << " " << std::endl;
    std::cout << "BEGIN PRINT QUEUE" << std::endl;
    std::cout << " " << std::endl;
    while (curr->vertex != LARGEST_KEY) {
        std::cout << "WEIGHT: " << curr->weight << std::endl;
        std::cout << "VERTEX: " << curr->vertex << std::endl;
        std::cout << " " << std::endl;
        curr = curr->next;
    }
    std::cout << "WEIGHT: " << curr->weight << std::endl;
    std::cout << "VERTEX: " << curr->vertex << std::endl;
    std::cout << " " << std::endl;
    std::cout << "END PRINT QUEUE" << std::endl;
    std::cout << " " << std::endl;
}