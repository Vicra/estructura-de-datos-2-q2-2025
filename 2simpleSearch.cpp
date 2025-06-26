#include <iostream>
#define ORDER_M 10

struct BTreeNode {
  int keys[ORDER_M-1];
  int keys_length;
  BTreeNode* children[ORDER_M];
  bool is_leaf;

  BTreeNode(bool is_leaf){
    // constructor de inicializacion
    keys_length = 0;
    for (auto i = 0; i < ORDER_M; i++)
    {
      children[i] = nullptr;
    }
    this->is_leaf = is_leaf;
  }
};

BTreeNode* buildSimpleTree(){
  auto root = new BTreeNode(false);
  root->keys[0] = 10;
  root->keys[1] = 20;
  root->keys_length = 2;

  auto child1 = new BTreeNode(true);
  child1->keys[0] = 1;
  child1->keys[1] = 5;
  child1->keys_length = 2;

  auto child2 = new BTreeNode(true);
  child2->keys[0] = 11;
  child2->keys[1] = 15;
  child2->keys_length = 2;

  auto child3 = new BTreeNode(true);
  child3->keys[0] = 21;
  child3->keys[1] = 30;
  child3->keys_length = 2;

  root->children[0] = child1;
  root->children[1] = child2;
  root->children[3] = child3;

  return root;
}

BTreeNode* searchKey(BTreeNode* node, int searchValue) {
  // caso base 
  int index = 0;
  while(index < node->keys_length 
    && searchValue > node->keys[index]) {
    index += 1;
  }

  // case primario search
  if(index < node->keys_length 
    && node->keys[index] == searchValue) {
      return node;
  }

  // caso recursivo
  if(node->is_leaf) return nullptr;

  return searchKey(node->children[index], searchValue);
}

int main(int argc, char const *argv[])
{
  auto treeRoot = buildSimpleTree();
  int searchValue = 3;
  auto nodeFound = searchKey(treeRoot, searchValue);
  if(nodeFound != nullptr) {
    std::cout << "Found value: " << searchValue << std::endl;
  }
  else {
    std::cout << "Value: " << searchValue << " not found"<< std::endl;
  }

  nodeFound = searchKey(treeRoot, 5);
  if(nodeFound != nullptr) {
    std::cout << "Found value: " << 5 << std::endl;
  }
  else {
    std::cout << "Value: " << 5 << " not found"<< std::endl;
  }
  return 0;
}
