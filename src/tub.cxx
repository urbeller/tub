#include <iostream>
#include <cstring>
#include <cstdio>
#include <libusb.h>
#include <string.h>
#include "tub.h"

using namespace std;

namespace TUB{

	int Context::refresh( ){
		libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
		libusb_device_descriptor desc;
		struct libusb_device_handle *handle = NULL; 
		ssize_t cnt; //holding number of devices in list

		if( libusb_init(&ctx) < 0 )
			return INIT_ERROR;

		list.clear();
		cnt = libusb_get_device_list(ctx, &devs); //get the list of devices

		for(int i=0;i<cnt;i++){
			int r = libusb_get_device_descriptor(devs[i], &desc);
			if (r < 0) {
				cout<<"Warning: Failed to get device descriptor."<<endl;
				continue;
			}

			//Skip if not a video device (class EF and subclass 2 )
			if( ! ( int(desc.bDeviceClass) == 0xEF && int(desc.bDeviceSubClass) == 0x02 ) )
				continue;

			if( libusb_open(devs[i], &handle) < 0 ){
				cout<<"Warning: Failed to get a handle to device."<<endl;
				continue;
			}

			libusb_close( handle );

			list.push_back( Device( devs[i] ) );
		}

		return SUCCESS;
	}



	Device::Device( libusb_device *_d ) : lusb_dev(_d){
		int err;
		unsigned char buff [512];
		struct libusb_device_handle *handle = NULL; 

		sn="";

		libusb_get_device_descriptor(lusb_dev, &desc);
		libusb_open( lusb_dev , &handle);
		err = libusb_get_string_descriptor_ascii( handle , int(desc.iSerialNumber) , buff , sizeof(buff) );

		if( err > 0 )
			sn =  reinterpret_cast<const char*>(buff) ;

		cout<<"Adding....Vendor/Product/SN: "<<hex<<desc.idVendor <<"/"<<desc.idProduct<<dec<<"/"<<sn<<endl;

		libusb_close( handle );
	}	

}

