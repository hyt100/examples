#include <iostream>
#include <gtest/gtest.h>
#include "func.h"
using namespace std;

TEST(Funtest, test_sum)
{
  EXPECT_EQ(Func::sum(1, 2), 3);
}