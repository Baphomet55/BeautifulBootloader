typedef unsigned long uint32;
typedef unsigned long uint64;
typedef unsigned long uintn;

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef long (*EFI_TEXT_RESET)
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, bool);

typedef long (*EFI_TEXT_STRING) 
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, unsigned short*);

typedef long (*EFI_TEXT_TEST_STRING)
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, short*);

typedef long (*EFI_TEXT_QUERY_MODE) 
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, uintn, uintn*, uintn*);

typedef long (*EFI_TEXT_SET_MODE)
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, uintn);

typedef long (*EFI_TEXT_SET_ATTRIBUTE)
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, uintn);

typedef long (*EFI_TEXT_CLEAR_SCREEN)
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*);

typedef long (*EFI_TEXT_SET_CURSOR_POSITION)
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, uintn, uintn);

typedef long (*EFI_TEXT_ENABLE_CURSOR)
(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, bool);

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
	EFI_TEXT_RESET Reset;
	EFI_TEXT_STRING OutputString;
	EFI_TEXT_TEST_STRING TestString;
	EFI_TEXT_QUERY_MODE QueryMode;
	EFI_TEXT_SET_MODE SetMode;
	EFI_TEXT_SET_ATTRIBUTE SetAttribute;
	EFI_TEXT_CLEAR_SCREEN ClearScreen;
	EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
	EFI_TEXT_ENABLE_CURSOR EnableCursor;
	void* Mode;
};

typedef struct {
	unsigned long signature;
	unsigned int revision;
	unsigned int header_size;
	unsigned int crc;
	unsigned int reserved;
} EFI_TABLE_HEADER;

typedef enum {
	AllocateAnyPages,
	AllocateMaxAddress,
	AllocateAddress,
	MaxAllocateType
} EFI_ALLOCATE_TYPE;

typedef enum {
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef struct {
	uint32 type;
	uint64* physical_address;
	uint64* virtual_address;
	uint64 number_of_pages;
	uint64 attribute;
	uint64 reserved;
} EFI_MEMORY_DESCRIPTOR;

typedef long (*EFI_ALLOCATE_PAGES)
(EFI_ALLOCATE_TYPE, EFI_MEMORY_TYPE, uintn, uint64*);

typedef long (*EFI_FREE_PAGES)
(uintn, uintn);

typedef long (*EFI_GET_MEMORY_MAP)
(uintn*, EFI_MEMORY_DESCRIPTOR*, uintn*, uintn*, uint32*);

typedef long (*EFI_ALLOCATE_POOL)
(EFI_MEMORY_TYPE, uintn, void**);

typedef long (*EFI_FREE_POOL)
(void*);

typedef long (*EFI_EXIT_BOOT_SERVICES)
(void*, uintn);

typedef long (*EFI_SET_WATCHDOG_TIMER)
(uintn, uint64, uintn, short*);

typedef long (*EFI_CALCULATE_CRC32)
(void*, uintn, uint32*);

typedef struct {
	EFI_TABLE_HEADER hdr;
	// Task Priority Services
	void* RaiseTPL;
	void* RestoreTPL;
	// Memory Services
	EFI_ALLOCATE_PAGES AllocatePages;
	EFI_FREE_PAGES FreePages;
	EFI_GET_MEMORY_MAP GetMemoryMap;
	EFI_ALLOCATE_POOL AllocatePool;
	EFI_FREE_POOL FreePool;
	// Event & Timer Services
	void* CreateEvent;
	void* SetTimer;
	void* WaitForEvent;
	void* SignalEvent;
	void* CloseEvent;
	void* CheckEvent;
	// Protocol Handler Services
	void* InstallProtocolInterface;
	void* ReinstallProtocolInterface;
	void* UninstallProtocolInterface;
	void* HandleProtocol;
	void* Reserved;
	void* RegisterProtocolNotify;
	void* LocateHandle;
	void* LocateDevicePath;
	void* InstallConfigurationTable;
	// Image Services
	void* ImageLoad;
	void* ImageStart;
	void* Exit;
	void* ImageUnload;
	EFI_EXIT_BOOT_SERVICES ExitBootServices;
	// Miscellaneous Services
	void* GetNextMonotonicCount;
	void* Stall;
	EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;
	// DriverSupport Services
	void* ConnectController;
	void* DisconnectController;
	// Open and Close Protocol Services
	void* OpenProtocol;
	void* CloseProtocol;
	void* OpenProtocolInterface;
	// Library Services
	void* ProtocolsPerHandle;
	void* LocateHandleBuffer;
	void* LocateProtocol;
	void* InstallMultipleProtocolIntrefaces;
	void* UninstallMultipleProtocolIntrefaces;
	// 32-bit CRC Services
	EFI_CALCULATE_CRC32 CalculateCrc32;
	// Miscellaneous Services
	void* CopyMem;
	void* SetMem;
	void* CreateEventEx;

	
} EFI_BOOT_SERVICES;

typedef struct {
	EFI_TABLE_HEADER Hdr;
	short* FirmwareVendor;
	uint32 FirmwareRevision;
	void* ConsoleInHandle;
	void* ConIn;
	void* ConsoleOutHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
	void* StandardErrorHandle;
	void* StdError;
	void* RuntimeServices;
	EFI_BOOT_SERVICES* BootServices;
	uint64 NumberOfTableEntries;
	void* ConfigurationTable;
	
} EFI_SYSTEM_TABLE;
