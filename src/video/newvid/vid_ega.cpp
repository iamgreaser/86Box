#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <86box/86box.h>
#include <86box/device.h>
extern "C" {
#include <86box/io.h>
#include <86box/mem.h>
#include <86box/rom.h>
}
#include <86box/timer.h>

#include <86box/video.h>

#include "ega_regs.h"

#define DISPLAY_RGB          0
#define DISPLAY_COMPOSITE    1
#define DISPLAY_RGB_NO_BROWN 2
#define DISPLAY_GREEN        3
#define DISPLAY_AMBER        4
#define DISPLAY_WHITE        5

static video_timings_t timing_ega = { .type = VIDEO_ISA, .write_b = 8, .write_w = 16, .write_l = 32, .read_b = 8, .read_w = 16, .read_l = 32 };

#define BIOS_IBM_PATH "roms/video/ega/ibm_6277356_ega_card_u44_27128.bin"

// IMPORTANT REFERENCE: "OA - IBM Enhanced Graphics Adapter.pdf" - dated 1984-08-02.
// This is the official IBM reference.
// There's a lot of stuff in it that is completely wrong.
// But it DOES have a schematic you can (and SHALL) peruse.
// References to various sheets are as follows:
// - Enhanced Graphics Adapter Sheet x = SHT x (PDF pg 90+x, printed pg 86+x)
// - Graphics Memory Expansion Card Sheet x = MEMSHT x (PDF pg 101+x, printed pg 97+x)

struct ega_ar_t {
    uint8_t cpu_addr;
    bool    ff_is_data;
    bool    is_video;

    // The 4-bit to 6-bit palette.
    uint8_t pal[1 << 4];

    uint8_t ar10_mode;
    uint8_t ar11_overscan_color;
    uint8_t ar12_plane_enable;
    uint8_t ar13_pel_panning;
};

struct ega_sr_t {
    uint8_t cpu_addr;

    uint8_t sr02_map_mask;
    uint8_t sr04_memory_mode;
};

//
// The Graphics Controllers are responsible for three completely different things:
// - Processing and filtering data going to and from the CPU.
// - Latching in and shifting out pixel data in graphics modes.
//   (In text modes, the attribute controller handles this instead.)
// - Some miscellaneous output pins which control other parts of the circuit.
//   (This is what GR06 does.)
//
// Yes, that does say "controllers". Plural. That is not a typo.
// In EGA, there are 2 of these chips and they handle 2 planes each.
//

struct ega_gr_t {
    uint8_t cpu_addr;

    // Which Graphics Controller (GC) does each GC think it is?
    // - GC #1 should be set to 0.
    // - GC #2 should be set to 1.
    // Anything else will be weird.
    // Registers:
    // - 03CC W, low 2 bits: Set GC #1 position
    // - 03CA W, low 2 bits: Set GC #2 position
    uint8_t position[2];

    // All of these are 2 bits broadcast 4 times.
    // That is, these are the only valid values:
    // - 00 = 0x00
    // - 01 = 0x55
    // - 10 = 0xAA
    // - 11 = 0xFF
    uint8_t gr00_set_reset[2];
    uint8_t gr01_enable_set_reset[2];
    uint8_t gr02_color_compare[2];
    uint8_t gr06_misc[2];
    uint8_t gr07_color_dont_care[2];

    // These are shared among both GCs.
    uint8_t gr03_data_rotate;
    uint8_t gr04_read_map_select; // Low 3 bits are the map.
    uint8_t gr05_mode;
    uint8_t gr08_bit_mask;

    // And here's our read latch data.
    uint32_t latch;
};

struct ega_cr_t {
    uint8_t cpu_addr;

    // Address settings
    uint16_t start_vaddr;
    uint16_t cursor_vaddr;

    // CRxx horizontal timing registers
    uint16_t htotal;
    uint16_t hdispend;
    uint16_t hblankbeg;
    uint16_t hsyncbeg;
    uint16_t hsyncend;
    uint16_t hblankend;

    // CRxx vertical timing registers
    uint16_t vtotal;
    uint16_t vdispend;
    uint16_t vblankbeg;
    uint16_t vsyncbeg;
    uint16_t vsyncend;
    uint16_t vblankend;
    uint16_t linecompare;

    // CRxx skew registers
    uint8_t skew_disp;
    uint8_t skew_hsync;
    uint8_t skew_cursor;

    // Character height handling
    uint8_t vfinescroll;
    uint8_t vnextcharidx;
    uint8_t cursorbeg;
    uint8_t cursorend;
    uint8_t underlinepos;

    // Display pitch
    uint16_t offset;

    // Interrupt handling
    bool clearvint;
    bool enablevint;

    // Mode Control
    uint8_t a13_bits_from_scanline;
    uint8_t vdivide;
    uint8_t addrdivide;
    bool    floatoutputs;
    uint8_t wrapbit;
    uint8_t addrshift;
    bool    hwreset;

    // Other settings
    uint8_t hoeoffs; // Horizontal Odd/Even Offset
};

struct ega_t {
    uint32_t     *vram_buf;
    size_t        vram_size_vaddrs;
    mem_mapping_t vram_mapping;

    uint8_t    monitor_type;
    rom_t      bios_rom;
    pc_timer_t scan_timer;

    // Registers

    // Attribute Controller (ARxx)
    ega_ar_t ar;

    // Miscellaneous Output Register (U37, 74LS273 8-bit D-latch, SHT 9)
    // Default: All 0 (confirmed on schematic), which means:
    // - EGA_W3C2_IOBASE_3BX
    // - EGA_W3C2_RAMENABLE_OFF
    // - EGA_W3C2_CLOCKSEL_14MHZ
    // - EGA_W3C2_VIDDRIVERS_ON
    // - EGA_W3C2_OEPAGE_HI
    // - EGA_W3C2_POLARITY_VP_HP_200
    uint8_t misc_out_3c2;

