#include <iostream>
#include "tub.h"


using namespace TUB;
using namespace std;

int main( int argc , char *argv[] ){

	//Create the usb context.
	Context ctx;

	//Refresh the device list...consider only video devices.
        if( ctx.refresh( ) != TUB::SUCCESS ){
            cout<<"Can't initialize libusb.\n";
            exit(1);
        }

        for(int i=0;i<ctx.nb_devices();i++){
            Device &dev = ctx.list[i];
            cout<<dev.get_vn()<<":::"<<dev.get_pn()<<endl;
        }


	return 0;
}
