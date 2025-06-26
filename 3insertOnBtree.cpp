#include <iostream>
#include <vector>
#include <queue>
#include <cmath>

using std::cout;
using std::endl;
using std::vector;

class BTreeNode;

/**
 * t determines the minimum and maximum number of keys a node can have.
 * Every node (except the root) must have at least t-1 keys.
 * Every node can have at most 2t-1 keys.
 * Each internal node has between t and 2t children.
 */
class BTree {
private:
    BTreeNode* root;
    int t;

public:
    BTree(int _t) {
        root = nullptr;
        t = _t;
    }

    void insert(int k);
    void print();
    int getHeight();
};

class BTreeNode {
public:
    vector<int> keys;
    vector<BTreeNode*> children;
    int t;
    bool leaf;

    BTreeNode(int _t, bool _leaf) {
        t = _t;
        leaf = _leaf;
    }

    void insertNonFull(int k);
    void splitChild(int i, BTreeNode* y);

    int getHeight(); // Height from this node down

    void collectLevels(vector<vector<BTreeNode*>>& levels, int depth);
};

// Get height of subtree rooted at this node
int BTreeNode::getHeight() {
    if (leaf)
        return 1;
    return 1 + children[0]->getHeight();
}

int BTree::getHeight() {
    if (!root) return 0;
    return root->getHeight();
}

void BTreeNode::collectLevels(vector<vector<BTreeNode*>>& levels, int depth) {
    if (levels.size() <= depth)
        levels.push_back({});
    levels[depth].push_back(this);
    for (BTreeNode* child : children)
        child->collectLevels(levels, depth + 1);
}

void BTree::print() {
    if (!root) return;

    int height = getHeight();
    vector<vector<BTreeNode*>> levels;

    root->collectLevels(levels, 0);

    for (int level = 0; level < levels.size(); ++level) {
        int tabs = height - level;

        for (int i = 0; i < tabs; ++i)
            cout << '\t';

        for (BTreeNode* node : levels[level]) {
            cout << "[";
            for (int i = 0; i < node->keys.size(); ++i) {
                cout << node->keys[i];
                if (i < node->keys.size() - 1)
                    cout << ",";
            }
            cout << "] ";
        }

        cout << '\n';
    }
}

void BTree::insert(int k) {
    if (!root) {
        root = new BTreeNode(t, true);
        root->keys.push_back(k);
    } else {
        if (root->keys.size() == 2 * t - 1) {
            BTreeNode* newParent = new BTreeNode(t, false);
            newParent->children.push_back(root);
            newParent->splitChild(0, root);

            int i = 0;
            if (newParent->keys[0] < k)
                i++;
            newParent->children[i]->insertNonFull(k);

            root = newParent;
        } else {
            root->insertNonFull(k);
        }
    }
}

void BTreeNode::splitChild(int i, BTreeNode* y) {
    BTreeNode* z = new BTreeNode(y->t, y->leaf);
    for (int j = 0; j < t - 1; j++)
        z->keys.push_back(y->keys[j + t]);

    if (!y->leaf) {
        for (int j = 0; j < t; j++)
            z->children.push_back(y->children[j + t]);
    }
    y->keys.resize(t - 1);
    if (!y->leaf)
        y->children.resize(t);

    children.insert(children.begin() + i + 1, z);
    keys.insert(keys.begin() + i, y->keys[t - 1]);
}

void BTreeNode::insertNonFull(int k) {
    int i = keys.size() - 1;

    if (leaf) {
        keys.push_back(0);
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = k;
    } else {
        while (i >= 0 && keys[i] > k)
            i--;
        i++;
        if (children[i]->keys.size() == 2 * t - 1) {
            splitChild(i, children[i]);
            if (keys[i] < k)
                i++;
        }
        children[i]->insertNonFull(k);
    }
}

int main() {
    BTree tree(2); 

    vector<int> values = {10, 20, 5, 6, 12, 30, 7, 17};

    for (int val : values){
        tree.insert(val);
        tree.print();
    }

    return 0;
}
