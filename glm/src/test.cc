// 参考文章：万向节死锁Gimbal Lock的本质 (https://zhuanlan.zhihu.com/p/112449913)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>  //for debug
#include <glm/gtx/euler_angles.hpp> //for euler
#include <iostream>
#include <stdio.h>

using std::cout;
using std::endl;

void rotate_test()
{
    glm::mat4 mat1 = glm::rotate(glm::mat4(1.0f), (float)glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 mat2 = glm::rotate(glm::mat4(1.0f), (float)glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 mat3 = glm::rotate(glm::mat4(1.0f), (float)glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    printf("mat =>    %s \n", glm::to_string(mat1 * mat2 * mat3).c_str());

    mat1 = glm::rotate(glm::mat4(1.0f), (float)glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mat2 = glm::rotate(glm::mat4(1.0f), (float)glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mat3 = glm::rotate(glm::mat4(1.0f), (float)glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    printf("mat =>    %s \n", glm::to_string(mat1 * mat2 * mat3).c_str());

    //输出结果：两个矩阵相同。这个时候绕 x 轴旋转20度和绕 z 轴旋转 20 度得到的是完全一样的旋转矩阵。
}

void euler_test()
{
    //假定旋转顺序规则为：XYZ
    glm::mat4 mat;
    float x_angle = 0.0f, y_angle = 0.0f, z_angle = 0.0f;

    x_angle = 20.0f;
    y_angle = 90.0f;
    z_angle = 0.0f;
    mat = glm::eulerAngleXYZ((float)glm::radians(x_angle), (float)glm::radians(y_angle), (float)glm::radians(z_angle));
    printf("m =>    %s \n", glm::to_string(mat).c_str());

    x_angle = 0.0f;
    y_angle = 90.0f;
    z_angle = 20.0f;
    mat = glm::eulerAngleXYZ((float)glm::radians(x_angle), (float)glm::radians(y_angle), (float)glm::radians(z_angle));
    printf("m =>    %s \n", glm::to_string(mat).c_str());

    //输出结果：两个矩阵相同。这个时候绕 x 轴旋转20度和绕 z 轴旋转 20 度得到的是完全一样的旋转矩阵。
}

void other_test()
{
    // glm::mat4 mat = glm::mat4(1.0f, 2.0f, 3.0f, 4.0f,
    //                             0.0f, 1.0f, 0.0f, 0.0f,
    //                             0.0f, 0.0f, 1.0f, 0.0f,
    //                             0.0f, 0.0f, 0.0f, 1.0f
    //                             );
    // glm::mat4 mmm = glm::translate(mat, glm::vec3(1.0f, 2.0f, 3.0f));
    // printf("m =>    %s \n", glm::to_string(mmm).c_str());

    // // glm::mat4 a = glm::mat4(1.0f);
    // glm::mat4 aa = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    // printf("m =>    %s \n", glm::to_string(mat*aa).c_str());
    // printf("m =>    %s \n", glm::to_string(aa*mat).c_str());
}

int main(int argc, char *argv[])
{
    rotate_test();
    // euler_test();
    // other_test();

    return 0;
}