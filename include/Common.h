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

enum Mode { RGB , HSV };

enum FrobelType {CIRCLE, STICK, GREEN, ORANGE, RED};


/* Angle can be in range
  circle : -1
  stick : 0 - 180
  arc: 0 - 360 (green, orange, red) 
*/
struct Shape{
    FrobelType type;
    Point2f center;
    double angle;
    Shape(FrobelType t, Point2f c, double a) :  type(t), center(c), angle(a){}
};
