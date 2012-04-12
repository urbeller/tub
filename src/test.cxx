#include <iostream>
#include <libusb.h>
#include "tub.h"

using namespace std;

void printdev(libusb_device *dev , int filter_dev_class  , int vid); //prototype of the function

void parse_vcontrol( struct libusb_device_handle *handle  , unsigned char *ptr ){

    unsigned int term_type;
    unsigned int nbytes_controls;
    int ctrl;

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
							cout<<"\t\t\t\t\t"<<TUB::controls_list[ctrl].name<<endl;

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
             cout<<" PROCESSING_UNIT ";

             cout<<endl;
        break;
        
        case TUB::VC_EXTENSION_UNIT :
             cout<<" EXTENSION_UNIT |"<<endl;
        break;

        default:
            cout<<"Unknown or not handled video control sub-type: "<<int(ptr[2])<<endl;;
            break;
    }

}

void parse_vstreaming( struct libusb_device_handle *handle  , unsigned char *ptr ){

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

			cout<<"\tAlt Interface "<<j<<buff<<" --> "
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

			for(int k=0; k<nb_ep; k++) {
				epdesc = &interdesc->endpoint[k];
				cout<<"\t\t\tEndpoint "<<k<<" | ";
				
				uint8_t ep_adr = char(epdesc->bEndpointAddress);
				cout<<"EP Address: "<<(ep_adr&15) <<","<< ( (ep_adr>>4) & 7) << "," <<  ( (ep_adr>>7) & 1)<< " | ";

				unsigned char ep_attr = char(epdesc->bmAttributes);
				cout<<"Attrib: "<< (ep_attr&3) <<" | ";

				cout<<"Packet Sz: "<<  (uint) epdesc->wMaxPacketSize<<" | ";
				cout<<epdesc->extra_length<<endl;
			}

			cout<<endl;
		}

		 libusb_release_interface( handle , iface );

		 if(did_detach)
			 libusb_attach_kernel_driver(handle, iface);
	}
	cout<<endl;
	libusb_free_config_descriptor(config);
	libusb_close( handle );
}
