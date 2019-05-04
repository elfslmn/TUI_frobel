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

	void extractShapes();
	void processFrobel(vector<vector<Point> > & contours);
	void processTangram(vector<vector<Point> > & contours);

};
