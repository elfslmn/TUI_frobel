#include "Detector.h"
#include "Calibrator.h"
#include "Game.h"
#include "AudioPlayer.h"
#include "Reporter.h"
#include <cxxopts.hpp>

using namespace std;
using namespace cv;

//Global variables
int frameTimer;
GameState game_state; // NOTE can be a part of game class
Game game;
AudioPlayer audio;
VideoCapture gif;
Mat animation;
int level = 1;

string childName = "Child";
int childAge = 0;
Reporter reporter;

//Helper methods;
pair<Shape, string> getLevelParameters(int level);
void giveFeedback(int feedback_id);
void playConfettiAnimation(bool loop);

int main (int argc, char *argv[]){
    // Parse options
    cxxopts::Options options("TUI Frobel");
    options
    .allow_unrecognised_options()
    .add_options()
    ("c", "Camera", cxxopts::value<int>(), "camera")
    ("n", "Name", cxxopts::value<string>(), "name and surname")
    ("a", "Age", cxxopts::value<int>(), "age");
    cout << options.help() << endl;
    cout << "Press 'ESC' for exit" << endl << endl;
    auto result = options.parse(argc, argv);

    if (result.count("n")) childName = result["n"].as<std::string>();
    if (result.count("a")) childAge = result["a"].as<int>();
    cout << "Child: " << childName << "\t Age:" << childAge << endl;
    reporter = Reporter(childName, childAge);

    // windows
    namedWindow ("Projector", WINDOW_NORMAL);
    namedWindow ("Camera", WINDOW_NORMAL);
    namedWindow ("Snapshot", WINDOW_NORMAL);
    namedWindow ("Debug", WINDOW_NORMAL);
    namedWindow ("Diff", WINDOW_NORMAL);
    setWindowProperty("Projector", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    moveWindow("Projector", 1366, 0); // 1366 width of my pc screen

    //initialization
    Calibrator calibrator;
    calibrator.initCalibrationPattern("images/pattern.png");
    game_state = CALIBRATING; // TODO should be  CALIBRATING, NEW_GAME for debug
    LOGD("Game state: CALIBRATING");

    Detector detector;

    // open camera
    int camId = 0;
    if(result.count("c")){
        camId = result["c"].as<int>();
    }
    cout << "Cam id = " << camId << endl;
    VideoCapture cap(camId);
    Mat frame, projImage;

    while (true)
    {
		// Read a frame from cam
		cap >> frame;
		if(frame.empty()){
			cerr << "No camera capture" << endl;
			break;
		}
		flip(frame, frame, -1); // flip 180 degree

        if(game_state == CALIBRATING){
            calibrator.findHomography(frame);
            if(calibrator.isHomographyFound == true){
                game_state = NEW_GAME;
                LOGD("Game state: NEW_GAME");
            }
        }
		else if(game_state == NEW_GAME){
			auto param = getLevelParameters(level);
			game = Game(level,param.first, param.second, projImage);
		    imshow("Projector", projImage);

			frameTimer = 30; // wait few frames, important to capture snapshot
			game_state = WAIT_SNAPSHOT;
			LOGD("Game state: WAIT_SNAPSHOT");

            audio.playStory(game.level);
            reporter.addLevelStartingLog(game.level);
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
		else if(game_state == IN_LEVEL && Mix_PlayingMusic() == 0 ){
			detector.processFrame(frame);
            calibrator.applyHomography(detector.shapes);

            int feedback = game.processShapes(detector.shapes);
            if(feedback  != -1){
                giveFeedback(feedback);
            }
            if(feedback == 0){
                game_state = LEVEL_FINISHED;
                LOGD("Game state: LEVEL_FINISHED");
                reporter.addLevelEndingLog(game.level);
            }
		}
        else if(game_state == LEVEL_FINISHED){
            //LOGD("Level %d is cleared", game.level);
            //playConfettiAnimation(true);
            imshow("Projector", animation);
            if(Mix_PlayingMusic() == 0){
                game_state = PICKUP_SHAPES;
                LOGD("Game state: PICKUP_SHAPES");
            }
        }
        else if(game_state == PICKUP_SHAPES){
            // TODO tasları al audio
        }

        imshow("Camera", frame);
        // Handle key presses
        char key = (char) waitKey(30);
        if(key == 'p') {key = (char) waitKey();}
        if (key == 'q' || key == 27) {
            reporter.saveReport(false);
            break;
        }
        if(key == 83 && game_state == PICKUP_SHAPES){
            if(game.level == Params::levelCount){
                LOGD("All levels are cleared");
                reporter.saveReport(true);
                // TODO game finished audio, show results
                break;
            }
            else{
                level++;
                detector.resetSnapshot();
                audio.resetFeedbackIndices();
                frameTimer = 3;
                game_state = NEW_GAME;
                LOGD("Game state: NEW_GAME");
            }
        }
    }


    return 0;
}


pair<Shape, string> getLevelParameters(int level){
    Shape shape;
    string filepath;
    if(Params::game_mode == FROBEL){
        switch (level) {
            case 1:
            shape.center = Point2f(695, 468);
            shape.type = GREEN;
            shape.angle = 70.0;
            filepath = "images/episode1.png";
            animation = imread("images/episode1f.png");
            break;

            case 2:
            shape.center = Point2f(780 , 250);
            shape.type = STICK;
            shape.angle = 103.0;
            filepath = "images/episode2.png";
            animation = imread("images/episode2f.png");
            break;

            case 3:
            shape.center = Point2f(812, 530);
            shape.type = CIRCLE;
            shape.angle = -1;
            filepath = "images/episode3.png";
            animation = imread("images/episode3f.png");
            break;

            case 4:
            shape.center = Point2f(450, 590);
            shape.type = GREEN;
            shape.angle = 100.0;
            filepath = "images/episode4.png";
            animation = imread("images/episode4f.png");
            break;
        }
    }
    else if(Params::game_mode == TANGRAM){
        switch (level) {
            case 1:
            shape.center = Point2f(621, 425);
            shape.type = GREEN;
            shape.angle = 102.0;
            filepath = "images/tangram1.png";
            animation = imread("images/tangram1f.png");
            break;

            case 2:
            shape.center = Point2f(530 , 271);
            shape.type = PARALLEL;
            shape.angle = 173.0;
            filepath = "images/tangram2.png";
            animation = imread("images/tangram2f.png");
            break;

            case 3:
            shape.center = Point2f(861, 461);
            shape.type = SQUARE;
            shape.angle = 45.0;
            filepath = "images/tangram3.png";
            animation = imread("images/tangram3f.png");
            break;

            case 4:
            shape.center = Point2f(335, 428);
            shape.type = GREEN;
            shape.angle = 350.0;
            filepath = "images/tangram4.png";
            animation = imread("images/tangram4f.png");
            break;
        }
    }
    else{
        LOGE("Undefined game mode");
        assert(false);
    }

    return {shape, filepath};
}

void giveFeedback(int feedback_id){
    switch (feedback_id) {
        case 0:
            audio.playCorrectSound();
            audio.playCongratulations();
        break;
        case 1:
            audio.playAngleFeedback();
            reporter.addFeedbackLog(1);
            LOGI("location and type correct, angle wrong");
        break;
        case 2:
            audio.playTypeFeedback();
            reporter.addFeedbackLog(2);
            LOGI("location correct, type wrong");
        break;
        case 3:
            audio.playLocationFeedback(game.level);
            reporter.addFeedbackLog(3);
            LOGI("location wrong");
        break;

        case 4:
            audio.playNoObjectFeedback(game.level);
            reporter.addFeedbackLog(4);
            LOGI("no object");
        break;
    }
}

void playConfettiAnimation(bool loop){
    gif >> animation;
    if(!animation.empty()){
        imshow("Projector", animation);
    }
    else if(loop){
        gif = VideoCapture("gifs/confetti.gif");
    }
}
