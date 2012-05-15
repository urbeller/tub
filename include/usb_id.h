#ifndef __USB_ID__H
#define __USB_ID__H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class USBProduct{
	private:
		int 	pid;//product_id
		string	product_str;

	public:
		USBProduct(int _pid , string _str) : pid(_pid) , product_str(_str){};
		USBProduct(int _pid ) : pid(_pid) {};
		int	 	get_pid(){ return pid;};
		string	get_str(){ return product_str;};

		friend bool operator<( const USBProduct &lhs , const USBProduct &rhs   ) ;
		friend class USBId;
};

class USBVendor{
	private:
		int					vid;
		string				vendor_str;
		vector<USBProduct> 	prods;

	public:
		USBVendor( int _vid , string _str) : vid(_vid) , vendor_str(_str){};
		USBVendor( int _vid ) : vid(_vid) {};
		void add_product(int pid , const string & str){prods.push_back( USBProduct(pid,str));};

		friend bool operator<( const USBVendor &lhs , const USBVendor &rhs   ) ;
		friend class USBId;
};

class USBId {
	private:
		vector< USBVendor > list;

	public:
		USBId( const char* path );//Fetch the ids from path.

		//look for vendor/product id strings.
		//if pid==-1...don't look-up pid.
		void look_up( int vid , int pid , string res[2] );

};

#endif
