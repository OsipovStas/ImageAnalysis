#include "circles.hpp"

const int ORDER_THRESH = 9;
const float CIRCLE_THRESHOLD = 4;

void subtractCol(const cv::Mat& src_, const cv::Mat& col_, cv::Mat& dst) {
    cv::Mat src(src_);
    cv::Mat col(col_);

    dst = cv::Mat(src.rows, src.cols, src.type());
    for (int i = 0; i < src.cols; ++i) {
        dst.col(i) = src.col(i) - col;
    }
}

bool Circle::operator<(const Circle& o) const {
    float err1 = std::sqrt(mse);
    float err2 = std::sqrt(o.mse);
    return err1 < err2;
}

void Circle::calcMSE() {
    mse = 0;

    for (VVPit vvpit = chains -> begin(); vvpit != chains -> end(); ++vvpit) {
        float mse_ = 0.0;
        for (VPit vpit = vvpit -> begin(); vpit != vvpit -> end(); ++vpit) {
            float err = cv::norm((*vpit) - center) - radius;
            mse_ += (err * err);
        }
        mse_ /= vvpit -> size();
        mse += mse_;
    }

    CV_Assert(chains -> size() != 0);

    mse = std::sqrt(mse) / chains -> size();
}

void getCircle(VPit begin, VPit end, Circle& c) {

    int size = end - begin;

    cv::Mat pts(2, size, CV_32F);
    for (int i = 0; i < pts.cols; ++i) {
        cv::Mat(*(begin + i)).copyTo(pts.col(i));
    }
    //    pts /= norm;

    cv::Mat left;
    subtractCol(pts.colRange(1, pts.cols), pts.col(0), left);

    cv::Mat right;
    cv::reduce(pts.mul(pts), right, 0, CV_REDUCE_AVG);
    subtractCol(right.colRange(1, right.cols), right.col(0), right);

    cv::Mat origin;
    cv::solve(left.t(), right.t(), origin, cv::DECOMP_QR);

    cv::Mat rad;
    subtractCol(pts, origin, rad);
    cv::reduce(rad.mul(rad), rad, 0, CV_REDUCE_SUM);
    cv::reduce(rad, rad, 1, CV_REDUCE_AVG);

    c.center.x = cvRound(origin.at<float>(0, 0));
    c.center.y = cvRound(origin.at<float>(1, 0));
    c.radius = cvRound(std::sqrt(rad.at<float>(0, 0)));
    c.chains -> push_back(VP(begin, end));
    c.calcMSE();
}