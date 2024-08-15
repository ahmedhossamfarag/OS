#define KERNEL_OFFSET 0xA000000
#define KERNEL_END 0x10000000
#define DRIVERS_OFFSET 0xF0000000


#define MULTIBOOT_MAGIC 0x2BADB002
#define MYBOOTLOADER_MAGIC 0x12345678

#include <stdint.h>

typedef struct  {
    uint64_t baseAddr;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
}memory_region_t;


typedef struct  {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed)) vbe_mode_info_t;

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    struct {
        uint32_t tabsize;
        uint32_t strsize;
        uint32_t addr;
        uint32_t reserved;
    } syms;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    // Framebuffer information
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;       // Bits per pixel
    uint8_t framebuffer_type;
    union {
        struct {
            uint32_t framebuffer_red_field_position;
            uint32_t framebuffer_red_mask_size;
            uint32_t framebuffer_green_field_position;
            uint32_t framebuffer_green_mask_size;
            uint32_t framebuffer_blue_field_position;
            uint32_t framebuffer_blue_mask_size;
        };
        struct {
            uint32_t framebuffer_palette_addr;
            uint32_t framebuffer_palette_num_colors;
        };
    };
} __attribute__((packed)) multiboot_info_t;

typedef struct
{
    uint32_t FameBufferBase;
    uint32_t FrameBufferSize;
    uint32_t Width;
    uint32_t Height;
    uint32_t PixelFormat;
    uint32_t PixelsPerScanLine;
    uint32_t RedMask;
    uint32_t GreenMask;
    uint32_t BlueMask;                  
} graphics_info_t;

typedef struct
{
    uint32_t MomorySizeInMB;
    uint32_t RSDP;
} memory_info_t;


void info_init();

uint32_t info_get_processor_no();

uint32_t info_get_memory_size();

void info_add_apic_id();

uint32_t info_get_apic_id(uint8_t i);

uint8_t info_get_processor_id();

graphics_info_t* info_get_graphics();

uint32_t info_get_rsdp();

uint8_t info_is_gpt();

#pragma region VESA

#define VESA_MODE 0xD000
#define VESA_MODE_INFO 0xD100
#define VESA_FONT_MAP 0xE000

#pragma endregion