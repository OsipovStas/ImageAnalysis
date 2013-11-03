/* 
 * File:   circles.hpp
 * Author: stasstels
 *
 * Created on October 29, 2013, 11:35 PM
 */

#ifndef CIRCLES_HPP
#define	CIRCLES_HPP

#include <vector>

#include <cv.h>

typedef std::vector<std::vector<cv::Point> > VVP;
typedef std::vector<std::vector<cv::Point> >::iterator VVPit;

typedef std::vector<cv::Point> VP;
typedef std::vector<cv::Point>::iterator VPit;
typedef std::vector<cv::Point>::const_iterator VPCit;


void subtractCol(const cv::Mat& src_, const cv::Mat& col_, cv::Mat& dst);

struct Circle;

// Approximates set of points by circle
void getCircle(VPit begin, VPit end, Circle& c);

struct Circle {
    cv::Ptr<VP> chain;
    int radius;
    cv::Point center;
    float mse;

    Circle() : chain(new VP), radius(0), center(0, 0), mse(0.0) {
    };

    // Merges two contours and approximates new contour by circle
    void average(Circle& c2) {
        chain -> insert(chain -> end(), c2.chain -> begin(), c2.chain -> end());
        getCircle(chain -> begin(), chain -> end(), *this);
    }


    void calcMSE();

    bool operator<(const Circle& o) const;

};

typedef std::vector<Circle> VC;
typedef std::vector<Circle>::iterator VCit;
typedef std::vector<Circle>::const_iterator VCCit;



// This functor build circles from contours
class CircleBuilder {
public:

    CircleBuilder(int minSize, VC& circles) : minSize(minSize), circles(circles) {
    }

    void operator()(VP& contour) {
        if (contour.size() < minSize) {
            return;
        }
        int cnt = contour.size() / minSize;
        for (VPit vpit = contour.begin(); vpit != contour.begin() + minSize * cnt; vpit += minSize) {
            Circle c;
            getCircle(vpit, vpit + minSize, c);
            circles.push_back(c);
        }
    }
private:
    int minSize;
    VC& circles;

};

// Filter circles exceeding error limit 
struct rmseFilter {
    float limit;

    rmseFilter(float limit) : limit(limit) {
    }

    bool operator()(const Circle& c) {
        return c.mse > limit;
    }
};

// Functor tests if circle approximates the same contour
class inTouch {
public:

    inTouch(float distance, float scale, const Circle& c) : distance(distance), scale(scale), c(c) {
    };

    bool operator()(const Circle& o) {
        return cv::norm(c.center - o.center) < distance && (c.radius > o.radius * scale) && (c.radius * scale < o.radius);
    }

    const float distance;
    const float scale;
    const Circle& c;
};

// Functor merges similar circles
class CircleMergeReducer {
public:

    CircleMergeReducer(float diff, float scale, VC& circles) : distance(diff), scale(scale), circles(circles) {
    }

    void operator()(Circle& c) {
        VCit cur = std::find_if(circles.begin(), circles.end(), inTouch(distance, scale, c));
        if (cur == circles.end()) {
            circles.push_back(c);
            return;
        }
        cur -> average(c);
    }
private:
    float distance;
    float scale;
    VC& circles;

};

// This functor removes nested circles
struct NestedCirclesReducer {

    NestedCirclesReducer(VC& circles) : circles(circles) {
    }

    struct Nested {

        Nested(Circle& c) : c(c) {
        }

        bool operator()(Circle& o) {
            return (cv::norm(c.center - o.center) < c.radius + 5) || (cv::norm(c.center - o.center) < o.radius + 5);

        }

        Circle& c;
    };

    void operator()(Circle& c) {
        VCit cur = std::find_if(circles.begin(), circles.end(), Nested(c));
        if (cur == circles.end() && c.radius > 15) {
            circles.push_back(c);
        }
    }

    VC& circles;

};

// This functor collects circle radiuses

struct Sizes {

    Sizes(std::vector<int>& sizes) : sizes(sizes) {
    }

    void operator()(Circle& c) {
        sizes.push_back(c.radius);
    }

    std::vector<int>& sizes;
};

// This functor split circles by groups and collects stats

struct SizeCounter {

    SizeCounter(std::vector<int>& sizes, cv::Mat& sizeMat_) : sizeMat(sizeMat_) {
        cv::Mat(sizes.size(), 2, CV_32S, cv::Scalar(0)).copyTo(sizeMat);
        cv::Mat(sizes).copyTo(sizeMat.col(0));
    }

    void operator()(Circle& c) {
        cv::Mat tmp(sizeMat.rows, 1, CV_32S, cv::Scalar(0));
        sizeMat.col(0).copyTo(tmp);
        tmp = cv::abs(tmp - (c.radius + 1));
        cv::Point minLoc;

        cv::minMaxLoc(tmp, 0, 0, &minLoc);

        c.radius = sizeMat.at<int>(minLoc.y, 0);
        ++sizeMat.at<int>(minLoc.y, 1);

    }

    cv::Mat& sizeMat;
};

// This functor checks is circle fit in image

struct FitImagePartitioner {

    FitImagePartitioner(int rows, int cols) : height(rows), width(cols) {
    }

    bool isInImage(const cv::Point& p) {
        return (p.x > 0) && (p.x < width) && (p.y > 0) && (p.y < height);
    }

    bool operator()(const Circle& c) {
        cv::Point p1(c.center.x + c.radius, c.center.y + c.radius);
        cv::Point p2(c.center.x - c.radius, c.center.y - c.radius);

        return isInImage(p1) && isInImage(p2);
    }

    const int height;
    const int width;
};
#endif	/* CIRCLES_HPP */

