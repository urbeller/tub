#ifndef __TUB_CONTROLS_H
#define __TUB_CONTROLS_H

#include "tub_enums.h"

namespace TUB{

    typedef struct{
        uint8_t type;
        uint8_t req;
        uint16_t value;
        uint16_t ndx;
        unsigned char *data;
        uint16_t len;
    } req_t;

    typedef struct {
        unsigned int    bit;
        unsigned int    unit;
        unsigned int    prop;
        unsigned int    min;
        unsigned int    max;
        const char*     name;
    } control_t;


    enum control_op {SET_CUR=0x01,GET_CUR=0x81,GET_MIN=0x82,GET_MAX=0x83};

    static control_t std_controls[][24]={
        {
            {0,VC_INPUT_TERMINAL,0x01,0,0,"Scan Mode"},
            {1,VC_INPUT_TERMINAL,0x02,0,0,"Auto-Exposure Mode"},
            {2,VC_INPUT_TERMINAL,0x03,0,0,"Auto-Exposure Priority"},
            {3,VC_INPUT_TERMINAL,0x04,0,0,"Exposure Time (Abs)"},
            {4,VC_INPUT_TERMINAL,0x05,0,0,"Exposure Time (Rel)"},
            {5,VC_INPUT_TERMINAL,0x06,0,0,"Focus (Abs)"},
            {6,VC_INPUT_TERMINAL,0x07,0,0,"Focus (Rel)"},
            {7,VC_INPUT_TERMINAL,0x09,0,0,"Iris (Abs)"},
            {8,VC_INPUT_TERMINAL,0x0A,0,0,"Iris (Rel)"},
            {9,VC_INPUT_TERMINAL,0x0B,0,0,"Zoom (Abs)"},
            {10,VC_INPUT_TERMINAL,0x0C,0,0,"Zoom (Rel)"},
            {11,VC_INPUT_TERMINAL,0x0D,0,0,"PanTilt (Abs)"},
            {12,VC_INPUT_TERMINAL,0x0E,0,0,"PanTilt (Rel)"},
            {13,VC_INPUT_TERMINAL,0x0F,0,0,"Roll (Abs)"},
            {14,VC_INPUT_TERMINAL,0x10,0,0,"Roll (Rel)"},
            {15,VC_INPUT_TERMINAL,0x00,0,0,"Reserved 0"},
            {16,VC_INPUT_TERMINAL,0x00,0,0,"Reserved 1"},
            {17,VC_INPUT_TERMINAL,0x08,0,0,"Auto-Focus"},
            {18,VC_INPUT_TERMINAL,0x11,0,0,"Privacy"},
            {19,VC_INPUT_TERMINAL,0x00,0,0,"Reserved 2"},
            {20,VC_INPUT_TERMINAL,0x00,0,0,"Reserved 3"},
            {21,VC_INPUT_TERMINAL,0x00,0,0,"Reserved 4"},
            {22,VC_INPUT_TERMINAL,0x00,0,0,"Reserved 5"},
            {23,VC_INPUT_TERMINAL,0x00,0,0,"Reserved 6"}
        }
        ,
            {
                {0,VC_PROCESSING_UNIT,0x02,0,0,"Brightness"},
                {1,VC_PROCESSING_UNIT,0x03,0,0,"Contrast"},
                {2,VC_PROCESSING_UNIT,0x06,0,0,"Hue"},
                {3,VC_PROCESSING_UNIT,0x07,0,0,"Saturation"},
                {4,VC_PROCESSING_UNIT,0x08,0,0,"Sharpness"},
                {5,VC_PROCESSING_UNIT,0x09,0,0,"Gamma"},
                {6,VC_PROCESSING_UNIT,0x0A,0,0,"White-Bal Temp"},
                {7,VC_PROCESSING_UNIT,0x0C,0,0,"White-Bal Comp"},
                {8,VC_PROCESSING_UNIT,0x01,0,0,"Backlight Compensation"},
                {9,VC_PROCESSING_UNIT,0x04,0,0,"Gain"},
                {10,VC_PROCESSING_UNIT,0x00,0,0,"Power Line Freq"},//NOT Found !
                {11,VC_PROCESSING_UNIT,0x10,0,0,"Auto-Hue"},
                {12,VC_PROCESSING_UNIT,0x0B,0,0,"White-Bal Temp (Auto)"},
                {13,VC_PROCESSING_UNIT,0x0D,0,0,"White-Bal Comp (Auto)"},
                {14,VC_PROCESSING_UNIT,0x0E,0,0,"Digital Multiplier"},
                {15,VC_PROCESSING_UNIT,0x0F,0,0,"Digital Multiplier Limit"},
                {16,VC_PROCESSING_UNIT,0x11,0,0,"Analog Video Std"},
                {17,VC_PROCESSING_UNIT,0x12,0,0,"Analog Video Lock Status"},
                {18,VC_PROCESSING_UNIT,0x00,0,0,"Reserved 0"},
                {19,VC_PROCESSING_UNIT,0x00,0,0,"Reserved 1"},
                {20,VC_PROCESSING_UNIT,0x00,0,0,"Reserved 2"},
                {21,VC_PROCESSING_UNIT,0x00,0,0,"Reserved 3"},
                {22,VC_PROCESSING_UNIT,0x00,0,0,"Reserved 4"},
                {23,VC_PROCESSING_UNIT,0x01,0,0,"Reserved 5"}
            }

    };

};


#endif
