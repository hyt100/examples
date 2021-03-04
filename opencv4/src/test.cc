#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    cv::Mat m = cv::imread("../image_data/lena.png");
    if (m.empty()) {
        cout << "read failed" << endl;
        return -1;
    }
    cv::imshow("image", m);
    cv::waitKey(0);

    return 0;
}