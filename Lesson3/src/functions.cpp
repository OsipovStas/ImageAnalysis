#include <cv.h>
#include <highgui.h>

#include "functions.hpp"
#include "circles.hpp"


static const std::string path("./gen/");
static const std::string res("./res/");

static cv::RNG rng(666);

inline cv::Scalar getColor(int radius) {
    int base = radius * 111;
    return cv::Scalar(255 - base % 255, 255 - (base * base) % 255, 255 - (base * base * base) % 255);
}

bool groupSize(int s1, int s2) {
    return std::abs(s1 - s2) < 3;
}

bool printResult(cv::Mat& image, cv::Mat& circleImage, cv::Mat& circleContours, cv::Mat& contourImage, const VC& reducedCircles, cv::Mat& result, const char* name) {
    for (VCCit cit = reducedCircles.begin(); cit != reducedCircles.end(); ++cit) {
        cv::Scalar color = getColor(cit -> radius);
        cv::circle(circleImage, cit -> center, cit -> radius, color, -1);
        cv::drawContours(circleContours, VVP(1, *(cit -> chain)), -1, color, 1);
    }
    std::cout << name << " Result: " << std::endl;
    result.convertTo(result, CV_32F);
    for (int i = 0; i < result.rows; ++i) {
        std::cout << "Average Circle radius: " << result.row(i).col(0) << " Amount: " << result.row(i).col(1) << std::endl;
    }
    std::cout << "Group number: " << result.rows << std::endl;
    cv::reduce(result, result, 0, CV_REDUCE_SUM);
    std::cout << "Overall Circles: " << result.col(1) << std::endl;

    cv::vconcat(circleImage, image - circleImage, image);
    cv::vconcat(circleContours, contourImage, contourImage);

    cv::hconcat(image, contourImage, image);

    cv::namedWindow(name, CV_WINDOW_NORMAL);
    cv::imshow(name, image);
    cv::waitKey();
    return cv::imwrite(path + std::string(name) + ".jpg", image);
}

bool coins_5_full() {
    int thresh = 100;
    cv::Mat orig, image, gray, tmp;

    orig = cv::imread(res + "coins_5.jpg", 1);
    orig.copyTo(image);
    cv::cvtColor(image, gray, CV_BGR2GRAY);

    // Apply some filters and find contours
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);
    cv::medianBlur(gray, gray, 5);

    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    // Images for drawing
    cv::Mat circleImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat detectedContours = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat contourImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);

    // Draw the found contours
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(contourImage, contours, (int) i, color, 1);
    }


    VC circles, reducedCircles;
    // Generate circles from contours
    std::for_each(contours.begin(), contours.end(), CircleBuilder(23, circles));
    // Merge similar contours
    std::for_each(circles.begin(), circles.end(), CircleMergeReducer(6, 0.8, reducedCircles));

    circles.swap(reducedCircles);
    reducedCircles.clear();

    std::for_each(circles.begin(), circles.end(), CircleMergeReducer(6, 0.8, reducedCircles));
    //    VCit end = std::remove_if(reducedCircles.begin(), reducedCircles.end(), rmseFilter((std::min_element(reducedCircles.begin(), reducedCircles.end()) -> mse) * 40));
    std::sort(reducedCircles.begin(), reducedCircles.end());

    circles.swap(reducedCircles);
    reducedCircles.clear();
    // Erase nested circles
    std::for_each(circles.begin(), circles.end(), NestedCirclesReducer(reducedCircles));

    // Define most likely circle sizes and split circles by groups
    std::vector<int> sizes;
    cv::Mat result;
    std::for_each(reducedCircles.begin(), reducedCircles.begin() + 15, Sizes(sizes));
    std::sort(sizes.begin(), sizes.end());
    std::vector<int>::iterator begin = std::unique(sizes.begin(), sizes.end(), groupSize);
    sizes.erase(begin, sizes.end());
    // Remove circles which doesn't fit to image
    VCit end = std::partition(reducedCircles.begin(), reducedCircles.end(), FitImagePartitioner(circleImage.rows, circleImage.cols));
    std::for_each(reducedCircles.begin(), end, SizeCounter(sizes, result));
    reducedCircles.erase(end, reducedCircles.end());
    return printResult(image, circleImage, detectedContours, contourImage, reducedCircles, result, "Task1.4");

}

