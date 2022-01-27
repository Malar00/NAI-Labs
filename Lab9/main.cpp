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

vector<Rect> prevFrameSides;

int main(int argc, char **argv) {
    int totalFaces = 0;
    CascadeClassifier side_cascade;
    VideoCapture cap1("../faces2.mp4");
    if (!cap1.isOpened()) {
        return -1;
    }
    Mat frame;

    if (!side_cascade.load("../haarcascade_frontalface_alt.xml")) {
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

        int faces = 0;
        for (size_t i = 0; i < detectedSides.size(); i++) {
            rectangle(frame, Point(detectedSides[i].x, detectedSides[i].y),
                      Point(detectedSides[i].x + detectedSides[i].width, detectedSides[i].y + detectedSides[i].height),
                      Scalar(0, 0, 255), 4);
            putText(frame, "Face nr: " + to_string(i + 1),
                    Point(detectedSides[i].x, detectedSides[i].y + detectedSides[i].height + 20), FONT_HERSHEY_SIMPLEX,
                    0.55, Scalar(0, 255, 255), 2);
            Rect face = Rect(Point(detectedSides[i].x, detectedSides[i].y),
                             Point(detectedSides[i].x + detectedSides[i].width,
                                   detectedSides[i].y + detectedSides[i].height));


            faces = 0;
            //Old faces
            for (auto &oldSide: prevFrameSides) {
                if (abs(face.x - oldSide.x) <= 250 &&
                    abs(face.y - oldSide.y) <= 250 &&
                    abs(face.width - oldSide.width) <= 250 &&
                    abs(face.height - oldSide.height) <= 250) {
                    faces++;
                }
                if (faces == 0) {
                    totalFaces++;
                }
            }


            prevFrameSides.clear();
            prevFrameSides.push_back(face);
        }
        putText(frame, "Total faces: " + to_string(totalFaces + 1),
                Point(10, 20), FONT_HERSHEY_SIMPLEX,
                0.69, Scalar(255, 0, 255), 2);

        imshow("frame", frame);

        int key_pressed = waitKey(15);
        if (key_pressed == 27) {
            break;
        }
    }
    return 0;
}



