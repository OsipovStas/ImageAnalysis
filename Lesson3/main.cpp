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

#include "circles.hpp"
#include "functions.hpp"

static const std::string path("./build/Debug/gen/");
static const std::string res("./res/");

const int POSITION_THRESHOLD = 5;
cv::RNG rng(666);

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
    int thresh = 100;
    int max_thresh = 255;
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
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(drawing, contours, (int) i, color, -1, 8, hierarchy, 0, cv::Point());
        cv::putText(drawing, genNumberString(i + 1), getCircleCenter(contours[i]) + cv::Point(-10, 10), cv::FONT_HERSHEY_SIMPLEX,
                0.5, cv::Scalar(0, 0, 0));
    }
    cv::namedWindow("Task1.3", CV_WINDOW_NORMAL);
    cv::imshow("Task1.3", drawing);
    cv::waitKey();
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

bool coins3() {
    int thresh = 100;
    int max_thresh = 255;
    cv::Mat image, gray;

    image = cv::imread(res + "coins_3.jpg", 1);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 1);

    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    VC circles, avg_circles;
    std::for_each(contours.begin(), contours.end(), CircleBuilder(50, circles));
    std::for_each(circles.begin(), circles.end(), CircleMergeReducer(6, 0.8, avg_circles));
    circles = avg_circles;
    avg_circles.clear();
    std::for_each(circles.begin(), circles.end(), CircleMergeReducer(6, 0.9, avg_circles));
    VCit end = std::remove_if(avg_circles.begin(), avg_circles.end(), rmseFilter((std::min_element(avg_circles.begin(), avg_circles.end()) -> mse) * 5));

    /// Draw circles
    cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);

    for (VCit cit = avg_circles.begin(); cit != end; ++cit) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::circle(drawing, cit -> center, cit -> radius, color, -1);
        cv::circle(drawing, cit -> center, cit -> radius, color, 2);
    }

    cv::hconcat(image, drawing, image);

    cv::namedWindow("Task1.3", CV_WINDOW_NORMAL);
    cv::imshow("Task1.3", image);
    cv::waitKey();

    return cv::imwrite(path + "task1_3Coins3.jpg", image);
}

bool drawPriority(const Circle& c1, const Circle& c2) {
    int in1 = 0;
    for (VPCit vpit = c1.chain -> begin(); vpit != c1.chain -> end(); ++vpit) {
        if ((cv::norm(c2.center - (*vpit)) - c2.radius) < 3) {
            ++in1;
        }
    }
    int in2 = 0;
    for (VPCit vpit = c2.chain -> begin(); vpit != c2.chain -> end(); ++vpit) {
        if ((cv::norm(c1.center - (*vpit)) - c1.radius) < 5) {
            ++in2;
        }
    }
    if (in1 == in2) {
        return c1.center.y > c2.center.y;
    }
    return in1 < in2;
}

bool coins4() {
    int thresh = 90;
    int max_thresh = 255;
    cv::Mat image, gray;

    image = cv::imread(res + "coins_4.jpg", 1);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 1);
    cv::medianBlur(gray, gray, 5);

    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    VC circles, avg_circles;
    std::for_each(contours.begin(), contours.end(), CircleBuilder(50, circles));
    std::for_each(circles.begin(), circles.end(), CircleMergeReducer(6, 0.8, avg_circles));
    circles = avg_circles;
    avg_circles.clear();
    std::for_each(circles.begin(), circles.end(), CircleMergeReducer(6, 0.8, avg_circles));
    VCit end = std::remove_if(avg_circles.begin(), avg_circles.end(), rmseFilter((std::min_element(avg_circles.begin(), avg_circles.end()) -> mse) * 10));

    /// Draw circles
    cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    std::sort(avg_circles.begin(), end, drawPriority);

    for (VCit cit = avg_circles.begin(); cit != end; ++cit) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::circle(drawing, cit -> center, cit -> radius, color, -1);
        cv::circle(drawing, cit -> center, cit -> radius, color, 2);
    }

    cv::hconcat(image, drawing, image);

    cv::namedWindow("Task1.3", CV_WINDOW_NORMAL);
    cv::imshow("Task1.3", image);
    cv::waitKey();

    return cv::imwrite(path + "task1_3Coins4.jpg", image);
}

bool task1_3() {
    return coins3() && coins4();
}

bool task1_4() {
    return coins_5_full();
}

bool task1_5() {
    return coins_5_1_full() && coins_5_2_full() && coins_5_3_full();
}

bool task1_6() {
    return coins_5_all() && coins_5_1_all() && coins_5_2_all() && coins_5_3_all();
}

bool task3_3() {
    cv::Mat image, hor, vert, hat, hat_text, tmp, text, table;

    cv::Mat element(9, 3, CV_8U, cv::Scalar(0));
    element.col(1) = cv::Scalar(1);

    image = cv::imread(res + "table.jpg", 1);
    
    cv::morphologyEx(image, hat, cv::MORPH_DILATE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)), cv::Point(-1, -1), 1);
    cv::morphologyEx(hat, hat, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)), cv::Point(-1, -1), 2);
    cv::morphologyEx(image, vert, cv::MORPH_CLOSE, element, cv::Point(-1, -1), 2);
    cv::morphologyEx(image, hor, cv::MORPH_CLOSE, element.t(), cv::Point(-1, -1), 2);

    hat_text = (image - hat);
    hat = cv::Scalar(255, 255, 255) - hat;
    vert = (cv::Scalar(255, 255, 255) - vert);
    hor =  (cv::Scalar(255, 255, 255) - hor);
    tmp = hor + vert + hat - hat_text;
    text = image + tmp - hat_text;
    table = image + (cv::Scalar(255) - text) - hat_text;

    cv::hconcat(image,  table, image);
    cv::hconcat(image,  text, image);

    return cv::imwrite(path + "Task3_3.jpg", image);
}


int main() {
    std::cout << "Task 1.1 Status: " << (task1_1() ? "OK" : "ERROR") << std::endl;
    std::cout << "Task 1.2 Status: " << (task1_2() ? "OK" : "ERROR") << std::endl;
    std::cout << "Task 1.3 Status: " << (task1_3() ? "OK" : "ERROR") << std::endl;
    std::cout << "Task 1.4 Status: " << (task1_4() ? "OK" : "ERROR") << std::endl;
    std::cout << "Task 1.5 Status: " << (task1_5() ? "OK" : "ERROR") << std::endl;
    std::cout << "Task 1.6 Status: " << (task1_6() ? "OK" : "ERROR") << std::endl;

    return 0;
}