    // Sequencer (SRxx)
    ega_sr_t sr;

    // Graphics Controllers (GRxx)
    // EGA has 2 of them, they control 2 planes each.
    // And you can potentially have up to 4.
    ega_gr_t gr;

    // CRT Controller (CRTC) (CRxx)
    ega_cr_t cr;

    // Feature Control Register (U49, 74LS175 4-bit D-latch, SHT 9)
    // Only bits 0-1 are used.
    // Bits 2-3 are written, but not used.
    // Default: All 0 (confirmed on schematic), which means:
    // - EGA_W3XA_FEATCTRL_00
    uint8_t feat_ctrl_3xa;

    uint8_t status_in_3xa;
};

static const double ega_clock_frequencies_hz[4] = {
    // 00: ~14 MHz, from the ISA bus
    (157500000.0 / 11.0),
    // 01: ~16 MHz, on the card itself
    16257000.0,
    // 10: External oscillator on the feature connector, in practice this is floating
    0.0,
    // 11: Floating
    0.0,
};

static void ega_tick_frame(void *priv);

static void ega_update_output(ega_t *ega);
static void ega_update_screen_timings(ega_t *ega);

static uint8_t ega_vram_read(uint32_t addr, void *priv);
static void    ega_vram_write(uint32_t addr, uint8_t val, void *priv);
static uint8_t ega_io_in(uint16_t addr, void *priv);
static void    ega_io_out(uint16_t addr, uint8_t val, void *priv);

static void
ega_close(void *priv)
{
    ega_t *ega = (ega_t *) priv;

    // Stop timers
    timer_on_auto(&ega->scan_timer, 0.0);

    // Free substructures
    if (ega->vram_buf != NULL) {
        free(ega->vram_buf);
        ega->vram_buf = NULL;
    }

    // Free the outer structure
    free(ega);
}

static void *
ega_init(const device_t *info)
{
    ega_t *ega = (ega_t *) calloc(1, sizeof(ega_t));

    ega->monitor_type = device_get_config_int("monitor_type");

    video_inform(VIDEO_FLAG_TYPE_SPECIAL, &timing_ega);

    rom_init(&ega->bios_rom, BIOS_IBM_PATH,
             0xc0000, 0x8000, 0x7fff, 0, MEM_MAPPING_EXTERNAL);

    // Overallocate and fill with 0xFF to make it easier to render
    ega->vram_size_vaddrs = (device_get_config_int("memory") * 1024) >> 2;
    ega->vram_buf         = (uint32_t *) calloc(256 * 1024, 1);
    memset(ega->vram_buf, 0xFF, 256 * 1024);
    mem_mapping_add(&ega->vram_mapping,
                    0xa0000, 0x20000,
                    ega_vram_read, NULL, NULL,
                    ega_vram_write, NULL, NULL,
                    NULL, MEM_MAPPING_EXTERNAL, ega);
    mem_mapping_disable(&ega->vram_mapping);

    // Default: EGA_W3C2_IOBASE_3BX (0).
    io_sethandler(0x03B0, 0x0020,
                  ega_io_in, NULL, NULL,
                  ega_io_out, NULL, NULL,
                  ega);

    // Typical settings (WARNING: from the IBM EGA manual, which is known to contain errors!):
    // CGA 640 x 200: ~14MHz, 912 wide, 260 high
    // MDA 640 x 350: ~14MHz, 882 wide, 368 high
    // EGA 640 x 350: ~16MHz(?!), 744 wide, 364 high
    // MDA 720 x 350: ~16MHz, 882 wide, 368 high
    // EGA 720 x 350: ~16MHz, 837 wide, 364 high
    video_res_x = 640;
    video_res_y = 350;
    overscan_x  = 0;
    overscan_y  = 0;
    xsize       = video_res_x;
    ysize       = video_res_y;

    set_screen_size(xsize, ysize);
    if (video_force_resize_get())
        video_force_resize_set(0);

    timer_add(&(ega->scan_timer), ega_tick_frame, ega, 0);
    timer_on_auto(&ega->scan_timer, 1.0);
    return ega;
}

static int
ega_available(void)
{
    return rom_present(BIOS_IBM_PATH);
}

