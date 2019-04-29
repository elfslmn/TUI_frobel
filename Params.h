#pragma once
#include "Common.h"

class Params {
public:
    const static Mode color_space = RGB;

    const static int blue_thresh = 60;
    const static int green_thresh = 70;
    const static int red_thresh = 70;

    const static int hue_thresh = 70;

    const static int projector_width = 1280;
    const static int projector_height = 720;

    const static int pattern_width = 9;
    const static int pattern_height= 6;



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
