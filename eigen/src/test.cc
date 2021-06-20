#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "Eigen"

int main(int argc, char *argv[])
{
    //流赋值
    Eigen::Matrix4i mat;
    
    mat <<  0, 1, 2, 3,
            4, 5, 6, 7, 
            8, 9, 10, 11,
            12, 13, 14, 15;

    std::cout << mat << std::endl;

    std::cout << mat(0, 3) << std::endl;

    //创建一个单位矩阵
    Eigen::Matrix4i mat2 = Eigen::Matrix4i::Identity();

    std::cout << mat + mat2 << std::endl;

    return 0;
}