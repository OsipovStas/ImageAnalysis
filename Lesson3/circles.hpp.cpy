/* 
 * File:   circles.hpp
 * Author: stasstels
 *
 * Created on October 29, 2013, 11:00 PM
 */

#ifndef CIRCLES_HPP
#define	CIRCLES_HPP
#include <vector>

//#include <cv.h>

typedef std::vector<std::vector<cv::Point> > VVP;
typedef std::vector<std::vector<cv::Point> >::iterator VVPit;

typedef std::vector<cv::Point> VP;
typedef std::vector<cv::Point>::iterator VPit;

void subtractCol(const cv::Mat& src_, const cv::Mat& col_, cv::Mat& dst);


struct Circle {
    cv::Ptr<VP> chain;
    int radius;
    cv::Point center;
    float mse;

    Circle() : chain(new VP), radius(0), center(0, 0), mse(0.0) {
    };
};

typedef std::vector<Circle> VC;
typedef std::vector<Circle>::iterator VCit;

class CirclePartitioner {
public:

    CirclePartitioner(Circle& c) : c(c) {
    }

    bool operator()(cv::Point& next);


private:

    void buildCircle(float norm = 512);
    void calcMSE();


    Circle& c;
};

class CircleAggregator {
public:

    CircleAggregator(VC& circles) : circles(circles) {
    }

    void accept(VVP& contours);
    
    void operator()(VP& contour);
private:
    
    bool isValidCircle(const Circle& c) const {
        return c.chain -> size() > 6;
    }
    
    VC& circles;
};

class OrderPartitioner {
public:

    OrderPartitioner(int order) : order(order), i(0) {
    }

    bool operator()(const cv::Point& next) {
        if (++i % order == 0) {
            return true;
        }
        return false;
    }

private:
    int order;
    int i;

};


#endif	/* CIRCLES_HPP */

