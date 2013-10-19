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


// POSIX Specific
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef std::vector<cv::Mat>::iterator VMit;
typedef cv::MatIterator_<uchar> Mit;

static const std::string PATH("./build/Debug/gen/");

static const double task1v[] = {0.5, 0, 1, -125, 2, 0, 2, -255, 1, 125};
static const int task1c = 5;

static const double task3v[] = {0.05, 0.1, 0.15f};
static const int task3c = 3;

static const double task4v[] = {0, 30, 0, 60, 50, 30};
static const int task4c = 3;

static const int task5v[] = {3, 3, 5, 5, 7, 7};
static const int task5c = 3;

void scaleImage(const cv::Mat& src, cv::Mat& dst, double alpha, double beta) {
    CV_Assert(src.channels() == 3);

    dst = alpha * src + cv::Scalar(beta, beta, beta);
}

void concatChannels(const cv::Mat& src, cv::Mat& dst) {
    std::vector<cv::Mat> channels;
    cv::split(src, channels);
    dst = cv::Mat();
    for (VMit vmit = channels.begin(); vmit != channels.end(); ++vmit) {
        dst.push_back(*vmit);
    }
}

void concatImages(const cv::Mat& image1, const cv::Mat& image2, cv::Mat& dst) {
    CV_Assert(image1.rows == image2.rows);
    CV_Assert(image1.type() == image2.type());
    CV_Assert(image1.channels() == image2.channels());

    cv::Mat tmp(image1.rows, image1.cols + image2.cols, image1.type());
    cv::Mat tmp1 = tmp.colRange(0, image1.cols);
    cv::Mat tmp2 = tmp.colRange(image1.cols, tmp.cols);
    image1.copyTo(tmp1);
    image2.copyTo(tmp2);
    dst = tmp;
}

bool task1(const cv::Mat& image) {
    cv::Mat manual, buildIn, diff, tmp;
    std::vector<cv::Mat> channels;

    scaleImage(image, manual, 2, 100);
    image.convertTo(buildIn, -1, 2, 100);
    cv::absdiff(manual, buildIn, diff);
    cv::split(diff, channels);

    std::cout << "Max difference element: ";
    for (VMit it = channels.begin(); it != channels.end(); ++it) {
        int max = *(std::max_element((*it).begin<uchar>(), (*it).end<uchar>()));
        std::cout << max << " ";
    }
    std::cout << std::endl;

    std::vector<cv::Mat> scales(5, cv::Mat());
    std::vector<cv::Mat> dst(2, cv::Mat());
    for (int i = 0; i < task1c; ++i) {
        scaleImage(image, scales[i], task1v[2 * i], task1v[2 * i + 1]);
    }

    concatImages(scales[0], scales[1], dst[0]);
    concatChannels(dst[0], dst[0]);

    concatImages(scales[2], scales[3], dst[1]);
    concatImages(dst[1], scales[4], dst[1]);
    concatChannels(dst[1], dst[1]);

    return cv::imwrite(PATH + "Task1Lena01.jpg", dst[0]) && cv::imwrite(PATH + "Task1Lena345.jpg", dst[1]);

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

    return cv::imwrite(PATH + "Task2.jpg", res);
}

void addSaltAndPepper(const cv::Mat& src, cv::Mat& dst, double p, double q) {
    CV_Assert(src.type() == CV_8UC1);

    cv::RNG rng;
    src.copyTo(dst);
    for (Mit mit = dst.begin<uchar>(); mit != dst.end<uchar>(); ++mit) {
        double r = rng.uniform(0.0, 1.0);
        if (r < p) {
            *mit = 255;
            continue;
        }
        if (r < p + q) {
            *mit = 0;
        }
    }
}

std::string generateName(const std::string& base, int suff = 0, const char* ext = ".jpg") {
    std::string res(base);
    if (suff) {
        res += ((char) (((int) '0') + suff));
    }
    res += ext;
    return res;
}

bool task3(const cv::Mat& image) {
    int i = 1;
    cv::Mat tmp;
    bool res = true;

    for (const double* dp = task3v; dp != task3v + task3c; ++dp) {
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        for (VMit vmit = channels.begin(); vmit != channels.end(); ++vmit) {
            addSaltAndPepper(*vmit, *vmit, *dp, *dp);
        }
        cv::merge(channels, tmp);
        res &= cv::imwrite(PATH + generateName("noize/Task3-", i++), tmp);
    }
    return res;
}

void addAdditiveNoize(const cv::Mat& src, cv::Mat& dst, double mean, double stddev) {
    CV_Assert(src.type() == CV_8UC1);

    cv::RNG rng;
    src.copyTo(dst);
    cv::Mat noize(dst.rows, dst.cols, dst.type(), cv::Scalar(0));
    cv::randn(noize, mean, stddev);
    dst += noize;
}

bool task4(const cv::Mat& image) {
    int suff = 1;
    cv::Mat tmp;
    bool res = true;

    for (int i = 0; i < task4c; ++i) {
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        for (VMit vmit = channels.begin(); vmit != channels.end(); ++vmit) {
            addAdditiveNoize(*vmit, *vmit, task4v[2 * i], task4v[2 * i + 1]);
        }
        cv::merge(channels, tmp);
        res &= cv::imwrite(PATH + generateName("noize/Task4-", suff++), tmp);
    }
    return res;
}

void filterImage(const cv::Mat& image, cv::Mat& dst) {
    dst = cv::Mat(image.rows * 3, 0, image.type());
    std::vector<cv::Mat> filters(3, cv::Mat());
    std::vector<cv::Mat> result(task5c, cv::Mat());
    for (int i = 0; i < task5c; ++i) {
        cv::blur(image, filters[0], cv::Size(task5v[2 * i], task5v[2 * i + 1]));
        cv::GaussianBlur(image, filters[1], cv::Size(task5v[2 * i], task5v[2 * i + 1]), 0);
        cv::medianBlur(image, filters[2], task5v[2 * i]);
        for(VMit vmit = filters.begin(); vmit != filters.end(); ++vmit) {
            result[i].push_back(*vmit);
        }
    }
    for(VMit vmit = result.begin(); vmit != result.end(); ++vmit) {
        concatImages(dst, *vmit, dst);
    }
}

bool task5(const char* path) {
    struct dirent *entry;
    DIR *dp;
    bool res = true;
    int i = 1;
    
    dp = opendir(path);
    if (dp == NULL) {
        std::cerr << "opendir: Path does not exist or could not be read." << std::endl;
        return -1;
    }

    while ((entry = readdir(dp))) {
        struct stat s;
        std::string filename(path);
        filename += (entry -> d_name);
        if (stat(filename.c_str(), &s) == 0) {
            if (s.st_mode & S_IFREG) {
                cv::Mat image, filtered;
                image = cv::imread(filename.c_str(), 1);
                filterImage(image, filtered);
                res &= cv::imwrite(PATH + generateName("Task5-", i++), filtered);
            }
        }
    }

    closedir(dp);
    return true;
}

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/lena" << std::endl;
        exit(1);
    }

    cv::Mat image;
    image = cv::imread(argv[1], 1);

    std::cout << "Task 1 Status: " << (task1(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 2 Status: " << (task2(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3 Status: " << (task3(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 4 Status: " << (task4(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 5 Status: " << (task5((PATH + "noize/").c_str()) ? "OK" : "Error") << std::endl;



    //    cv::namedWindow( "Lesson 2", CV_WINDOW_NORMAL);
    //    cv::imshow( "Lesson 2", dst);                   
    //    cv::waitKey(0);    
    return 0;

}

