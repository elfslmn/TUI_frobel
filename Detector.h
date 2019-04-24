#include "Common.h"
#include "Util.h"

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
	enum Mode { RGB , HSV };

	Mode colorMode = HSV; // TODO params a tası

	void extractShapes();
};
