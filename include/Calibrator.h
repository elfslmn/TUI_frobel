#include "Params.h"

class Calibrator{
public:
	Calibrator();
	bool initCalibrationPattern(const string filepath);
	bool findHomography(Mat & image);
	bool applyHomography(vector<Shape> & shapes);

	bool isHomographyFound;


private:
	const Size patternSize = Size(Params::pattern_width, Params::pattern_height);
	vector<Point2f> pCorners;
	vector<Point2f> cCorners;
	Mat homography;
	const Size pSize = Size(Params::projector_width, Params::projector_height);
};
