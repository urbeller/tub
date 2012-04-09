#include <iostream>
#include <libusb.h>

using namespace std;

void printdev(libusb_device *dev , int filter_dev_class = -1 , int vid=0x46d); //prototype of the function

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
		printdev(devs[i] , 0xEF); //print specs of this device
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

			//Try to retrieve the interface description string.
			if( interdesc->iInterface > 0 && have_access){
				err = libusb_get_string_descriptor_ascii( handle , uint8_t(interdesc->iInterface) , buff , sizeof(buff) );
			}else buff[0]='\0';

			int iclass = int(interdesc->bInterfaceClass);
			int isubclass = int(interdesc->bInterfaceSubClass);
			
			cout<<"\tAlt Interface "<<j<<buff<<" --> "
				<<"Class/SubClass:"<<hex<<iclass<<"/"<<isubclass<<" | "<<dec
				<<"Nb of Endpoints: "<<uint(interdesc->bNumEndpoints)<<endl;

			if( interdesc->extra_length ){
				ptr = interdesc->extra;
				cout<<"\t Extra:"<<uint(ptr[0])<<" bytes , "<<hex<<uint(ptr[1]);

				if( isubclass == 1){
					//This is a video control interface
					cout<<" , "<<dec<<int( ptr[5] | (ptr[6] << 8) )<<dec;
					cout<<" (Control Interface)"<<endl;
				}else if(isubclass == 2 ){
					//This is a video streaming interface
					cout<<" , "<<dec<<int( ptr[4] | (ptr[5] << 8) )<<dec;
					cout<<" (Streaming Interface)"<<endl;
				
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
