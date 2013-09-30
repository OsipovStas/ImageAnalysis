/* 
 * File:   main.cpp
 * Author: stasstels
 *
 * Created on September 30, 2013, 3:51 PM
 */
#include <iostream>
#include <vector>


#include <cv.h>
#include <highgui.h>

static const std::string path("./build/Debug/gen/");

bool task1(const cv::Mat& image) {
    return cv::imwrite(path + "copyLena.jpg", image);
}

bool task2(const cv::Mat& image) {
    cv::Mat leftSide = image.colRange(0, image.cols / 2);
    cv::Mat bottomSide = image.rowRange(image.rows / 2, image.rows);
    return cv::imwrite(path + "leftSideLena.jpg", leftSide) && cv::imwrite(path + "bottomSideLena.jpg", bottomSide);
}


bool task3(cv::Mat& image) {
    cv::Mat topLeft(image, 
                cv::Range(0, image.rows / 2),
                cv::Range(0, image.cols / 2));
    cv::Mat bottomRight(image,
                    cv::Range(image.rows - topLeft.rows, image.rows),
                    cv::Range(image.cols - topLeft.cols, image.cols));
    cv::Mat tmp;
    topLeft.copyTo(tmp);
    bottomRight.copyTo(topLeft);
    tmp.copyTo(bottomRight);
    return cv::imwrite(path + "swapedLena.jpg", image);
}


void createHalfTone(const cv::Mat& channel, int chIdx, cv::Mat& out) {
    std::vector<cv::Mat> channels(3, cv::Mat::zeros(channel.size(), CV_8U));
    channels[chIdx] = channel;
    cv::merge(channels, out);
}


bool task4(const cv::Mat& image) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);

    cv::Mat Blue;
    createHalfTone(channels[0], 0, Blue);
    
    cv::Mat Green;
    createHalfTone(channels[1], 1, Green);
    
    cv::Mat Red;
    createHalfTone(channels[2], 2, Red);
    return cv::imwrite(path + "GreenLena.jpg", Green) && 
            cv::imwrite(path + "BlueLena.jpg", Blue) && 
            cv::imwrite(path + "RedLena.jpg", Red);
    
}


bool task5(const cv::Mat& image) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    
    cv::Mat mean = (channels[0] / 3) + (channels[1] / 3) + (channels[2] / 3);
    return cv::imwrite(path + "LenaGreyMean.jpg", mean);
}


bool task6(const cv::Mat& original, const cv::Mat& mean) {
    cv::Mat grey, diff;
    cv::cvtColor(original, grey, cv::COLOR_BGR2GRAY);
    cv::absdiff(grey, mean, diff);
    return cv::imwrite(path + "LenaDiff.jpg", diff);
}


bool task7(const cv::Mat& image) {
    cv::Mat maxValue(image.size(), CV_8U, cv::Scalar(255));
    std::vector<cv::Mat> channels;
    cv::Mat tmp, result;
    
    cv::cvtColor(image, tmp, cv::COLOR_BGR2HSV);
    cv::split(tmp, channels);
    channels[1] = maxValue;
    channels[2] = maxValue;
    cv::merge(channels, tmp);
    cv::cvtColor(tmp, result, cv::COLOR_HSV2BGR);
    
    return cv::imwrite(path + "MaxSatValLena.jpg", result);
}


int main( int argc, char** argv ) {
    
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << "path/to/lena";
        exit(1);
    }
    
    cv::Mat image;
    image = cv::imread(argv[1], 1);

    std::cout << "Lesson 1 tasks: " << std::endl;
    
    if(!image.data ) {
        std::cerr << "No image data" << std::endl;
        exit(1);
    }

    std::cout << "Task 1 Status: " << (task1(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 2 Status: " << (task2(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3 Status: " << (task3(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 4 Status: " << (task4(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 5 Status: " << (task5(image) ? "OK" : "Error") << std::endl;
    
    cv::Mat mean = cv::imread(path + "LenaGreyMean.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    std::cout << "Task 6 Status: " << (task6(image, mean) ? "OK" : "Error") << std::endl;
    std::cout << "Task 7 Status: " << (task7(image) ? "OK" : "Error") << std::endl;

    return 0;
}