static void
ega_tick_frame(void *priv)
{
    ega_t *ega = (ega_t *) priv;

    ega_update_output(ega);

    uint32_t pal[1 << 4];
    for (int32_t i = 0; i < (1 << 4); i++) {
        uint8_t v = ega->ar.pal[i & 0xF];
        if (true) {
            // CGA monitor mapping
            pal[i] = makecol32(
                (((v >> 2) & 0x1) * 0xAA) | (((v >> 4) & 0x1) * 0x55),
                (((v >> 1) & 0x1) * 0xAA) | (((v >> 4) & 0x1) * 0x55),
                (((v >> 0) & 0x1) * 0xAA) | (((v >> 4) & 0x1) * 0x55));
        } else {
            // EGA monitor mapping
            pal[i] = makecol32(
                (((v >> 2) & 0x1) * 0xAA) | (((v >> 5) & 0x1) * 0x55),
                (((v >> 1) & 0x1) * 0xAA) | (((v >> 4) & 0x1) * 0x55),
                (((v >> 0) & 0x1) * 0xAA) | (((v >> 3) & 0x1) * 0x55));
        }
    }

    for (size_t y = 0; y < ysize; y++) {
        size_t memy = y / 14;
        size_t sy   = y % 14;
        for (size_t x = 0; x < xsize / 8; x++) {
            uint32_t vaddr = (memy * 80) + x;

            // Odd/Even mode
            // TODO: Read this from the CRTC --GM
            vaddr = (vaddr << 1) | ((vaddr >> 15) & 0b1);

            uint32_t data = ega->vram_buf[vaddr];

            // Remap text mode
            // TODO: Actually treat this how the EGA treats it instead of remapping text to graphics --GM
            uint8_t bg       = (data >> 12) & 0x0F;
            uint8_t fg       = (data >> 8) & 0x0F;
            uint8_t ch       = (data >> 0) & 0xFF;
            uint8_t fontline = (uint8_t) (ega->vram_buf[(((uint32_t) ch) << 5) | (sy & 0x1F)] >> 16);
            // if (sy == 0 && ch != 0x20) printf("ch %02X fg %01X bg %01X\n", ch, fg, bg);

            uint32_t mask_base = 0;
            uint32_t mask_xor  = 0;
            for (size_t i = 0; i < 4; i++) {
                if (((bg >> i) & 0b1) != 0) {
                    mask_base |= (0xFF << (i * 8));
                }
                if (((fg >> i) & 0b1) != 0) {
                    mask_xor |= (0xFF << (i * 8));
                }
            }
            mask_xor ^= mask_base;

            data = mask_base ^ ((0x01010101 * (uint32_t) fontline) & mask_xor);

            for (size_t sx = 0; sx < 8; sx++) {
                uint8_t c = 0;
                for (size_t i = 0; i < 4; i++) {
                    if (((data >> ((sx ^ 0b111) + (8 * i))) & 0b1) != 0) {
                        c |= (1 << i);
                    }
                }
                buffer32->line[y][(x * 8) + sx] = pal[c];
            }
        }
    }

    video_blit_memtoscreen(0, 0, xsize, ysize);

    // Refire after 1 frame
    timer_on_auto(&ega->scan_timer,
                  (1000000.0 * 912.0 * 260.0) / ega_clock_frequencies_hz[0]);
}

static void
ega_update_output(ega_t *ega)
{
    // TODO: Work out how much more we have to draw --GM
    (void) ega;
}

static void
ega_update_screen_timings(ega_t *ega)
{
    // TODO: Work out how much more we have to draw --GM
    // TODO: Adjust the actual timings --GM
    (void) ega;
}

static __inline uint32_t
ega_cpu_addr_to_vaddr(ega_t *ega, uint32_t addr)
{
    uint32_t vaddr = addr;

    // Remap given mapping
    switch (ega->gr.gr06_misc[1] & EGA_GR06_1_MEMORYMAP_MASK) {
        case EGA_GR06_1_MEMORYMAP_A000_128K:
            // We will read the source A16 explicitly for the 128K Odd/Even remap.
            vaddr &= 0x0FFFF;
            break;
        case EGA_GR06_1_MEMORYMAP_A000_64K:
            vaddr &= 0x0FFFF;
            break;
        case EGA_GR06_1_MEMORYMAP_B000_32K:
            vaddr &= 0x07FFF;
            break;
        case EGA_GR06_1_MEMORYMAP_B800_32K:
            vaddr &= 0x07FFF;
            break;
        default:
            __builtin_unreachable();
            break;
    }

    // Remap Odd/Even addresses
    if ((ega->gr.gr06_misc[0] & EGA_GR06_0_ODDEVEN_MASK) == EGA_GR06_0_ODDEVEN_ON) {
        if ((ega->gr.gr06_misc[1] & EGA_GR06_1_MEMORYMAP_MASK) == EGA_GR06_1_MEMORYMAP_A000_128K) {
            if (ega->vram_size_vaddrs > (64 * 1024 / 4)) {
                // Memory is expanded! ~A0 is ~A16.
                vaddr = (vaddr & ~0b1) | ((addr >> 16) & 0b1);
            }
            // If not expanded, ~A0 is ~A0.
        } else {
            if (ega->vram_size_vaddrs > (64 * 1024 / 4)) {
                // Memory is expanded! ~A0 is ~PGSEL.
                if ((ega->misc_out_3c2 & EGA_W3C2_OEPAGE_MASK) == EGA_W3C2_OEPAGE_HI) {
                    vaddr |= 0b1;
                } else {
                    vaddr &= ~0b1;
                }
            } else {
                // If not expanded, ~A0 is ~A14.
                vaddr = (vaddr & ~0b1) | ((addr >> 14) & 0b1);
            }
        }
    }

    // TODO: 64 KB wrap when set up in the sequencer --GM

    return vaddr;
}

static void
ega_map_vram(ega_t *ega, uint8_t map)
{
    switch (map & EGA_GR06_1_MEMORYMAP_MASK) {
        case EGA_GR06_1_MEMORYMAP_A000_128K:
            mem_mapping_set_addr(&ega->vram_mapping, 0xa0000, 0x20000);
            break;
        case EGA_GR06_1_MEMORYMAP_A000_64K:
            mem_mapping_set_addr(&ega->vram_mapping, 0xa0000, 0x10000);
            break;
        case EGA_GR06_1_MEMORYMAP_B000_32K:
            mem_mapping_set_addr(&ega->vram_mapping, 0xb0000, 0x08000);
            break;
        case EGA_GR06_1_MEMORYMAP_B800_32K:
            mem_mapping_set_addr(&ega->vram_mapping, 0xb8000, 0x08000);
            break;
        default:
            __builtin_unreachable();
            break;
    }
}

static void
ega_unmap_vram(ega_t *ega)
{
    mem_mapping_disable(&ega->vram_mapping);
}

