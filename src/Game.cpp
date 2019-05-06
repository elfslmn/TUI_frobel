#include "Game.h"

Game::Game(){}

Game::Game(int level, Shape correctShape, string imagepath, Mat & projImage){
    this->level = level;
    this->correctShape = correctShape;

    projImage = imread(imagepath ,IMREAD_UNCHANGED);
    resize(projImage,projImage,Size(Params::projector_width ,Params::projector_height));
    LOGD("Game object initialized: Level: %d", level);
}

int Game::processShapes(vector<Shape> & shapes){
    for(Shape shape : shapes){
        // Discard shapes that outside the projector view
        if( shape.center.x < 0 ||
            shape.center.x > Params::projector_width ||
            shape.center.y < 0 ||
            shape.center.y > Params::projector_height){ continue; }

        if( shape.type == correctShape.type &&
            norm(shape.center - correctShape.center) < 50 &&
            abs(((int)(shape.angle - correctShape.angle)) % 360 ) < 20 ){ return 1;}

    }
    return false;
}
