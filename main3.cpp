#include "Common.h"
#include "Detector.h"
#include "Calibrator.h"
#include <cxxopts.hpp>

using namespace std;
using namespace cv;

void updateBrightness(Mat & image, int beta){
	for( int y = 0; y < image.rows; y++ ) {
		for( int x = 0; x < image.cols; x++ ) {
			for( int c = 0; c < image.channels(); c++ ) {
				image.at<Vec3b>(y,x)[c] =
				  saturate_cast<uchar>( image.at<Vec3b>(y,x)[c] + beta );
			}
		}
	}
}

int main (int argc, char *argv[])
{
    // Parse options
    cxxopts::Options options("TUI Frobel");
    options
    .allow_unrecognised_options()
    .add_options()
    ("c", "Camera", cxxopts::value<int>(), "camera");
    cout << options.help() << endl;
    cout << "Press 'ESC' for exit" << endl << endl;
    auto result = options.parse(argc, argv);

    // windows
    namedWindow ("Projector", WINDOW_NORMAL);
    namedWindow ("Camera", WINDOW_NORMAL);
    //setWindowProperty("Projector", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    moveWindow("Projector", 1366, 0); // 1366 width of my pc screen

    // open camera
    int camId = 0;
    if(result.count("c"))
    {
        camId = result["c"].as<int>();
    }
    cout << "Cam id = " << camId << endl;
    VideoCapture cap(camId);
    Mat frame;

    // Calculate homography -----------------------------------------
    Calibrator calibrator;
    calibrator.initCalibrationPattern("images/pattern.png");
    while(calibrator.isHomographyFound == false){
        cap >> frame;
        if(frame.empty()){
            cerr << "No camera capture" << endl;
            return 0;
        }
        flip(frame, frame, -1); // flip 180 degree
        calibrator.findHomography(frame);
        imshow("Camera", frame);
        
        if (waitKey(10) == 27)  return 0;

    }
    //cap.release();
    cap.open(camId);

    // Start levels -------------------------------------------------
    Detector detector;
    // project level image
    Mat projImage = imread("images/espas_011-intro.png" ,IMREAD_UNCHANGED); // TODO save in a mat
    resize(projImage,projImage,Size(1280,720)); // TODO move to params
    //updateBrightness(projImage, 50);
    imshow("Projector", projImage);
    waitKey(500); // important to capture

    while (true)
    {
        cap >> frame;
        if(frame.empty()){
            cerr << "No camera capture" << endl;
            break;
        }
        flip(frame, frame, -1); // flip 180 degree
        bool res = detector.processFrame(frame);
        // TODO for debug
        calibrator.applyHomography(detector.centers);
        for(Point2f p : detector.centers){
            circle(projImage, p, 10, Scalar(255,255,255), 3);
            imshow("Projector", projImage);
        }
        // end debug

        imshow("Camera", frame);

        char key = (char) waitKey(30);
        if(key == 'p')
        {
            key = (char) waitKey();
        }
        if (key == 'q' || key == 27)
        {
            break;
        }

    }


    return 0;
}