static uint8_t
ega_vram_read(uint32_t addr, void *priv)
{
    ega_t *ega = (ega_t *) priv;

    // If VRAM is disabled, return open bus.
    if ((ega->misc_out_3c2 & EGA_W3C2_RAMENABLE_MASK) == EGA_W3C2_RAMENABLE_OFF) {
        return 0xFF;
    }

    // Get VRAM address number
    uint32_t vaddr = ega_cpu_addr_to_vaddr(ega, addr);

    // Get data
    uint32_t data = (vaddr < ega->vram_size_vaddrs)
        ? ega->vram_buf[vaddr]
        : ~0;

    // Update latch
    ega->gr.latch = data;

    // Actually return something
    switch (ega->gr.gr05_mode & EGA_GR05_READMODE_MASK) {
        case EGA_GR05_READMODE_0_SINGLE:
            {
                uint8_t result = 0xFF;
                if ((ega->gr.gr05_mode & EGA_GR05_ODDEVEN_MASK) == EGA_GR05_ODDEVEN_ON) {
                    // Odd/Even read address
                    data >>= (addr & 0x1) << 3;
                } else {
                    // Regular read address
                    data >>= (ega->gr.gr04_read_map_select & 0x1) << 3;
                }

                for (size_t i = 0; i < 2; i++) {
                    if ((((ega->gr.gr04_read_map_select >> 1) ^ ega->gr.position[i]) & 0x3) == 0) {
                        result &= (uint8_t) (data >> (i * 16));
                    }
                }
                return result;
            }

        case EGA_GR05_READMODE_1_COMPARE:
            {
                // Compute masks
                uint32_t mask_xor = 0x00000000;
                uint32_t mask_and = 0x00000000;
                for (size_t i = 0; i < 4; i++) {
                    if ((ega->gr.gr02_color_compare[i >> 1] & (1 << (i & 0b1))) != 0) {
                        mask_xor |= (0xFF << (i * 8));
                    }
                    if ((ega->gr.gr07_color_dont_care[i >> 1] & (1 << (i & 0b1))) != 0) {
                        mask_and |= (0xFF << (i * 8));
                    }
                }

                // Construct planes for comparison
                uint32_t planes = ~((data ^ mask_xor) & mask_and);

                // AND each byte together and return
                planes &= (planes >> 16);
                planes &= (planes >> 8);
                return (uint8_t) planes;
            }

        default:
            __builtin_unreachable();
            return 0xFF;
    }
}

static void
ega_vram_write(uint32_t addr, uint8_t val, void *priv)
{
    ega_t *ega = (ega_t *) priv;

    // If VRAM is disabled, do nothing.
    if ((ega->misc_out_3c2 & EGA_W3C2_RAMENABLE_MASK) == EGA_W3C2_RAMENABLE_OFF) {
        return;
    }

    // Get VRAM address number and ensure that it is in range
    uint32_t vaddr = ega_cpu_addr_to_vaddr(ega, addr);
    if (vaddr < ega->vram_size_vaddrs) {
        // Update VRAM if possible
        uint32_t result = 0xFFFFFFFF;
        switch (ega->gr.gr05_mode & EGA_GR05_WRITEMODE_MASK) {
            case EGA_GR05_WRITEMODE_0:
                {
                    result = 0x01010101 * (uint32_t) val;

                    // Apply rotate
                    uint32_t ror_amount = EGA_GR03_ROR_READ(ega->gr.gr03_data_rotate);
                    if (ror_amount != 0) {
                        // Optimisation: Only rotate if we need to!
                        uint32_t ror_lmask = 0x01010101 * (0xFF >> ror_amount);
                        uint32_t ror_rmask = ~ror_lmask;

                        result = 0
                            | ((result >> ror_amount) & ror_lmask)
                            | ((result << (8 - ror_amount)) & ror_rmask);
                    }

                    // Apply logic op
                    // from the IBM EGA doc:
                    // "If rotated data is selected, the rotate applies before the logical function."
                    switch (ega->gr.gr03_data_rotate & EGA_GR03_FUNC_MASK) {
                        case EGA_GR03_FUNC_SET:
                            // Leave it as-is.
                            break;

                        case EGA_GR03_FUNC_AND:
                            result &= ega->gr.latch;
                            break;

                        case EGA_GR03_FUNC_OR:
                            result |= ega->gr.latch;
                            break;

                        case EGA_GR03_FUNC_XOR:
                            result ^= ega->gr.latch;
                            break;

                        default:
                            __builtin_unreachable();
                            break;
                    }

                    // Apply Set/Reset logic
                    uint32_t sr_mask = 0;
                    uint32_t sr_val  = 0;
                    for (size_t i = 0; i < 4; i++) {
                        if ((ega->gr.gr01_enable_set_reset[i >> 1] & (1 << (i & 0b1))) != 0) {
                            sr_mask |= (0xFF << (i * 8));
                        }
                        if ((ega->gr.gr00_set_reset[i >> 1] & (1 << (i & 0b1))) != 0) {
                            sr_val |= (0xFF << (i * 8));
                        }
                    }
                    result = (result & ~sr_mask) | (sr_val & sr_mask);
                    break;
                }

            // from the IBM EGA doc:
            // "Each memory plane is written with the contents of the processor latches."
            // "These latches are loaded by a processor read operation."
            case EGA_GR05_WRITEMODE_1:
                result = ega->gr.latch;
                break;

            case EGA_GR05_WRITEMODE_2:
                result = 0;
                for (size_t i = 0; i < 4; i++) {
                    if ((val & (1 << i)) != 0) {
                        result |= 0xFF << (i * 8);
                    }
                }
                break;

            default:
                // TODO: REAL HARDWARE NEEDED: What does the invalid setting of 11 actually do? --GM
                break;
        }

        // Apply bit mask
        // Technically redundant for WM 1 (copy latches directly).
        // But it keeps the code a bit simpler and reduces stress on your CPU's branch predictor.
        uint32_t bit_mask = 0x01010101 * (uint32_t) (ega->gr.gr08_bit_mask);
        result            = (result & bit_mask) | (ega->gr.latch & ~bit_mask);

        // Compute plane mask
        uint32_t map_mask = 0;
        for (size_t i = 0; i < 4; i++) {
            if ((ega->sr.sr02_map_mask & (1 << i)) != 0) {
                map_mask |= 0xFF << (i * 8);
            }
        }
        // Also apply Odd/Even
        if ((ega->sr.sr04_memory_mode & EGA_SR04_ODDEVEN_MASK) == EGA_SR04_ODDEVEN_ON) {
            map_mask &= 0x00FF00FF << (8 * (addr & 0b1));
        }

        result = (result & map_mask) | (ega->vram_buf[vaddr] & ~map_mask);

        // And now for the actual write!
        ega->vram_buf[vaddr] = result;
    }

    // Update output
    ega_update_output(ega);
}

