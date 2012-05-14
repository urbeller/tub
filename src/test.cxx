#include <iostream>
#include <cstring>
#include <cstdio>
#include <libusb.h>
#include "tub.h"

using namespace std;

void printdev(libusb_device *dev , int filter_dev_class  , int vid); //prototype of the function

static const char *get_guid(unsigned char *ptr)
{
	static char guid[39];

	sprintf(guid, "{%02x%02x%02x%02x"
			"-%02x%02x"
			"-%02x%02x"
			"-%02x%02x"
			"-%02x%02x%02x%02x%02x%02x}",
	       ptr[0], ptr[1], ptr[2], ptr[3],
	       ptr[4], ptr[5],
	       ptr[6], ptr[7],
	       ptr[8], ptr[9],
	       ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15]);
	return guid;
}

void parse_endpoint( struct libusb_device_handle *handle , const libusb_interface_descriptor *iface , const libusb_endpoint_descriptor *ep){

	static const char *ep_type[] = { "Control", "Isochronous", "Bulk", "Interrupt" };
	static const char *ep_sync[] = { "None", "Asynchronous", "Adaptive", "Synchronous" };

        cout<< "TYPE: "<<ep_type[ep->bmAttributes & 3]<<" | SYNC: " << ep_sync[(ep->bmAttributes >> 2) & 3]<<endl;
	if (ep->extra_length){
            const unsigned char *ptr = ep->extra;
            cout<<"\t\t\t\t CLASS "<<ptr[1]<<endl;
        }else cout<<"\t\t\t\t No extra CLASS.\n";

        cout<<endl;
}

void parse_vcontrol( struct libusb_device_handle *handle  , unsigned char *ptr ){

    unsigned int term_type;
    unsigned int nbytes_controls;
    int ctrl;

    if( ptr[1] != TUB::CS_INTERFACE ){
        cout<<"Warning: Invalid descriptor.\n";
        return;
    }

    switch( ptr[2] ){

        case TUB::VC_HEADER :
            cout<<" HEADER |"<<endl;
            break;

        case TUB::VC_INPUT_TERMINAL:
            cout<<" INPUT_TERMINAL ";
            term_type = ptr[4] | ( ptr[5]<<8);

            if( term_type == TUB::ITT_CAMERA ){
                nbytes_controls=ptr[14];
                cout<<"(CAMERA) | Controls ("<<nbytes_controls<<" bytes) : \n";
                ctrl=0;
                for(int b=0;b<nbytes_controls;b++)
                    for(int f=0;f<8;f++){
                        if( (1<<f) & ptr[15+b])
                            cout<<"\t\t\t\t\t"<<TUB::ct_controls_list[ctrl].name<<endl;

                        ctrl++;
                    }

            }else
                cout<<"Input-Terminal type not handled.";

            cout<<endl;
        break;

        case TUB::VC_OUTPUT_TERMINAL :
             cout<<" OUTPUT_TERMINAL ";
             term_type = ptr[4] | ( ptr[5]<<8);

            switch( term_type ){
                case TUB::OTT_VENDOR_SPECIFIC :
                    cout<<"(VENDOR_SPECIFIC) | ";
                break;

                case TUB::OTT_DISPLAY :
                    cout<<"(DISPLAY) | ";
                break;

                case TUB::OTT_MEDIA_TRANSPORT :
                    cout<<"(MEDIA_TRANSPORT) | ";
                break;

                case TUB::TT_STREAMING :
                    cout<<"(STREAMING) | ";
                break;

                default:
                    cout<<hex<<term_type<<dec;
                break;
            }
    
             cout<<endl;
        break;

        case TUB::VC_SELECTOR_UNIT :
             cout<<" SELECTOR_UNIT |"<<endl;
        break;

        case TUB::VC_PROCESSING_UNIT :
        nbytes_controls = ptr[7];
        cout<<" PROCESSING_UNIT | Controls ("<<nbytes_controls<<" bytes):"<<endl;

        ctrl=0;
        for(int b=0;b<nbytes_controls;b++)
            for(int f=0;f<8;f++){
                if( (1<<f) & ptr[8+b])
                    cout<<"\t\t\t\t\t"<<TUB::pu_controls_list[ctrl].name<<endl;

                ctrl++;
            }

        cout<<endl;
        break;
        
        case TUB::VC_EXTENSION_UNIT :
             cout<<" EXTENSION_UNIT | GUID "<<get_guid( &ptr[4] );

             cout<<endl;
        break;

        default:
            cout<<"Unknown or not handled video control sub-type: "<<int(ptr[2])<<endl;;
            break;
    }

}

