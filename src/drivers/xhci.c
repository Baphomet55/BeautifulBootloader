#include "uefi.h"
#include "pcie.h"
#include "xhci.h"
#include "phys_mem_m.h"
#include "virt_mem_m.h"

static xhci_cap_registers* cap_reg_address;
static xhci_op_registers* op_reg_address;
static xhci_rt_registers* runtime_reg_address;
static xhci_port_register* port_registers;
static interrupt_register* int_registers;
static xhci_doorbell_registers db_registers;

static event_trb* event_ring;
static command_trb* command_ring_eq_p;
static device_context** dc_array;

static uint32* output_device_contexts;
static uint32* input_device_context;

static uint64* vaddr_space;

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

	vaddr_space = (uint64*) alloc_vaddr_space();
			
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

	int max_dc = cap_reg_address->hcs_params_1 & 0xff;
	max_dc = (0x400 * max_dc) >> 12;

	for(int i = 0; i < max_dc + 2; i++)
	{
		alloc_virt_mem((uint64)vaddr_space);		
	}

	reset_hc();
	
	while((op_reg_address->usb_status & 0x800)) {};

	op_reg_address->config |= (cap_reg_address->hcs_params_1 & 0xff);	

	uint64 xhci_page = virt_to_phys((uint64) vaddr_space);
	op_reg_address->dcbaap = (uint64*) xhci_page;
	op_reg_address->dcbaap[0] = 0;
	xhci_page += 0x800;

	op_reg_address->crcr = (command_trb*) xhci_page;
	command_ring_eq_p = (command_trb*) xhci_page;

	command_ring_eq_p[15].dword_1 = (uint32) command_ring_eq_p; 
	command_ring_eq_p[15].trb_type = 6;
	command_ring_eq_p[15].pad_1 = 1;
	
	xhci_page += 0x400;	

	int_registers[0].erdq_p = xhci_page >> 4;
	xhci_page += 0x3c0;
	*((uint64*) xhci_page) = xhci_page-0x3c0;
	*((uint64*) xhci_page+1) = 0x400;

	int_registers[0].erst_base_address = xhci_page >> 6;
	int_registers[0].erst_size = 1;

	start_hc();

	input_device_context = (uint32*) ((uint64)vaddr_space+0x1000);
	output_device_contexts = (uint32*) ((uint64) vaddr_space+0x2000);
		
	return 0;
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
		case ADDRESS_DEVICE:
			command_ring_eq_p->dword_1 = virt_to_phys((uint64)input_device_context);
			command_ring_eq_p->pad_2 = 0x100;
			command_ring_eq_p->trb_type = 11;
			command_ring_eq_p->cycle = 1;
		break;
		case CONFIGURE_ENDPOINT:
			command_ring_eq_p++;
			command_ring_eq_p->dword_1 = virt_to_phys((uint64) input_device_context);
			command_ring_eq_p->trb_type = 12;
			command_ring_eq_p->pad_2 = 0x100;
			command_ring_eq_p->cycle = 1;
		break;
		case EVALUATE_CONTEXT:
			command_ring_eq_p++;
			command_ring_eq_p->dword_1 = virt_to_phys((uint64) input_device_context);
			command_ring_eq_p->trb_type = 13;
			command_ring_eq_p->pad_2 = 0x100;
			command_ring_eq_p->cycle = 1;
		break;
	}
	return 0;
}

int handle_cmd(commands cmd)
{
	
	event_trb* event;
	db_registers[0].db_target = 0;	
		
	int ret = 0;
	switch(cmd)
	{
		case ENABLE_SLOT:
			dequeue_event(0, &event);
			int slot_id = event->pad_3 >> 7;
			ret =init_device_slot(slot_id);
		break;
	}
	return ret;

}

int init_device_slot(int slot_id)
{
	uint64 default_ctrl_ep = virt_to_phys((uint64) vaddr_space+0x3000);

	*(input_device_context) = 0;
	*(input_device_context+1) = 0x3;
	*(input_device_context+8) = 1 << 27;
	*(input_device_context+9) = 0x10000;
	*(input_device_context+17) = 0x400026; 
	*(input_device_context+18) = default_ctrl_ep | 1; 

	op_reg_address->dcbaap[1] = virt_to_phys((uint64) output_device_contexts);
	return (int) default_ctrl_ep;
}

