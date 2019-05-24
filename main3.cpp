#include "Detector.h"
#include "Calibrator.h"
#include "Game.h"
#include "AudioPlayer.h"
#include <cxxopts.hpp>
#include <SDL2/SDL.h>
#include <SDL_mixer.h>

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
    ("c", "Camera", cxxopts::value<int>(), "camera");
    cout << options.help() << endl;
    cout << "Press 'ESC' for exit" << endl << endl;
    auto result = options.parse(argc, argv);

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
            giveFeedback(feedback);
            if(feedback == 0){
                game_state = LEVEL_FINISHED;
                LOGD("Game state: LEVEL_FINISHED");
            }
		}
        else if(game_state == LEVEL_FINISHED){
            //LOGD("Level %d is cleared", game.level);
            playConfettiAnimation(true);
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
        if (key == 'q' || key == 27) {break;}
        if(key == 83 && game_state == PICKUP_SHAPES){
            if(game.level == Params::levelCount){
                LOGD("All levels are cleared");
                // TODO game finished audio, show results
                break;
            }
            else{
                level++;
                detector.resetSnapshot();
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
            break;

            case 2:
            shape.center = Point2f(780 , 250);
            shape.type = STICK;
            shape.angle = 103.0;
            filepath = "images/episode2.png";
            break;

            case 3:
            shape.center = Point2f(812, 530);
            shape.type = CIRCLE;
            shape.angle = -1;
            filepath = "images/episode3.png";
            break;

            case 4:
            shape.center = Point2f(450, 590);
            shape.type = GREEN;
            shape.angle = 100.0;
            filepath = "images/episode4.png";
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
            break;

            case 2:
            shape.center = Point2f(530 , 271);
            shape.type = PARALLEL;
            shape.angle = 173.0;
            filepath = "images/tangram2.png";
            break;

            case 3:
            shape.center = Point2f(861, 461);
            shape.type = SQUARE;
            shape.angle = 45.0;
            filepath = "images/tangram3.png";
            break;

            case 4:
            shape.center = Point2f(335, 428);
            shape.type = GREEN;
            shape.angle = 350.0;
            filepath = "images/tangram4.png";
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
            if(frameTimer <= 0){ // FIXME not a good way to handle feedback time
                audio.playAngleFeedback();
                frameTimer = 3; // 10 sec
            }
            else{
                frameTimer--;
            }
            LOGI("location and type correct, angle wrong");
        break;
        case 2:
            if(frameTimer <= 0){
                audio.playTypeFeedback();
                frameTimer = 3; // 10 sec
            }
            else{
                frameTimer--;
            }
            LOGI("location correct, type wrong");
        break;
        case 3:
            if(frameTimer <= 0){
                audio.playLocationFeedback(game.level);
                frameTimer = 3; // 10 sec
            }
            else{
                frameTimer--;
            }
            LOGI("location wrong");
        break;

        case 4:
            if(frameTimer <= 0){
                audio.playNoObjectFeedback(game.level);
                frameTimer = 3; // 10 sec
            }
            else{
                frameTimer--;
            }
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
