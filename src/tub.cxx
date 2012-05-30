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



	Device::Device( libusb_device *_d ) : device(_d) , vendor_name("") , product_name(""){
		int err;
		unsigned char buff [512];
		struct libusb_device_handle *handle = NULL; 


		sn="";

		libusb_get_device_descriptor(device, &desc);
		libusb_open( device , &handle);
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
                libusb_get_config_descriptor(device, 0, &config);
                for(int iface=0; iface < uint8_t(config->bNumInterfaces); iface++){
                    bool did_detach = false;

                    //Valid only on Linux ??
                    if(libusb_kernel_driver_active( handle , iface ) ){
                        //Try to detach the driver.
                        libusb_detach_kernel_driver(handle, iface);
                        did_detach = true;
                    }

                    int r = libusb_claim_interface(handle, iface);
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
                        case CS_INTERFACE :
                            switch( desc->bInterfaceClass ){
                                case CC_VIDEO :
                                    switch( desc->bInterfaceSubClass ){
                                        case SC_VIDEOCONTROL:
                                            altset_list.push_back(AltSettingVideoControl(desc, handle,ptr));
                                            break;

                                        case SC_VIDEOSTREAMING:
                                            altset_list.push_back(AltSettingVideoStreaming(desc,ptr));
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
                            altset_list.push_back( AltSetting(desc->bInterfaceClass , desc->bInterfaceSubClass,desc) );
                    }


                    //Move the pointer.
                    size -= ptr[0];
                    ptr  += ptr[0];
                }

            }

        }

        void AltSettingVideoControl::parse_video_control( libusb_device_handle *handle , unsigned char *ptr ){
            unsigned int term_type;
            unsigned int nbytes_controls;
            int ctrl;

            switch( ptr[2] ){
                case VC_INPUT_TERMINAL :
                    term_type = ptr[4] | ( ptr[5]<<8);
                    if( term_type == ITT_CAMERA ){
                        nbytes_controls=ptr[14];
                        ctrl=0;
                        for(int b=0;b<nbytes_controls;b++)
                            for(int f=0;f<8;f++){
                                if( (1<<f) & ptr[15+b])
                                    controls.push_back( std_controls[0][ctrl] );

                                ctrl++;
                            }
                    }

                    break;

                case VC_PROCESSING_UNIT :
                    nbytes_controls = ptr[7];
                    ctrl=0;
                    for(int b=0;b<nbytes_controls;b++)
                        for(int f=0;f<8;f++){
                            if( (1<<f) & ptr[8+b])
                                controls.push_back( std_controls[1][ctrl] );

                            ctrl++;
                        }

                    break;

                default:
                    //cout<<"\t UNKW VIDEO_CTRL: "<<hex<<int(ptr[2])<<dec<<endl;
                    break;

            }
        }

    void AltSettingVideoStreaming::parse_video_streaming( unsigned char *ptr ){
        int width , height;

        switch (ptr[2]) {
            case  VS_FORMAT_UNCOMPRESSED:
                //cout<<"# of UNCOMPRESSED desc: "<<int(ptr[4])<<endl;
            break;

            case  VS_FORMAT_MJPEG:
                //cout<<"# of MJPEG desc: "<<int(ptr[4])<<endl;
            break;

            case VS_FRAME_UNCOMPRESSED:
            case VS_FRAME_MJPEG:
                if( ptr[2]==VS_FRAME_MJPEG )
                    cout<<"\t MJPEG...";
                else
                    cout<<"\t UNCOMPRESSED...";

                width =  ptr[5] | (ptr[6] << 8);
                height=  ptr[7] | (ptr[8] << 8);

                cout<<width<<"x"<<height<<","<<float(width)/height<<","<<float(width*height)/1E6<<"Mpixels...";

                if( ptr[25] != 0 ){
                    //Not continuous
                    int nb_fps=int(ptr[25]);
                    for(int i=0;i<nb_fps;i++){
                        int interval_ns = (ptr[26+4*i] | (ptr[27+4*i] << 8) | (ptr[28+4*i] << 16) | (ptr[29+4*i] << 24))*100;
                        float fps = float(1E9)/float(interval_ns);
                        cout<<fps<<"FPS/";
                    }

                }
                cout<<endl;

            break;

            default:
                //cout<<"\t UNKW FRAME/FORMAT HEADER: "<<hex<<int(ptr[2])<<dec<<endl;
            break;
        }
    }

}

