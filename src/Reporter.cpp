#include "Reporter.h"

Reporter::Reporter(){}

Reporter::Reporter(string name, int age){
    // Child credentials
    stream << ("Name: " + name + "\t Age: " + to_string(age) + "\n");
    filepath = "reports/" + name;

    start_game = time(nullptr);
    strftime(log, 20, "%H-%M-%S", localtime(&start_game));

    // Game mode
    if(Params::game_mode == FROBEL){
        stream << "FROBEL REPORT\n";
        filepath += "_frobel_"+ string(log) +".txt";
    }
    else if(Params::game_mode == TANGRAM){
        stream << "TANGRAM REPORT\n";
        filepath += "_tangram_"+ string(log) +".txt";
    }
    cout << "Filepath: " << filepath << endl;
    // Start time
    stream << "Date: " << asctime(localtime(&start_game)) << "\n";
    stream << "-----------------------------------------------\n";
}

void Reporter::addLevelStartingLog(int level){
    start_level = time(nullptr);
    stream << "Level " << level << endl;

    current = time(nullptr) - start_level;
    strftime(log, 20, "%X-", localtime(&current));
    stream << log << "Level started.\n";
}

void Reporter::addLevelEndingLog(int level){
    current = time(nullptr) - start_level;
    strftime(log, 10, "%X-", localtime(&current));
    stream << log << "Level ended.\n";
    stream << "-----------------------------------------------\n";
}

void Reporter::addFeedbackLog(int feedback){
    current = time(nullptr) - start_level;
    strftime(log, 10, "%X-", localtime(&current));
    stream << log;
    switch (feedback) {
        case 1:
            stream << "Orientation feedback is given\n";
        break;
        case 2:
            stream << "Shape feedback is given\n";
        break;
        case 3:
            stream << "Location feedback is given\n";
        break;
        case 4:
            stream << "No object feedback is given\n";
        break;
    }
}

bool Reporter::saveReport(bool finished){
    ofstream file;
    file.open (filepath);
    if (!file.is_open()){
        LOGE("File %s cannot be opened.", filepath);
        return false;
    }
    double diff = difftime(time(nullptr) , start_game);
    if(finished) stream << "Game is finished in " << diff << " seconds.\n" ;
    else stream << "Quit after " << diff << " seconds.\n" ;

    file << stream.rdbuf();
    file.close();
    LOGI("File %s saved.", filepath.c_str());
    return true;
}