static uint8_t
ega_io_in(uint16_t addr, void *priv)
{
    ega_t *ega = (ega_t *) priv;

    // printf("EGA in %04X\n", addr);

    switch (addr | 0x100) {
        // 03C2 R: Input Status Register Zero
        case 0x3C2:
            {
                ega_update_output(ega);

                // TODO: Compute the actual result --GM
                uint8_t result = (uint8_t) (~EGA_R3C2_MASK)
                    | EGA_R3C2_FEATCODE_11
                    | EGA_R3C2_CRTINT_ACTIVEVID;

                uint8_t sw_shift = EGA_W3C2_CLOCKSEL_READ(ega->misc_out_3c2);
                // FIXME: Work out what we need to do to get this to behave correctly! --GM
                //
                // Official notes from EGA BIOS listing:
                // - 0000 = MDA primary, EGA uses CGA 40
                // - 0001 = MDA primary, EGA uses CGA 80
                // - 0010 = MDA primary, EGA uses EGA 200
                // - 0011 = MDA primary, EGA uses EGA 350
                // - 0100 = CGA 40 primary, EGA uses MDA
                // - 0101 = CGA 80 primary, EGA uses MDA
                // - 0110 = MDA secondary, EGA uses CGA 40
                // - 0111 = MDA secondary, EGA uses CGA 80
                // - 1000 = MDA secondary, EGA uses EGA 200
                // - 1001 = MDA secondary, EGA uses EGA 350
                // - 1010 = CGA 40 secondary, EGA uses MDA
                // - 1011 = CGA 80 secondary, EGA uses MDA
                // Everything else is reserved.
                //
                // 40:87 bit 1: 1=EGA is monochrome, 0=EGA is colour
                //
                // 0x00 through 0x02: CGA 80
                // 0x03: EGA compat?
                // 0x04 through 0x05: MDA, borked edition
                // 0x06 through 0x08: CGA 80
                // 0x09: EGA compat?
                // 0x0A through 0x0B: MDA, borked edition
                // 0x0C through 0x0F: CGA 80
                //
                // Proper EGA setup requirements:
                // See PDF page 117 / printed page 113 in the IBM EGA doc for the POD14 function listing.
                //
                // result |= ((0x0F & (0x08 >> sw_shift)) == 0)
                result |= ((ega->monitor_type & (0x08 >> sw_shift)) == 0)
                    ? EGA_R3C2_SWITCHSENSE_ON
                    : EGA_R3C2_SWITCHSENSE_OFF;
                printf("sw %02X %02X %2d\n", result, ega->monitor_type, sw_shift);

                return result;
            }

        // 03B5/03D5 R: Data for CRT Controller (CRTC)
        case 0x3B5:
        case 0x3D5:
            switch (ega->cr.cpu_addr) {
                case 0x0C: // Start Address High
                    return (uint8_t) (EGA_CR0C_STARTHI_READ(ega->cr.start_vaddr >> 8) | ~EGA_CR0C_MASK);

                case 0x0D: // Start Address Low
                    return (uint8_t) (EGA_CR0D_STARTLO_READ(ega->cr.start_vaddr >> 0) | ~EGA_CR0D_MASK);

                case 0x0E: // Cursor Location High
                    return (uint8_t) (EGA_CR0E_CURSORHI_READ(ega->cr.cursor_vaddr >> 8) | ~EGA_CR0E_MASK);

                case 0x0F: // Cursor Location Low
                    return (uint8_t) (EGA_CR0F_CURSORLO_READ(ega->cr.cursor_vaddr >> 0) | ~EGA_CR0F_MASK);

                default:
                    return 0xFF;
            }

        // 03BA/03DA R: Input Status Register One
        case 0x3BA:
        case 0x3DA:
            {
                ega_update_output(ega);

                // Reset the flip-flop for attribute controller access
                ega->ar.ff_is_data = false;

                // HACK: Toggle the diagnostic lines so the IBM EGA BIOS can boot
                // FIXME: Implement this properly --GM
                ega->status_in_3xa ^= EGA_R3XA_DIAGOUT0_MASK ^ EGA_R3XA_DIAGOUT1_MASK;
                // FIXME: This is even worse --GM
                ega->status_in_3xa ^= EGA_R3XA_DISPENABLE_MASK;
                ega->status_in_3xa ^= EGA_R3XA_VRETRACE_MASK;
                if ((ega->status_in_3xa & EGA_R3XA_VRETRACE_MASK) == EGA_R3XA_VRETRACE_RETRACE) {
                    ega->status_in_3xa &= ~EGA_R3XA_DISPENABLE_MASK;
                    ega->status_in_3xa |= EGA_R3XA_DISPENABLE_RETRACE;
                }

                return ega->status_in_3xa | ~EGA_R3XA_MASK;
            }

        default:
            printf("EGA in %04X\n", addr);
            return 0xFF;
    }
}

