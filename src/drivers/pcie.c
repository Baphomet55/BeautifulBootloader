#include "uefi.h"
#include "palloc.h"
#include "io.h"
#include "pcie.h"

static int pci_device_count = 0;
static pci_dev_address* pci_device_addresses;

pci_dev_address* enumerate_pci_bus()
{
	pci_device_addresses = (pci_dev_address*) alloc_page();
	check_bus(0);	
	return pci_device_addresses;	
}

pci_dev_address find_device(int class_code, int sub_class, int prog_if)
{
	pci_dev_address pci_device;
	
	if(class_code == -1) { 
		pci_device.enable = 0;
		return pci_device;
	}

	int device_base_class;
	int device_sub_class;
	int device_prog_if;
	for(int i = 0; i < pci_device_count; i++)
	{
		pci_device = pci_device_addresses[i];
		device_base_class = get_base_class(
			pci_device.bus,
			pci_device.device,
			pci_device.function
		);
		device_sub_class = get_sub_class(
			pci_device.bus,
			pci_device.device,
		 	pci_device.function
		);
		device_prog_if = get_prog_if(
			pci_device.bus,
			pci_device.device,
			pci_device.function
		);
		
		if(device_base_class == class_code)
			break;
		
	}

	if(device_sub_class != sub_class && sub_class != -1)
		pci_device.enable = 0;

	if(device_prog_if != prog_if && prog_if != -1)
		pci_device.enable = 0;
	
	return pci_device;
}

int pci_get_header(int bus, int device, int function, pci_common_header* pci_buffer)
{
	uint32* buffer = (uint32*) pci_buffer;
	pci_dev_address pci_reg_addr; 
	for(int i = 0; i < 4; i++)
	{

		pci_reg_addr.enable = 1;
		pci_reg_addr.bus = bus;
		pci_reg_addr.device = device;
		pci_reg_addr.function = function;
		pci_reg_addr.offset = i << 2;

		int reg = pci_io_get(pci_reg_addr);

		buffer[i] = reg;
	}

	return pci_buffer->header_type & 0x7f;

}

void check_bus(int bus)
{
	for(int device = 0; device < 32; device++)
	{
		check_device(bus, device);
	}
}


void check_device(int bus, int device)
{
	unsigned char function = 0;	

	int vendor_id = get_vendor_id(bus,device,function);
	if(vendor_id == 0xffff) return;
	check_function(bus,device,function);
	int header_type = get_header_type(bus, device, function);

	if((header_type & 0x80) == 0)
	{ return; }

	for(function = 1; function < 8; function++)
	{
		if(get_vendor_id(bus,device,function) != 0xffff)
			check_function(bus, device, function);
	}
	
}

void check_function(int bus, int device, int function)
{
	int class_code = get_base_class(bus,device,function);
	int subclass_code = get_sub_class(bus,device,function);
	
	pci_dev_address reg_base_address;
	reg_base_address.enable = 1;
	reg_base_address.bus = bus;
	reg_base_address.device = device;
	reg_base_address.function = function;

	if(class_code == 0x6 && subclass_code == 0x4)
	{
		int secondary_bus = get_secondary_bus(bus,device,function);
		check_bus(secondary_bus);
	}
	
	pci_device_addresses[pci_device_count] = reg_base_address;
	pci_device_count++;
}


int pci_io_get(pci_dev_address device_address)
{
	int pci_reg_base = device_address.enable << 31 |
			   device_address.bus << 16 |
			   device_address.device << 11 |
			   device_address.function << 8 |
			   device_address.offset; 
	
	outl(pci_reg_base, 0xcf8);	
	return inl(0xcfc);
}

int get_vendor_id(int bus, int device, int function)
{
	pci_dev_address pci_reg_addr; 

	pci_reg_addr.enable = 1;
	pci_reg_addr.bus = bus;
	pci_reg_addr.device = device;
	pci_reg_addr.function = function;
	pci_reg_addr.offset = 0;

	return pci_io_get(pci_reg_addr) & 0xffff;
}
int get_device_id(int bus, int device, int function)
{
	pci_dev_address pci_reg_addr; 

	pci_reg_addr.enable = 1;
	pci_reg_addr.bus = bus;
	pci_reg_addr.device = device;
	pci_reg_addr.function = function;
	pci_reg_addr.offset = 0;

	int reg = pci_io_get(pci_reg_addr);
	return (reg >> 16) ;

}
int get_header_type(int bus, int device, int function)
{
	pci_dev_address pci_reg_addr; 

	pci_reg_addr.enable = 1;
	pci_reg_addr.bus = bus;
	pci_reg_addr.device = device;
	pci_reg_addr.function = function;
	pci_reg_addr.offset = 0xc;

	int reg = pci_io_get(pci_reg_addr);

	return (reg >> 16) & 0xff;
}

int get_base_class(int bus, int device, int function)
{
	pci_dev_address pci_reg_addr; 

	pci_reg_addr.enable = 1;
	pci_reg_addr.bus = bus;
	pci_reg_addr.device = device;
	pci_reg_addr.function = function;
	pci_reg_addr.offset = 0x8;

	int reg = pci_io_get(pci_reg_addr);

	return (reg >> 24) & 0xff;
}

int get_sub_class(int bus, int device, int function)
{
	pci_dev_address pci_reg_addr; 

	pci_reg_addr.enable = 1;
	pci_reg_addr.bus = bus;
	pci_reg_addr.device = device;
	pci_reg_addr.function = function;
	pci_reg_addr.offset = 0x8;

	int reg = pci_io_get(pci_reg_addr);

	return (reg >> 16) & 0xff;
}

int get_prog_if(int bus, int device, int function)
{
	pci_dev_address pci_reg_addr; 

	pci_reg_addr.enable = 1;
	pci_reg_addr.bus = bus;
	pci_reg_addr.device = device;
	pci_reg_addr.function = function;
	pci_reg_addr.offset = 0x8;

	int reg = pci_io_get(pci_reg_addr);

	return (reg >> 8) & 0xff;
}

int get_secondary_bus(int bus, int device, int function)
{
	pci_dev_address pci_reg_addr; 

	pci_reg_addr.enable = 1;
	pci_reg_addr.bus = bus;
	pci_reg_addr.device = device;
	pci_reg_addr.function = function;
	pci_reg_addr.offset = 0x18;

	int reg = pci_io_get(pci_reg_addr);

	return (reg >> 8) & 0xff;
}

void* get_base_address(int bus, int device, int function, int index)
{	
	pci_dev_address pci_reg_addr; 

	pci_reg_addr.enable = 1;
	pci_reg_addr.bus = bus;
	pci_reg_addr.device = device;
	pci_reg_addr.function = function;
	pci_reg_addr.offset = 0x10 + (index*4);

	int header_type = get_header_type(bus,device,function);
	uint64 reg = (uint64) pci_io_get(pci_reg_addr);	
	reg &= 0xffffffff;
	
	return (void*) reg;
	
}