bool coins_5_all() {
    int thresh = 100;
    cv::Mat orig, image, gray, tmp;

    orig = cv::imread(res + "coins_5.jpg", 1);
    orig.copyTo(image);
    cv::cvtColor(image, gray, CV_BGR2GRAY);

    // Apply some filters and find contours
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);
    cv::medianBlur(gray, gray, 5);

    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    // Images for drawing
    cv::Mat circleImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat detectedContours = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat contourImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);

    // Draw the found contours
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(contourImage, contours, (int) i, color, 1);
    }


    VC circles, reducedCircles;
    // Generate circles from contours
    std::for_each(contours.begin(), contours.end(), CircleBuilder(23, circles));
    // Merge similar contours
    std::for_each(circles.begin(), circles.end(), CircleMergeReducer(6, 0.8, reducedCircles));

    circles.swap(reducedCircles);
    reducedCircles.clear();

    std::for_each(circles.begin(), circles.end(), CircleMergeReducer(6, 0.8, reducedCircles));
    //    VCit end = std::remove_if(reducedCircles.begin(), reducedCircles.end(), rmseFilter((std::min_element(reducedCircles.begin(), reducedCircles.end()) -> mse) * 40));
    std::sort(reducedCircles.begin(), reducedCircles.end());

    circles.swap(reducedCircles);
    reducedCircles.clear();
    // Erase nested circles
    std::for_each(circles.begin(), circles.end(), NestedCirclesReducer(reducedCircles));

    // Define most likely circle sizes and split circles by groups
    std::vector<int> sizes;
    cv::Mat result;
    std::for_each(reducedCircles.begin(), reducedCircles.begin() + 15, Sizes(sizes));
    std::sort(sizes.begin(), sizes.end());
    std::vector<int>::iterator begin = std::unique(sizes.begin(), sizes.end(), groupSize);
    sizes.erase(begin, sizes.end());
    std::for_each(reducedCircles.begin(), reducedCircles.end(), SizeCounter(sizes, result));
    return printResult(orig, circleImage, detectedContours, contourImage, reducedCircles, result, "Task1.6Image0");
}

bool coins_5_1_full() {
    int thresh = 60;
    cv::Mat orig, image, gray, tmp;

    orig = cv::imread(res + "coins_5_1.jpg", 1);
    orig.copyTo(image);
    cv::medianBlur(image, tmp, 5);
    cv::bilateralFilter(tmp, image, 15, 15, 15);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::medianBlur(gray, gray, 7);
    cv::morphologyEx(gray, gray, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
    // Images for drawing
    cv::Mat circleImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat detectedContours = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat contourImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);


    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(contourImage, contours, (int) i, color, 1);
    }



    VC circles, reducedCircles;
    VCit end;
    std::for_each(contours.begin(), contours.end(), CircleBuilder(26, reducedCircles));
    for (int i = 0; i < 3; ++i) {
        circles.swap(reducedCircles);
        reducedCircles.clear();
        std::for_each(circles.begin(), circles.end(), CircleMergeReducer(7, 0.7, reducedCircles));
    }
    end = std::remove_if(reducedCircles.begin(), reducedCircles.end(), rmseFilter((std::min_element(reducedCircles.begin(), reducedCircles.end()) -> mse) * 33));
    reducedCircles.erase(end, reducedCircles.end());
    std::sort(reducedCircles.begin(), reducedCircles.end());

    circles.swap(reducedCircles);
    reducedCircles.clear();

    std::for_each(circles.begin(), circles.end(), NestedCirclesReducer(reducedCircles));

    std::vector<int> sizes;
    cv::Mat result;
    std::for_each(reducedCircles.begin(), reducedCircles.begin() + 9, Sizes(sizes));
    std::sort(sizes.begin(), sizes.end());
    std::vector<int>::iterator begin = std::unique(sizes.begin(), sizes.end(), groupSize);
    sizes.erase(begin, sizes.end());
    std::for_each(reducedCircles.begin(), reducedCircles.end(), SizeCounter(sizes, result));
    end = std::partition(reducedCircles.begin(), reducedCircles.end(), FitImagePartitioner(circleImage.rows, circleImage.cols));
    std::for_each(reducedCircles.begin(), end, SizeCounter(sizes, result));
    reducedCircles.erase(end, reducedCircles.end());

    cv::cvtColor(gray, gray, CV_GRAY2BGR);
    return printResult(orig, circleImage, detectedContours, contourImage, reducedCircles, result, "Task1.5Image1");
}

