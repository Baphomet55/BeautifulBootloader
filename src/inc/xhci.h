typedef enum{
	NO_OP,
	ENABLE_SLOT,	
	ADDRESS_DEVICE,
	CONFIGURE_ENDPOINT,
	EVALUATE_CONTEXT,
	
} commands;

typedef struct {
	uint32 dword_1;
	uint32 dword_2;
	uint32 pad_1 : 24;
	uint32 completion_code : 8;
	uint32 cycle : 1;
	uint32 pad_2 : 9;
	uint32 trb_type : 6;
	uint32 pad_3 : 16;
} event_trb;

typedef struct {
	uint32 dword_1;
	uint32 dword_2;
	uint32 dword_3;
	uint32 cycle : 1;
	uint32 pad_1 : 9;
	uint32 trb_type : 6;
	uint32 pad_2 : 16;

	
} command_trb;

typedef struct{
	uint8 cap_length;
	uint8 reserved;
	uint16 hci_version;
	uint32 hcs_params_1;
	uint32 hcs_params_2;
	uint32 hcs_params_3;
	uint32 hcc_params_1;
	uint32 db_off;
	uint32 rts_off;
	uint32 hcc_params_2;
	
} xhci_cap_registers;


typedef struct {
	uint32 port_sc;
	uint32 port_pmsc;
	uint32 port_li;
	uint32 port_hlpmc;
} xhci_port_register;

typedef struct {
	uint32 usb_command;
	uint32 usb_status;
	uint32 page_size;
	uint32 reserved_1[2];
	uint32 dn_ctrl;
	command_trb* crcr;
	uint32 reserved_2[4];
	uint64* dcbaap;
	uint32 config;
	uint32 reserved_3[241];
	xhci_port_register port_reg;
	
} xhci_op_registers;

typedef struct{
	uint32 ip : 1;
	uint32 ie : 1;
	uint32 reserved_1 : 30;
	uint32 int_mod_interval : 16;
	uint32 int_mod_counter : 16; 
	uint32 erst_size : 16;
	uint32 reserved_2 : 16;
	uint32 reserved_3;
	uint32 reserved_4 : 6;
	uint64 erst_base_address : 58;
	uint32 desi : 3;
	uint32 ehb : 1;
	uint64 erdq_p : 60;
	 
	
} interrupt_register;

typedef struct{
	uint32 mf_index;
	uint32 reserved[7];
	interrupt_register int_reg_set;
} xhci_rt_registers;

typedef struct{
	uint32 db_target : 8;
	uint32 reserved : 8;
	uint32 db_task_id : 16;
} doorbell_register;

typedef doorbell_register* xhci_doorbell_registers;

typedef struct {
	uint32 route_string : 20;
	uint32 speed : 4;
	uint32 reserved_1 : 1;
	uint32 mtt : 1;
	uint32 hub : 1;
	uint32 context_entries : 5;
	uint32 max_exit_latency : 16;
	uint32 root_hub_port_number : 8;
	uint32 number_of_ports : 8;
	uint32 tt_hub_slot_id : 8;
	uint32 tt_port_number : 8;
	uint32 ttt : 2;
	uint32 reserved_2 : 4;
	uint32 interrupter_target : 10;
	uint32 usb_device_address : 8;
	uint32 reserved_3 : 19;
	uint32 slot_state : 5;
	uint32 reserved_4[4];
} slot_context_t;

typedef struct {
	uint32 ep_state : 3;
	uint32 reserved_1 : 5;
	uint32 mult : 2;
	uint32 max_p_streams : 5;
	uint32 lsa : 1;
	uint32 interval : 8;
	uint32 max_esit_payload_hi : 8;
	uint32 reserved_2 : 1;
	uint32 c_err : 2;
	uint32 ep_type : 3;
	uint32 reserved_3 : 1;
	uint32 hid : 1;
	uint32 max_burst_size : 8;
	uint32 max_packet_size : 16;
	uint32 dcs : 1;
	uint32 reserved_4 : 3;
	uint64 tr_dequeue_p : 60;
	uint32 average_trb_len : 16;
	uint32 max_esit_payload_lo : 16;
	uint32 reserved_5[3];
} endpoint_context_t;


typedef struct {
	slot_context_t slot_context;
	endpoint_context_t endpoint_context[31];
} device_context;

int load_xhci_driver();

int reset_hc();

int start_hc();

int init_hc();

int dequeue_event(int, event_trb**);

int enqueue_command(commands);

long init_device(int);

int init_device_slot(int);

int dev_conf(uint32*);

int add_contexts(uint32);