void parse_vstreaming( struct libusb_device_handle *handle  , unsigned char *ptr ){

    unsigned int nb_formats , nb_compression;
    unsigned int nbytes_controls;
	unsigned int width , height,compression;
	unsigned char flags;
	bool still_support;
    int ctrl;

   if( ptr[1] != TUB::CS_INTERFACE ){
        cout<<"Warning: Invalid descriptor.\n";
        return;
    }


    switch( ptr[2] ){
        case TUB::VS_INPUT_HEADER :
             nb_formats = ptr[3];
             nbytes_controls = ptr[12];
             cout<<"INPUT HEADER: "<<nb_formats<<" formats | Controls "<<nbytes_controls<< " byte(s) : ";

             for(int p=0;p<nb_formats;p++)
                cout<<int(ptr[13+p*nbytes_controls])<<",";

             cout<<endl;
        break;

        case TUB::VS_OUTPUT_HEADER :
             cout<<"OUTPUT HEADER. \n";
        break;

		case TUB::VS_STILL_IMAGE_FRAME :
			nb_formats = ptr[4];
			nb_compression = ptr[6+4*nb_formats];
			cout<<"STILL IMAGE, "<<nb_formats<<" Formats (W,H): ";

			for(int i=0; i<nb_formats; i++){
			       width =  ptr[5+4*i] | (ptr[6+4*i] << 8);
			       height=  ptr[7+4*i] | (ptr[8+4*i] << 8);
				cout<<width<<"x"<<height<<",";
			}

			cout<<" | Compression: "<<endl;

			for(int i=0; i<nb_compression; i++){
				compression = ptr[6+4*nb_formats+i];
				cout<<compression<<" , ";
			}

			cout<<endl;
		break;

		case TUB::VS_FORMAT_UNCOMPRESSED :
			cout<<"FORMAT UNCOMPRESSED, ";

			cout<<endl;
		break;
		
		case TUB::VS_FRAME_UNCOMPRESSED :
		case TUB::VS_FRAME_MJPEG:
			cout<<"FRAME UNCOMPRESSED/MJPEG | ";
			flags = ptr[4];
			still_support = ((flags & 1)!=0);
			cout<<"Support still image ("<<boolalpha<<still_support<<") | SIZE (";
			width = ptr[5] | (ptr[6] <<  8);
			height = ptr[7] | (ptr[8] << 8);
			cout<<width<<"x"<<height<<")";
			cout<<endl;
		break;

		case TUB::VS_FORMAT_MJPEG :
			cout<<"FORMAT MJPEG | ";

			cout<<endl;
		break;

		case TUB::VS_COLOR_FORMAT :
			cout<<"COLOR FORMAT | ";

			cout<<endl;
		break;

        default:
            cout<<"Sub-type not handled: "<<int(ptr[2])<<endl;
        break;

    }


}
int main() {
	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	libusb_context *ctx = NULL; //a libusb session
	int r; //for return values
	ssize_t cnt; //holding number of devices in list
	r = libusb_init(&ctx); //initialize a library session
	if(r < 0) {
		cout<<"Init Error "<<r<<endl; //there was an error
		return 1;
	}
	//libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
	cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
	if(cnt < 0) {
		cout<<"Get Device Error"<<endl; //there was an error
	}
	cout<<endl<<endl<<cnt<<" Devices in list.\n\n"<<endl; //print total number of usb devices
	ssize_t i; //for iterating through the list
	for(i = 0; i < cnt; i++) {
		printdev(devs[i] , 0xEF, 0x046d); //print specs of this device
	}
	libusb_free_device_list(devs, 1); //free the list, unref the devices in it
	libusb_exit(ctx); //close the session
	return 0;
}

