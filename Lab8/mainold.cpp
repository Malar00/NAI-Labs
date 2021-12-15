#include <algorithm>
#include <iostream>
#include <numeric>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
    const int max_value_H = 360 / 2;
    const int max_value = 255;
    int low_H = 0;
    int low_S = 0;
    int low_V = 0;
    int high_H = max_value_H;
    int high_S = max_value;
    int high_V = max_value;
    int down_width = 320;
    int down_height = 200;

    namedWindow("parametry", WINDOW_AUTOSIZE);
    createTrackbar("low_H", "parametry", &low_H, max_value_H);
    createTrackbar("low_S", "parametry", &low_S, max_value_H);
    createTrackbar("low_V", "parametry", &low_V, max_value);
    createTrackbar("high_H", "parametry", &high_H, max_value);
    createTrackbar("high_S", "parametry", &high_S, max_value);
    createTrackbar("high_V", "parametry", &high_V, max_value);


    if (argc < 2) throw std::invalid_argument("Podaj argumenty");
    down_width = atoi(argv[1]);
    down_height = atoi(argv[2]);


    VideoCapture cap1(0);
    if (!cap1.isOpened())
        return -1;


    // VideoCapture backgroundvid("Multiwave.wmv");
    list<Point> tracking_path;

    while (waitKey(1) != 27) {
        Mat frame;
        Mat backgroundScaled;
        Mat frameMask, frameNegMask;
        Mat frameWithMask, backgroundScaledWithMask;
        Mat meinniceplace;
        //(with animated background) Mat background;
        //(with animated background) backgroundvid >> background;
        cap1 >> frame;
        flip(frame, frame, 1);
        resize(frame, frame, Size(down_width, down_height), INTER_LINEAR);
        cvtColor(frame, frameMask, COLOR_RGB2HSV);
        inRange(frameMask, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frameNegMask);

        int dilation_size = 5;
        auto structElem = getStructuringElement(MORPH_ELLIPSE,
                                                Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                                                Point(dilation_size, dilation_size));
        erode(frameNegMask, frameNegMask, structElem);
        dilate(frameNegMask, frameNegMask, structElem);

        imshow("parametry", frameNegMask);
        vector<vector<Point>> contours;
        findContours(frameNegMask, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        std::sort(contours.begin(), contours.end(), [](auto& a, auto& b) {
            return contourArea(a, false) > contourArea(b, false);
        });
        if ((contours.size() > 0) && (contourArea(contours[0], false) > 400)) {
            approxPolyDP(contours[0], contours[0], 2, true);
            int area = contourArea(contours[0], true);
            float radius;
            Point2f center;
            minEnclosingCircle(contours[0], center, radius);
            putText(frame, to_string(area), center, FONT_HERSHEY_PLAIN, 1.0, {255, 33, 0});
            tracking_path.push_back(center);
        } else {
            if (tracking_path.size()) tracking_path.pop_front();
        }
        if (tracking_path.size()) {
            vector<Point> shape_contour(tracking_path.begin(),tracking_path.end());
            approxPolyDP(shape_contour, shape_contour, 15, true);
            vector<vector<Point>> to_show = {shape_contour};
            drawContours(frame, to_show,0,{0,200,100});

            to_show = {{tracking_path.begin(), tracking_path.end()}};
            drawContours(frame, to_show,0,{0,100,200});

            if ((shape_contour.size() == 4) && (contourArea(shape_contour,false) > 10000)) {
                vector<double> lengths(4);
                for (int i = 0 ; i < 4; i ++) {
                    Point2i vec = shape_contour[i]-shape_contour[(i+1)%4];
                    lengths[i] = sqrt(vec.x*vec.x + vec.y*vec.y);
                }
                if ((abs(lengths[0]/lengths[2]-1.0) < 0.1) || (abs(lengths[1]/lengths[3]-1.0) < 0.1)) {
                    cout << "jest prostokat!! " << contourArea(shape_contour,false) << endl;
                    tracking_path.clear();
                    system("gimp");
                }
            }

            if (tracking_path.size() > 60) {
                tracking_path.pop_front();
            }
        }
        imshow("oryginal", frame);

        // bitwise_not(frameNegMask,frameMask); // negation
        // frame.copyTo(frameWithMask,frameMask); // copy with mask (keying)
        // backgroundScaled.copyTo(backgroundScaledWithMask,frameNegMask);
        //
        // meinniceplace = backgroundScaledWithMask + frameWithMask;
    }
    return 0;
}