long init_device(int port_id)
{
	if(!(port_registers[port_id].port_sc & 1))
		return 1;

	if(!(port_registers[port_id].port_sc & 2))
	{
		port_registers[port_id].port_sc |= 0x10;
		dequeue_event(0, 0);
	}
	
	int ret = 0;
	enqueue_command(ENABLE_SLOT);
	ret = handle_cmd(ENABLE_SLOT);

	enqueue_command(ADDRESS_DEVICE);
	handle_cmd(ADDRESS_DEVICE);


	return ret;
}

int set_configuration(uint32* ctrl_ep)
{
	*(ctrl_ep) = 0x0010900;
	*(ctrl_ep+2) = 0x8;
	*(ctrl_ep+3) = 0x30841;

	*(ctrl_ep+7) = 0x1021;

	db_registers[1].db_target = 1;
}

int get_descriptor(uint32* ctrl_ep)
{
	*(ctrl_ep) = 0x2000680;
	*(ctrl_ep+1) = 0x200000;
	*(ctrl_ep+2) = 0x8;
	*(ctrl_ep+3) = 0x30841;

	*(ctrl_ep+4) = (uint32) ctrl_ep + 0x200;
	*(ctrl_ep+6) = 0x20;
	*(ctrl_ep+7) = 0x10c01;

	*(ctrl_ep+11) = 0x1021;

	db_registers[1].db_target = 1;
}

int dev_conf(uint32* ctrl_ep)
{
	
	enqueue_command(CONFIGURE_ENDPOINT);
	handle_cmd(0);

	set_configuration(ctrl_ep);

	//get_descriptor(ctrl_ep);
	return 0x6969;
}

int add_contexts(uint32 add_flags)
{
	uint64 out_ep = virt_to_phys((uint64) vaddr_space+0x4000);
	uint64 in_ep = virt_to_phys((uint64) vaddr_space+0x5000);
	*(input_device_context+1) = 4;
	
	*(input_device_context+8) = 0x18000000;

	*(input_device_context+25) = 0x2000016;
	*(input_device_context+26) = (uint32) out_ep | 1;

	enqueue_command(CONFIGURE_ENDPOINT);
	handle_cmd(0);
	set_configuration((uint32*) 0xf020);

	*(input_device_context+1) = 0xc;

	*(input_device_context+33) = 0x2000036;
	*(input_device_context+34) = (uint32) in_ep | 1;

	enqueue_command(CONFIGURE_ENDPOINT);
	handle_cmd(0);
	set_configuration((uint32*) 0xf040);
	return out_ep;
}

int endpoint_test(uint32* out_ep)
{
	uint32* data_buffer_o = (uint32*) virt_to_phys((uint64) vaddr_space+0x6000);
	uint32* data_buffer_i = (uint32*) virt_to_phys((uint64) vaddr_space+0x7000);

	uint32* in_ep = (uint32*) 0x11000;

	*(out_ep) = (uint32) data_buffer_o;
	*(out_ep+1) = 0;
	*(out_ep+2) = 0x1f;
	*(out_ep+3) = 0x421;

	*(data_buffer_o) = 0x43425355;
	*(data_buffer_o+1) = 0x69;
	*(data_buffer_o+2) = 0x8000;
	*(data_buffer_o+3) = 0x280a0080;
	*(data_buffer_o+4) = 0x00000000;
	*(data_buffer_o+5) = 0x40000000;

	*(in_ep) = (uint32) data_buffer_i;
	*(in_ep+1) = 0;
	*(in_ep+2) = 0x8000;
	*(in_ep+3) = 0x421;
	
	if(int_registers[0].ip == 1) { int_registers[0].ip = 0;}
	db_registers[1].db_target = 2;	
	
	while(int_registers[0].ip == 0) {;}
	
	db_registers[1].db_target = 3;
	return 0;
}

