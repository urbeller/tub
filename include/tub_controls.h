#ifndef __TUB_CONTROLS_H
#define __TUB_CONTROLS_H

#include "tub_enums.h"

namespace TUB{

    typedef struct _UVC_CTRL{
        unsigned int    bit;
        bool            supported;
        unsigned int    selector;
        unsigned int    curr;
        unsigned int    min;
        unsigned int    max;
        const char*     name;

    } uvc_ctrl;


    uvc_ctrl ct_controls_list[]={
        {0,false,0x01,0,0,0,"Scan Mode"},
        {1,false,0x02,0,0,0,"Auto-Exposure Mode"},
        {2,false,0x03,0,0,0,"Auto-Exposure Priority"},
        {3,false,0x04,0,0,0,"Exposure Time (Abs)"},
        {4,false,0x05,0,0,0,"Exposure Time (Rel)"},
        {5,false,0x06,0,0,0,"Focus (Abs)"},
        {6,false,0x07,0,0,0,"Focus (rel)"},
        {7,false,0x09,0,0,0,"Iris (Abs)"},
        {8,false,0x0A,0,0,0,"Iris (Rel)"},
        {9,false,0x0B,0,0,0,"Zoom (Abs)"},
        {10,false,0x0C,0,0,0,"Zoom (Rel)"},
        {11,false,0x0D,0,0,0,"PanTilt (Abs)"},
        {12,false,0x0E,0,0,0,"PanTilt (Rel)"},
        {13,false,0x0F,0,0,0,"Roll (Abs)"},
        {14,false,0x10,0,0,0,"Roll (Rel)"},
        {15,false,0x00,0,0,0,"Reserved 0"},
        {16,false,0x00,0,0,0,"Reserved 1"},
        {17,false,0x08,0,0,0,"Auto-Focus"},
        {18,false,0x11,0,0,0,"Privacy"},
        {19,false,0x00,0,0,0,"Reserved 2"},
        {20,false,0x00,0,0,0,"Reserved 3"},
        {21,false,0x00,0,0,0,"Reserved 4"},
        {22,false,0x00,0,0,0,"Reserved 5"},
        {23,false,0x00,0,0,0,"Reserved 6"}
        };

    uvc_ctrl pu_controls_list[]={
        {0,false,0x02,0,0,0,"Brightness"},
        {1,false,0x03,0,0,0,"Contrast"},
        {2,false,0x06,0,0,0,"Hue"},
        {3,false,0x07,0,0,0,"Saturation"},
        {4,false,0x08,0,0,0,"Sharpness"},
        {5,false,0x09,0,0,0,"Gamma"},
        {6,false,0x0A,0,0,0,"White-Bal Temp"},
        {7,false,0x0C,0,0,0,"White-Bal Comp"},
        {8,false,0x01,0,0,0,"Backlight Compensation"},
        {9,false,0x04,0,0,0,"Gain"},
        {10,false,0x00,0,0,0,"Power Line Freq"},//NOT Found !
        {11,false,0x10,0,0,0,"Auto-Hue"},
        {12,false,0x0B,0,0,0,"White-Bal Temp (Auto)"},
        {13,false,0x0D,0,0,0,"White-Bal Comp (Auto)"},
        {14,false,0x0E,0,0,0,"Digital Multiplier"},
        {15,false,0x0F,0,0,0,"Digital Multiplier Limit"},
        {16,false,0x11,0,0,0,"Analog Video Std"},
        {17,false,0x12,0,0,0,"Analog Video Lock Status"},
        {18,false,0x00,0,0,0,"Reserved 0"},
        {19,false,0x00,0,0,0,"Reserved 1"},
        {20,false,0x00,0,0,0,"Reserved 2"},
        {21,false,0x00,0,0,0,"Reserved 3"},
        {22,false,0x00,0,0,0,"Reserved 4"},
        {23,false,0x01,0,0,0,"Reserved 5"}

    };

};


#endif
