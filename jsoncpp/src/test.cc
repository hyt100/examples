#include <iostream>
#include <vector>
#include <string>
#include "JsonIO.h"


int main(int argc, char *argv[])
{
  Json::Value root;

  bool ret = JsonIO::Load("../example.json", root);
  if (!ret)
    return -1;
  
  auto allMemberName = root.getMemberNames();
  for (auto &name: allMemberName) {
    std::cout << "list member: " << name << std::endl;
  }

  if (root.isMember("indent")) {
    std::cout << "exists!!! " << std::endl;
  }

  JsonIO::Save("./out.json", root);

  return 0;
}