#include "Common.h"
#include "Params.h"
#include <SDL2/SDL.h>
#include <SDL_mixer.h>

class AudioPlayer{
public:
	AudioPlayer();
    ~AudioPlayer();

    void playStory(int level);
    void playLocationFeedback(int level);
    void playTypeFeedback();
    void playAngleFeedback();
	void playNoObjectFeedback(int level);
    void playCongratulations();
    void playCorrectSound();
    void playWrongSound();
	void resetFeedbackIndices();

private:
    void initSDL();
    void loadSounds();
    void closeSDL();
	// 1 indexed, 0 empty, level dependent audios
    vector<Mix_Music*> stories = vector<Mix_Music*>(Params::levelCount + 1, NULL);
    vector<Mix_Music*> location_fbs = vector<Mix_Music*>(Params::levelCount + 1, NULL);
	vector<vector<Mix_Music*>> no_object_fbs = vector<vector<Mix_Music*>>(Params::levelCount + 1);

	// level independent audios
    vector<Mix_Music*> type_fbs;
    vector<Mix_Music*> angle_fbs;

    Mix_Music* congratulations = NULL;
    Mix_Chunk *correctSound = NULL;  // NOTE chunk only works with wav files
    Mix_Chunk *wrongSound = NULL;

	int no_object_idx = 0;
	int type_idx = 0;
	int angle_idx = 0;

};
