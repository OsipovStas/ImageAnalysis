/* 
 * File:   main.cpp
 * Author: stasstels
 *
 * Created on September 30, 2013, 3:51 PM
 */
#include <cmath>

#include <iostream>
#include <vector>


#include <cv.h>
#include <highgui.h>

static const std::string path("./build/Debug/gen/");

bool task1(const cv::Mat& image) {
    cv::namedWindow( "Task 1.1", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.1", image);                   
    cv::waitKey(0);
    
    return cv::imwrite(path + "copyLena.jpg", image);
    
}

bool task2(const cv::Mat& image) {
    cv::Mat leftSide = image.colRange(0, image.cols / 2);
    cv::Mat bottomSide = image.rowRange(image.rows / 2, image.rows);
    
    cv::namedWindow( "Task 1.2 LeftSide", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.2 LeftSide", leftSide);                   
    cv::waitKey(0);
    
    cv::namedWindow( "Task 1.2 BottomSide", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.2 BottomSide", bottomSide);                   
    cv::waitKey(0);
    
    return cv::imwrite(path + "leftSideLena.jpg", leftSide) && cv::imwrite(path + "bottomSideLena.jpg", bottomSide);
}


bool task3(const cv::Mat& image) {
    cv::Mat copy;
    image.copyTo(copy);
    cv::Mat topLeft(copy, 
                cv::Range(0, copy.rows / 2),
                cv::Range(0, copy.cols / 2));
    cv::Mat bottomRight(copy,
                    cv::Range(copy.rows - topLeft.rows, copy.rows),
                    cv::Range(copy.cols - topLeft.cols, copy.cols));
    cv::Mat tmp;
    topLeft.copyTo(tmp);
    bottomRight.copyTo(topLeft);
    tmp.copyTo(bottomRight);
    
    cv::namedWindow( "Task 1.3", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.3", copy);                   
    cv::waitKey(0);
    
    return cv::imwrite(path + "swapedLena.jpg", copy);
}


void createHalfTone(const cv::Mat& channel, int chIdx, cv::Mat& out) {
    std::vector<cv::Mat> channels(3, cv::Mat::zeros(channel.size(), CV_8U));
    channels[chIdx] = channel;
    cv::merge(channels, out);
}


bool task4(const cv::Mat& image) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);

    cv::Mat Blue;
    createHalfTone(channels[0], 0, Blue);
    
    cv::Mat Green;
    createHalfTone(channels[1], 1, Green);
    
    cv::Mat Red;
    createHalfTone(channels[2], 2, Red);
    
    cv::namedWindow( "Task 1.4 Blue", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.4 Blue", Blue);                   
    cv::waitKey(0);
    
    cv::namedWindow( "Task 1.4 Green", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.4 Green", Green);                   
    cv::waitKey(0);

    cv::namedWindow( "Task 1.4 Red", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.4 Red", Red);                   
    cv::waitKey(0);
    
    return cv::imwrite(path + "GreenLena.jpg", Green) && 
            cv::imwrite(path + "BlueLena.jpg", Blue) && 
            cv::imwrite(path + "RedLena.jpg", Red);
    
}


bool task5(const cv::Mat& image) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    
    cv::Mat mean = (channels[0] + channels[1] + channels[2]) / 3;
    
    cv::namedWindow("Task 1.5", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.5", mean);                   
    cv::waitKey(0);
    return cv::imwrite(path + "LenaGreyMean.jpg", mean);
}


bool task6(const cv::Mat& original, const cv::Mat& mean) {
    cv::Mat grey, diff;
    cv::cvtColor(original, grey, cv::COLOR_BGR2GRAY);
    cv::absdiff(grey, mean, diff);
    
    cv::namedWindow("Task 1.6", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.6", diff);                   
    cv::waitKey(0);
    
    return cv::imwrite(path + "LenaDiff.jpg", diff);
}


bool task7(const cv::Mat& image) {
    cv::Mat maxValue(image.size(), CV_8U, cv::Scalar(255));
    std::vector<cv::Mat> channels;
    cv::Mat tmp, result;
    
    cv::cvtColor(image, tmp, cv::COLOR_BGR2HSV);
    cv::split(tmp, channels);
    channels[1] = maxValue;
    channels[2] = maxValue;
    cv::merge(channels, tmp);
    cv::cvtColor(tmp, result, cv::COLOR_HSV2BGR);
    
    cv::namedWindow( "Task 1.7", CV_WINDOW_NORMAL);
    cv::imshow( "Task 1.7", result);                   
    cv::waitKey(0);
    
    return cv::imwrite(path + "MaxSatValLena.jpg", result);
}

bool task3_1(const cv::Mat& image) {
    std::vector<cv::Mat> src(1, image);
    std::vector<cv::Mat> dst(1, cv::Mat(image.size(), image.type())); 
    static int fromTo[][6] = {
        {0, 0, 1, 1, 2, 2},
        {0, 1, 1, 2, 2, 0},
        {0, 2, 1, 0, 2, 1},
        {0, 0, 1, 2, 2, 1},
        {0, 1, 1, 0, 2, 2},
        {0, 2, 1, 1, 2, 0}
    };
    bool res = true;
    static const char* names[] = {
        "123", "231", "312", "132", "213", "321"
    };
    for(int i = 0; i < 6; ++i) {
        cv::mixChannels(src, dst, fromTo[i], 3);
        res &= cv::imwrite(path + names[i] + ".jpg", dst[0]);
        cv::namedWindow( "Task 3.1", CV_WINDOW_NORMAL);
        cv::imshow( "Task 3.1", dst[0]);                   
        cv::waitKey(0);
    }
    return res;
}


bool task3_2(const cv::Mat& image) {
    cv::Mat HInvert;
    std::vector<cv::Mat> channels;
    cv::Mat White(image.size(), CV_8U, cv::Scalar(255));
    
    cv::cvtColor(image, HInvert, CV_BGR2HLS);
    cv::split(image, channels);
    
    channels[0] = White - channels[0];
    
    cv::merge(channels, HInvert);
    cv::cvtColor(HInvert, HInvert, CV_HLS2BGR);
    
    cv::namedWindow( "Task 3.2", CV_WINDOW_NORMAL);
    cv::imshow( "Task 3.2", HInvert);                   
    cv::waitKey(0);
    
    return cv::imwrite(path + "InvertHLena.jpg", HInvert);
}

float convertPixelBrightBGR2XYZ(float bright) {
    bright /= 255;
    if(bright > 0.04045) {
        bright = pow((bright + 0.055f) / 1.055f, 2.4f);
    } else {
        bright /= 12.92;
    }
    bright *= 100;
    return bright;
}


void convertBGR2XYZ(const cv::Mat& BGRImage, cv::Mat& XYZImage) {
    CV_Assert(BGRImage.channels() == 3);
    
    cv::Mat tmpImage;
    
    BGRImage.convertTo(tmpImage, CV_32FC3);
    
    for(cv::MatIterator_<cv::Vec3f> it = tmpImage.begin<cv::Vec3f>(); it != tmpImage.end<cv::Vec3f>(); ++it) {
        for(int i = 0; i < 3; ++i) {
            (*it)[i] = convertPixelBrightBGR2XYZ((*it)[i]);
        }
    }

    std::vector<cv::Mat> tmpChannels;
    cv::split(tmpImage, tmpChannels);
    
    std::vector<cv::Mat> XYZChannels(3);
    
    XYZChannels[0] = tmpChannels[2] * 0.4124f + tmpChannels[1] * 0.3576f + tmpChannels[0] * 0.1805f;
    XYZChannels[1] = tmpChannels[2] * 0.2126f + tmpChannels[1] * 0.7152f + tmpChannels[0] * 0.0722f;
    XYZChannels[2] = tmpChannels[2] * 0.0193f + tmpChannels[1] * 0.1192f + tmpChannels[0] * 0.9505f;
       
    cv::merge(XYZChannels, XYZImage);
}


float convertPixelBrightXYZ2CIE(float bright) {
    if (bright > 0.008856f) {
        bright = pow(bright, 1.0f / 3);
    } else {
        bright = (7.787f * bright) + (16.0f / 116.0f);
    }
    return bright;
}

void convertXYZ2CIELab(const cv::Mat& XYZImage, cv::Mat& CIEImage) {
    
    CV_Assert(XYZImage.channels() == 3);
    
    static const float ref_X =  95.047f;
    static const float ref_Y = 100.000f;
    static const float ref_Z = 108.883f;
    
    std::vector<cv::Mat> XYZchannels;
    
    cv::split(XYZImage, XYZchannels);
    
    XYZchannels[0] /= ref_X;
    XYZchannels[1] /= ref_Y;
    XYZchannels[2] /= ref_Z;
    
    for(std::vector<cv::Mat>::iterator it = XYZchannels.begin(); it != XYZchannels.end(); ++it) {
        for(cv::MatIterator_<float> ch_it = (*it).begin<float>(); ch_it != (*it).end<float>(); ++ch_it) {
            (*ch_it) = convertPixelBrightXYZ2CIE((*ch_it));
        }
    }
    
    std::vector<cv::Mat> CIEChannels(3);
    
    CIEChannels[0] = (116 * XYZchannels[1]) - 16;
    CIEChannels[1] = 500 * (XYZchannels[0] - XYZchannels[1]);
    CIEChannels[2] = 200 * (XYZchannels[1] - XYZchannels[2]);
       
    CIEChannels[0] *= 255;
    CIEChannels[0] /= 100;
    CIEChannels[1] += 128;
    CIEChannels[2] += 128;
  
    cv::merge(CIEChannels, CIEImage);
    CIEImage.convertTo(CIEImage, CV_8UC3);
}

void convertBGR2Lab(const cv::Mat& BGRImage, cv::Mat& LabImage) {
    convertBGR2XYZ(BGRImage, LabImage);
    convertXYZ2CIELab(LabImage, LabImage);
}

void concatChannels(const cv::Mat& src, cv::Mat& dst) {
    CV_Assert(src.channels() == 3);
    
    std::vector<cv::Mat> channels;
    
    cv::split(src, channels);
    dst = (channels[0] / 3 + channels[1] / 3 + channels[2] / 3);
    
}

bool task3_3(const cv::Mat& image) {
    cv::Mat manualLab, buildInLab, diff;
    
    cv::cvtColor(image, buildInLab, CV_BGR2Lab);
    convertBGR2Lab(image, manualLab);
    cv::absdiff(manualLab, buildInLab, diff);
    
    concatChannels(manualLab, manualLab);
    concatChannels(buildInLab, buildInLab);
    concatChannels(diff, diff);

    cv::namedWindow( "Task 3.3 BuildInCIELab", CV_WINDOW_NORMAL);
    cv::imshow( "Task 3.3 BuildInCIELab", buildInLab);                   
    cv::waitKey(0);    
    
    cv::namedWindow( "Task 3.3 ManualCIELab", CV_WINDOW_NORMAL);
    cv::imshow( "Task 3.3 ManualCIELab", manualLab);                   
    cv::waitKey(0);

    cv::namedWindow("Task3.3 DIfference", CV_WINDOW_NORMAL);
    cv::imshow("Task3.3 DIfference", diff);                   
    cv::waitKey(0);
    
    return cv::imwrite(path + "CIELabBuildInLena.jpg", buildInLab) &&
             cv::imwrite(path + "CIELabManualLena.jpg", manualLab) &&
             cv::imwrite(path + "CIELabDiffLena.jpg", diff);
    
}

int main( int argc, char** argv ) {
    
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/lena" << std::endl;
        exit(1);
    }
    
    cv::Mat image;
    image = cv::imread(argv[1], 1);

    std::cout << "Lesson 1 tasks: " << std::endl;
    
    if(!image.data ) {
        std::cerr << "No image data" << std::endl;
        exit(1);
    }

    std::cout << "Task 1 Status: " << (task1(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 2 Status: " << (task2(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3 Status: " << (task3(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 4 Status: " << (task4(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 5 Status: " << (task5(image) ? "OK" : "Error") << std::endl;
    
    cv::Mat mean = cv::imread(path + "LenaGreyMean.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    std::cout << "Task 6 Status: " << (task6(image, mean) ? "OK" : "Error") << std::endl;
    std::cout << "Task 7 Status: " << (task7(image) ? "OK" : "Error") << std::endl;
    
    std::cout << "Task 3_1 Status: " << (task3_1(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3_2 Status: " << (task3_2(image) ? "OK" : "Error") << std::endl;
    std::cout << "Task 3_3 Status: " << (task3_3(image) ? "OK" : "Error") << std::endl;

    return 0;
}




