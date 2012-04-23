#include <iostream>
#include <cstring>
#include <cstdio>
#include <libusb.h>
#include "tub.h"

using namespace std;

namespace TUB{

	int Context::refresh( int class_type ){
		ssize_t cnt; //holding number of devices in list

		if( libusb_init(&ctx) < 0 )
			return INIT_ERROR;

		
		
	}

}

