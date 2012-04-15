/* to build: llvm-gcc -Wall `pkg-config --cflags libusb-1.0 --libs libusb-1.0` -o uvc-ctrl uvc-controller.c */

/* TODO:
 * fix user-set speed of CT_FOCUS_RELATIVE_CONTROL
 * add PU_WHITE_BALANCE_COMPONENT_(AUTO_)CONTROL
 * note how both these fuckers need two set values...
 *
 * fix list_usb_devices so it doesnt break certain active devices
 *
 * fix print_uvc_info to be callable in stdin session
 */

/* bitdumper, skipping msb
void dumpres(long r)
{
	uint16_t i;
	for(i = (1 << 15); i > 1; printf("%d", (r & (i = i >> 1)) ? 1 : 0));
	printf("b\n");
} */
/* itoa
static char *itoa(int v)
{
	static char	buf[11]	= {0};
	int i = 9;
	for(; v && i; --i, v /= 10) buf[i] = "0123456789"[v % 10];
	return &buf[i+1];
} */

#include <stdio.h>
#include <stdlib.h>		/* atoi, exit */
#include <string.h>		/* strcpy.. known source! */
#include <unistd.h>		/* getopt */
#include <sys/types.h>
#include <regex.h>
#include <libusb.h>
#include "uvc-controller.h"

#define BUFSZ		128
#define DESC_ERR	"- not avaliable -"
#define SESSION_ERR	"invalid request\n"

/* getflags translated.. */
const uint8_t getflags[] =
{
	UVC_GET_CUR,
	UVC_GET_MIN,
	UVC_GET_MAX,
	UVC_GET_DEF
};
/* ops to control requests...  bmRequestType and bRequest is set by getflags
 * or set.. remember that wIndex needs to be or'd with the iface so it is
 * high byte: term id, low byte: iface num */
const struct ctrlreq uvcc_cr[] =
{	/* reqtype, req, value, index, len, data */
	{ 0,0, CT_FOCUS_AUTO_CONTROL, UVC_CT_INDEX, 0x01, NULL },
	{ 0,0, CT_FOCUS_ABSOLUTE_CONTROL, UVC_CT_INDEX, 0x02, NULL },
	/* high byte: value, low byte: speed */
	{ 0,0, CT_FOCUS_RELATIVE_CONTROL, UVC_CT_INDEX, 0x02, NULL },
	{ 0,0, CT_AE_MODE_CONTROL, UVC_CT_INDEX, 0x01, NULL },
	{ 0,0, CT_EXPOSURE_TIME_ABSOLUTE_CONTROL, UVC_CT_INDEX, 0x04, NULL },
	{ 0,0, CT_EXPOSURE_TIME_RELATIVE_CONTROL, UVC_CT_INDEX, 0x01, NULL },
	{ 0,0, CT_IRIS_ABSOLUTE_CONTROL, UVC_CT_INDEX, 0x02, NULL },
	{ 0,0, CT_IRIS_RELATIVE_CONTROL, UVC_CT_INDEX, 0x01, NULL },
	{ 0,0, PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL, UVC_PU_INDEX, 0x01, NULL },
	{ 0,0, PU_WHITE_BALANCE_TEMPERATURE_CONTROL, UVC_PU_INDEX, 0x02, NULL },
	{ 0,0, PU_BRIGHTNESS_CONTROL, UVC_PU_INDEX, 0x02, NULL },
	{ 0,0, PU_CONTRAST_CONTROL, UVC_PU_INDEX, 0x02, NULL },
	{ 0,0, PU_GAIN_CONTROL, UVC_PU_INDEX, 0x02, NULL },
	{ 0,0, PU_SATURATION_CONTROL, UVC_PU_INDEX, 0x02, NULL },
	{ 0,0, PU_SHARPNESS_CONTROL, UVC_PU_INDEX, 0x02, NULL }/*,
	{ 0,0, PU_GAMMA_CONTROL, UVC_PU_INDEX, 0x02, NULL } */
};
char *ename;					/* executable name */
libusb_context *ctx = NULL;	/* session context */

/* used everywhere! */
void close_uvi(struct uviface *uvi);
struct uviface *open_uvi_index(int index);

