#include "Util.h"
#include "Params.h"

class Detector{
public:
	Detector();
	void processFrame(Mat & frame);
	void resetSnapshot();
	bool isSnapshotCaptured;

	vector<Shape> shapes;

private:
    Mat snapshot;
    Mat diff;
    Mat mask;
	Mat debug;

	void extractShapes();
	void processFrobel(vector<vector<Point> > & contours);
	void processTangram(vector<vector<Point> > & contours);

};
