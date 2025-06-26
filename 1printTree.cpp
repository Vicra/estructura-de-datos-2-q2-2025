#include <iostream>
#include <vector>
#include <memory>

struct NodeB {
  std::vector<int> keys;
  std::vector<std::shared_ptr<NodeB>> children;
};

// funcion para construir un arbolb

std::shared_ptr<NodeB> buildSimpleTree () {
  auto root = std::make_shared<NodeB>();
  root->keys = {7, 16};

  auto child1 = std::make_shared<NodeB>();
  child1->keys = {3,5,6};

  auto granchild = std::make_shared<NodeB>();
  granchild->keys = {1,2};

  child1->children = {granchild};

  auto child2 = std::make_shared<NodeB>();
  child2->keys = {9,12};

  auto child3 = std::make_shared<NodeB>();
  child3->keys = {18,21};

  root->children = {child1, child2, child3};
  return root;
}

void printBTree(const std::shared_ptr<NodeB>& node, int level){
  // caso base 
  if(!node) return;

  // imprimir el valor
  std::string indent(level*4, ' ');
  std::cout << indent;
  for(int key : node->keys) {
    std::cout << key << " ";
  }
  std::cout << "\n";
  // caso recursivo
  for (const auto child : node->children){
    printBTree(child, level + 1);
  }
}

int main() {
  auto tree = buildSimpleTree();
  printBTree(tree, 0);
  return 0;
}

