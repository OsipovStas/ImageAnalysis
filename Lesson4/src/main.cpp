/* 
 * File:   main.cpp
 * Author: stasstels
 *
 * Created on November 16, 2013, 6:32 PM
 */

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <cv.h>
#include <highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

using namespace boost::filesystem;

const std::string PATTERN("./res/Corel/TN_28008.JPG");
const std::string COREL("./res/Corel");
const std::string BRODATZ("./res/brodatz");

enum Metric {
    L1 = 1,
    ChiSquare = 2
};

void calcHistHSV(const cv::Mat& hsvImage, const int* histSizes, cv::Mat& hist) {
    float hRanges[] = {0, 180};
    float sRanges[] = {0, 256};
    float vRanges[] = {0, 256};

    const float* ranges[] = {hRanges, sRanges, vRanges};
    int channels[] = {0, 1, 2};
    cv::calcHist(&hsvImage, 1, channels, cv::Mat(), hist, 3, histSizes, ranges, true, false);
}

double compareHistL1(const path& d1, const path& d2, const int* bins) {
    cv::Mat image1, image2, hist1, hist2;
    image1 = cv::imread(d1.c_str(), 1);
    image2 = cv::imread(d2.c_str(), 1);
    cv::cvtColor(image1, image1, CV_BGR2HSV);
    cv::cvtColor(image2, image2, CV_BGR2HSV);

    calcHistHSV(image1, bins, hist1);
    calcHistHSV(image2, bins, hist2);

    cv::normalize(hist1, hist1);
    cv::normalize(hist2, hist2);

    return (1 - cv::compareHist(hist1, hist2, CV_COMP_INTERSECT));
}

double compareHistChiSquare(const path& d1, const path& d2, const int* bins) {
    cv::Mat image1, image2, hist1, hist2;
    image1 = cv::imread(d1.c_str(), 1);
    image2 = cv::imread(d2.c_str(), 1);

    cv::cvtColor(image1, image1, CV_BGR2HSV);
    cv::cvtColor(image2, image2, CV_BGR2HSV);

    calcHistHSV(image1, bins, hist1);
    calcHistHSV(image2, bins, hist2);

    return cv::compareHist(hist2, hist1, CV_COMP_HELLINGER);
}

double compareHist(const path& d1, const path& d2, Metric metric, const int* bins) {
    switch (metric) {
        case L1:
            return compareHistL1(d1, d2, bins);
        case ChiSquare:
            return compareHistChiSquare(d1, d2, bins);
        default:
            return 0;
    }
}

bool compare(const directory_entry& d1, const directory_entry& d2, const path& pattern, Metric metric, const int* bins) {
    return compareHist(d1.path(), pattern, metric, bins) < compareHist(d2.path(), pattern, metric, bins);
}

bool task1() {
    path pattern(PATTERN.c_str());
    path corel(COREL.c_str());
    std::vector<directory_entry> images;
    int bins[] = {42, 45, 15};
    std::ofstream ofL1("gen/task1_l1.txt", std::ofstream::out);
    std::ofstream ofChi("gen/task1_chi.txt", std::ofstream::out);

    std::copy(directory_iterator(corel), directory_iterator(), // directory_iterator::value_type
            std::back_insert_iterator<std::vector<directory_entry> >(images));
    std::sort(images.begin(), images.end(), boost::bind(compare, _1, _2, pattern, L1, bins));

    std::copy(images.begin(), images.end(), // directory_iterator::value_type
            std::ostream_iterator<directory_entry>(ofL1, "\n"));

    std::sort(images.begin(), images.end(), boost::bind(compare, _1, _2, pattern, ChiSquare, bins));

    std::copy(images.begin(), images.end(), // directory_iterator::value_type
            std::ostream_iterator<directory_entry>(ofChi, "\n"));

    return true;
}

static const int kernel_size = 21;
static const int pos_sigma = 5;
static const int pos_lm = 50;
static const int pos_th = 0;
static const int pos_psi = 90;

cv::Mat getKernel(int ks, double sig, double th, double lm, double ps) {
    int hks = (ks - 1) / 2;
    double theta = th * CV_PI / 180;
    double psi = ps * CV_PI / 180;
    double del = 2.0 / (ks - 1);
    double lmbd = lm;
    double sigma = sig / ks;
    double x_theta;
    double y_theta;
    cv::Mat kernel(ks, ks, CV_32F);
    for (int y = -hks; y <= hks; y++) {
        for (int x = -hks; x <= hks; x++) {
            x_theta = x * del * std::cos(theta) + y * del * std::sin(theta);
            y_theta = -x * del * std::sin(theta) + y * del * std::cos(theta);
            kernel.at<float>(hks + y, hks + x) = (float) std::exp(-0.5 * (std::pow(x_theta, 2) + std::pow(y_theta, 2)) / std::pow(sigma, 2)) * std::cos(2 * CV_PI * x_theta / lmbd + psi);
        }
    }
    return kernel;
}

