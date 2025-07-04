#include "uefi.h"
#include "pcie.h"
#include "xhci.h"
#include "phys_mem_m.h"

static xhci_cap_registers* cap_reg_address;
static xhci_op_registers* op_reg_address;
static xhci_rt_registers* runtime_reg_address;
static xhci_port_register* port_registers;
static interrupt_register* int_registers;
static xhci_doorbell_registers db_registers;

static event_trb* event_ring;
static command_trb* command_ring_eq_p;
static device_context* dc_array;

static void* output_device_contexts;
static void* input_device_context;

int load_xhci_driver()
{
	pci_dev_address xhci_addr = find_device(0xc, 0x3, 0x30);
	if(xhci_addr.enable == 0) return -1;

	cap_reg_address = (xhci_cap_registers*) get_base_address(
		xhci_addr.bus,
		xhci_addr.device,
		xhci_addr.function,
		0
	);
	
	uint8* reg_offset = (uint8*) cap_reg_address + cap_reg_address->cap_length;
	op_reg_address = (xhci_op_registers*) reg_offset;
	
	reg_offset = (uint8*) cap_reg_address + cap_reg_address->db_off;
	db_registers = (xhci_doorbell_registers) reg_offset;

	reg_offset = (uint8*) cap_reg_address + cap_reg_address->rts_off;
	runtime_reg_address = (xhci_rt_registers*) reg_offset;

	int_registers = &runtime_reg_address->int_reg_set;

	port_registers = &op_reg_address->port_reg;
	
	return 0;
}

int reset_hc()
{
	op_reg_address->usb_command |= 2;
	return 0;
}

int start_hc()
{
	op_reg_address->usb_command |= 1;
	return 0;
}

int init_hc()
{
	reset_hc();
	
	while((op_reg_address->usb_status & 0x800)) {};

	op_reg_address->config |= (cap_reg_address->hcs_params_1 & 0xff);	

	uint64 xhci_page = (uint64) alloc_page();
	op_reg_address->dcbaap = (uint64*) xhci_page;
	op_reg_address->dcbaap[0] = 0;
	xhci_page += 0x800;

	op_reg_address->crcr = (command_trb*) xhci_page;
	command_ring_eq_p = (command_trb*) xhci_page;
	
	xhci_page += 0x400;	

	int_registers[0].erdq_p = xhci_page >> 4;
	xhci_page += 0x3c0;
	*((uint64*) xhci_page) = xhci_page-0x3c0;
	*((uint64*) xhci_page+1) = 0x400;

	int_registers[0].erst_base_address = xhci_page >> 6;
	int_registers[0].erst_size = 1;

	start_hc();
	
	int max_dc = cap_reg_address->hcs_params_1 & 0xff;
	max_dc = (0x400 * max_dc) >> 12;
	//output_device_contexts = page_alloc(max_dc);
	//input_device_context = page_alloc();

		
	return max_dc;
}

int dequeue_event(int er_index, event_trb** event)
{
	while(!(int_registers[er_index].ip)) {}

	uint64 raw_event_ring_p = int_registers[er_index].erdq_p << 4;
	
	event_trb* event_ring_dq_p = (event_trb*) raw_event_ring_p;

	if(event_ring_dq_p->completion_code != 1)
		return event_ring_dq_p->completion_code;

	int_registers[er_index].erdq_p++;
	event_ring_dq_p->cycle = !event_ring_dq_p->cycle;

	int_registers[er_index].ip |= 1;

	if(event != 0)
		*(event) = event_ring_dq_p;
	return 0;
}



int enqueue_command(commands cmd)
{
	switch(cmd)
	{
		case NO_OP:
			command_ring_eq_p->trb_type = 23;	
		break;
		case ENABLE_SLOT:
			command_ring_eq_p->trb_type = 9;	
		break;
	}
	command_ring_eq_p++;
	return 0;
}

int handle_cmd(commands cmd)
{
	
	event_trb* event;
	db_registers[0].db_target = 0;	

	switch(cmd)
	{
		case ENABLE_SLOT:
			dequeue_event(0, &event);
		break;
	}

	return 0;

}

int init_device(int port_id)
{
	if(!(port_registers[port_id].port_sc & 1))
		return 1;

	if(!(port_registers[port_id].port_sc & 2))
	{
		port_registers[port_id].port_sc |= 0x10;
		dequeue_event(0, 0);
	}
	
	enqueue_command(ENABLE_SLOT);
	handle_cmd(ENABLE_SLOT);

	return 0;
}


