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

    while (true) {
        int i = 0;

        Mat original_image, m;
        cap1 >> original_image;
        Mat k = getStructuringElement(MORPH_ELLIPSE, {5, 5});

        cvtColor(original_image, m, COLOR_BGR2HSV);
        imshow("original image" + to_string(i++), m);
        resize(m, m, Size(down_width, down_height), INTER_LINEAR);
        GaussianBlur(m, m, Size(9, 9), 0, 0);
        inRange(m, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), m);
        putText(m, "H: " + to_string(low_H) + " - " + to_string(high_H), cv::Point(0, 20), cv::FONT_HERSHEY_DUPLEX, 1,
                100, 2, false);
        putText(m, "S: " + to_string(low_S) + " - " + to_string(high_S), cv::Point(0, 50), cv::FONT_HERSHEY_DUPLEX, 1,
                100, 2, false);
        putText(m, "V: " + to_string(low_V) + " - " + to_string(high_V), cv::Point(0, 80), cv::FONT_HERSHEY_DUPLEX, 1,
                100, 2, false);
        imshow("capture " + to_string(i++), m);


        if (waitKey(1) == 120) {
            Rect2d r = selectROI(m, false);
            Mat imCrop = m(r);
            imshow("Image", imCrop);
            imwrite("test.jpg", imCrop);
        }
        if (waitKey(1) == 27) {
            break;
        }
    }
    return 0;
}