//cv::Mat src;
//cv::Mat dest;
//
//void Process(int, void *) {
//    double sig = pos_sigma;
//    double lm = 0.5 + pos_lm / 100.0;
//    double th = pos_th;
//    double ps = pos_psi;
//    cv::Mat kernel = getKernel(kernel_size, sig, th, lm, ps);
//    cv::filter2D(src, dest, CV_32F, kernel);
//    cv::imshow("Process window", dest);
//    cv::Mat kernel_scaled(kernel_size * 20, kernel_size * 20, CV_32F);
//    cv::resize(kernel, kernel_scaled, kernel_scaled.size());
//    kernel_scaled /= 2.;
//    kernel_scaled += 0.5;
//    cv::imshow("Gabor kernel", kernel_scaled);
//    cv::Mat mag;
//    cv::pow(dest, 2.0, mag);
//    cv::imshow("Magnitude", mag);
//}

struct ImagePair {
    path image1;
    path image2;
    double dist;

    ImagePair(const path& image1, const path& image2) : image1(image1), image2(image2), dist(0) {
        evalDistance();
    }

    bool operator<(const ImagePair& o) const {
        return dist < o.dist;
    }

    friend std::ostream& operator<<(std::ostream& os, const ImagePair& ip) {
        return os << ip.image1 << " " << ip.image2 << " " << ip.dist;
    }

    static std::vector<cv::Mat> kernels;

    static void generateKernels(int numScales, int numAngles, float minSigma = 2, float maxSigma = 5, float minAngle = 0, float maxAngle = 180) {
        float angleDiff = (maxAngle - minAngle) / numAngles;
        float scaleDiff = (maxSigma - minSigma) / numScales;
        double lm = 0.5 + pos_lm / 100.0;
        double ps = pos_psi;

        kernels.clear();
        for (float s = minSigma; s < maxSigma; s += scaleDiff) {
            for (float a = minAngle; a < maxAngle; a += angleDiff) {
                kernels.push_back(getKernel(kernel_size, s, a, lm, ps));
            }
        }

    }
private:
    typedef boost::shared_ptr<std::vector<float> > vPtr;

    static void addFeatures(const cv::Mat& image, const cv::Mat& kernel, vPtr features) {
        cv::Mat dest;
        std::vector<double> mean, stddev;

        cv::filter2D(image, dest, CV_32F, kernel);
        cv::meanStdDev(dest, mean, stddev);

        features -> push_back(mean.front());
        features -> push_back(stddev.front());
    }

    static void process(path p, vPtr features) {
        cv::Mat image = cv::imread(p.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        image.convertTo(image, CV_32F, 1.0 / 255, 0);
        std::for_each(kernels.begin(), kernels.end(),
                boost::bind(addFeatures, image, _1, features));
    }

    void evalDistance() {
        std::vector<float> result;
        vPtr f1(new std::vector<float>());
        vPtr f2(new std::vector<float>());
        process(image1, f1);
        process(image2, f2);
        std::cout << f1 -> size() << " " << f2 -> size() << std::endl;
        cv::matchTemplate(*f1, *f2, result, CV_TM_SQDIFF_NORMED);
        dist = result.front();
    }

};

std::vector<cv::Mat> ImagePair::kernels;

bool task2() {
    typedef std::vector<directory_entry>::iterator VDIt;

    ImagePair::generateKernels(3, 9);
    path brodatz(BRODATZ.c_str());
    std::vector<directory_entry> images;
    std::vector<ImagePair> pairs;
    std::ofstream ofBr("gen/task2_brodatz.txt", std::ofstream::out);

    std::copy(directory_iterator(brodatz), directory_iterator(), // directory_iterator::value_type
            std::back_insert_iterator<std::vector<directory_entry> >(images));
    for (VDIt it1 = images.begin(); it1 != images.end(); ++it1) {
        for (VDIt it2 = it1 + 1; it2 != images.end(); ++it2) {
            pairs.push_back(ImagePair(it1 -> path(), it2 -> path()));
        }
    }
    std::sort(pairs.begin(), pairs.end());

    std::copy(pairs.begin(), pairs.end(), // directory_iterator::value_type
            std::ostream_iterator<ImagePair>(ofBr, "\n"));

    return true;
}

int main(int argc, const char** argv) {
    //    std::cout << "Task 1 Status: " << (task1() ? "OK" : "Error") << std::endl;
    std::cout << "Task 2 Status: " << (task2() ? "OK" : "Error") << std::endl;

    return 0;
}


