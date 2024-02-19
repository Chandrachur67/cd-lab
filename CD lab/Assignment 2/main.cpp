#include <iostream>
#include <set>

using namespace std;

class Node {
public:
    set<int> firstPos, lastPos;
    char nodeVal;
    Node *left, *right;
    int nodeNo;
    bool nullable, isLeaf, hasSignleChild;
    static int leafNodeCount;

    Node(char nodeVal) {
        this->nodeVal = nodeVal;
    }

    Node(char nodeVal, Node* left, Node* right) {
        this->nodeVal = nodeVal;
        this->left = left;
        this->right = right;
        this->isLeaf = left == nullptr && right == nullptr;
        this->hasSignleChild = (!this->isLeaf && right == nullptr);
        setNodeNo();
        setIsNullable();
        populateFirstAndLastPos();
    }

    void setNodeNo() {
        if(!isLeaf) return;
        this->nodeNo = Node::leafNodeCount++;
    }

    void setIsNullable() {
        if(nodeVal == '*') {
            this->nullable = true;
        } else if(nodeVal == '|') {
            this->nullable = left->isNullable() || right->isNullable();
        } else if(nodeVal == '.') {
            this->nullable = left->isNullable() && right->isNullable();
        }
    }

    void populateFirstAndLastPos() {
        if(isLeaf) {
            firstPos = {this->nodeNo};
            lastPos = {this->nodeNo};
        } 
        else if(hasSignleChild) {
            firstPos = left->firstPos;
            lastPos = left->lastPos;
        } 
        else {
            if(nodeVal == '|') {
                firstPos = left->firstPos;
                firstPos.insert(right->firstPos.begin(), right->firstPos.end());

                lastPos = left->lastPos;
                lastPos.insert(right->lastPos.begin(), right->lastPos.end());
            }
            if(nodeVal == '.') {
                if(nullable) {
                    firstPos = left->firstPos;
                    firstPos.insert(right->firstPos.begin(), right->firstPos.end());
                
                    lastPos = left->lastPos;
                    lastPos.insert(right->lastPos.begin(), right->lastPos.end());
                } else {
                    firstPos = left->firstPos;

                    lastPos = right->lastPos;
                }
            }
        }
    }

    bool isNullable() {
        return this->nullable;
    }
};

int main() {
    Node::leafNodeCount = 0;
}