static void
ega_io_out(uint16_t addr, uint8_t val, void *priv)
{
    ega_t *ega = (ega_t *) priv;

    // printf("EGA out %04X value %02X\n", addr, val);

    switch (addr | 0x100) {
        // 03C0 W: Attribute Controller
        // TODO: Confirm if the 03C1 mirror actually does exist --GM
        case 0x3C0:
        case 0x3C1:
            if (!ega->ar.ff_is_data) {
                // Select address
                ega_update_output(ega);
                ega->ar.cpu_addr   = EGA_W3C0_ADDR_READ(val);
                ega->ar.is_video   = (val & EGA_W3C0_PALSRC_MASK) == EGA_W3C0_PALSRC_DISPLAY;
                ega->ar.ff_is_data = true;
            } else {
                // Write a register
                if (ega->ar.cpu_addr < 0x10) {
                    // TODO: REAL HARDWARE NEEDED: What happens when you try to write to the palette when the screen is not blanked? --GM
                    if (!ega->ar.is_video) {
                        ega_update_output(ega);
                        ega->ar.pal[ega->ar.cpu_addr] = val & 0x3F;
                    }

                } else {
                    // TODO: REAL HARDWARE NEEDED: Are any address bits ignored? --GM
                    switch (ega->ar.cpu_addr) {
                        case 0x10: // Mode Control (AFFECTS OUTPUT)
                            ega_update_output(ega);
                            ega->ar.ar10_mode = val & 0x0F;
                            break;

                        case 0x11: // Overscan Color (AFFECTS OUTPUT)
                            ega_update_output(ega);
                            ega->ar.ar11_overscan_color = val & 0x3F;
                            break;

                        case 0x12: // Color Plane Enable (+ Video Status Mux) (AFFECTS OUTPUT)
                            ega_update_output(ega);
                            ega->ar.ar12_plane_enable = val & 0x3F;
                            break;

                        case 0x13: // Horizontal Pel Panning (AFFECTS OUTPUT)
                            ega_update_output(ega);
                            ega->ar.ar13_pel_panning = val & 0x0F;
                            break;

                        default:
                            break;
                    }
                }

                // This does reset the flip-flop, otherwise the IBM EGA video BIOS cannot function correctly
                ega->ar.ff_is_data = false;
            }
            break;

        // 03C2 W: Miscellaneous Output Register (AFFECTS OUTPUT)
        case 0x3C2:
            ega_update_output(ega);

            if (((ega->misc_out_3c2 ^ val) & EGA_W3C2_IOBASE_MASK) != 0) {
                // I/O base changed.
                io_removehandler((ega->misc_out_3c2 & EGA_W3C2_IOBASE_MASK) == EGA_W3C2_IOBASE_3BX ? 0x3B0 : 0x3C0,
                                 0x0020,
                                 ega_io_in, NULL, NULL,
                                 ega_io_out, NULL, NULL,
                                 ega);
                io_sethandler((val & EGA_W3C2_IOBASE_MASK) == EGA_W3C2_IOBASE_3BX ? 0x3B0 : 0x3C0,
                              0x0020,
                              ega_io_in, NULL, NULL,
                              ega_io_out, NULL, NULL,
                              ega);
            }
            if (((ega->misc_out_3c2 ^ val) & EGA_W3C2_RAMENABLE_MASK) != 0) {
                if ((val & EGA_W3C2_RAMENABLE_MASK) == EGA_W3C2_RAMENABLE_ON) {
                    ega_map_vram(ega, ega->gr.gr06_misc[1]);
                } else {
                    ega_unmap_vram(ega);
                }
            }
            ega->misc_out_3c2 = val;
            break;

        // 03C4 W: Address for Sequencer
        case 0x3C4:
            ega->sr.cpu_addr = val & 0x1F;
            break;
        // 03C5 W: Data for Sequencer
        case 0x3C5:
            switch (ega->sr.cpu_addr) {
                // Map Mask
                case 0x02:
                    ega->sr.sr02_map_mask = val & EGA_SR02_MASK;
                    break;

                // Memory Mode (AFFECTS OUTPUT)
                case 0x04:
                    ega_update_output(ega);
                    ega->sr.sr04_memory_mode = val & EGA_SR04_MASK;
                    break;

                default:
                    break;
            }
            break;

        // 03CA W: Graphics 2 Position (index 1)
        case 0x3CA:
            ega->gr.position[1] = val & 0b11;
            break;

        // 03CC W: Graphics 1 Position (index 0)
        case 0x3CC:
            ega->gr.position[0] = val & 0b11;
            break;

        // 03CE W: Address for Graphics Controllers
        case 0x3CE:
            ega->gr.cpu_addr = val & 0x0F;
            break;
        // 03CF W: Data for Graphics Controllers
        case 0x3CF:
            switch (ega->gr.cpu_addr) {
                case 0x00: // Set/Reset
                    ega->gr.gr00_set_reset[0] = ((val >> (ega->gr.position[0] << 1)) & 0b11) * 0x55;
                    ega->gr.gr00_set_reset[1] = ((val >> (ega->gr.position[1] << 1)) & 0b11) * 0x55;
                    break;

                case 0x01: // Enable Set/Reset
                    ega->gr.gr01_enable_set_reset[0] = ((val >> (ega->gr.position[0] << 1)) & 0b11) * 0x55;
                    ega->gr.gr01_enable_set_reset[1] = ((val >> (ega->gr.position[1] << 1)) & 0b11) * 0x55;
                    break;

                case 0x02: // Color Compare
                    ega->gr.gr02_color_compare[0] = ((val >> (ega->gr.position[0] << 1)) & 0b11) * 0x55;
                    ega->gr.gr02_color_compare[1] = ((val >> (ega->gr.position[1] << 1)) & 0b11) * 0x55;
                    break;

                case 0x03: // Data Rotate
                    ega->gr.gr03_data_rotate = val & EGA_GR03_MASK;
                    break;

                case 0x04: // Read Map Select
                    ega->gr.gr04_read_map_select = val & 0x07;
                    break;

                case 0x05: // Mode (AFFECTS OUTPUT - bits 2,5)
                    if (((ega->gr.gr05_mode ^ val)
                         & (EGA_GR05_TESTCOND_MASK | EGA_GR05_SHIFTMODE_MASK))
                        != 0) {
                        ega_update_output(ega);
                    }
                    ega->gr.gr05_mode = val & EGA_GR05_MASK;
                    break;

                case 0x06: // Miscellaneous Output (AFFECTS OUTPUT - bits 0,1)
                    {
                        ega_update_output(ega);
                        uint8_t old_map      = ega->gr.gr06_misc[1];
                        ega->gr.gr06_misc[0] = ((val >> (ega->gr.position[0] << 1)) & 0b11) * 0x55;
                        ega->gr.gr06_misc[1] = ((val >> (ega->gr.position[1] << 1)) & 0b11) * 0x55;
                        if (((old_map ^ ega->gr.gr06_misc[1]) & EGA_GR06_1_MEMORYMAP_MASK) != 0) {
                            if ((ega->misc_out_3c2 & EGA_W3C2_RAMENABLE_MASK) == EGA_W3C2_RAMENABLE_ON) {
                                ega_map_vram(ega, ega->gr.gr06_misc[1]);
                            }
                        }
                    }
                    break;

                case 0x07: // Color Don't Care
                    ega->gr.gr07_color_dont_care[0] = ((val >> (ega->gr.position[0] << 1)) & 0b11) * 0x55;
                    ega->gr.gr07_color_dont_care[1] = ((val >> (ega->gr.position[1] << 1)) & 0b11) * 0x55;
                    break;

                case 0x08: // Bit Mask
                    ega->gr.gr08_bit_mask = val;
                    break;

                default:
                    break;
            }
            break;

        // 03B4/03D4 W: Address for CRT Controller (CRTC)
        case 0x3B4:
        case 0x3D4:
            ega->cr.cpu_addr = val & 0b11111;
            break;

        // 03B5/03D5 W: Data for CRT Controller (CRTC)
        case 0x3B5:
        case 0x3D5:
            switch (ega->cr.cpu_addr) {
                case 0x00: // Horizontal Total (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.htotal = EGA_CR00_HTOTAL_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x01: // Horizontal Display Enable End (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.hdispend = EGA_CR01_HDISPEND_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x02: // Start Horizontal Blanking (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.hblankbeg = EGA_CR02_HBLANKBEG_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x03: // End Horizontal Blanking (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.hblankend = EGA_CR03_HBLANKEND_READ(val);
                    ega->cr.skew_disp = EGA_CR03_DISPSKEW_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x04: // Start Horizontal Retrace (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.hsyncbeg = EGA_CR04_HSYNCBEG_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x05: // End Horizontal Retrace (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.hsyncend   = EGA_CR05_HSYNCEND_READ(val);
                    ega->cr.skew_hsync = EGA_CR05_HSYNCSKEW_READ(val);
                    ega->cr.hoeoffs    = EGA_CR05_HOEOFFS0_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x06: // Vertical Total (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vtotal = (ega->cr.vtotal & ~0xFF) | EGA_CR06_VTOTAL_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x07: // Overflow (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vtotal      = (ega->cr.vtotal & 0xFF) | (EGA_CR07_VTOTAL_8_READ(val) << 8);
                    ega->cr.vdispend    = (ega->cr.vdispend & 0xFF) | (EGA_CR07_VDISPEND_8_READ(val) << 8);
                    ega->cr.vsyncbeg    = (ega->cr.vsyncbeg & 0xFF) | (EGA_CR07_VSYNCBEG_8_READ(val) << 8);
                    ega->cr.vblankbeg   = (ega->cr.vblankbeg & 0xFF) | (EGA_CR07_VBLANKBEG_8_READ(val) << 8);
                    ega->cr.linecompare = (ega->cr.linecompare & 0xFF) | (EGA_CR07_LINECOMPARE_8_READ(val) << 8);
                    ega_update_screen_timings(ega);
                    break;

                case 0x08: // Preset Row Scan (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vfinescroll = EGA_CR08_VFINESCROLL_READ(val);
                    break;

                case 0x09: // Maximum Scan Line (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vnextcharidx = ((EGA_CR09_CHARHEIGHT_READ(val)) - 1) & 0x1F;
                    break;

                case 0x0A: // Cursor Start (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.cursorbeg = EGA_CR0A_CURSORBEG_READ(val);
                    break;

                case 0x0B: // Cursor End (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.cursorbeg   = EGA_CR0B_CURSOREND_READ(val);
                    ega->cr.skew_cursor = EGA_CR0B_CURSORSKEW_READ(val);
                    break;

                case 0x0C: // Start Address High (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.start_vaddr = (ega->cr.start_vaddr & 0x00FF)
                        | (((uint16_t) EGA_CR0C_STARTHI_READ(val)) << 8);
                    break;

                case 0x0D: // Start Address Low (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.start_vaddr = (ega->cr.start_vaddr & 0xFF00)
                        | (((uint16_t) EGA_CR0D_STARTLO_READ(val)) << 0);
                    break;

                case 0x0E: // Cursor Location High (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.cursor_vaddr = (ega->cr.cursor_vaddr & 0x00FF)
                        | (((uint16_t) EGA_CR0E_CURSORHI_READ(val)) << 8);
                    break;

                case 0x0F: // Cursor Location Low (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.cursor_vaddr = (ega->cr.cursor_vaddr & 0xFF00)
                        | (((uint16_t) EGA_CR0F_CURSORLO_READ(val)) << 0);
                    break;

                case 0x10: // Vertical Retrace Start (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vsyncbeg = (ega->cr.vsyncbeg & ~0xFF) | EGA_CR10_VSYNCBEG_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x11: // Vertical Retrace End (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vsyncend   = EGA_CR11_VSYNCEND_READ(val);
                    ega->cr.clearvint  = (val & EGA_CR11_CLEARVINT_MASK) == EGA_CR11_CLEARVINT_ON;
                    ega->cr.enablevint = (val & EGA_CR11_ENABLEVINT_MASK) == EGA_CR11_ENABLEVINT_ON;
                    ega_update_screen_timings(ega);
                    break;

                case 0x12: // Vertical Display Enable End (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vdispend = (ega->cr.vdispend & ~0xFF) | EGA_CR12_VDISPEND_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x13: // Offset (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.offset = EGA_CR13_OFFSET_READ(val);
                    break;

                case 0x14: // Underline Location (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.underlinepos = EGA_CR14_UNDERLINE_READ(val);
                    break;

                case 0x15: // Start Vertical Blanking (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vblankbeg = (ega->cr.vblankbeg & ~0xFF) | EGA_CR15_VBLANKBEG_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x16: // End Vertical Blanking (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.vblankend = EGA_CR16_VBLANKEND_READ(val);
                    ega_update_screen_timings(ega);
                    break;

                case 0x17: // Mode Control (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.a13_bits_from_scanline = val & 0b11;
                    ega->cr.vdivide                = (val & EGA_CR17_VDIVIDE_MASK) == EGA_CR17_VDIVIDE_DIV2 ? 2 : 1;
                    ega->cr.addrdivide             = (val & EGA_CR17_ADDRDIVIDE0_MASK) == EGA_CR17_ADDRDIVIDE0_DIV2 ? 2 : 1;
                    ega->cr.floatoutputs           = (val & EGA_CR17_OUTCTRL_MASK) == EGA_CR17_OUTCTRL_FLOATOUTPUTS;
                    ega->cr.wrapbit                = (val & EGA_CR17_WRAPBIT_MASK) == EGA_CR17_WRAPBIT_A13 ? 13 : 15;
                    ega->cr.addrshift              = (val & EGA_CR17_ADDRSHIFT0_MASK) == EGA_CR17_ADDRSHIFT0_1 ? 1 : 0;
                    ega->cr.hwreset                = (val & EGA_CR17_RESET_MASK) == EGA_CR17_RESET_RESETTING;
                    ega_update_screen_timings(ega);
                    break;

                case 0x18: // Line Compare (AFFECTS OUTPUT)
                    ega_update_output(ega);
                    ega->cr.linecompare = (ega->cr.linecompare & ~0xFF) | EGA_CR18_LINECOMPARE_READ(val);
                    break;

                default:
                    break;
            }
            break;

        default:
            printf("EGA out %04X value %02X\n", addr, val);
            break;
    }
}

