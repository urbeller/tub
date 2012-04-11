#ifndef __TUB_CONTROLS_H
#define __TUB_CONTROLS_H

#include "tub_enums.h"

namespace TUB{

    typedef struct _UVC_CTRL{
        unsigned int    bit;
        bool            have;
        unsigned int    unit_type;
        unsigned int    curr;
        unsigned int    min;
        unsigned int    max;
        char*           name;

    } uvc_ctrl;


    uvc_ctrl *controls_list={
        {0,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {1,false,VC_INPUT_TERMINAL,0,0,0,"Auto-E Mode"},
        {2,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {3,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {4,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {5,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {6,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {7,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {8,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {9,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {10,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {11,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {12,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {13,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {14,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {15,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {16,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {17,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {18,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {19,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {20,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {21,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {22,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},
        {23,false,VC_INPUT_TERMINAL,0,0,0,"Scan Mode"},



    };

};


#endif
