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
typedef cv::MatIterator_<float> Mfit;


static const std::string PATH("./build/Debug/gen/");
static const std::string DIAG("./res/diagonal.jpg");
static const std::string ORIG("./res/original.tif");
static const std::string NOIZE("./res/noize.tif");
static const std::string LENA("./res/lena.tif");
static const std::string TOOL("./res/tool_scene.bmp");



static const double task1v[] = {0.5, 0, 1, -125, 2, 0, 2, -255, 1, 125};
static const int task1c = 5;

static const double task3v[] = {0.05, 0.1, 0.15f};
static const int task3c = 3;

static const double task4v[] = {0, 30, 0, 60, 50, 30};
static const int task4c = 3;

static const int task5v[] = {3, 3, 5, 5, 7, 7};
static const int task5c = 3;

static const int task6v[] = {5, 15, 30, 255, 380};
static const int task6c = 5;

static const double task3_2v[] = {2, 4};
static const int task3_2c = 2;

static const int task3_3v[] = {20, 30, 40, 100};
static const int task3_3c = 5;

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
        for (VMit vmit = filters.begin(); vmit != filters.end(); ++vmit) {
            result[i].push_back(*vmit);
        }
    }
    for (VMit vmit = result.begin(); vmit != result.end(); ++vmit) {
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

void rearrangeQuadrants(cv::Mat& image) {
    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = image.cols / 2;
    int cy = image.rows / 2;

    cv::Mat q0(image, cv::Rect(0, 0, cx, cy)); // Top-Left - Create a ROI per quadrant
    cv::Mat q1(image, cv::Rect(cx, 0, cx, cy)); // Top-Right
    cv::Mat q2(image, cv::Rect(0, cy, cx, cy)); // Bottom-Left
    cv::Mat q3(image, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp); // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

}

void lowPassFilter(cv::Mat& magnitude, int radius) {
    int cx = magnitude.cols / 2;
    int cy = magnitude.rows / 2;
    int curX, curY;
    int r2 = radius * radius;

    for (int i = 0; i < magnitude.rows; ++i) {
        curY = cy - i;
        for (int j = 0; j < magnitude.cols; ++j) {
            curX = cx - j;
            if (curY * curY + curX * curX >= r2) {
                magnitude.at<float>(i, j) = 0.0f;
            }
        }
    }
}

void highPassFilter(cv::Mat& magnitude, int radius) {
    int cx = magnitude.cols / 2;
    int cy = magnitude.rows / 2;
    int curX, curY;
    int r2 = radius * radius;

    for (int i = 0; i < magnitude.rows; ++i) {
        curY = cy - i;
        for (int j = 0; j < magnitude.cols; ++j) {
            curX = cx - j;
            if (curY * curY + curX * curX < r2) {
                magnitude.at<float>(i, j) = 0.0f;
            }
        }
    }
}

void visualization(const cv::Mat& magnitude, cv::Mat& res) {
    res = magnitude + cv::Scalar::all(1); // switch to logarithmic scale
    cv::log(res, res);
    rearrangeQuadrants(res);
    cv::normalize(res, res, 0, 1, CV_MINMAX);
    res *= 255;
    res.convertTo(res, CV_8U);
}

void performLowPass(const cv::Mat& image, cv::Mat& res, int rad) {
    cv::Mat grey, tmp;
    cv::cvtColor(image, grey, CV_BGR2GRAY);


    grey.convertTo(grey, CV_32F);
    grey.copyTo(res);
    res.convertTo(res, CV_8U);
    std::vector<cv::Mat> planes(2, cv::Mat());
    std::vector<cv::Mat> polar(2, cv::Mat());

    cv::dft(grey, tmp, cv::DFT_COMPLEX_OUTPUT);
    cv::split(tmp, planes);
    cv::cartToPolar(planes[0], planes[1], polar[0], polar[1]);
    visualization(polar[0], tmp);
    concatImages(res, tmp, res);

    rearrangeQuadrants(polar[0]);
    lowPassFilter(polar[0], rad);
    rearrangeQuadrants(polar[0]);

    visualization(polar[0], tmp);
    tmp.convertTo(tmp, res.type());
    concatImages(res, tmp, res);

    cv::polarToCart(polar[0], polar[1], planes[0], planes[1]);
    cv::merge(planes, tmp);
    cv::dft(tmp, tmp, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
    tmp.convertTo(tmp, CV_8U);
    concatImages(res, tmp, res);
}

void performHighPass(const cv::Mat& image, cv::Mat& res, int rad) {
    cv::Mat grey, tmp;
    cv::cvtColor(image, grey, CV_BGR2GRAY);


    grey.convertTo(grey, CV_32F);
    grey.copyTo(res);
    res.convertTo(res, CV_8U);
    std::vector<cv::Mat> planes(2, cv::Mat());
    std::vector<cv::Mat> polar(2, cv::Mat());

    cv::dft(grey, tmp, cv::DFT_COMPLEX_OUTPUT);
    cv::split(tmp, planes);
    cv::cartToPolar(planes[0], planes[1], polar[0], polar[1]);
    visualization(polar[0], tmp);
    concatImages(res, tmp, res);

    rearrangeQuadrants(polar[0]);
    highPassFilter(polar[0], rad);
    rearrangeQuadrants(polar[0]);

    visualization(polar[0], tmp);
    tmp.convertTo(tmp, res.type());
    concatImages(res, tmp, res);

    cv::polarToCart(polar[0], polar[1], planes[0], planes[1]);
    cv::merge(planes, tmp);
    cv::dft(tmp, tmp, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
    tmp.convertTo(tmp, CV_8U);
    concatImages(res, tmp, res);
}

bool task6(const cv::Mat& image) {
    int suff = 1;
    bool res = true;

    for (int i = 0; i < task6c; ++i) {
        cv::Mat tmp1, tmp2;
        performLowPass(image, tmp1, task6v[i]);
        res &= cv::imwrite(PATH + generateName("Task6-", suff++), tmp1);
        performHighPass(image, tmp2, task6v[i]);
        res &= cv::imwrite(PATH + generateName("Task6-", suff++), tmp2);
    }
    return res;
}

bool task3_1(const cv::Mat& image) {
    cv::Mat grey(image);
    grey.convertTo(grey, CV_32F);
    grey /= 255;
    grey += cv::Scalar(1);
    cv::log(grey, grey);
    grey *= 255;
    grey.convertTo(grey, CV_8U);
    grey.push_back(image);
    return cv::imwrite(PATH + "Task3_1.jpg", grey);
}

bool task3_2(const cv::Mat& image) {
    int suff = 1;
    bool res = true;

    for (int i = 0; i < task3_2c; ++i) {
        cv::Mat grey(image);
        grey.convertTo(grey, CV_32F);
        grey /= 255;
        cv::pow(grey, task3_2v[i], grey);
        grey *= 255;
        grey.convertTo(grey, CV_8U);
        grey.push_back(image);
        res &= cv::imwrite(PATH + generateName("Task3_2-", suff++), grey);
    }
    return res;
}

void threshold(const cv::Mat& src, uchar t, cv::Mat& dst) {
    cv::Mat lut(1, 256, CV_8U, cv::Scalar(0));
    lut.colRange(t + 1, lut.cols) = cv::Scalar(255);
    cv::LUT(src, lut, dst);
}

bool task3_3(const cv::Mat& image) {
    int suff = 1;
    bool r = true;
    for (int i = 0; i < task3_3c; ++i) {
        cv::Mat t1, t2, res;
        threshold(image, task3_3v[i], t1);
        cv::threshold(image, t2, task3_3v[i], 255, CV_8U);
        cv::absdiff(t1, t2, res);
        res.push_back(t1);
        res.push_back(t2);
        r &= cv::imwrite(PATH + generateName("Task3_3-", suff++), res);
    }
    return r;
}

bool task3_5(const cv::Mat& image, const cv::Mat& orig) {
    cv::Mat grey, tmp, res;
    image.copyTo(grey);
    grey.convertTo(grey, CV_32F);

    grey.copyTo(res);
    res.convertTo(res, CV_8U);
    std::vector<cv::Mat> planes(2, cv::Mat());
    std::vector<cv::Mat> polar(2, cv::Mat());

    cv::dft(grey, tmp, cv::DFT_COMPLEX_OUTPUT);
    cv::split(tmp, planes);
    cv::cartToPolar(planes[0], planes[1], polar[0], polar[1]);

    int cx = polar[0].cols / 2;
    int cy = polar[0].rows / 2;
    cv::Point max;

    cv::Mat top = polar[0].rowRange(0, cx);
    cv::Mat bot = polar[0].rowRange(cx, polar[0].rows);

    int row = 0;
    do {
        cv::minMaxLoc(top.rowRange(row++, top.rows), 0, 0, 0, &max);
    } while (max.x == 0);


    int r = 3;

    cv::Mat noizeCol = polar[0].colRange(max.x - r, max.x + r);
    cv::Mat noizeRow = polar[0].rowRange(max.y - r, max.y + r);
    cv::Mat blurCol = polar[0].colRange(max.x - 12, max.x - 12 + 2 * r);
    cv::Mat blurRow = polar[0].rowRange(max.y - 3 * r, max.y - r);

    blurCol.copyTo(noizeCol);
    blurRow.copyTo(noizeRow);


    cv::Mat noizeColB = polar[0].colRange(polar[0].cols - max.x - r, polar[0].cols - max.x + r);
    cv::Mat noizeRowB = polar[0].rowRange(polar[0].rows - max.y - r, polar[0].rows - max.y + r);

    blurCol.copyTo(noizeColB);
    blurRow.copyTo(noizeRowB);

    visualization(polar[0], tmp);

    cv::polarToCart(polar[0], polar[1], planes[0], planes[1]);
    cv::merge(planes, tmp);
    cv::dft(tmp, tmp, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

    tmp.convertTo(tmp, CV_8U);
    cv::Mat result;
    cv::matchTemplate(orig, tmp, result, CV_TM_SQDIFF);
    std::cout << "RMSE Task 3.5: " << result / (orig.cols * orig.rows) << std::endl;

    concatImages(res, tmp, res);
    cv::absdiff(tmp, orig, tmp);
    concatImages(res, tmp, res);

    cv::absdiff(image, orig, tmp);
    concatImages(res, tmp, res);

    return cv::imwrite(PATH + "Task3_5.jpg", res);
}

bool task3_6(const cv::Mat& orig, const cv::Mat& noize) {
    cv::Mat image, tmp;
    image = noize;
    std::vector<cv::Mat> channels;

    cv::matchTemplate(orig, noize, tmp, CV_TM_SQDIFF);
    std::cout << "Noize RMSE: " << tmp / (orig.rows * orig.cols * orig.channels()) << std::endl;

    cv::split(image, channels);

    for (VMit vmit = channels.begin(); vmit != channels.end(); ++vmit) {
        cv::medianBlur(*vmit, *vmit, 5);
    }
    cv::merge(channels, image);
    cv::GaussianBlur(image, image, cv::Size(5, 5), 1, 80);

    cv::matchTemplate(image, orig, tmp, CV_TM_SQDIFF);
    std::cout << "RMSE: " << tmp / (orig.rows * orig.cols * orig.channels()) << std::endl;

    image.push_back(orig);
    return cv::imwrite(PATH + "Task3_6.jpg", image);
    //    cv::namedWindow("Lesson 2", CV_WINDOW_NORMAL);
    //    cv::imshow("Lesson 2", image);
    //    cv::waitKey(0);
    //    
    //    orig.copyTo(image);
    //    image.push_back(noize);
    //
    //
    //    cv::split(image, channels);
    //    for(VMit vmit = channels.begin(); vmit != channels.end(); ++vmit) {
    //        cv::medianBlur(*vmit, *vmit, 5);
    //    }
    //    image = channels[0];
    //    concatImages(image, channels[1], image);
    //    concatImages(image, channels[2], image);
    //    cv::namedWindow("Lesson 2", CV_WINDOW_NORMAL);
    //    cv::imshow("Lesson 2", image);
    //    cv::waitKey(0);
}

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/lena" << std::endl;
        exit(1);
    }

    cv::Mat image, orig, lena, noize, diag, tool;
    image = cv::imread(argv[1], 1);
    diag = cv::imread(DIAG, CV_LOAD_IMAGE_GRAYSCALE);
    orig = cv::imread(ORIG, CV_LOAD_IMAGE_GRAYSCALE);
    lena = cv::imread(LENA, 1);
    noize = cv::imread(NOIZE, 1);
    tool = cv::imread(TOOL, CV_LOAD_IMAGE_GRAYSCALE);
    
    std::cout << "Task 1 Status: " << (task1(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 2 Status: " << (task2(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3 Status: " << (task3(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 4 Status: " << (task4(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 5 Status: " << (task5((PATH + "noize/").c_str()) ? "OK" : "Error") << std::endl;
    std::cout << "Task 6 Status: " << (task6(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3.1 Status: " << (task3_1(orig) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3.2 Status: " << (task3_2(orig) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3.3 Status: " << (task3_3(tool) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3.5 Status: " << (task3_5(diag, orig) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3.6 Status: " << (task3_6(lena, noize) ? "OK" : "Error") << std::endl;

    //    cv::namedWindow("Lesson 2", CV_WINDOW_NORMAL);
    //    cv::imshow("Lesson 2", image);
    //    cv::waitKey(0);
    return 0;

}