bool coins_5_1_all() {
    int thresh = 60;
    cv::Mat orig, image, gray, tmp;

    orig = cv::imread(res + "coins_5_1.jpg", 1);
    orig.copyTo(image);
    cv::medianBlur(image, tmp, 5);
    cv::bilateralFilter(tmp, image, 15, 15, 15);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::medianBlur(gray, gray, 7);
    cv::morphologyEx(gray, gray, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
    // Images for drawing
    cv::Mat circleImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat detectedContours = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat contourImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);


    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(contourImage, contours, (int) i, color, 1);
    }



    VC circles, reducedCircles;
    VCit end;
    std::for_each(contours.begin(), contours.end(), CircleBuilder(26, reducedCircles));
    for (int i = 0; i < 3; ++i) {
        circles.swap(reducedCircles);
        reducedCircles.clear();
        std::for_each(circles.begin(), circles.end(), CircleMergeReducer(7, 0.7, reducedCircles));
    }
    end = std::remove_if(reducedCircles.begin(), reducedCircles.end(), rmseFilter((std::min_element(reducedCircles.begin(), reducedCircles.end()) -> mse) * 33));
    reducedCircles.erase(end, reducedCircles.end());
    std::sort(reducedCircles.begin(), reducedCircles.end());

    circles.swap(reducedCircles);
    reducedCircles.clear();

    std::for_each(circles.begin(), circles.end(), NestedCirclesReducer(reducedCircles));

    std::vector<int> sizes;
    cv::Mat result;
    std::for_each(reducedCircles.begin(), reducedCircles.begin() + 9, Sizes(sizes));
    std::sort(sizes.begin(), sizes.end());
    std::vector<int>::iterator begin = std::unique(sizes.begin(), sizes.end(), groupSize);
    sizes.erase(begin, sizes.end());
    std::for_each(reducedCircles.begin(), reducedCircles.end(), SizeCounter(sizes, result));
    std::partition(reducedCircles.begin(), reducedCircles.end(), FitImagePartitioner(circleImage.rows, circleImage.cols));


    cv::cvtColor(gray, gray, CV_GRAY2BGR);
    return printResult(orig, circleImage, detectedContours, contourImage, reducedCircles, result, "Task1.6Image1");

}

