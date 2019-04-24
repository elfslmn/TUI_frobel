#include "Common.h"

class Calibrator{
public:
	Calibrator();
	bool initCalibrationPattern(const string filepath);
	bool findHomography(Mat & image);
	bool applyHomography(vector<Point2f> & points);

	bool isHomographyFound;


private:
	const Size patternSize = Size(9,6); // TODO move to params
	vector<Point2f> pCorners;
	vector<Point2f> cCorners;
	Mat homography;
	const Size pSize = Size(1280,720); // TODO move to params
};
