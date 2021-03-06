#pragma once
#include "Common.h"

class Params {
public:
    const static ColorMode color_space = RGB;
    const static GameMode game_mode = TANGRAM;

    const static int blue_thresh = 70;
    const static int green_thresh = 70;
    const static int red_thresh = 70;

    const static int hue_thresh = 70;

    const static int projector_width = 1280;
    const static int projector_height = 720;

    const static int pattern_width = 9;
    const static int pattern_height= 6;

    const static int minFrobelArea = 300;
    const static int maxFrobelArea = 3000;

    const static int minDefectDepth = 5;

    const static int levelCount = 4;

    //Shared pointer to ObjectParams instance
    /*typedef std::shared_ptr<Params> Ptr;

    //Instance of ObjectParams initialized with default parameters
    static Params::Ptr DEFAULT;

    // Create a new ObjectParams instance, wrapped in a smart pointer
    // and initialized with default parameter values
    static Params::Ptr create() {
        return std::make_shared<Params>();
    }*/
};
