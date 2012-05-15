#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

#include "usb_id.h"

using namespace std;

void trim(std::string& word)
{
   word.erase(word.find_last_not_of(" \n\r\t")+1);
   word.erase(0, word.find_first_not_of(" \n\r\t"));
}


USBId::USBId( const char *path ){
	ifstream in(path);
	string line;
	list.clear();
	int vid , pid ;
	string name , name2;

	if (in.is_open()){

		while (std::getline(in, line))
		{

			//skip comments.
			if( line.c_str()[0] == '#' || line=="")
				continue;

			//It's a product...add it to active vendor.
			if( line.c_str()[0] == '\t' ){
				std::istringstream iss(line);
				iss>>hex>>pid>>dec;
				getline(iss,name2);
				trim(name2);
				if( name2 != "" )
					list.back().add_product( pid , name2 );

				continue;
			}

			//It's the begining of special blocks.
			if( line.c_str()[0] == 'C' )
				break;
			
			//It's a vendor
			std::istringstream iss(line);
			iss>>hex>>vid>>dec;
			getline(iss,name);
			trim(name);
			if( name != "" )
				list.push_back( USBVendor(vid,name));
		}
	} 

}

bool operator<( const USBVendor &lhs ,  const USBVendor &rhs ){
	return (lhs.vid < rhs.vid);
}

bool operator<( const USBProduct &lhs , const USBProduct &rhs   ) {
	return (lhs.pid<rhs.pid);
}

void USBId::look_up( int vid , int pid , string res[2] ){

	vector<USBVendor>::iterator vendor_it = lower_bound( list.begin() , list.end() , vid );

	if( vendor_it != list.end() ){
		int pos = vendor_it - list.begin();
		res[0] = list[pos].vendor_str;

		//Look for the product.
		vector<USBProduct>::iterator product_it = lower_bound( list[pos].prods.begin() , list[pos].prods.end() , pid );
		if( product_it != list[pos].prods.end() )
			res[1] = product_it->product_str;
	}

}
