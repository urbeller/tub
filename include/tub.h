#ifndef __TUB__H
#define __TUB__H


#include <vector>
#include <libusb.h>
#include <stdlib.h>
#include "tub_enums.h"
#include "tub_controls.h"
#include "usb_id.h"

using namespace std;

namespace TUB{

	class Device;
	class Context;
	class Interface;
	class AltSetting;
	class Endpoint;
	class PropTalk;

	/*This class holds the USB context and list of devices.*/
	/*Devices can be filtered by class_type...ex: video_class*/
	class Context{
		private:
			libusb_context *ctx ;

		public:
			vector<Device> list;//Not the best structure if devices must be removed!


			int refresh( );

			//ctor
			Context( ) : ctx(NULL) {}
			~Context(){
				if( ctx != NULL ){
					libusb_exit(ctx);
				}
			}
	};

	class Device{
		private:
			libusb_device *device;
			libusb_device_descriptor desc;
                        vector< Interface > interfaces;
			string vendor_name;
			string product_name;
			string	sn;//Serial number
		public:
			Device( libusb_device *_d );

	};

	class Interface{
            private:
                int iface_id;
                const libusb_interface *iface;
                vector< AltSetting > altset_list;
                void parse_altsetting( libusb_device_handle *handle ,
                                             const libusb_interface_descriptor *desc );

            public:
                Interface(libusb_device_handle *handle ,
                          libusb_config_descriptor *config ,
                          int _iface);

	};

        class AltSetting{
            private:
                int altset_id;
                int altset_class;
                int altset_subclass;
                const libusb_interface_descriptor *desc;

            public:
                AltSetting( int _id , int _class , int _subclass) : altset_id(_id) , altset_class(_class)  , altset_subclass(_subclass){}
        };

        class AltSettingVideoControl : public AltSetting{

        };

        class AltSettingVideoStreaming : public AltSetting{

        };

	class Endpoint {

	};

        class PropTalk{
            private:
                Device  *dev;//The device
                int	iface;//The interface

            public:
                PropTalk( ){}
                PropTalk( Device *_d ) : dev(_d) {}
                PropTalk( Device *_d , int _iface) : dev(_d) , iface(_iface) {}

                int setDev( Device *_d){ dev = _d;}
                int setIface( int _iface){ iface = _iface;}

                int send( int prop , int val );
                int get( int prop , int val );
                int get_range( int prop , int &min , int &max );
        };

};


#endif