static void usage(FILE *f)
{
	fprintf(f, "usage: %s [-h] [-d D] [-e] [[-c] [[-g gv] [-s val] op] [-i] N]\n"
			"\t-h\t\tdisplay this Help\n"
			"\t-d\t\tset debug mode to D, where D = 0/1/2/3 (combine with other args)\n"
			"\t-e\t\tenumerate usb devices\n"
			"\t-c N\t\tshow Configurations on device N\n"
			"\t-g gv op N\tGet gv of op on device N\n"
			"\t-s val op N\tSet value of op to val on device N\n"
			"\t-i N\tuse stdin to specify requests on device N\n"
			"\n\tgv\tdescription\n"
			"\t0\tcurrent value\n"
			"\t1\tmin value\n"
			"\t2\tmax value\n"
			"\t3\tdefault value\n"
			"\n\top\tname\t\t\tset value\tvalue description\n"
			"\t0\tauto-focus\t\t0/1\t\toff/on\n"
			"\t1\tabsolute focus\t\t[min] - [max]\tDistance to target in mm\n"
			"\t2\trelative focus\t\t-1/0/1\t\t(set only) lens group movement, near/stop/far\n"
			"\t3\tauto-exposure\t\t1/2/4/8\t\tManual/Auto/Manual exposure/Manual iris\n"
			"\t4\tabsolute exposure\t[min] - [max]\tshutter speed, value * 100 microseconds\n"
			"\t5\trelative exposure\t-1/0/1\t\t(set only) shutter speed, decrease/default/increase\n"
			"\t6\tabsolute iris\t\t[min] - [max]\taperture setting, f-stop * 100 * value\n"
			"\t7\trelative iris\t\t-1/0/1\t\t(set only) aperture f-number, decrese/default/increase\n"
			"\t8\tauto-white balance temp\t0/1\t\toff/on\n"
			"\t9\twhite balance temp\t[min] - [max]\twhite balance temperature\n"
			"\t10\tbrightness\t\t[min] - [max]\tbrightness\n"
			"\t11\tcontrast\t\t[min] - [max]\tcontrast\n"
			"\t12\tgain\t\t\t[min] - [max]\tgain\n"
			"\t13\tsaturation\t\t[min] - [max]\tsaturation\n"
			"\t14\tsharpness\t\t[min] - [max]\tsharpness\n"
		//	"\t15\tgamma\t[min] - [max]\tgamma\n"
			"\nstdin format:\n"
			"\tavailable commands are help (h), get (g) and set (s). only "
			"one reqeust per line are allowed. same request syntax as when "
			"given as args to %s applies but without the dashes (-). "
			"session is terminated by empty line.\n"
			"\nexample usage:\n"
			"\t%s -d1 -g0 1 4\n"
			"\tget the current absolute focus on device 4 with debug level set to 1\n"
			"\n\t%s -s500 4 2\n"
			"\tset absolute exposure to 0.05 seconds on device 2\n",
			ename, ename, ename, ename);
	/* lazy, crappy flow and so on..
	 * but it's late and i only need it to work */
	if(ctx != NULL) libusb_exit(ctx);
	exit(1);
}
void session_usage()
{
	printf("h\t\tshow this help\n"
			"g gv op\t\tGet gv of op\n"
			"s val op\tSet value of op to val\n"
			"\ngv\tdescription\n"
			"0\tcurrent value\n"
			"1\tmin value\n"
			"2\tmax value\n"
			"3\tdefault value\n"
			"\nop\tname\t\t\tset value\tvalue description\n"
			"0\tauto-focus\t\t0/1\t\toff/on\n"
			"1\tabsolute focus\t\t[min] - [max]\tDistance to target in mm\n"
			"2\trelative focus\t\t-1/0/1\t\t(set only) lens group movement, near/stop/far\n"
			"3\tauto-exposure\t\t1/2/4/8\t\tManual/Auto/Manual shutter/Manual iris\n"
			"4\tabsolute exposure\t[min] - [max]\tshutter speed, value * 100 microseconds\n"
			"5\trelative exposure\t-1/0/1\t\t(set only) shutter speed, decrease/default/increase\n"
			"6\tabsolute iris\t\t[min] - [max]\taperture setting, f-stop * 100 * value\n"
			"7\trelative iris\t\t-1/0/1\t\t(set only) aperture f-number, decrese/default/increase\n"
			"8\tauto-white balance temp\t0/1\t\toff/on\n"
			"9\twhite balance temp\t[min] - [max]\twhite balance temperature\n"
			"10\tbrightness\t\t[min] - [max]\tbrightness\n"
			"11\tcontrast\t\t[min] - [max]\tcontrast\n"
			"12\tgain\t\t\t[min] - [max]\tgain\n"
			"13\tsaturation\t\t[min] - [max]\tsaturation\n"
			"14\tsharpness\t\t[min] - [max]\tsharpness\n"
		//	"15\tgamma\t[min] - [max]\tgamma\n"
			);
}

