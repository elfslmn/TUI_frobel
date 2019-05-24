#include "Game.h"

Game::Game(){}

Game::Game(int level, Shape correctShape, string imagepath, Mat & projImage){
    this->level = level;
    this->correctShape = correctShape;

    projImage = imread(imagepath ,IMREAD_UNCHANGED);
    resize(projImage,projImage,Size(Params::projector_width ,Params::projector_height));

    memset(feedbackCounter, 0, sizeof(feedbackCounter));
    LOGD("Game object initialized: Level: %d", level);
}

int Game::processShapes(vector<Shape> & shapes){
    int feedback = 4; // no object
    for(Shape shape : shapes){
        // Discard shapes that outside the projector view
        if( shape.center.x < 0 ||
            shape.center.x > Params::projector_width ||
            shape.center.y < 0 ||
            shape.center.y > Params::projector_height){ continue; }

        if(norm(shape.center - correctShape.center) < 50){
            if(shape.type == correctShape.type ){
                //if((((int)correctShape.angle - 20)%360) < shape.angle &&
                //                     shape.angle < (((int)correctShape.angle + 20)%360)){
                if(abs(((int)(shape.angle - correctShape.angle)) % 360 ) < 20){
                    feedback = 0;
                    break;
                }
                else{ // location and type correct, angle wrong
                    feedback = min(feedback, 1);
                }
            }
            else{ // location correct, type wrong
                feedback = min(feedback, 2);
            }
        }
        else{ // location wrong
            feedback = min(feedback, 3);
        }
    }
    feedbackCounter[feedback]++;
    for(int i=0; i<5; i++){
        if(feedbackCounter[i] >= 10*(i+1)){ // onemsiz feedbacki daha gec ver
            LOGD("Feedback %d is sent",i);
            memset(feedbackCounter, 0, sizeof(feedbackCounter));
            return i;
        }
    }
    return -1;
}
