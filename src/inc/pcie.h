typedef struct {
	uint16 vendor_id;
	uint16 device_id;
	uint16 command;
	uint16 status;
	uint8 revision_id;
	uint8 prog_if;
	uint8 subclass;
	uint8 class_code;
	uint8 cache_line_size;
	uint8 latency_timer;
	uint8 header_type;
	uint8 bist;
} pci_common_header;

typedef struct{
	uint32 enable : 1;
	uint32 reserved : 7;
	uint32 bus : 8;
	uint32 device : 5;
	uint32 function : 3;
	uint32 offset : 8;
} pci_dev_address;

int get_vendor_id(int, int, int);
int get_device_id(int, int, int);
int get_header_type(int, int, int);
int get_base_class(int, int, int);
int get_sub_class(int, int, int);
int get_prog_if(int, int, int);
int get_secondary_bus(int, int, int);
void* get_base_address(int,int,int,int);
int pci_io_get(pci_dev_address);

int pci_get_header(int,int,int,pci_common_header*);

void check_function(int, int, int);

void check_device(int, int);

void check_bus(int);

pci_dev_address* enumerate_pci_bus();
pci_dev_address find_device(int, int, int);
