#include "Detector.h"
#include "Calibrator.h"
#include "Game.h"
#include <cxxopts.hpp>

using namespace std;
using namespace cv;

//Global variables
static int frameTimer;
static GameState game_state; // NOTE can be a part of game class

int main (int argc, char *argv[]){
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
    if(result.count("c")){
        camId = result["c"].as<int>();
    }
    cout << "Cam id = " << camId << endl;
    VideoCapture cap(camId);
    Mat frame, projImage;

    // Calculate homography -----------------------------------------
    Calibrator calibrator;
    calibrator.initCalibrationPattern("images/pattern.png");
	game_state = CALIBRATING;
	LOGD("Game state: CALIBRATING");
    /*while(calibrator.isHomographyFound == false){
		// Read a frame from cam
		cap >> frame;
		if(frame.empty()){
			cerr << "No camera capture" << endl;
			break;
		}
		flip(frame, frame, -1); // flip 180 degree
        calibrator.findHomography(frame);
        imshow("Camera", frame);

        if (waitKey(10) == 27)  return 0;
    }*/
	game_state = CALIBRATED;
	LOGD("Game state: CALIBRATED");
    //cap.release();
    cap.open(camId);

    // Start levels -------------------------------------------------
    Detector detector;
	Game game;
    while (true)
    {
		// Read a frame from cam
		cap >> frame;
		if(frame.empty()){
			cerr << "No camera capture" << endl;
			break;
		}
		flip(frame, frame, -1); // flip 180 degree

		if(game_state == CALIBRATED){
			Shape dummy;
			game = Game(1,dummy,"images/espas_011-intro.png", projImage);
		    imshow("Projector", projImage);

			frameTimer = 30; // wait few frames, important to capture snapshot
			game_state = WAIT_SNAPSHOT;
			LOGD("Game state: WAIT_SNAPSHOT");
		}
		else if(game_state == WAIT_SNAPSHOT){
			frameTimer--;
			if(frameTimer == 0){
				game_state = CAPTURING_SNAPSHOT;
				LOGD("Game state: CAPTURING_SNAPSHOT");
			}
		}
		else if(game_state == CAPTURING_SNAPSHOT){
			detector.processFrame(frame);
			if(detector.isSnapshotCaptured == true){
				game_state = IN_LEVEL;
				LOGD("Game state: IN_LEVEL");
			}
		}
		else if(game_state == IN_LEVEL){
			detector.processFrame(frame);
			//TODO get shapes and process in game level

			// for debug
	        /*calibrator.applyHomography(detector.centers);
	        for(Point2f p : detector.centers){
	            circle(projImage, p, 10, Scalar(255,255,255), 3);
	            imshow("Projector", projImage);
	        }*/
	        // end debug
		}




        imshow("Camera", frame);

		// Handle key presses
        char key = (char) waitKey(30);
        if(key == 'p') {key = (char) waitKey();}
        if (key == 'q' || key == 27) {break;}
    }


    return 0;
}
