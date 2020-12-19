#include <iostream>
#include <gtest/gtest.h>
using namespace std;

class GlobalEnvironment: public testing::Environment
{
 public:
  virtual void SetUp()
  {
    cout << "Global SetUp" << endl;
  }

  virtual void TearDown()
  {
    cout << "Global TearDown" << endl;
  }
};

int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);

  testing::AddGlobalTestEnvironment(new GlobalEnvironment);

  return RUN_ALL_TESTS();
}