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



cv::Mat src;
cv::Mat src_gray;
int thresh = 100;
int max_thresh = 255;
cv::RNG rng(12345);

/**
 * @function thresh_callback
 */
void thresh_callback(int, void*) {
    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    /// Detect edges using canny
    cv::Canny(src_gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    /// Draw contours
    cv::Mat drawing = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(drawing, contours, (int) i, color, 2, 8, hierarchy, 0, Point());
    }

    
    
    /// Show in a window
    namedWindow("Contours", WINDOW_NORMAL);
    imshow("Contours", drawing);
}

/*
 * 
 */
int main(int argc, char** argv) {

    src = cv::imread(res + "coins_5_3.jpg", 1);

    cv::cvtColor(src, src_gray, CV_BGR2GRAY);
    cv::medianBlur(src_gray, src_gray, 5);
    cv::Mat lut(1, 256, CV_8U);
    
    for(int i = 0; i < 256; ++i) {
        lut.at<uchar>(0, i) = i;
    }
    
    lut.colRange(120, 256) = cv::Scalar(0);
    
    cv::LUT(src_gray, lut, src_gray);
    
    cv::namedWindow("Lesson3", CV_WINDOW_NORMAL);
    cv::imshow("Lesson3", src_gray);
    cv::waitKey(0);

    cv::createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback);
    thresh_callback(0, 0);
    cv::waitKey(0);

    return 0;
}
