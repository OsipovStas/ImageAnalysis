#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <unordered_map>

#include <boost/range/algorithm/copy.hpp>
#include <boost/bind.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/count_if.hpp>
#include <boost/range/algorithm/min_element.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"

enum {
    GOOD = 5
};

void readAffineMatrix(const char* filename, cv::Mat& dst) {
    std::ifstream ifs(filename, std::ifstream::in);
    std::vector<float> v((std::istream_iterator<float>(ifs)), std::istream_iterator<float>());
    cv::Mat(v).reshape(0, 3).copyTo(dst);
}

void calcHarrisFeatures(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors) {
    auto detector = cv::FeatureDetector::create("HARRIS");
    cv::BriefDescriptorExtractor extractor;

    detector -> detect(image, keypoints);

    extractor.set("bytes", 16);
    extractor.compute(image, keypoints, descriptors);
}

void calcSiftFeatures(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors) {
    auto detector = cv::FeatureDetector::create("SIFT");
    cv::SiftDescriptorExtractor extractor;

    detector -> set("nFeatures", 500);
    detector -> detect(image, keypoints);
    extractor.compute(image, keypoints, descriptors);
}

void calcMatches(const cv::Mat& queryDescriptors, const cv::Mat& trainDescriptors, std::vector<cv::DMatch>& matches) {
    auto matcher = cv::DescriptorMatcher::create("BruteForce");
    matcher -> match(queryDescriptors, trainDescriptors, matches);
}

enum {
    HARRIS = 1,
    SIFT = 2
};

typedef void (*Handler)(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors);
typedef std::unordered_map<u_int32_t, Handler> HandlerMap;

HandlerMap handlers = {
    {HARRIS, &calcHarrisFeatures},
    {SIFT, &calcSiftFeatures}
};

float task3(const cv::Mat& query, const cv::Mat& train, const cv::Mat& affine, int method) {

    auto handler = handlers[method];
    if (!handler) {
        std::cerr << "Wrong method!" << std::endl;
        return -1;
    }
    std::vector<cv::KeyPoint> queryKeyPoints;
    std::vector<cv::KeyPoint> trainKeyPoints;
    cv::Mat queryDescriptors;
    cv::Mat trainDescriptors;
    std::vector< cv::DMatch > matches;
    std::vector<double> compares;

    handler(query, queryKeyPoints, queryDescriptors);
    handler(train, trainKeyPoints, trainDescriptors);
    calcMatches(queryDescriptors, trainDescriptors, matches);

    auto affT = [](const cv::Point2f & p, const cv::Mat & affine) {
        cv::Mat before = (cv::Mat_<float>(3, 1) << p.x, p.y, 1);
        cv::Mat after = affine * before;
        return cv::Point2f(after.at<float>(0, 0), after.at<float>(1, 0));
    };

    auto getNewOrigin = [&](const cv::Size& src, const cv::Size& dst, const cv::Mat & affine) {
        cv::Point2f tl = affT(cv::Point2f(0, 0), affine);
        cv::Point2f br = affT(cv::Point2f(src.width, src.height), affine);
        return cv::Point2f((tl.x / 2) + (br.x / 2) - (dst.width / 2), (tl.y / 2) + (br.y / 2) - (dst.height / 2));
    };

    cv::Point2f origin = getNewOrigin(query.size(), train.size(), affine);

    auto match2query = boost::bind<cv::Point2f>([&](const cv::DMatch & m) {
        return queryKeyPoints[m.queryIdx].pt;
    }, _1);

    auto match2train = boost::bind<cv::Point2f>([&](const cv::DMatch & m) {
        return trainKeyPoints[m.trainIdx].pt + origin;
    }, _1);

    auto queryAffineTranform = boost::bind<cv::Point2f>(affT, _1, affine);

    auto dist = boost::bind<double>([&](const cv::Point2f& p1, const cv::Point2f & p2) {
        return cv::norm(p1 - p2);
    }, _1, _2);

    auto isGood = boost::bind<bool>([&](double d) {
        return d < GOOD;
    }, _1);

    boost::transform(matches
            | boost::adaptors::transformed(match2query)
            | boost::adaptors::transformed(queryAffineTranform),
            matches
            | boost::adaptors::transformed(match2train),
            std::back_inserter(compares),
            dist);

//    std::vector<char> mask;
//    boost::transform(compares,
//            std::back_inserter(mask),
//            isGood);
//
//    cv::Mat out;
//    cv::drawMatches(query, queryKeyPoints, train, trainKeyPoints, matches, out, cv::Scalar::all(-1), cv::Scalar::all(-1), mask);
//    cv::namedWindow("Test", CV_WINDOW_NORMAL);
//    cv::imshow("Test", out);
//    cv::waitKey();


    float goodMatchTotal = boost::count_if(compares, isGood);
    return goodMatchTotal / std::min(queryKeyPoints.size(), trainKeyPoints.size());
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Wrong input" << std::endl;
        return -1;
    }
    cv::Mat affine;
    cv::Mat query = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat train = cv::imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
    readAffineMatrix(argv[3], affine);

    std::cout << task3(query, train, affine, HARRIS) << " " << task3(query, train, affine, SIFT) << std::endl;
}
