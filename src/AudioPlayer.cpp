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

    for(int i=1; i<= Params::levelCount; i++){
        stories[i] = Mix_LoadMUS( ("sound/f"+to_string(i)+".mp3").c_str());
        location_fbs[i] = Mix_LoadMUS( ("sound/wrong_location"+to_string(i)+".mp3").c_str());
        if(!(stories[i] && location_fbs[i])){
            LOGE( "%s",Mix_GetError() );
        }
    }

    Mix_Music* temp = NULL;
    for(int i=1; i<=4; i++){
        temp = Mix_LoadMUS( ("sound/wrong_type"+to_string(i)+".mp3").c_str());
        if(temp == NULL){
            LOGE( "%s",Mix_GetError() );
        }
        else{
            type_fbs.push_back(temp);
        }
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
    Mix_Music* music = type_fbs.front();
    Mix_PlayMusic(music, 0 );

    // put recently played to the last
    type_fbs.pop_front();
    type_fbs.push_back(music);
    // NOTE feedback may have to given in order, ask elif
}

void AudioPlayer::playAngleFeedback(){
    Mix_Music* music = angle_fbs.front();
    Mix_PlayMusic(music, 0 );

    // put recently played to the last
    angle_fbs.pop_front();
    angle_fbs.push_back(music);
}
