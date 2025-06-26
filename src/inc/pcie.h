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

int get_vendor_id(int, int, int);
int get_device_id(int, int, int);
int get_header_type(int, int, int);
int get_base_class(int, int, int);
int get_sub_class(int, int, int);
int get_secondary_bus(int, int, int);

int pci_get_header(int,int,int,pci_common_header*);

void check_function(int, int, int);

void check_device(int, int);

void check_bus(int);

uint64* enumerate_pci_bus();