libusb_device_handle *open_uvi_vidpid(char *s)
{
	/* todo */
	return NULL;
}
/* this is freaking pointless.. but i can rewrite it some other time */
struct uviface *open_uvi_index(int index)
{
	int i, j, k, n, found = 0;
	libusb_device **uds, *ud;
	/* and this one has to be freed */
	struct libusb_config_descriptor *ucd;
	struct libusb_device_descriptor udd;
	const struct libusb_interface *ui;
	const struct libusb_interface_descriptor *uid;

	struct uviface *uvi;
	/* sanity checks */
	if(index < 0) return NULL;
	/* alloc or uvi and set iface to -1 */
	if(!(uvi = malloc(sizeof(struct uviface))))
	{
		perror("could not allocate uvi");
		return NULL;
	}
	/* select interace by index */
	if((n = libusb_get_device_list(ctx, &uds)) < 0)
	{
		perror("Could not retrive device list");
		return NULL;
	}
	if(index >= n) fprintf(stderr, "Index out of bounds: i = %d, N = %d\n",
							index, n);
	/* open it and free the list after storing device pointer */
	else if(libusb_open(uds[index], &uvi->udh) != 0)
	{
		perror("Could not open device");
		libusb_free_device_list(uds, 1);
		return NULL;
	}
	else ud = uds[index];
	/* **list, unref devices with int */
	libusb_free_device_list(uds, 1);
	/* get the descriptor */
	if(libusb_get_device_descriptor(ud, &udd) != 0)
	{
		perror("Could not get device descriptor");
		libusb_close(uvi->udh);
		free(uvi);
		return NULL;
	}
	/* find interface number with class 0e:01 */
	for(i = 0; i < udd.bNumConfigurations && !found; i++)
	{
		if(libusb_get_config_descriptor(ud, i, &ucd) != 0)
		{
			perror("Could not get device config descriptor");
			continue;
		}
		for(j = 0; j < ucd->bNumInterfaces &&!found; j++)
		{
			ui = &ucd->interface[j];
			for(k = 0; k < ui->num_altsetting; k++)
			{
				uid = &ui->altsetting[k];
				if(uid->bInterfaceClass == BASE_CLASS_VID &&
					uid->bInterfaceSubClass == SUB_CLASS_VID_CTRL)
				{
					uvi->iface = uid->bInterfaceNumber;
					found = 1;
					break;
				}
			}
		}
		libusb_free_config_descriptor(ucd);
	}
	if(!found)
	{	/* the config have been freed in the loop */
		close_uvi(uvi);
		return NULL;
	}
	return uvi;
}

void close_uvi(struct uviface *uvi)
{
	libusb_close(uvi->udh);
	free(uvi);
}

