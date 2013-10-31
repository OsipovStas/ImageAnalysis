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

void subtractCol(const cv::Mat& src_, const cv::Mat& col_, cv::Mat& dst);

struct Circle {
    cv::Ptr<VVP> chains;
    int radius;
    cv::Point center;
    float mse;

    Circle() : chains(new VVP), radius(0), center(0, 0), mse(0.0) {
    };

    void average(Circle& c2) {
        center += c2.center;
        center.x /= 2;
        center.y /= 2;
        radius += c2.radius;
        radius /= 2;

        for (VVPit vvpit = c2.chains -> begin(); vvpit != c2.chains -> end(); ++vvpit) {
            chains -> push_back(*vvpit);
        }
        calcMSE();
    }


    void calcMSE();

    bool operator<(const Circle& o) const;

};

typedef std::vector<Circle> VC;
typedef std::vector<Circle>::iterator VCit;

void getCircle(VPit begin, VPit end, Circle& c);

class CircleBuilder {
public:

    CircleBuilder(int minSize, VC& circles) : minSize(minSize), circles(circles) {
    }

    void operator()(VP& contour) {
        if (contour.size() < minSize) {
            return;
        }
        int cnt = contour.size() / minSize;
        for (VPit vpit = contour.begin(); vpit != contour.() + minSize * cnt; vpit += minSize) {
            Circle c;
            getCircle(vpit, vpit + minSize, c);
            circles.push_back(c);
        }
    }
private:
    int minSize;
    VC& circles;

};

struct rmseFilter {
    float limit;

    rmseFilter(float limit) : limit(limit * 2) {
    }

    bool operator()(const Circle& c) {
        return c.mse > limit;
    }
};

class inTouch {
public:

    inTouch(float distance, const Circle& c) : distance(distance), c(c) {
    };

    bool operator()(const Circle& o) {
        return cv::norm(c.center - o.center) < distance;
    }

    const float distance;
    const Circle& c;
};

class circleReducer {
public:

    circleReducer(float diff, VC& circles) : distance(diff), circles(circles) {
    }

    void operator()(Circle& c) {
        VCit cur = std::find_if(circles.begin(), circles.end(), inTouch(distance, c));
        if (cur == circles.end()) {
            circles.push_back(c);
            return;
        }
        cur -> average(c);
    }
private:
    float distance;
    VC& circles;

};


#endif	/* CIRCLES_HPP */

