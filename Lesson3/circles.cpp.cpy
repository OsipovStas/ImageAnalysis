#include "circles.hpp"

void subtractCol(const cv::Mat& src_, const cv::Mat& col_, cv::Mat& dst) {
    cv::Mat src(src_);
    cv::Mat col(col_);

    dst = cv::Mat(src.rows, src.cols, src.type());
    for (int i = 0; i < src.cols; ++i) {
        dst.col(i) = src.col(i) - col;
    }
}

bool CirclePartitioner::operator()(cv::Point& next) {
    if (c.chain -> size() < 3) {
        c.chain -> push_back(next);
        return true;
    }

    if (c.chain -> size() == 3) {
        buildCircle();
        calcMSE();
    }

    float err = cv::norm(next - c.center) - c.radius;

    if (err * err > 3 * c.mse) {
        return false;
    }

    c.mse = c.mse * (c.chain -> size()) + (err * err);
    c.chain -> push_back(next);
    c.mse /= (c.chain -> size());
    return true;
}

void CirclePartitioner::buildCircle(float norm) {
    CV_Assert(c.chain -> size() > 2);

    cv::Mat pts(2, 3, CV_32F);
    for (int i = 0; i < pts.cols; ++i) {
        cv::Mat(c.chain -> at(i)).copyTo(pts.col(i));
    }
    pts /= norm;

    cv::Mat left;
    subtractCol(pts.colRange(1, pts.cols), pts.col(0), left);

    cv::Mat right;
    cv::reduce(pts.mul(pts), right, 0, CV_REDUCE_AVG);
    subtractCol(right.colRange(1, right.cols), right.col(0), right);

    cv::Mat origin;
    cv::solve(left.t(), right.t(), origin);

    cv::Mat rad;
    subtractCol(pts, origin, rad);
    cv::reduce(rad.mul(rad), rad, 0, CV_REDUCE_SUM);
    cv::reduce(rad, rad, 1, CV_REDUCE_AVG);

    c.center.x = cvRound(origin.at<float>(0, 0) * norm);
    c.center.y = cvRound(origin.at<float>(1, 0) * norm);
    c.radius = cvRound(std::sqrt(rad.at<float>(0, 0)) * norm);
}

void CirclePartitioner::calcMSE() {
    float mse_ = 0.0;

    for (VP::const_iterator cit = c.chain -> begin(); cit != c.chain -> end(); ++cit) {
        float err = cv::norm((*cit) - c.center) - c.radius;
        mse_ += (err * err);
    }

    c.mse = mse_ / (c.chain -> size());
}

void CircleAggregator::operator()(VP& contour) {
    VPit end = std::partition(contour.begin(), contour.end(), OrderPartitioner(5));
    VPit start = contour.begin();
    while (start != end) {
        Circle c;
        start = std::partition(start, end, CirclePartitioner(c));
        if (isValidCircle(c)) {
            circles.push_back(c);
        }
    }
}

void CircleAggregator::accept(VVP& contours) {
    std::for_each(contours.begin(), contours.end(), *this);
}