int list_usb_devices(int dbg)
{
	int i, n, cams = 0;
	struct uviface *uvi;
	libusb_device **uds;
	struct libusb_device_descriptor udd;
	unsigned char buf[BUFSZ];

	/* context, ***list */
	if((n = libusb_get_device_list(ctx, &uds)) < 0)
	{
		perror("Could not retrive devcice list");
		return n;
	}
	if(dbg) printf("N\tVendor:Prod\tBus:Addr\tIface\tManufacturer\n"
					"\t\t\t\t\t\tProduct\n\t\t\t\t\t\tSN\n\n");
	for(i = 0; i < n; i++)
	{
		if((uvi = open_uvi_index(i)) == NULL) continue;
		else cams++;

		if(libusb_get_device_descriptor(uds[i], &udd) != 0)
		{
			perror("Could not get device descriptor");
			close_uvi(uvi);
			continue;
		}
		printf("%d\t%04x:%04x\t%02d:%02d\t\t%d\t", i,
				udd.idVendor, udd.idProduct,
				libusb_get_bus_number(uds[i]),
				libusb_get_device_address(uds[i]),
				uvi->iface);
		/* args: handle, desc_index, buf, sizeof(buf) */
		if(libusb_get_string_descriptor_ascii(uvi->udh,
			udd.iManufacturer, buf, BUFSZ) < 1)
			strcpy((char *)&buf, DESC_ERR);
		printf("%s\n", buf);

		if(libusb_get_string_descriptor_ascii(uvi->udh,
			udd.iProduct, buf, BUFSZ) < 1)
			strcpy((char *)&buf, DESC_ERR);
		printf("\t\t\t\t\t\t%s\n", buf);

		if(libusb_get_string_descriptor_ascii(uvi->udh,
			udd.iSerialNumber, buf, BUFSZ) < 1)
			strcpy((char *)&buf, DESC_ERR);
		printf("\t\t\t\t\t\t%s\n\n", buf);
		close_uvi(uvi);
	}
	/* **list, unref devices with int */
	libusb_free_device_list(uds, 1);

	if(dbg) printf("summary: %d devices found, %d are video devices.\n",
					n, cams);
	return 0;
}

int print_uvc_info(int ud)
{
	libusb_device **ds;	/* device list */
	struct libusb_config_descriptor *ucd;
	struct libusb_device_descriptor udd;
	const struct libusb_interface *ui;
	const struct libusb_interface_descriptor *uid;
	const struct libusb_endpoint_descriptor *uepd;
	int r, i, j, k, l;

	/* populate the device list, args: context, ***device */
	if((r = libusb_get_device_list(ctx, &ds)) < 0)
	{
		perror("Could not retrive device list");
		return r;
	}
	/* check index */
	if(ud >= r || ud < 0) fprintf(stderr,
								  "Index out of bounds, i: %d, N: %d\n",
								  i, r);
	if((r = libusb_get_device_descriptor(ds[ud], &udd)) != 0)
	{
		perror("Could not get device descriptor");
		return r;
	}
	/* this is just disgusting.. four nested loops. that shit is wack! */
	printf("Device %d:\nMax Packet size for ep 0: %d\nNum configs: %d\n",
			ud, udd.bMaxPacketSize0, udd.bNumConfigurations);
	for(i = 0; i < udd.bNumConfigurations; i++)
	{
		printf("Configuration %d:\n", i);
		libusb_get_config_descriptor(ds[ud], i, &ucd);
		printf("\tInterfaces: %d\n", ucd->bNumInterfaces);
		for(j = 0; j < ucd->bNumInterfaces; j++)
		{
			printf("\tInterface %d:\n", j);
			ui = &ucd->interface[j];
			printf("\t\tNumber of alt. settings: %d\n", ui->num_altsetting);
			for(k = 0; k < ui->num_altsetting; k++)
			{
				printf("\t\tAlt. setting %d:\n", k);
				uid = &ui->altsetting[k];
				printf("\t\t\tDescriptor type: %02x\n"
						"\t\t\tInterface number: %d\n"
						"\t\t\tNumber of eps: %d\n"
						"\t\t\tInterface class: %02x:%02x:%02x\n",
						uid->bDescriptorType, uid->bInterfaceNumber,
						uid->bNumEndpoints, uid->bInterfaceClass,
						uid->bInterfaceSubClass, uid->bInterfaceProtocol);
				for(l = 0; l < uid->bNumEndpoints; l++)
				{
					printf("\t\t\tEP %d:\n", l);
					uepd = &uid->endpoint[l];
					printf("\t\t\t\tDesc. type: %d\n"
							"\t\t\t\tEP addr: %d\n",
							uepd->bDescriptorType, uepd->bEndpointAddress);
				}
			}
		}
		libusb_free_config_descriptor(ucd);
	}
	/* **list, unref devices with int */
	libusb_free_device_list(ds, 1);
	return 0;
}

