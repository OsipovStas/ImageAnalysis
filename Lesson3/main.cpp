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

bool cmpCircleRadius(const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2) {
    cv::Point2f center1;
    float r1;
    cv::minEnclosingCircle(c1, center1, r1);

    cv::Point2f center2;
    float r2;
    cv::minEnclosingCircle(c2, center2, r2);

    return r1 > r2;
}


std::string genNumberString(int number) {
    char buf[16];
    sprintf(buf, "%d", number);
    return std::string(buf);
}

cv::Point getCircleCenter(const std::vector<cv::Point>& circleContour) {
    cv::Point2f center;
    float radius;
    cv::minEnclosingCircle(circleContour, center, radius);

    return cv::Point((int) (center.x), (int) (center.y));
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

bool task1_2() {
    cv::Mat image, gray, coins, text;

    image = cv::imread(res + "coins_2.jpg", 1);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);
    cv::threshold(gray, gray, 240, 255, CV_8U);
    cv::dilate(gray, gray, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 3);
    cv::erode(gray, gray, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11, 11)), cv::Point(-1, -1), 2);
    cv::cvtColor(gray, gray, CV_GRAY2BGR);

    coins = image - gray;
    gray = cv::Scalar(255, 255, 255) - gray;

    text = image + cv::Scalar(1, 1, 1);
    text -= gray;

    cv::Mat lut(1, 256, CV_8U, cv::Scalar(255));

    for (int i = 1; i < 256; ++i) {
        lut.at<uchar>(0, i) = i;
    }

    cv::LUT(text, lut, text);

    //fills black area by white
    cv::floodFill(coins, cv::Point(1, 1), cv::Scalar(255, 255, 255));
    return cv::imwrite(path + "Task1_2_coins.jpg", coins) &&
            cv::imwrite(path + "Task1_2_text.jpg", text);
}

int main() {
    std::cout << "Task 1.1 Status: " << (task1_1() ? "OK" : "ERROR") << std::endl;
    std::cout << "Task 1.2 Status: " << (task1_2() ? "OK" : "ERROR") << std::endl;

}

//    cv::Mat lut(1, 256, CV_8U);


//for (int i = 0; i < 256; ++i) {
//    lut.at<uchar>(0, i) = i;
//}
//
//lut.colRange(120, 256) = cv::Scalar(0);
//
//cv::LUT(src_gray, lut, src_gray);