#include <iostream>
#include <vector>


#include <cv.h>
#include <highgui.h>



bool task3(cv::Mat& img) {
    cv::Mat topLeft(img, 
                cv::Range(0, img.rows / 2),
                cv::Range(0, img.cols / 2));
    cv::Mat bottomRight(img,
                    cv::Range(img.rows - topLeft.rows, img.rows),
                    cv::Range(img.cols - topLeft.cols, img.cols));
    cv::Mat tmp;
    topLeft.copyTo(tmp);
    bottomRight.copyTo(topLeft);
    tmp.copyTo(bottomRight);
    return cv::imwrite("swapedLena.jpg", img);
}


bool task2(const char* path) {
    cv::Mat image = cv::imread(path, 1);
    cv::Mat leftSide = image.colRange(0, image.cols / 2);
    cv::Mat bottomSide = image.rowRange(image.rows / 2, image.rows);
    return cv::imwrite("leftSideLena.jpg", leftSide) && cv::imwrite("bottomSideLena.jpg", bottomSide);
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
    return cv::imwrite("GreenLena.jpg", Green) && 
            cv::imwrite("BlueLena.jpg", Blue) && 
            cv::imwrite("RedLena.jpg", Red);
    
}

bool task5(const cv::Mat& image) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    
    cv::Mat mean = (channels[0] / 3) + (channels[1] / 3) + (channels[2] / 3);
    return cv::imwrite("LenaGreyMean.jpg", mean);
}

bool task6(const cv::Mat& original, const cv::Mat& mean) {
    cv::Mat grey, diff;
    cv::cvtColor(original, grey, cv::COLOR_BGR2GRAY);
    cv::absdiff(grey, mean, diff);
    return cv::imwrite("LenaDiff.jpg", diff);
}

bool task7(const cv::Mat& image) {
    cv::Mat maxValue(image.size(), CV_8U, cv::Scalar(255));
    std::vector<cv::Mat> channels;
    cv::Mat tmp, result;
    
    cv::cvtColor(image, tmp, cv::COLOR_BGR2HSV);
    cv::split(tmp, channels);
//    channels[1] = maxValue;
//    channels[2] = maxValue;
    cv::merge(channels, tmp);
    cv::cvtColor(tmp, result, cv::COLOR_HSV2BGR);
    
    return cv::imwrite("MaxSatValLena.jpg", result);
}


int main( int argc, char** argv ) {
    cv::Mat image;
    image = cv::imread(argv[1], 1);

    std::cout << argv[1] << std::endl;

    
    if( argc != 2 || !image.data ) {
        printf( "No image data \n" );
        return -1;
    }

    std::cout << task4(image) << std::endl;
    std::cout << "Task 5 Status: " << (task5(image) ? "OK" : "Error") << std::endl;
    
    cv::Mat mean = cv::imread("LenaGreyMean.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    std::cout << "Task 6 Status: " << (task6(image, mean) ? "OK" : "Error") << std::endl;
    
    std::cout << "Task 7 Status: " << (task7(image) ? "OK" : "Error") << std::endl;

    //  cv::namedWindow( "Display Image", cv::CV_WINDOW_AUTOSIZE);
    //  cv::imshow( "Display Image", image );

    cv::waitKey(0);

    return 0;
}