/* returns number of bytes sent */
int send_uvc_req(struct uviface *uvi, struct ctrlreq *cr)
{
	int r, yesno = 0;

	/* _WIN32 is avail on both win32 and win64 */
	#ifndef _WIN32
	if(libusb_kernel_driver_active(uvi->udh, uvi->iface))
	{
		#ifdef __MACH__ || __APPLE__
		fprintf(stderr, "Device or interface busy, cannot detach!\n");
		close_uvi(uvi);
		return -1;
		#else
		yesno = 1;
		fprintf(stderr, "Device or interface busy, detaching from kernel..\n");
		libusb_detach_kernel_driver(uvi->udh, uvi->iface);
		#endif
	}
	#else
	fprintf(stderr, "Cannot check availability in this OS, will just try"
					"to claim interface.\n");
	#endif

	r = libusb_claim_interface(uvi->udh, uvi->iface);
	if(r == 0)
	{
		r = libusb_control_transfer(uvi->udh,
									cr->bmRequestType,
									cr->bRequest,
									cr->wValue,
									cr->wIndex,
									cr->data,
									cr->wLength,
									0); /* timeout */
		if(r < 0)
		{
			fprintf(stderr, "request failed: ");
			switch(r)
			{
				case LIBUSB_ERROR_TIMEOUT:
					fprintf(stderr, "timeout\n");
				break;
				case LIBUSB_ERROR_PIPE:
					fprintf(stderr, "pipe error\n");
				break;
				case LIBUSB_ERROR_OVERFLOW:
					fprintf(stderr, "overflow\n");
				break;
				case LIBUSB_ERROR_NO_DEVICE:
					fprintf(stderr, "no device\n");
				break;
				default: fprintf(stderr, "error %d\n", r);
			}
		}
		/* else printf("%ld\n", buf); */
		libusb_release_interface(uvi->udh, uvi->iface);
	}
	else
	{
		fprintf(stderr, "request failed! could not claim interface: ");
		switch(r)
		{
			case LIBUSB_ERROR_NOT_FOUND:
				fprintf(stderr, "not found\n");
			break;
			case LIBUSB_ERROR_BUSY:
				fprintf(stderr, "in use\n");
			break;
			case LIBUSB_ERROR_NO_DEVICE:
				fprintf(stderr, "device gone\n");
			break;
			case LIBUSB_ERROR_ACCESS:
				fprintf(stderr, "access denied\n");
			break;
			default: fprintf(stderr, "error %d\n", r);
		}

	}
	if(yesno) libusb_attach_kernel_driver(uvi->udh, uvi->iface);

	return r;
}
/* args: uviface, getflag, value, operation, dbg-lvl */
int getset_value(struct uviface *uvi, int gv, long val, int op, int dbg)
{
	struct ctrlreq cr;
	long res = 0;
	int r;

	/* set up our control request */
	cr = uvcc_cr[op];
	cr.wIndex 			= (cr.wIndex | uvi->iface);
	cr.bmRequestType 	= (val > -2 ? UVC_BMRT_SET : UVC_BMRT_GET);
	cr.bRequest 		= (val > -2 ? UVC_SET_CUR : getflags[gv]);
	cr.data 			= (unsigned char *)(val > -2 ? &val : &res);
	printf("wIndex=%d , bmRequestType=%d bRequest=%d data=%d\n" , 
		cr.wIndex 			,
        cr.bmRequestType,
        cr.bRequest 	,
        cr.data[0] 		
			);
	/* and send it! */
	if((r = send_uvc_req(uvi, &cr)) > 0)
	{
		if(dbg > -1) printf("Bytes sent: %d\n", r);
		else if(val > -2) printf("%d\n", r);
		if(gv > -1) printf("%ld\n", res);
		r = 0;
	}
	else
	{
		printf("request failed\n");
		r = 1;
	}
	return r;
}

