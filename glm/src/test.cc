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

// 结论：以世界坐标系为旋转轴，旋转"不会"发生 万向节死锁(gimbal lock)
void rotate_test()
{
    //假定旋转顺序规则为：ZYX
    glm::mat4 mat = glm::mat4(1.0f);

    //初始矩阵：绕Y轴旋转90度
    mat = glm::rotate(mat, (float)glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 mat1 = glm::rotate(mat, (float)glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    printf("x =>    %s \n", glm::to_string(mat1).c_str());

    glm::mat4 mat2 = glm::rotate(mat, (float)glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    printf("z =>    %s \n", glm::to_string(mat2).c_str());

    //输出结果：两个矩阵不相同
}

// 结论：以物体local坐标系为旋转轴，旋转"会"发生 万向节死锁(gimbal lock)
void euler_test()
{
    //假定旋转顺序规则为：XYZ
    glm::mat4 mat = glm::mat4(1.0f);
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

int main(int argc, char *argv[])
{
    rotate_test();
    euler_test();

    return 0;
}