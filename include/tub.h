#ifndef __TUB__H
#define __TUB__H


#include "tub_enums.h"
#include "tub_controls.h"

namespace TUB{

	class Device{

	};

	class Iface{


	};

	class Endpoint {

	};

	class PropTalk{
		private:
			Device			dev;//The device
			int				iface;//The interface

		public:
			PropReq( ){}
			PropReq( Device _d ) : dev(_d) {}
			PropReq( Device _d , _iface) : dev(_d) , iface(_iface) {}
			
			setDev( Device _d){ dev = _d;}
			setIface( int _iface){ iface = _iface;}

			send( int prop , int val );
			get( int prop , int val );
			get_range( int prop , int &min , int &max );
	};

};


#endif
