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
        uint16_t        unit;
        uint16_t        prop;
        uint16_t        length;
        unsigned int    min;
        unsigned int    max;
        const char*     name;
    } control_t;


    enum control_op {SET_CUR=0x01,GET_CUR=0x81,GET_MIN=0x82,GET_MAX=0x83};

    static control_t std_controls[][24]={
        {
            {VC_INPUT_TERMINAL,(0x01<<8),0x01,0,0,"Scan Mode"},
            {VC_INPUT_TERMINAL,(0x02<<8),0x01,0,0,"Auto-Exposure Mode"},
            {VC_INPUT_TERMINAL,(0x03<<8),0x01,0,0,"Auto-Exposure Priority"},
            {VC_INPUT_TERMINAL,(0x04<<8),0x04,0,0,"Exposure Time (Abs)"},
            {VC_INPUT_TERMINAL,(0x05<<8),0x01,0,0,"Exposure Time (Rel)"},
            {VC_INPUT_TERMINAL,(0x06<<8),0x02,0,0,"Focus (Abs)"},
            {VC_INPUT_TERMINAL,(0x07<<8),0x02,0,0,"Focus (Rel)"},
            {VC_INPUT_TERMINAL,(0x09<<8),0x02,0,0,"Iris (Abs)"},
            {VC_INPUT_TERMINAL,(0x0A<<8),0x01,0,0,"Iris (Rel)"},
            {VC_INPUT_TERMINAL,(0x0B<<8),0x02,0,0,"Zoom (Abs)"},
            {VC_INPUT_TERMINAL,(0x0C<<8),0x03,0,0,"Zoom (Rel)"},
            {VC_INPUT_TERMINAL,(0x0D<<8),0x08,0,0,"PanTilt (Abs)"},
            {VC_INPUT_TERMINAL,(0x0E<<8),0x04,0,0,"PanTilt (Rel)"},
            {VC_INPUT_TERMINAL,(0x0F<<8),0x02,0,0,"Roll (Abs)"},
            {VC_INPUT_TERMINAL,(0x10<<8),0x02,0,0,"Roll (Rel)"},
            {VC_INPUT_TERMINAL,(0x00<<8),0x0,0,0,"Reserved 0"},
            {VC_INPUT_TERMINAL,(0x00<<8),0x0,0,0,"Reserved 1"},
            {VC_INPUT_TERMINAL,(0x08<<8),0x01,0,0,"Auto-Focus"},
            {VC_INPUT_TERMINAL,(0x11<<8),0x01,0,0,"Privacy"},
            {VC_INPUT_TERMINAL,(0x00<<8),0x0,0,0,"Reserved 2"},
            {VC_INPUT_TERMINAL,(0x00<<8),0x0,0,0,"Reserved 3"},
            {VC_INPUT_TERMINAL,(0x00<<8),0x0,0,0,"Reserved 4"},
            {VC_INPUT_TERMINAL,(0x00<<8),0x0,0,0,"Reserved 5"},
            {VC_INPUT_TERMINAL,(0x00<<8),0x0,0,0,"Reserved 6"}
        }
        ,
            {
                {VC_PROCESSING_UNIT,(0x02<<8),0x02,0,0,"Brightness"},
                {VC_PROCESSING_UNIT,(0x03<<8),0x02,0,0,"Contrast"},
                {VC_PROCESSING_UNIT,(0x06<<8),0x02,0,0,"Hue"},
                {VC_PROCESSING_UNIT,(0x07<<8),0x02,0,0,"Saturation"},
                {VC_PROCESSING_UNIT,(0x08<<8),0x02,0,0,"Sharpness"},
                {VC_PROCESSING_UNIT,(0x09<<8),0x02,0,0,"Gamma"},
                {VC_PROCESSING_UNIT,(0x0A<<8),0x02,0,0,"White-Bal Temp"},
                {VC_PROCESSING_UNIT,(0x0C<<8),0x04,0,0,"White-Bal Comp"},
                {VC_PROCESSING_UNIT,(0x01<<8),0x02,0,0,"Backlight Compensation"},
                {VC_PROCESSING_UNIT,(0x04<<8),0x02,0,0,"Gain"},
                {VC_PROCESSING_UNIT,(0x00<<8),0x01,0,0,"Power Line Freq"},//NOT Found !
                {VC_PROCESSING_UNIT,(0x10<<8),0x01,0,0,"Auto-Hue"},
                {VC_PROCESSING_UNIT,(0x0B<<8),0x01,0,0,"White-Bal Temp (Auto)"},
                {VC_PROCESSING_UNIT,(0x0D<<8),0x01,0,0,"White-Bal Comp (Auto)"},
                {VC_PROCESSING_UNIT,(0x0E<<8),0x02,0,0,"Digital Multiplier"},
                {VC_PROCESSING_UNIT,(0x0F<<8),0x02,0,0,"Digital Multiplier Limit"},
                {VC_PROCESSING_UNIT,(0x11<<8),0x01,0,0,"Analog Video Std"},
                {VC_PROCESSING_UNIT,(0x12<<8),0x01,0,0,"Analog Video Lock Status"},
                {VC_PROCESSING_UNIT,(0x00<<8),0x0,0,0,"Reserved 0"},
                {VC_PROCESSING_UNIT,(0x00<<8),0x0,0,0,"Reserved 1"},
                {VC_PROCESSING_UNIT,(0x00<<8),0x0,0,0,"Reserved 2"},
                {VC_PROCESSING_UNIT,(0x00<<8),0x0,0,0,"Reserved 3"},
                {VC_PROCESSING_UNIT,(0x00<<8),0x0,0,0,"Reserved 4"},
                {VC_PROCESSING_UNIT,(0x01<<8),0x0,0,0,"Reserved 5"}
            }

    };

};


#endif
