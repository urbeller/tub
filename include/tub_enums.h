#ifndef __TUB_ENUMS_H
#define __TUB_ENUMS_H


namespace TUB{


//Status
enum status_type {DEV_ERROR = -2 , INIT_ERROR = -1 , SUCCESS = 1 };

//Class Specific.
enum cs_type {CS_INTERFACE=0x24 , CS_ENDPOINT=0x25 };


//Class Codes.
enum cc_type {CC_NONE=0x00 , CC_VIDEO=0x0e , CC_AUDIO=0x01 };

//Sub-Classes codes.
enum sc_type {SC_UNKNOWN=0x00 , SC_VIDEOCONTROL=0x01,SC_VIDEOSTREAMING=0x02,SC_VIDEO_INTERFACE_COLLECTION=0x03,
              SC_AUDIOCONTROL=0x01, SC_AUDIOSTREAMING=0x02
            };

//Video-Control subtypes.
enum vc_subtype { VC_UNDEF=0x00 , VC_HEADER , VC_INPUT_TERMINAL , VC_OUTPUT_TERMINAL , VC_SELECTOR_UNIT,
                  VC_PROCESSING_UNIT , VC_EXTENSION_UNIT
                 };

//Video-Streaming subtypes
enum vs_subtype { VS_UNDEF=0x00 , VS_INPUT_HEADER, VS_OUTPUT_HEADER, VS_STILL_IMAGE_FRAME,
                  VS_FORMAT_UNCOMPRESSED, VS_FRAME_UNCOMPRESSED , VS_FORMAT_MJPEG, VS_FRAME_MJPEG,
                  VS_RES0 , VS_RES1 , VS_FORMAT_MPEG2TS, VS_RES2 , VS_FORMAT_DV, VS_COLOR_FORMAT,
                  VS_RES3, VS_RES4, VS_FORMAT_FRAME_BASED, VS_FRAME_FRAME_BASED, VS_FORMAT_STREAM_BASED
                };


//Video Terminal types
enum tt_type { TT_VENDOR_SPECIFIC=0x0100 , TT_STREAMING=0x0101,
               ITT_VENDOR_SPECIFIC=0x0200 , ITT_CAMERA=0x0201 , ITT_MEDIA_TRANSPORT_INPUT=0x0202 ,
               OTT_VENDOR_SPECIFIC=0x0300 , OTT_DISPLAY=0x0301 , OTT_MEDIA_TRANSPORT=0x0302 };


};


#endif
