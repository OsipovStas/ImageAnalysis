/* 
 * File:   main.cpp
 * Author: stels
 *
 * Created on October 22, 2013, 6:40 PM
 */

#include <cmath>

#include <iostream>
#include <vector>

#include <cv.h>
#include <highgui.h>

static const std::string path("./build/Debug/gen/");
static const std::string res("./res/");

using namespace cv;

int thresh = 100;
int max_thresh = 255;
cv::RNG rng(12345);

bool cmpX(const cv::Point& p1, const cv::Point& p2) {
    return p1.x < p2.x;
}

bool cmpY(const cv::Point& p1, const cv::Point& p2) {
    return p1.y < p2.y;
}

bool cmpContourLen(const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
    return c1.size() > c2.size();
}

std::string genNumberString(int number) {
    char buf[16];
    sprintf(buf, "%d", number);
    return std::string(buf);
}

cv::Point getCircleCenter(const std::vector<cv::Point>& circleContour) {
    int maxX = (*std::max_element(circleContour.begin(), circleContour.end(), cmpX)).x;
    int maxY = (*std::max_element(circleContour.begin(), circleContour.end(), cmpY)).y;
    int minX = (*std::min_element(circleContour.begin(), circleContour.end(), cmpX)).x;
    int minY = (*std::min_element(circleContour.begin(), circleContour.end(), cmpY)).y;

    int xc = (maxX + minX) / 2;
    int yc = (maxY + minY) / 2;

    return cv::Point(xc, yc);
}

bool task1_1() {
    cv::Mat image, gray;

    image = cv::imread(res + "coins_1.jpg", 1);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);

    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    std::sort(contours.begin(), contours.end(), cmpContourLen);
    /// Draw contours
    cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        drawContours(drawing, contours, (int) i, color, -1, 8, hierarchy, 0, Point());
        cv::putText(drawing, genNumberString(i + 1), getCircleCenter(contours[i]) + cv::Point(-10, 10), FONT_HERSHEY_SIMPLEX,
                0.5, cv::Scalar(0, 0, 0));
    }
    
    return cv::imwrite(path + "Task1_1.jpg", drawing);
}

int main() {
    std::cout << "Task 1.1 Status: " << (task1_1() ? "OK" : "ERROR") << std::endl;
}

//    cv::Mat lut(1, 256, CV_8U);


//for (int i = 0; i < 256; ++i) {
//    lut.at<uchar>(0, i) = i;
//}
//
//lut.colRange(120, 256) = cv::Scalar(0);
//
//cv::LUT(src_gray, lut, src_gray);