static const device_config_t ega_config[] = {
    // clang-format off
    {
        .name           = "memory",
        .description    = "Memory size",
        .type           = CONFIG_SELECTION,
        .default_string = NULL,
        .default_int    = 256,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = {
            { .description =  "64 KB", .value =  64 },
            { .description = "128 KB", .value = 128 },
            { .description = "192 KB", .value = 192 },
            { .description = "256 KB", .value = 256 },
            { .description = ""                     }
        },
        .bios           = { { 0 } }
    },
    {
        .name           = "monitor_type",
        .description    = "Monitor type",
        .type           = CONFIG_SELECTION,
        .default_string = NULL,
        .default_int    = 9,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = {
            { .description = "Monochrome (5151/MDA) (white)",             .value = 0x0b | (DISPLAY_WHITE << 4) },
            { .description = "Monochrome (5151/MDA) (green)",             .value = 0x0b | (DISPLAY_GREEN << 4) },
            { .description = "Monochrome (5151/MDA) (amber)",             .value = 0x0b | (DISPLAY_AMBER << 4) },
            { .description = "Color 40x25 (5153/CGA)",                    .value = 0x06                        },
            { .description = "Color 80x25 (5153/CGA)",                    .value = 0x07                        },
            { .description = "Enhanced Color - Normal Mode (5154/ECD)",   .value = 0x08                        },
            { .description = "Enhanced Color - Enhanced Mode (5154/ECD)", .value = 0x09                        },
            { .description = ""                                                                                }
        },
        .bios           = { { 0 } }
    },
    {
        .name           = "base",
        .description    = "Address",
        .type           = CONFIG_HEX16,
        .default_string = NULL,
        .default_int    = 0x03c0,
        .file_filter    = NULL,
        .spinner        = { 0 },
        .selection      = {
            { .description = "0x3C0", .value = 0x03c0 },
            { .description = "0x2C0", .value = 0x02c0 },
            { .description = ""                       }
        },
        .bios           = { { 0 } }
    },
    { .name = "", .description = "", .type = CONFIG_END }
    // clang-format on
};

extern "C" const device_t ega_newvid_device = {
    .name          = "IBM EGA (*newvid*)",
    .internal_name = "ega_newvid",
    .flags         = DEVICE_ISA,
    .local         = 0,
    .init          = ega_init,
    .close         = ega_close,
    .reset         = NULL,
    .available     = ega_available,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = ega_config
};
