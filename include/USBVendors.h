#ifndef __USB_VENDORS__H
#define __USB_VENDORS__H


class USBProduct{
	private:
		int 	pid;//product_id
		string	product_str;

	public:
		USBProduct(int _pid , string _str) : pid(_pid) , str(_str){};
		bool operator< (const USBProduct &other) const {return pid<other.pid;};
		int	 	get_pid(){ return pid;};
		string	get_str(){ return product_str;};
};

class USBVendor{
	private:
		int					vid;
		string				vendor_str;
		vector<USBProduct> 	prods;

	public:
		USBVendor( int _vid , string _str) : vid(_vid) , vendor_str(_str){};
};

#endif
