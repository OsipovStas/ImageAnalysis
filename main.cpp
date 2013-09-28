#include <iostream>
#include <vector>


#include <cv.h>
#include <highgui.h>

void createHalfTone(const cv::Mat& channel, int chIdx, cv::Mat& out) {
    std::vector<cv::Mat> channels(3, cv::Mat::zeros(channel.size(), CV_8U));
    channels[chIdx] = channel;
    cv::merge(channels, out);
}


bool task4(const cv::Mat& mat) {
    std::vector<cv::Mat> channels;
    cv::split(mat, channels);

    cv::Mat Blue;
    createHalfTone(channels[0], 0, Blue);
    
    cv::Mat Green;
    createHalfTone(channels[1], 1, Green);
    
    cv::Mat Red;
    createHalfTone(channels[2], 2, Red);
    return cv::imwrite("GreenLena.jpg", Green) && 
            cv::imwrite("BlueLena.jpg", Blue) && 
            cv::imwrite("RedLena.jpg", Red);
    
}


int main( int argc, char** argv ) {
    cv::Mat image;
    image = cv::imread(argv[1], 1);

    std::cout << argv[1] << std::endl;

    if( argc != 2 || !image.data ) {
        printf( "No image data \n" );
        return -1;
    }

    std::cout << task4(image) << std::endl;

    //  cv::namedWindow( "Display Image", cv::CV_WINDOW_AUTOSIZE);
    //  cv::imshow( "Display Image", image );

    cv::waitKey(0);

    return 0;
}

