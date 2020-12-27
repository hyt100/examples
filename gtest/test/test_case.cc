#include <iostream>
#include <gtest/gtest.h>
#include "func.h"
using namespace std;

TEST(Funtest, test_sum)
{
  EXPECT_EQ(Func::sum(1, 2), 3);
}

TEST(Funtest, test_sum2)
{
  EXPECT_EQ(Func::sum(1, 4), 5);
}

TEST(DISABLED_My, s)
{
  EXPECT_EQ(Func::sum(1, 4), 5);
}

TEST(Our, DISABLED_s)
{
  EXPECT_EQ(Func::sum(1, 4), 5);
}

// gtest提供了一些参数，常用的如下：
//    ./test_run                          # 运行所有案例
//    ./test_run --help                   # 帮助命令
//    ./test_run --gtest_list_tests       # 列出所有的测试用例
//    ./test_run --gtest_filter=Funtest.test_sum     # 运行指定的案例，过滤还支持通配符
//    ./test_run --gtest_also_run_disabled_tests     # 执行案例时，同时也执行被置为无效的测试案例(带DISABLED_前缀的test_suite_name或test_name)
//    ./test_run --gtest_repeat=100       #设置案例重复运行次数
//