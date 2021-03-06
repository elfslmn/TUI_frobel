#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(){
    initSDL();
    loadSounds();
}

AudioPlayer::~AudioPlayer(){
    closeSDL();
}

void AudioPlayer::initSDL(){
    //Initialize SDL
    if( SDL_Init(SDL_INIT_AUDIO ) < 0 ){
        LOGE( "SDL could not initialize! SDL Error: %s", SDL_GetError() );
        return;
    }
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
        LOGE( "SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError() );
    }
}

void AudioPlayer::loadSounds(){
    correctSound = Mix_LoadWAV( "sound/success.wav" );
    if( correctSound == NULL ){
        LOGE( "s", Mix_GetError() );
    }

    wrongSound = Mix_LoadWAV( "sound/error.wav" );
    if( wrongSound == NULL ){
        LOGE( "%s", Mix_GetError() );
    }

    congratulations = Mix_LoadMUS( "sound/tebrik.mp3" ); // TODO make it generic for other levels
    if( congratulations == NULL ){
        LOGE( "%s", Mix_GetError() );
    }

    Mix_Music* temp = NULL;

    if(Params::game_mode == FROBEL){
        // level specific sounds
        for(int i=1; i<= Params::levelCount; i++){
            stories[i] = Mix_LoadMUS( ("sound/f"+to_string(i)+".mp3").c_str());
            if(!stories[i]) LOGE( "%s",Mix_GetError() );

            location_fbs[i] = Mix_LoadMUS( ("sound/wrong_location"+to_string(i)+".mp3").c_str());
            if(!location_fbs[i]) LOGE( "%s",Mix_GetError() );

            for(int j=1; j<=4; j++){
                temp = Mix_LoadMUS( ("sound/no_object_"+to_string(i)+"_"+to_string(j)+".mp3").c_str());
                if(temp) no_object_fbs[i].push_back(temp);
                else LOGE( "%s",Mix_GetError() );
            }
        }
    }
    else if(Params::game_mode == TANGRAM){
        for(int i=1; i<= Params::levelCount; i++){
            stories[i] = Mix_LoadMUS( ("sound/T"+to_string(i)+".mp3").c_str());
            if(!stories[i]) LOGE( "%s",Mix_GetError() );

            location_fbs[i] = Mix_LoadMUS( ("sound/t_wrong_location"+to_string(i)+".mp3").c_str());
            if(!location_fbs[i]) LOGE( "%s",Mix_GetError() );

            for(int j=1; j<=4; j++){
                temp = Mix_LoadMUS( ("sound/t_no_object_"+to_string(i)+"_"+to_string(j)+".mp3").c_str());
                if(temp) no_object_fbs[i].push_back(temp);
                else LOGE( "%s",Mix_GetError() );
            }
        }
    }
    else{
        LOGE("Undefinded game mode");
        assert(false);
    }


    for(int i=1; i<=4; i++){
        temp = Mix_LoadMUS( ("sound/wrong_angle"+to_string(i)+".mp3").c_str());
        if(temp == NULL){
            LOGE( "%s",Mix_GetError() );
        }
        else{
            angle_fbs.push_back(temp);
        }
    }

    for(int i=1; i<=4; i++){
        temp = Mix_LoadMUS( ("sound/wrong_type"+to_string(i)+".mp3").c_str());
        if(temp == NULL){
            LOGE( "%s",Mix_GetError() );
        }
        else{
            type_fbs.push_back(temp);
        }
    }

}

void AudioPlayer::closeSDL(){
    Mix_FreeChunk( correctSound );
    Mix_FreeChunk( wrongSound );

    //Free the music
    Mix_FreeMusic( congratulations );
    for(int i=1; i<= Params::levelCount; i++){
        if(stories[i]) Mix_FreeMusic(stories[i]);
        if(location_fbs[i]) Mix_FreeMusic(location_fbs[i]);
    }

    for(auto music : type_fbs){
         Mix_FreeMusic(music);
    }

    for(auto music : angle_fbs){
         Mix_FreeMusic(music);
    }

    //Quit SDL subsystems
    Mix_Quit();
    SDL_Quit();
    LOGD("Audio player closed");
}

void AudioPlayer::playStory(int level){
    if(stories[level] != NULL){
        Mix_PlayMusic( stories[level], 0 );
    }
    else{
        LOGE("No story audio for level %d", level);
    }
}

void AudioPlayer::playLocationFeedback(int level){
    if(location_fbs[level] != NULL){
        Mix_PlayMusic( location_fbs[level], 0 );
    }
    else{
        LOGE("No slocation feedback for level %d", level);
    }
}

void AudioPlayer::playCorrectSound(){
    if(correctSound != NULL){
        Mix_PlayChannel( -1, correctSound, 0 );
    }
    else{
        LOGE("No correct audio file loaded");
    }
}

void AudioPlayer::playWrongSound(){
    if(wrongSound != NULL){
        Mix_PlayChannel( -1, wrongSound, 0 );
    }
    else{
        LOGE("No wrong audio file loaded");
    }
}

void AudioPlayer::playCongratulations(){
    if(congratulations != NULL){
        Mix_PlayMusic(congratulations, 0 );
    }
    else{
        LOGE("No wrong audio file loaded");
    }
}

void AudioPlayer::playTypeFeedback(){
    if(type_idx >= type_fbs.size()) return;
    Mix_Music* music = type_fbs[type_idx];
    Mix_PlayMusic(music, 0 );
    type_idx ++;
}

void AudioPlayer::playAngleFeedback(){
    if(angle_idx >= angle_fbs.size()) return;
    Mix_Music* music = angle_fbs[angle_idx];
    Mix_PlayMusic(music, 0 );
    angle_idx ++;
}

void AudioPlayer::playNoObjectFeedback(int level){
    if(no_object_idx >= no_object_fbs[level].size()) return;
    Mix_Music* music = no_object_fbs[level][no_object_idx];
    Mix_PlayMusic(music, 0 );
    no_object_idx ++;
}

void AudioPlayer::resetFeedbackIndices(){
    no_object_idx = 0;
    type_idx = 0;
    angle_idx = 0;
}
