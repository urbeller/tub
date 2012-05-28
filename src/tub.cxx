#include <iostream>
#include <cstring>
#include <cstdio>
#include <libusb.h>
#include <string>
#include <sstream>
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



	Device::Device( libusb_device *_d ) : lusb_dev(_d) , vendor_name("") , product_name(""){
		int err;
		unsigned char buff [512];
		struct libusb_device_handle *handle = NULL; 


		sn="";

		libusb_get_device_descriptor(lusb_dev, &desc);
		libusb_open( lusb_dev , &handle);
		err = libusb_get_string_descriptor_ascii( handle , int(desc.iSerialNumber) , buff , sizeof(buff) );

		if( err > 0 )
			sn =  reinterpret_cast<const char*>(buff) ;

		//Look-up the vendor/product name.
		USBId usbid("../ressources/usb.ids");
		string st[2];
		usbid.look_up( desc.idVendor , desc.idProduct , st );

		//If not found stringify vid/pid.
		if( st[0] != "" ){
			vendor_name = st[0];
		}else{
			std::ostringstream oss;
			oss<<hex<<desc.idVendor;
			vendor_name = oss.str();
		}

		if( st[1] != "" ){
			product_name = st[1];
		}else{
			ostringstream oss;
			oss<<hex<<desc.idProduct;
			product_name = oss.str();
		}

		cout<<"Adding....: "<<vendor_name<<" -- "<<product_name<<"::"<<sn<<endl;

                //Parsing interfaces.
                libusb_config_descriptor *config;
                libusb_get_config_descriptor(lusb_dev, 0, &config);
                for(int iface=0; iface < uint8_t(config->bNumInterfaces); iface++){
                    bool did_detach = false;

                    //Valid only on Linux ??
                    if(libusb_kernel_driver_active( handle , iface ) ){
                        //Try to detach the driver.
                        libusb_detach_kernel_driver(handle, iface);
                        did_detach = true;
                    }

                    int r = libusb_claim_interface(handle, iface);
                    //if( r != LIBUSB_SUCCESS && (iface == 0) )
                    if( r == LIBUSB_SUCCESS )
                        interfaces.push_back( Interface(handle,config, iface) );

                    libusb_release_interface( handle , iface );
                    if(did_detach)
                        libusb_attach_kernel_driver(handle, iface);
                }


		libusb_close( handle );
	}

    Interface::Interface(libusb_device_handle *handle ,
                          libusb_config_descriptor *config ,
                          int _id) : iface_id(_id) {

        iface = &config->interface[iface_id];

        for(int i=0;i<uint8_t(iface->num_altsetting);i++)
            parse_altsetting(handle , &iface->altsetting[i]) ;

    }

  void Interface::parse_altsetting( libusb_device_handle *handle , const libusb_interface_descriptor *desc ){

       //Parse Alt-Settings and push them into altset_list.
        if( desc->extra_length ){
            unsigned char *ptr=const_cast<unsigned char*>(desc->extra);
            unsigned int size = desc->extra_length;

            while( size >= sizeof(uint8_t) * 2 ){

                switch( ptr[1] ){
                    case TUB::CS_INTERFACE :
                        switch( desc->bInterfaceClass ){
                            case TUB::CC_VIDEO :
                                switch( desc->bInterfaceSubClass ){
                                    case SC_VIDEOCONTROL:
                                        break;

                                    case SC_VIDEOSTREAMING:
                                        break;

                                    default :
                                        goto dump_unknown;
                                }
                                break;

                            case TUB::CC_AUDIO :
                                switch( desc->bInterfaceSubClass ){
                                    case SC_AUDIOCONTROL:
                                        break;

                                    case SC_AUDIOSTREAMING:
                                        break;

                                    default :
                                        goto dump_unknown;

                                }
                                break;

                            default :
                                goto dump_unknown;
                        }
                        break;

                    default:
                    dump_unknown:
                        altset_list.push_back( AltSetting(desc->bInterfaceNumber , desc->bInterfaceClass , desc->bInterfaceSubClass) );
                }


                //Move the pointer.
                size -= ptr[0];
                ptr += ptr[0];
            }

        }

    }

}

