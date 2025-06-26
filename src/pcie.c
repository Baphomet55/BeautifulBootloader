#include "uefi.h"
#include "palloc.h"
#include "io.h"
#include "pcie.h"


int get_vendor_id(int bus, int device, int function)
{
		int pci_reg_base = (8 << 28) | (bus << 16) | (device << 11) | (function << 8) | 0;
		outl(pci_reg_base, 0xcf8);	
		int reg = inl(0xcfc);

		return reg & 0xffff;
}
int get_device_id(int bus, int device, int function)
{
		int pci_reg_base = (8 << 28) | (bus << 16) | (device << 11) | (function << 8) | 0;
		outl(pci_reg_base, 0xcf8);	
		int reg = inl(0xcfc);
	
		return (reg >> 16) & 0xffff;

}
int get_header_type(int bus, int device, int function)
{
		int pci_reg_base = (8 << 28) | (bus << 16) | (device << 11) | (function << 8) | 0xc;
		outl(pci_reg_base, 0xcf8);	
		int reg = inl(0xcfc);

		return (reg >> 16);
}

int get_base_class(int bus, int device, int function)
{
		int pci_reg_base = (8 << 28) | (bus << 16) | (device << 11) | (function << 8) | 0x8;
		outl(pci_reg_base, 0xcf8);	
		int reg = inl(0xcfc);

		return (reg >> 24) & 0xff;
}

int get_sub_class(int bus, int device, int function)
{
		int pci_reg_base = (8 << 28) | (bus << 16) | (device << 11) | (function << 8) | 0x8;
		outl(pci_reg_base, 0xcf8);	
		int reg = inl(0xcfc);

		return (reg >> 16) & 0xff;
}

int get_secondary_bus(int bus, int device, int function)
{
		int pci_reg_base = (8 << 28) | (bus << 16) | (device << 11) | (function << 8) | 0x18;
		outl(pci_reg_base, 0xcf8);	
		int reg = inl(0xcfc);

		return (reg >> 8) & 0xff;
}


int pci_get_header(int bus, int device, int function, pci_common_header* pci_buffer)
{
	uint32* buffer = (uint32*) pci_buffer;
	for(int i = 0; i < 4; i++)
	{
		int pci_reg_base = (8 << 28) | (bus << 16) | (device << 11) | (function << 8) | i << 2;
		outl(pci_reg_base, 0xcf8);	
		int reg = inl(0xcfc);

		buffer[i] = reg;
	}

	return pci_buffer->header_type & 0x7f;

}

static int pci_device_count = 0;
static uint64* pci_device_addresses;
void check_function(int bus, int device, int function)
{
	int class_code = get_base_class(bus,device,function);
	int subclass_code = get_sub_class(bus,device,function);
	
	if(class_code == 0x6 && subclass_code == 0x4)
	{
		int secondary_bus = get_secondary_bus(bus,device,function);
		check_bus(secondary_bus);
	}
	
	*(pci_device_addresses+pci_device_count) = (bus << 8) | (device << 4) | function;
	pci_device_count++;
}

void check_device(int bus, int device)
{
	unsigned char function = 0;	

	int vendor_id = get_vendor_id(bus,device,function);
	if(vendor_id == 0xffff) return;
	check_function(bus,device,function);
	int header_type = get_header_type(bus, device, function);

	if((header_type & 0x80) != 0)
	{
		for(function = 1; function < 8; function++)
		{
			if(get_vendor_id(bus,device,function) != 0xffff)
				check_function(bus, device, function);
		}
	}
}

void check_bus(int bus)
{
	for(int device = 0; device < 32; device++)
	{
		check_device(bus, device);
	}
}

uint64* enumerate_pci_bus()
{
	pci_device_addresses = (uint64*) alloc_page();
	check_bus(0);	
	return (uint64*) pci_device_addresses;	
}