void printdev(libusb_device *dev , int filter_dev_class , int vid) {
	libusb_device_descriptor desc;
	struct libusb_device_handle *handle = NULL; 
	unsigned char buff[256];
	const unsigned char *ptr;
	int err;

	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		cout<<"failed to get device descriptor"<<endl;
		return;
	}

	if( filter_dev_class >=0 && filter_dev_class !=  int(desc.bDeviceClass) )
		return;

	if( vid != desc.idVendor )
		return;

	err = libusb_open(dev, &handle); 

	if( err<0 ){
		cout<<"Can't open device ! \n";
		return;
	}


	cout<<"Vendor:Product ID: "<<hex<<desc.idVendor<<":"<<desc.idProduct<<endl<<dec;

	err = libusb_get_string_descriptor_ascii( handle , int(desc.iSerialNumber) , buff , sizeof(buff) );
	if( err > 0 ){
		cout<<"Serial Number: "<<buff<<endl;
	}

	cout<<"Device Class: "<<hex<<int(desc.bDeviceClass)<<endl;
	cout<<"Device Sub-Class: "<<hex<<int(desc.bDeviceSubClass)<<endl;
	cout<<"Nb of Configurations: "<<(int)desc.bNumConfigurations<<endl;

	libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);
	cout<<"Nb of Interfaces : "<<dec<<(int)config->bNumInterfaces<<endl;
	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;

	for(int iface=0; iface<(int)config->bNumInterfaces; iface++) {
		bool have_access = true;
		bool did_detach = false;
		//Valid only on Linux ??
		if(libusb_kernel_driver_active( handle , iface ) ){
			//Try to detach the driver.
			libusb_detach_kernel_driver(handle, iface);
			did_detach = true;
		}

		int r = libusb_claim_interface(handle, iface);
		if( r != LIBUSB_SUCCESS && (iface == 0) )
			have_access = false;

		cout<<"Interface Number: "<<iface<<" (Access="<<boolalpha<<have_access<<")"<<endl;

		inter = &config->interface[iface];
		cout<<"\tNb Alt Settings: "<<inter->num_altsetting<<endl;
		for(int j=0; j<inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];


			int iclass = int(interdesc->bInterfaceClass);
			int isubclass = int(interdesc->bInterfaceSubClass);

			cout<<"\tAlt Setting "<<j<<" --> "
				<<"Class/SubClass:"<<hex<<iclass<<"/"<<isubclass<<" | "<<dec
				<<"Nb of Endpoints: "<<uint(interdesc->bNumEndpoints)<<endl;


                        if( interdesc->extra_length ){
                            unsigned char *ptr=const_cast<unsigned char*>(interdesc->extra);
                            unsigned int size = interdesc->extra_length;

                            while( size > 4 ){
                                unsigned int desc_sz = ptr[0];

                                switch( ptr[1] ){
                                    case TUB::CS_INTERFACE:
                                        cout<<"\t\tClass Specific Interface:\n";
                                        switch( interdesc->bInterfaceClass ){
                                            case TUB::CC_AUDIO :
                                                cout<<"\t\t\t Audio Class:"<<endl;
                                                break;

                                            case TUB::CC_VIDEO:
                                                cout<<"\t\t\t Video Class ";
                                                switch (interdesc->bInterfaceSubClass){
                                                    case TUB::SC_VIDEOCONTROL :
                                                        cout<<"(CONTROL): ";
                                                        parse_vcontrol( handle , ptr );
                                                        break;
                                                    case TUB::SC_VIDEOSTREAMING :
                                                        cout<<"(STREAMING): ";
                                                        parse_vstreaming( handle , ptr );
                                                        break;

                                                    case TUB::SC_VIDEO_INTERFACE_COLLECTION :
                                                        cout<<"(INTERFACE_COLLECTION): "<<endl;
                                                        break;

                                                    default:
                                                        cout<<"(Unknown Sub-Class).";
                                                    break;

                                                }
                                                cout<<endl;
                                                break;

                                            default:
                                                cout<<"\t\t\tClass unknown or not handled.\n";
                                            break;
                                        }

                                    break;

                                    default:
                                        cout<<"Extra-Descriptor unknown or not handled.\n";
                                    break;
                                }

                                size -= desc_sz;
                                ptr  += desc_sz;
                            }


                        }


                        uint nb_ep = uint(interdesc->bNumEndpoints);

                        for(int k=0; k<nb_ep; k++){
                            cout<<"\t\tEP "<<k<<" : ";
                            epdesc = &interdesc->endpoint[k];
                            parse_endpoint( handle , interdesc , epdesc );
                        }

			cout<<endl;
		}

		 libusb_release_interface( handle , iface );

		 if(did_detach)
			 libusb_attach_kernel_driver(handle, iface);

	}// for(int iface=0...

	cout<<endl;
	libusb_free_config_descriptor(config);
	libusb_close( handle );
}
