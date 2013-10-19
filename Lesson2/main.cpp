/* 
 * File:   main.cpp
 * Author: stels
 *
 * Created on October 8, 2013, 5:51 PM
 */


#include <cstdlib>

#include <algorithm>
#include <sstream>


#include <cv.h>
#include <highgui.h>

typedef std::vector<cv::Mat>::iterator VMit;
typedef cv::MatIterator_<uchar> Mit;

static const std::string path("./build/Debug/gen/");

void scaleImage(const cv::Mat& src, cv::Mat& dst, double alpha, double beta) {
    CV_Assert(src.channels() == 3);
    
    dst = alpha * src + cv::Scalar(beta, beta, beta);
}

void concatChannels(const cv::Mat& src, cv::Mat& dst) {
    std::vector<cv::Mat> channels;
    cv::split(src, channels);
    dst = cv::Mat();
    for(VMit vmit = channels.begin(); vmit != channels.end(); ++vmit) {
        dst.push_back(*vmit);
    }
}

void concatImages(const cv::Mat& image1, const cv::Mat& image2, cv::Mat& dst) {
    CV_Assert(image1.rows == image2.rows);
    CV_Assert(image1.type() == image2.type());
    CV_Assert(image1.channels() == image2.channels());

    dst = cv::Mat(image1.rows, image1.cols + image2.cols, image1.type());
    cv::Mat tmp1 = dst.colRange(0, image1.cols);
    cv::Mat tmp2 = dst.colRange(image1.cols, dst.cols);
    image1.copyTo(tmp1);
    image2.copyTo(tmp2);
}

bool task1(const cv::Mat& image) {
    cv::Mat manual, buildIn, diff;
    std::vector<cv::Mat> channels;
    
    scaleImage(image, manual, 2, 100);
    image.convertTo(buildIn, -1, 2, 100);
    cv::absdiff(manual, buildIn, diff);
    cv::split(diff, channels);
    
    std::cout << "Max difference element: ";
    for(VMit it = channels.begin(); it != channels.end(); ++it) {
        int max = *(std::max_element((*it).begin<uchar>(), (*it).end<uchar>()));
        std::cout << max << " ";
    }
    std::cout << std::endl;
    
    cv::Mat scale1, scale2, scale3, scale4, scale5, dst1, dst2, tmp;
    scaleImage(image, scale1, 0.5, 0);
    scaleImage(image, scale2, 1, -125);
    scaleImage(image, scale3, 2, 0);
    scaleImage(image, scale4, 2, -255);
    scaleImage(image, scale5, 1, 125);

    concatImages(scale1, scale2, dst1);
    concatChannels(dst1, dst1);
   
    concatImages(scale3, scale4, tmp);
    concatImages(tmp, scale5, dst2);
    concatChannels(dst2, dst2);

    return cv::imwrite(path + "Task1Lena01.jpg", dst1) && cv::imwrite(path + "Task1Lena345.jpg", dst2);
    
}

bool task2(const cv::Mat& image) {
    cv::Mat grey, normalized, equalized, diff, res;
    cv::cvtColor(image, grey, CV_BGR2GRAY);
    cv::normalize(grey, normalized, 0, 255, cv::NORM_MINMAX);
    cv::equalizeHist(grey, equalized);
    cv::absdiff(normalized, equalized, diff);
    res = cv::Mat();
    res.push_back(normalized);
    res.push_back(equalized);
    res.push_back(diff);
    
    return cv::imwrite(path + "Task2.jpg", res);
}

void addSaltAndPepper(const cv::Mat& src, cv::Mat& dst, double p, double q) {
    CV_Assert(src.type() == CV_8UC1);
    
    cv::RNG rng;
    src.copyTo(dst);
    for(Mit mit = dst.begin<uchar>(); mit != dst.end<uchar>(); ++mit) {
        double r = rng.uniform(0.0, 1.0);
        if(r < p) {
            *mit = 255;
            continue;
        }
        if(r < p + q) {
            *mit = 0;
        }
    }
}

std::string generateName(const std::string& name, int suff = 0, const char* ext = ".jpg") {
    std::string res(name);
    if(suff) {
        res += ((char)(((int)'0') + suff));
    }
    res += ext;
    return res;
}

bool task3(const cv::Mat& image) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    int i = 1;
    cv::Mat tmp;
    bool res = true;

    for(VMit vmit = channels.begin(); vmit != channels.end(); ++vmit) {
        addSaltAndPepper(*vmit, tmp, 0.05, 0.05);
        res &= cv::imwrite(path + generateName("Task3", i++), tmp);
        addSaltAndPepper(*vmit, tmp, 0.1, 0.1);
        res &= cv::imwrite(path + generateName("Task3", i++), tmp);
        addSaltAndPepper(*vmit, tmp, 0.15, 0.15);
        res &= cv::imwrite(path + generateName("Task3", i++), tmp);
    }
    return res;
}


int main(int argc, char** argv) {
    
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/lena" << std::endl;
        exit(1);
    }
    
    cv::Mat image;
    image = cv::imread(argv[1], 1);
    
    std::cout << "Task 1 Status: " << (task1(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 2 Status: " << (task2(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3 Status: " << (task3(image) ? "OK" : "Error") << std::endl;

    
//    cv::namedWindow( "Lesson 2", CV_WINDOW_NORMAL);
//    cv::imshow( "Lesson 2", dst);                   
//    cv::waitKey(0);    
    return 0;

}

