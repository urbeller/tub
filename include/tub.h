#ifndef __TUB__H
#define __TUB__H


#include <vector>
#include <libusb.h>
#include <stdlib.h>
#include "tub_enums.h"
#include "tub_controls.h"

using namespace std;

namespace TUB{

	class Device;
	class Context;
	class Iface;
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
			libusb_device *lusb_dev;
			libusb_device_descriptor desc;
			string	sn;//Serial number
		public:
			Device( libusb_device *_d );

	};

	class Iface{


	};

	class Endpoint {

	};

	class PropTalk{
		private:
			Device			*dev;//The device
			int				iface;//The interface

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
