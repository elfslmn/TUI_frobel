#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//#define DEBUG

#define LOGI(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)
#define LOGD(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)
#define LOGE(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)

#define PI 3.14159265

enum ColorMode { RGB , HSV };
enum GameMode { FROBEL, TANGRAM};

enum ShapeType {CIRCLE, STICK, GREEN, ORANGE, RED, SQUARE, PARALLEL};

/* Angle can be in range
For Frobel types
  circle : -1
  stick : 0 - 180
  arc: 0 - 360 (green, orange, red)
*/
struct Shape{
    ShapeType type;
    Point2f center;
    double angle;
    Shape(){};
    Shape(ShapeType t, Point2f c, double a) :  type(t), center(c), angle(a){}
};

enum GameState {
    CALIBRATING,
    NEW_GAME,
    WAIT_SNAPSHOT,
    CAPTURING_SNAPSHOT,
    IN_LEVEL,
    LEVEL_FINISHED,
    PICKUP_SHAPES
};
