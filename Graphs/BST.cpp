#include "BST.hpp"

//////////////////////////////////////// Node ////////////////////////////////////////

NodeBST::NodeBST() {};

NodeBST::NodeBST(std::pair<long, long> weight_vertex_pair) {
    this->key = weight_vertex_pair;

    this->left = NULL;
    this->right = NULL;
    this->parent = NULL;
}

//////////////////////////////////////// BST ////////////////////////////////////////

FineBST::FineBST() {
    this->root = new NodeBST(std::pair<long, long>(FineBST::LARGEST_KEY, FineBST::LARGEST_KEY));
}

FineBST::~FineBST() {
    DeleteTree(this->root);
}

NodeBST* FineBST::search(NodeBST* root, std::pair<long, long> weight_vertex_pair) {
    NodeBST* pred = NULL;
    NodeBST* cur = root;
    cur->lock.lock();
    while ((weight_vertex_pair > cur->key && cur->right != NULL) || (weight_vertex_pair < cur->key && cur->left != NULL)) {
        if (pred != NULL) {
            pred->lock.unlock();
        }
        pred = cur;
        if (weight_vertex_pair > cur->key) {
            cur->right->lock.lock();
            cur = cur->right;
        } else {
            cur->left->lock.lock();
            cur = cur->left;
        }
    }
    return cur;
}

bool FineBST::contains(std::pair<long, long> weight_vertex_pair) {
    NodeBST* cur = FineBST::search(this->root, weight_vertex_pair);
    if (cur->parent != NULL) {
        cur->parent->lock.unlock();
    }
    std::unique_lock<std::mutex> lk(cur->lock, std::adopt_lock);
    bool result = (cur->key == weight_vertex_pair);
    return result;
}

bool FineBST::add(std::pair<long, long> weight_vertex_pair) {
    NodeBST* cur = FineBST::search(this->root, weight_vertex_pair);
    // std::cout << cur->key.first << " " << cur->key.second << std::endl;
    if (cur->parent != NULL) {
        cur->parent->lock.unlock();
    }
    std::unique_lock<std::mutex> lk(cur->lock, std::adopt_lock);
    if (cur->key == weight_vertex_pair) {
        return false;
    }
    NodeBST* new_node = new NodeBST(weight_vertex_pair);
    new_node->parent = cur;
    if (weight_vertex_pair < cur->key) {
        cur->left = new_node;
    } else {
        cur->right = new_node;
    }
    return true;
}

void FineBST::remove_node(NodeBST* n) {
    if (n->left == NULL || n->right == NULL) {
        NodeBST* replacement = NULL;
        if (n->left != NULL) {
            replacement = n->left;
        }
        if (n->right != NULL) {
            replacement = n->right;
        }
        if (replacement != NULL) {
            replacement->lock.lock();
            replacement->parent = n->parent;
        }
        if (n->parent->left == n) {
            n->parent->left = replacement;
        } else {
            n->parent->right = replacement;
        }
        delete n;
        if (replacement != NULL) {
            replacement->lock.unlock();
        }
    } else {
        NodeBST* replacement = FineBST::search(n->right, n->key);
        n->key = replacement->key;
        if (n != replacement->parent) {
            n->lock.unlock();
        }
        FineBST::remove_node(replacement);
    }
    if (n->parent != NULL) {
        n->parent->lock.unlock();
    }
}

bool FineBST::remove(std::pair<long, long> weight_vertex_pair) {
    NodeBST* cur = FineBST::search(this->root, weight_vertex_pair);
    if (cur->key != weight_vertex_pair) {
        cur->lock.unlock();
        if (cur->parent != NULL) {
            cur->parent->lock.unlock();
        }
        return false;
    }
    FineBST::remove_node(cur);
    return true;
}

bool FineBST::empty() {
    // std::cout << "test empty" << std::endl;
    return (this->root->left == NULL) && (this->root->right == NULL);
}

std::pair<long, long> FineBST::pop() {
    // std::cout << "in FineBST::pop" << std::endl;
    NodeBST* cur = this->root;
    while (cur->left != NULL) {
        // std::cout << "normal" << std::endl;
        cur = cur->left;
    }
    std::pair<long, long> val = cur->key;
    this->remove(val);
    return val;
}

void DeleteTree(NodeBST* root) {
    std::lock_guard<std::mutex> lk(root->lock);
    if (root->left != NULL) {
        DeleteTree(root->left);
    }
    if (root->right != NULL) {
        DeleteTree(root->right);
    }
    delete root;
}
