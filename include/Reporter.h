#include "Params.h"

class Reporter{
public:
    Reporter();
    Reporter(string name, int age);

    void addLevelStartingLog(int level);
    void addLevelEndingLog(int level);
    void addFeedbackLog(int feedback);
    bool saveReport(bool finished);

private:
    time_t start_game;
    time_t start_level;
    time_t current;
    char* log = new char[20];
    stringstream stream;
    string filepath;
};