int input_session(struct uviface *uvi, int dbg)
{
	const char *re = "^[[:blank:]]*([0-9]+)[[:blank:]]+([0-9]+)";
	/* first hit is the string boundaries of full match */
	regmatch_t rehit[3];
	regex_t ptrn;
	char buf[BUFSZ] = {0};
	long val;
	int gv, op, r = 0;

	if(regcomp(&ptrn, re, REG_EXTENDED))
	{
		perror("Could not compile regular expression");
		return -1;
	}
	while(buf[0] != '\n')
	{
		gv = -1;
		val = -2;
		printf("> ");
		fgets(buf, BUFSZ, stdin);
		switch(buf[0])
		{
			case 'h':
				session_usage();
				continue;
			break;
			case 'g':
			case 's':
				if(regexec(&ptrn, buf+1, 3, rehit, 0))
				{
					printf("%s", SESSION_ERR);
					continue;
				}
				/* so = start offset */
				if(buf[0] == 'g') gv = atoi(buf+1+rehit[1].rm_so);
				else val = atol(buf+1+rehit[1].rm_so);
				op = atoi(buf+1+rehit[2].rm_so);
				/* make sure op is right
				 * if get, gv is right
				 * if set, val is right */
				if(op >= sizeof(uvcc_cr)/sizeof(struct ctrlreq) || op < 0 ||
					(buf[0] == 'g' && (gv < 0 ||
					gv >= sizeof(getflags)/sizeof(uint8_t))) ||
					(buf[0] == 's' && val < -1))
				{
					printf("%s", SESSION_ERR);
					continue;
				}
				r = getset_value(uvi, gv, val, op, dbg);
			break;
			case '\n':
				printf("exiting\n");
				r = -1;
			break;
			default:
				printf("%s", SESSION_ERR);
				fflush(stdin);
		}
	}
	regfree(&ptrn);
	return 0;
}

int main(int argc, char **argv)
{
	int r, ch, op, dbg = -1, cfg = -1, get = -1, lst = 0, si = 0;
	long set = -2;
	extern char *optarg;
	extern int optind;
	/* things to send */
	struct uviface *uvi;

	ename = argv[0];
	while((ch = getopt(argc, argv, "hecd:g:s:i")) != -1)
	{
		switch(ch)
		{
			case 'h':	/* show help */
				usage(stdout);
			break;
			case 'e':	/* list devs */
				lst = 1;
			break;
			case 'c':	/* show device config */
				cfg = atoi(optarg);
			break;
			case 'd':	/* debugging (verbose) */
				dbg = atoi(optarg);
				if(dbg < 0 || dbg > 3) usage(stderr);
			break;
			case 'g':	/* get = gv */
				get = atoi(optarg);
				if(get >= sizeof(getflags)/sizeof(uint8_t) || get < 0)
					usage(stderr);
			break;
			case 's':	/* set = value */
				set = atol(optarg);
				if(set < -1) usage(stderr);
			break;
			case 'i':
				si = 1;
			break;
			case '?':
			default:
				usage(stderr);
		}
	}
	argc -= optind;
	argv += optind;

	/* args: context */
	if((r = libusb_init(&ctx)) != 0)
	{
		perror("Could not init libusb");
		return r;
	}
	/* dbg flies solo! */
	if(dbg > -1) libusb_set_debug(ctx, dbg);

	if(lst) r = list_usb_devices(dbg);
	else if(cfg > -1) print_uvc_info(cfg);
	else if(get > -1 || set > -2)
	{	/* first some sanity checks */
		if(argc < 2) usage(stderr);
		op = atoi(argv[0]);
		if(op < 0 || op >= sizeof(uvcc_cr)/sizeof(struct ctrlreq))
			usage(stderr);

		/* ultra special case for CT_FOCUS_RELATIVE_CONTROL,
		 * lets always use a speed of 1 */
		if(op == 2) set = (set << 8 | 0x01);
		/* open a uvc video interface */
		if((uvi = open_uvi_index(atoi(argv[1]))) == NULL)
		{
			fprintf(stderr, "No video control interface found on device %d\n",
					atoi(argv[1]));
			exit(1);
		}
		r = getset_value(uvi, get, set, op, dbg);
		close_uvi(uvi);
	}
	else if(si)
	{
		if(argc < 1) usage(stderr);
		if((uvi = open_uvi_index(atoi(argv[0]))) == NULL)
		{
			fprintf(stderr, "No video control interface found on device %d\n",
					atoi(argv[0]));
			exit(1);
		}
		r = input_session(uvi, dbg);
		close_uvi(uvi);
	}
	else usage(stderr);

	libusb_exit(ctx);
	return r;
}