#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>


#include <string>
#include <filesystem>
#include <unistd.h>

using std::cout; using std::cin;
using std::endl; using std::string;
using std::filesystem::current_path;

// g++ `pkg-config --cflags opencv4` cv.cpp `pkg-config --libs opencv4`

// see https://docs.opencv.org/4.x/dd/d49/tutorial_py_contour_features.html

using namespace std;
using namespace cv;


int main(int argc, char **argv) {
    CascadeClassifier side_cascade;
    VideoCapture cap1(0);
    if (!cap1.isOpened()) {
        return -1;
    }
    Mat frame;

    if (!side_cascade.load("../haarcascade_profileface.xml")) {
        return 1;
    }
    if (!cap1.isOpened()) {
        return -2;
    }


    while (cap1.read(frame)) {
        if (frame.empty()) return -1;

        vector<Rect> detectedSides;
        Mat frame_gray;

        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
        equalizeHist(frame_gray, frame_gray);

        side_cascade.detectMultiScale(frame_gray, detectedSides, 1.1, 4, 0, Size(30, 30));

        for (size_t i = 0; i < detectedSides.size(); i++) {

            rectangle(frame, Point(detectedSides[i].x, detectedSides[i].y),
                      Point(detectedSides[i].x + detectedSides[i].width, detectedSides[i].y + detectedSides[i].height),
                      Scalar(0, 0, 255), 4);
            putText(frame, "Face nr: " + to_string(i + 1),
                    Point(detectedSides[i].x, detectedSides[i].y + detectedSides[i].height + 20), FONT_HERSHEY_SIMPLEX,
                    0.55, Scalar(0, 255, 255), 2);
        }

        imshow("frame", frame);

        int key_pressed = waitKey(15);
        if (key_pressed == 27) {
            break;
        }
    }
    return 0;
}



