#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

// g++ `pkg-config --cflags opencv4` cv.cpp `pkg-config --libs opencv4`

// see https://docs.opencv.org/4.x/dd/d49/tutorial_py_contour_features.html


using namespace std;
using namespace cv;

void drawPictureOnPict(const Mat srcMat, const Mat imgToPut, Mat &resultMat, const vector<Point> contour) {
    Mat dstMat(Size(srcMat.cols, srcMat.rows), CV_8UC3);
    Mat cutter(Size(imgToPut.cols, imgToPut.rows), imgToPut.type());

    cutter = Scalar(0);
    bitwise_not(cutter, cutter);
    vector<Point2f> src = {{0,                               0},
                           {static_cast<float>(dstMat.cols), 0},
                           {static_cast<float>(dstMat.cols), static_cast<float>(dstMat.rows)},
                           {0,                               static_cast<float>(dstMat.rows)}};
    vector<Point2f> dst;
    for (auto p: contour)
        dst.push_back(Point2f(p.x, p.y));
    auto wrap_mtx = getPerspectiveTransform(src, dst);
    warpPerspective(imgToPut, dstMat, wrap_mtx, Size(dstMat.cols, dstMat.rows));

    warpPerspective(cutter, cutter, wrap_mtx, Size(dstMat.cols, dstMat.rows));
    bitwise_not(cutter, cutter);
    Mat dstMatClone;
    srcMat.copyTo(dstMatClone, cutter);

    dstMat = dstMat + dstMatClone;
    resultMat = dstMat;
}

int main(int argc, char **argv) {
    const int max_value_H = 360 / 2;
    const int max_value = 255;
    int low_H = 0;
    int low_S = 0;
    int low_V = 0;
    int high_H = max_value_H;
    int high_S = max_value;
    int high_V = max_value;
    int down_width = 720;
    int down_height = 480;

    namedWindow("parametry", WINDOW_AUTOSIZE);
    createTrackbar("low_H", "parametry", &low_H, max_value_H);
    createTrackbar("low_S", "parametry", &low_S, max_value_H);
    createTrackbar("low_V", "parametry", &low_V, max_value);
    createTrackbar("high_H", "parametry", &high_H, max_value_H);
    createTrackbar("high_S", "parametry", &high_S, max_value);
    createTrackbar("high_V", "parametry", &high_V, max_value);

    //low_S=115;
    //high_H=109;

    VideoCapture cap1(0);
    if (!cap1.isOpened())
        return -1;

    int framecount = 0;
    while (true) {
        Mat frame;
        cap1 >> frame;
        framecount++;

        flip(frame, frame, 1);
        resize(frame, frame, Size(down_width, down_height), INTER_LINEAR);


        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        Mat mask1, mask2;

        inRange(hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask1);
        inRange(hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask2);

        Mat mask = mask1 | mask2;

        imshow("mask", mask);




        int dilation_size = 8;
        auto structElem = getStructuringElement(MORPH_ELLIPSE,
                                                Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                                                Point(dilation_size, dilation_size));
        erode(mask, mask, structElem);
        dilate(mask, mask, structElem);

        Mat output;
        bitwise_and(hsv, hsv, output, mask);

        std::vector<std::vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
        std::sort(contours.begin(), contours.end(), [](auto& a, auto& b) {
            return contourArea(a, false) > contourArea(b, false);
        });


        if ((contours.size() > 0) && (contourArea(contours[0], false) > 10000)) {
            drawContours(frame, contours, -1, 255, 3);
        }
        double biggest1 = 0;
        double biggest2 = 0;
        Rect bounding_rect1, bounding_rect2;
        int biggest_idx1 = 0;
        int biggest_idx2 = 0;

        for (size_t i = 0; i < contours.size(); i++) {
            double area = contourArea(contours[i]);
            if (area > biggest1) {
                biggest1 = area;
                biggest_idx1 = i;
                bounding_rect1 = boundingRect(contours[i]);
            }
            else if (area > biggest2) {
                biggest2 = area;
                biggest_idx2 = i;
                bounding_rect2 = boundingRect(contours[i]);
            }
        }

        drawContours(frame, contours, biggest_idx1, Scalar(255,0,0), 3);
        drawContours(frame, contours, biggest_idx2, Scalar(255,0,0), 3);

        auto cx1 = bounding_rect1.x + bounding_rect1.width / 2;
        auto cy1 = bounding_rect1.y + bounding_rect1.height / 2;

        auto cx2 = bounding_rect2.x + bounding_rect2.width / 2;
        auto cy2 = bounding_rect2.y + bounding_rect2.height / 2;


        if (std::abs(cy1 - cy2) < 100.0) {
            line(frame, Point(cx1, cy1), Point(cx2, cy2), Scalar(0, 0, 255), 3);
        }

        imshow("frame", frame);

        int key_pressed = waitKey(10);
        if (key_pressed == 27) {
            break;
        }
    }
}