bool coins_5_2_full() {
    int thresh = 60;
    cv::Mat orig, image, gray, tmp;

    orig = cv::imread(res + "coins_5_2.jpg", 1);
    orig.copyTo(image);
    //    cv::medianBlur(image, tmp, 5);
    //    cv::bilateralFilter(tmp, image, 15, 15, 15);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::medianBlur(gray, gray, 3);
    cv::morphologyEx(gray, gray, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    //    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);


    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
    /// Draw circles
    // Images for drawing
    cv::Mat circleImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat detectedContours = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat contourImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);


    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(contourImage, contours, (int) i, color, 1);
    }

    VC circles, reducedCircles;
    VCit end;
    std::for_each(contours.begin(), contours.end(), CircleBuilder(26, reducedCircles));
    for (int i = 0; i < 3; ++i) {
        circles.swap(reducedCircles);
        reducedCircles.clear();
        std::for_each(circles.begin(), circles.end(), CircleMergeReducer(8, 0.8, reducedCircles));
    }
    end = std::remove_if(reducedCircles.begin(), reducedCircles.end(), rmseFilter((std::min_element(reducedCircles.begin(), reducedCircles.end()) -> mse) * 30));
    reducedCircles.erase(end, reducedCircles.end());
    std::sort(reducedCircles.begin(), reducedCircles.end());

    circles.swap(reducedCircles);
    reducedCircles.clear();

    std::for_each(circles.begin(), circles.end(), NestedCirclesReducer(reducedCircles));


    std::vector<int> sizes;
    cv::Mat result;
    std::for_each(reducedCircles.begin(), reducedCircles.begin() + 15, Sizes(sizes));
    std::sort(sizes.begin(), sizes.end());
    std::vector<int>::iterator begin = std::unique(sizes.begin(), sizes.end(), groupSize);
    sizes.erase(begin, sizes.end());
    std::for_each(reducedCircles.begin(), reducedCircles.end(), SizeCounter(sizes, result));
    end = std::partition(reducedCircles.begin(), reducedCircles.end(), FitImagePartitioner(circleImage.rows, circleImage.cols));
    std::for_each(reducedCircles.begin(), end, SizeCounter(sizes, result));
    reducedCircles.erase(end, reducedCircles.end());

    return printResult(orig, circleImage, detectedContours, contourImage, reducedCircles, result, "Task1.5Image2");

}

bool coins_5_2_all() {
    int thresh = 60;
    cv::Mat orig, image, gray, tmp;

    orig = cv::imread(res + "coins_5_2.jpg", 1);
    orig.copyTo(image);
    //    cv::medianBlur(image, tmp, 5);
    //    cv::bilateralFilter(tmp, image, 15, 15, 15);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::medianBlur(gray, gray, 3);
    cv::morphologyEx(gray, gray, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    //    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);


    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
    /// Draw circles
    // Images for drawing
    cv::Mat circleImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat detectedContours = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat contourImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);


    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(contourImage, contours, (int) i, color, 1);
    }

    VC circles, reducedCircles;
    VCit end;
    std::for_each(contours.begin(), contours.end(), CircleBuilder(26, reducedCircles));
    for (int i = 0; i < 3; ++i) {
        circles.swap(reducedCircles);
        reducedCircles.clear();
        std::for_each(circles.begin(), circles.end(), CircleMergeReducer(8, 0.8, reducedCircles));
    }
    end = std::remove_if(reducedCircles.begin(), reducedCircles.end(), rmseFilter((std::min_element(reducedCircles.begin(), reducedCircles.end()) -> mse) * 30));
    reducedCircles.erase(end, reducedCircles.end());
    std::sort(reducedCircles.begin(), reducedCircles.end());

    circles.swap(reducedCircles);
    reducedCircles.clear();

    std::for_each(circles.begin(), circles.end(), NestedCirclesReducer(reducedCircles));


    std::vector<int> sizes;
    cv::Mat result;
    std::for_each(reducedCircles.begin(), reducedCircles.begin() + 15, Sizes(sizes));
    std::sort(sizes.begin(), sizes.end());
    std::vector<int>::iterator begin = std::unique(sizes.begin(), sizes.end(), groupSize);
    sizes.erase(begin, sizes.end());
    std::for_each(reducedCircles.begin(), reducedCircles.end(), SizeCounter(sizes, result));


    return printResult(orig, circleImage, detectedContours, contourImage, reducedCircles, result, "Task1.6Image2");
}

