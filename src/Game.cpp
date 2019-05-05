#include "Game.h"

Game::Game(){}

Game::Game(int level, Shape correct, string imagepath, Mat & projImage){
    this->level = level;
    this->correctShape = correctShape;

    projImage = imread(imagepath ,IMREAD_UNCHANGED);
    resize(projImage,projImage,Size(Params::projector_width ,Params::projector_height));
    LOGD("Game object initialized: Level: %d", level);
}

void processShapes(vector<Shape> & shapes){
    
}
