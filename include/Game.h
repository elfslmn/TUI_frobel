#include "Common.h"
#include "Params.h"

class Game{
public:
	Game();
    Game(int level, Shape correct, string imagepath, Mat & projImage);

	void processShapes(vector<Shape> & shapes);

    int level;
    Shape correctShape;


private:

};
