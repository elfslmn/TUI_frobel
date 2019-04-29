#include "Common.h"
#include "Util.h"
#include "Params.h"

class Detector{
public:
	Detector();
	bool processFrame(Mat & frame);
	void resetSnapshot();

	vector<Point2f> centers; // TODO should be vector<Shape>

private:
    Mat snapshot;
    Mat diff;
    Mat mask;
	Mat debug;
	bool isSnapshotCaptured;

	Mode colorMode = Params::color_space;

	void extractShapes();
};
