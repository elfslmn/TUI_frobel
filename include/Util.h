#pragma once
#include "Common.h"

class Util{ // TODO define as namespace not class
public:
	static void drawText(Mat & image, string label, Point pt);
	static void drawRotetedRectAxis(Mat & image, RotatedRect rr);
};
