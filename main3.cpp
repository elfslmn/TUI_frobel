#include "Detector.h"
#include "Calibrator.h"
#include "Game.h"
#include <cxxopts.hpp>
#include <SDL2/SDL.h>
#include <SDL_mixer.h>

using namespace std;
using namespace cv;

//Helper methods;
pair<Shape, string> getLevelParameters(int level);
bool initSDL();
bool loadSounds();
void closeSDL();

//Global variables
static int frameTimer;
static GameState game_state; // NOTE can be a part of game class
//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
// NOTE playChannel only works with wav files
Mix_Chunk *gCorrect= NULL;
Mix_Chunk *gWrong = NULL;


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
    setWindowProperty("Projector", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
    moveWindow("Projector", 1366, 0); // 1366 width of my pc screen

    //initialization
    Calibrator calibrator;
    calibrator.initCalibrationPattern("images/pattern.png");
    game_state = CALIBRATING; // TODO should be  CALIBRATING, NEW_GAME for debug
    LOGD("Game state: CALIBRATING");
    Detector detector;
    Game game;
    int level = 1;
    if(initSDL() == false){
        LOGE("Failed to initialize SDL!" );
        return 0;
    }
    if(loadSounds() == false){
        LOGE("Failed to load sounds!" );
        return 0;
    }


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

            Mix_PlayMusic( gMusic, 0 );
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
            //for debug
	        /*for(Shape s : detector.shapes){
	            circle(projImage, s.center, 10, Scalar(255,255,255), 3);
	            imshow("Projector", projImage);
	        }*/
	        // end debug

            //TODO should return, feedback or finished etc
            int res = game.processShapes(detector.shapes);
            if(res == 1){
                Mix_PlayChannel( -1, gCorrect, 0 );
                game_state = LEVEL_FINISHED;
                LOGD("Game state: LEVEL_FINISHED");
            }
            else{
                // TODO feedack audios
            }
		}
        else if(game_state == LEVEL_FINISHED && Mix_Playing(-1) == 0){
            //TODO tebrikler audio
            LOGD("Level %d is cleared", game.level);
            game_state = PICKUP_SHAPES;
            LOGD("Game state: PICKUP_SHAPES");
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

bool initSDL()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init(SDL_INIT_AUDIO ) < 0 ){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else{
		//Initialize SDL_mixer
		if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
			printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
			success = false;
		}
	}

	return success;
}

bool loadSounds()
{
	//Loading success flag
	bool success = true;
	//Load sound effects
	gCorrect = Mix_LoadWAV( "sound/success.wav" );
	if( gCorrect == NULL )
	{
		printf( "Failed to load correct sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gWrong = Mix_LoadWAV( "sound/error.wav" );
	if( gWrong == NULL )
	{
		printf( "Failed to load wrong sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	return success;
}

void closeSDL()
{
	//Free the sound effects
	Mix_FreeChunk( gCorrect );
	Mix_FreeChunk( gWrong );
	gCorrect = NULL;
	gWrong = NULL;

	//Free the music
	Mix_FreeMusic( gMusic );
	gMusic = NULL;
	//Quit SDL subsystems
	Mix_Quit();
	SDL_Quit();
}


pair<Shape, string> getLevelParameters(int level){
    Shape shape;
    string filepath;
    switch (level) {
        case 1:
        shape.center = Point2f(0.537 * Params::projector_width , 0.723 * Params::projector_height );
        shape.type = GREEN;
        shape.angle = 70.0;
        filepath = "images/espas_011-intro.png";
        //Load music
        gMusic = Mix_LoadMUS( "sound/f1.mp3" ); // TODO make it generic for other levels
        if( gMusic == NULL )
        {
            printf( "Failed to load sound/f1.mp3! SDL_mixer Error: %s\n", Mix_GetError() );
        }
        break;
        case 2:
        shape.center = Point2f(0.585 * Params::projector_width , 0.519 * Params::projector_height );
        shape.type = STICK;
        shape.angle = 103.0;
        filepath = "images/espas_021-intro.png";
        gMusic = Mix_LoadMUS( "sound/f2.mp3" ); // TODO make it generic for other levels
        if( gMusic == NULL )
        {
            printf( "Failed to load sound/f1.mp3! SDL_mixer Error: %s\n", Mix_GetError() );
        }
        break;
        case 3:
        shape.center = Point2f(0.622 * Params::projector_width , 0.869 * Params::projector_height );
        shape.type = CIRCLE;
        shape.angle = -1;
        filepath = "images/espas_031-intro.png";
        gMusic = Mix_LoadMUS( "sound/f3.mp3" ); // TODO make it generic for other levels
        if( gMusic == NULL )
        {
            printf( "Failed to load sound/f1.mp3! SDL_mixer Error: %s\n", Mix_GetError() );
        }
        break;
        case 4:
        shape.center = Point2f(0.376 * Params::projector_width , 0.846 * Params::projector_height );
        shape.type = GREEN;
        shape.angle = 100.0;
        filepath = "images/espas_041-intro.png";
        gMusic = Mix_LoadMUS( "sound/f4.mp3" ); // TODO make it generic for other levels
        if( gMusic == NULL )
        {
            printf( "Failed to load sound/f1.mp3! SDL_mixer Error: %s\n", Mix_GetError() );
        }
        break;
    }
    return {shape, filepath};
}
