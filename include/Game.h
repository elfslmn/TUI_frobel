#include "Common.h"
#include "Params.h"

class Game{
public:
	Game();
    Game(int level, Shape correct, string imagepath, Mat & projImage);

	int processShapes(vector<Shape> & shapes);

    int level;
    Shape correctShape;


private:
	int feedbackCounter[5];

};
