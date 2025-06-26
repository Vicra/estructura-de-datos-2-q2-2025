#include <iostream>
#include <vector>
#include <queue>

using std::cout;
using std::endl;
using std::vector;

class BTreeNode;

class BTree {
private:
    BTreeNode* root;
    int t;  // Grado mínimo (cada nodo puede tener entre t-1 y 2t-1 claves)

public:
    BTree(int _t) {
        root = nullptr;
        t = _t;
    }

    void insert(int k);
    void remove(int k);
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
    void remove(int k);

    // Eliminar auxiliares
    int findKey(int k);
    int getPredecessor(int idx);
    int getSuccessor(int idx);
    void merge(int idx);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);

    void collectLevels(vector<vector<BTreeNode*>>& levels, int depth);
    int getHeight();
};

// ======= Funciones BTree =======

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

void BTree::remove(int k) {
    if (!root) return;

    root->remove(k);

    if (root->keys.size() == 0) {
        BTreeNode* oldRoot = root;
        if (root->leaf) {
            root = nullptr;
        } else {
            root = root->children[0];
        }
        delete oldRoot;
    }
}

int BTree::getHeight() {
    if (!root) return 0;
    return root->getHeight();
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

// ======= Funciones BTreeNode =======

void BTreeNode::collectLevels(vector<vector<BTreeNode*>>& levels, int depth) {
    if (levels.size() <= depth)
        levels.push_back({});
    levels[depth].push_back(this);
    for (BTreeNode* child : children)
        child->collectLevels(levels, depth + 1);
}

int BTreeNode::getHeight() {
    if (leaf)
        return 1;
    return 1 + children[0]->getHeight();
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

int BTreeNode::findKey(int k) {
    int idx = 0;
    while (idx < keys.size() && keys[idx] < k)
        ++idx;
    return idx;
}

void BTreeNode::remove(int k) {
    int idx = findKey(k);

    if (idx < keys.size() && keys[idx] == k) {
        if (leaf) {
            keys.erase(keys.begin() + idx); // Caso 1
        } else {
            if (children[idx]->keys.size() >= t) {
                int pred = getPredecessor(idx);
                keys[idx] = pred;
                children[idx]->remove(pred);
            } else if (children[idx + 1]->keys.size() >= t) {
                int succ = getSuccessor(idx);
                keys[idx] = succ;
                children[idx + 1]->remove(succ);
            } else {
                merge(idx);
                children[idx]->remove(k);
            }
        }
    } else {
        if (leaf) return;

        bool atLastChild = (idx == keys.size());

        if (children[idx]->keys.size() < t)
            fill(idx);

        if (atLastChild && idx > keys.size())
            children[idx - 1]->remove(k);
        else
            children[idx]->remove(k);
    }
}

int BTreeNode::getPredecessor(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->leaf)
        cur = cur->children.back();
    return cur->keys.back();
}

int BTreeNode::getSuccessor(int idx) {
    BTreeNode* cur = children[idx + 1];
    while (!cur->leaf)
        cur = cur->children.front();
    return cur->keys.front();
}

void BTreeNode::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys.push_back(keys[idx]);

    for (int i = 0; i < sibling->keys.size(); ++i)
        child->keys.push_back(sibling->keys[i]);

    if (!child->leaf) {
        for (int i = 0; i < sibling->children.size(); ++i)
            child->children.push_back(sibling->children[i]);
    }

    keys.erase(keys.begin() + idx);
    children.erase(children.begin() + idx + 1);

    delete sibling;
}

void BTreeNode::fill(int idx) {
    if (idx != 0 && children[idx - 1]->keys.size() >= t)
        borrowFromPrev(idx);
    else if (idx != keys.size() && children[idx + 1]->keys.size() >= t)
        borrowFromNext(idx);
    else {
        if (idx != keys.size())
            merge(idx);
        else
            merge(idx - 1);
    }
}

void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx - 1];

    child->keys.insert(child->keys.begin(), keys[idx - 1]);

    if (!child->leaf)
        child->children.insert(child->children.begin(), sibling->children.back());

    keys[idx - 1] = sibling->keys.back();
    sibling->keys.pop_back();
    if (!sibling->leaf)
        sibling->children.pop_back();
}

void BTreeNode::borrowFromNext(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys.push_back(keys[idx]);

    if (!child->leaf)
        child->children.push_back(sibling->children.front());

    keys[idx] = sibling->keys.front();
    sibling->keys.erase(sibling->keys.begin());
    if (!sibling->leaf)
        sibling->children.erase(sibling->children.begin());
}

// ======= MAIN =======

int main() {
    BTree tree(2);

    vector<int> values = {10, 20, 5, 6, 12, 30, 7, 17};
    for (int val : values) {
        tree.insert(val);
        cout << "Inserted " << val << ":\n";
        tree.print();
        cout << endl;
    }

    vector<int> toDelete = {6, 13, 7, 4, 2, 16, 17};
    for (int val : toDelete) {
        cout << "Removing " << val << ":\n";
        tree.remove(val);
        tree.print();
        cout << endl;
    }

    return 0;
}
