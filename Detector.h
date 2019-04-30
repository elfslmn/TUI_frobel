#include "Common.h"
#include "Util.h"
#include "Params.h"

class Detector{
public:
	Detector();
	bool processFrame(Mat & frame);
	void resetSnapshot();

	vector<Shape> shapes;

private:
    Mat snapshot;
    Mat diff;
    Mat mask;
	Mat debug;
	bool isSnapshotCaptured;

	Mode colorMode = Params::color_space;

	void extractShapes();
};