bool coins_5_3_full() {
    int thresh = 60;
    cv::Mat orig, image, gray, tmp;

    orig = cv::imread(res + "coins_5_3.jpg", 1);
    orig.copyTo(image);
    //    cv::medianBlur(image, tmp, 5);
    //    cv::bilateralFilter(tmp, image, 15, 15, 15);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::medianBlur(gray, gray, 3);
    //    cv::morphologyEx(gray, gray, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);


    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
    /// Draw circles
    cv::Mat circleImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat detectedContours = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat contourImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);


    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(contourImage, contours, (int) i, color, 1);
    }



    VC circles, reducedCircles;
    VCit end;
    std::for_each(contours.begin(), contours.end(), CircleBuilder(26, reducedCircles));
    for (int i = 0; i < 3; ++i) {
        circles.swap(reducedCircles);
        reducedCircles.clear();
        std::for_each(circles.begin(), circles.end(), CircleMergeReducer(8, 0.8, reducedCircles));
    }
    end = std::remove_if(reducedCircles.begin(), reducedCircles.end(), rmseFilter((std::min_element(reducedCircles.begin(), reducedCircles.end()) -> mse) * 30));
    reducedCircles.erase(end, reducedCircles.end());
    std::sort(reducedCircles.begin(), reducedCircles.end());

    circles.swap(reducedCircles);
    reducedCircles.clear();

    std::for_each(circles.begin(), circles.end(), NestedCirclesReducer(reducedCircles));


    std::vector<int> sizes;
    cv::Mat result;
    std::for_each(reducedCircles.begin(), reducedCircles.begin() + 15, Sizes(sizes));
    std::sort(sizes.begin(), sizes.end());
    std::vector<int>::iterator begin = std::unique(sizes.begin(), sizes.end(), groupSize);
    sizes.erase(begin, sizes.end());
    std::for_each(reducedCircles.begin(), reducedCircles.end(), SizeCounter(sizes, result));
    end = std::partition(reducedCircles.begin(), reducedCircles.end(), FitImagePartitioner(circleImage.rows, circleImage.cols));
    std::for_each(reducedCircles.begin(), end, SizeCounter(sizes, result));
    reducedCircles.erase(end, reducedCircles.end());


    return printResult(orig, circleImage, detectedContours, contourImage, reducedCircles, result, "Task1.5Image3");

}

bool coins_5_3_all() {

    int thresh = 60;
    cv::Mat orig, image, gray, tmp;

    orig = cv::imread(res + "coins_5_3.jpg", 1);
    orig.copyTo(image);
    //    cv::medianBlur(image, tmp, 5);
    //    cv::bilateralFilter(tmp, image, 15, 15, 15);
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::medianBlur(gray, gray, 3);
    //    cv::morphologyEx(gray, gray, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);


    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;

    /// Detect edges using canny
    cv::Canny(gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    cv::findContours(canny_output, contours, cv::noArray(), CV_RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
    /// Draw circles
    cv::Mat circleImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat detectedContours = cv::Mat::zeros(canny_output.size(), CV_8UC3);
    cv::Mat contourImage = cv::Mat::zeros(canny_output.size(), CV_8UC3);


    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(contourImage, contours, (int) i, color, 1);
    }



    VC circles, reducedCircles;
    VCit end;
    std::for_each(contours.begin(), contours.end(), CircleBuilder(26, reducedCircles));
    for (int i = 0; i < 3; ++i) {
        circles.swap(reducedCircles);
        reducedCircles.clear();
        std::for_each(circles.begin(), circles.end(), CircleMergeReducer(8, 0.8, reducedCircles));
    }
    end = std::remove_if(reducedCircles.begin(), reducedCircles.end(), rmseFilter((std::min_element(reducedCircles.begin(), reducedCircles.end()) -> mse) * 30));
    reducedCircles.erase(end, reducedCircles.end());
    std::sort(reducedCircles.begin(), reducedCircles.end());

    circles.swap(reducedCircles);
    reducedCircles.clear();

    std::for_each(circles.begin(), circles.end(), NestedCirclesReducer(reducedCircles));


    std::vector<int> sizes;
    cv::Mat result;
    std::for_each(reducedCircles.begin(), reducedCircles.begin() + 15, Sizes(sizes));
    std::sort(sizes.begin(), sizes.end());
    std::vector<int>::iterator begin = std::unique(sizes.begin(), sizes.end(), groupSize);
    sizes.erase(begin, sizes.end());
    std::for_each(reducedCircles.begin(), reducedCircles.end(), SizeCounter(sizes, result));


    return printResult(orig, circleImage, detectedContours, contourImage, reducedCircles, result, "Task1.6Image3");
}