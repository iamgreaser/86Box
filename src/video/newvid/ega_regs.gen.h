//
// NOTE: DO NOT EDIT ega_regs.h DIRECTLY.
// Edit ega_regs.gen.h for the template and schema.
// Edit ega_regs.gen.py for the generator.
// Run ega_regs.gen.py with a suitable version of Python 3 to generate ega_regs.h.
// (3.13 works, older versions probably also work as far back as maybe 3.6? Not tested though.)
//

!boardprefix EGA
// Helpers
#define EGA_MAKE_WRITE(_NAME, x) \
    ((((x) + _NAME##_ZERO) << _NAME##_SHIFT) & _NAME##_MASK)
#define EGA_MAKE_READ(_NAME, x) \
    ((((x) + (_NAME##_ZERO << _NAME##_SHIFT)) & _NAME##_MASK) >> _NAME##_SHIFT)

//
// 03C2 W: Miscellaneous Output Register
//
// U37, 74LS273 8-bit D-latch, SHT 9
//
!startreg W3C2

// 3BX/3DX CRTC I/O Address (~MONO MONITOR)
// This provides in input into the I/O address decoder PROM (U34, SHT 4).
!field IOBASE 0 1 0 3BX=0b0 3DX=0b1

// Enable RAM (RAM ENABLE)
// This provides an input into the memory address decoder PROM (U48, SHT 4).
!field RAMENABLE 1 1 0 OFF=0b0 ON=0b1

// Clock Select (CLK SEL 0/1)
// These select U15 (74LS153 dual 4-to-1 mux), which selects the following:
// - Sequencer clock source (SHT 2):
//   - 00 = ISA bus clock, ~14 MHz
//   - 01 = ~16 MHz on the card itself (U7)
//   - 10 = external oscillator from feature connector, float if absent
//   - 11 = floating
// - DIP switch values to report on input 3C2.4 (on U35, 74LS367 6-bit buffer, SHT 9):
//   (yes, that's the *other* port 3C2, the one you get when you do a *read*)
//   - 00 = SW1 switch 1 (0 = closed)
//   - 01 = SW1 switch 2 (0 = closed)
//   - 10 = SW1 switch 3 (0 = closed)
//   - 11 = SW1 switch 4 (0 = closed)
!field CLOCKSEL 2 2 0 14MHZ=0b00 16MHZ=0b01 EXTOSC=0b10

// Disable Internal Video Drivers (~INTERNAL)
// When high, this severs the outputs on U36 (74LS244 4x2=8 buffer, SHT 9).
// These go to DE-9 output connector J3 (SHT 9).
// The only output that isn't severed is pin 1 which is hard-wired to GND.
// The feature connector goes past U36 but before the resistors going to J3.
!field VIDDRIVERS 4 1 0 ON=0b0 OFF=0b1

// Page Bit For Odd/Even (~PGSEL)
// FREQUENCLY MISDOCUMENTED:
// - Doesn't mention the necessary conditions for this to be used
// - Gets the meaning inverted!
// ACTUAL BEHAVIOUR:
// One of the inputs to U29 (74LS151 8-to-1 mux, SHT 4).
// This provides the not-A0 line when all 3 conditions are met:
// - (A=0) A RAM expansion is present and plugged into the board
//   - J5 pin 50 (SHT 11) is tied to ground on the expansion (P1, MEMSHT 1)
// - (B=1) Odd/Even mode is enabled in GR06.1 (GC #1 on SHT 6)
// - (C=0) The mapping isn't 128 KB @ A000-BFFF (GR06.2-3 != 00, GC #2 on SHT 6)
//   - This is an input to the memory address decoder PROM (U48, SHT 4).
// That is, ~PGSEL provides the INVERSE of address bit 0.
!field OEPAGE 5 1 0 HI=0b0 LO=0b1

// Horizontal Retrace Polarity (H POL), bit 6
// Vertical Retrace Polarity (V POL), bit 7
// 0 is positive, 1 is negative.
// These XOR the HSYNC/VSYNC values from the CRTC (U8, 74LS86 4x 2-XOR, SHT 9, 2 pairs used).
// The numbers given are the typical expected number of scanlines.
// (EGA only cares about 200-line and 350-line screens.)
!field POLARITY 6 2 0 VP_HP_200=0b00 VP_HN_400=0b01 VN_HP_350=0b10 VN_HN_480=0b11

!endreg W3C2

//
// 03C2 R: Input Status Register Zero
//
// Bits 4-7 are from U35 (74LS367, SHT 9, when G1? asserted).
//

// Switch Sense (SW1 muxed through U15 (74LS153 4-to-1 mux), SHT 9)
// Selected via EGA_W3C2_CLOCKSEL_* - write to 03C2.2-3:
//   - 00 = SW1 switch 1 (0 = closed)
//   - 01 = SW1 switch 2 (0 = closed)
//   - 10 = SW1 switch 3 (0 = closed)
//   - 11 = SW1 switch 4 (0 = closed)
#define EGA_R3C2_SWITCHSENSE_SHIFT 4
#define EGA_R3C2_SWITCHSENSE_MASK  (0b1 << EGA_R3C2_SWITCHSENSE_SHIFT)
#define EGA_R3C2_SWITCHSENSE_ON    (0b0 << EGA_R3C2_SWITCHSENSE_SHIFT)
#define EGA_R3C2_SWITCHSENSE_OFF   (0b1 << EGA_R3C2_SWITCHSENSE_SHIFT)

// Feature Code (SHT 10)
// Received from FEAT 0, FEAT 1 (pins 20 and 21 respectively on J4, SHT 10)
#define EGA_R3C2_FEATCODE_SHIFT 5
#define EGA_R3C2_FEATCODE_MASK  (0b11 << EGA_R3C2_FEATCODE_SHIFT)
#define EGA_R3C2_FEATCODE_00    (0b00 << EGA_R3C2_FEATCODE_SHIFT)
#define EGA_R3C2_FEATCODE_01    (0b01 << EGA_R3C2_FEATCODE_SHIFT)
#define EGA_R3C2_FEATCODE_10    (0b10 << EGA_R3C2_FEATCODE_SHIFT)
#define EGA_R3C2_FEATCODE_11    (0b11 << EGA_R3C2_FEATCODE_SHIFT)

// CRT Interrupt (from CRTC output, U8, SHT 3)
// This goes directly to IRQ2 (which cascades to IRQ9 on AT and up).
#define EGA_R3C2_CRTINT_SHIFT     7
#define EGA_R3C2_CRTINT_MASK      (0b1 << EGA_R3C2_CRTINT_SHIFT)
#define EGA_R3C2_CRTINT_ACTIVEVID (0b0 << EGA_R3C2_CRTINT_SHIFT)
#define EGA_R3C2_CRTINT_VRETRACE  (0b1 << EGA_R3C2_CRTINT_SHIFT)

//
// 03BA/03DA W: Feature Control Register
//

// Feature Control Bits
// Sets FC0, FC1 (pins 19 and 17 respectively on J4, SHT 10)
//
// On boot (original IBM EGA), the following happens:
// - On both 3BA and 3DA, set FC1:FC0=01; then read 3C2
// - On both 3BA and 3DA, set FC1:FC0=10; then read 3C2
//
#define EGA_W3XA_FEATCTRL_SHIFT 0
#define EGA_W3XA_FEATCTRL_MASK  (0b11 << EGA_W3XA_FEATCTRL_SHIFT)
#define EGA_W3XA_FEATCTRL_00    (0b00 << EGA_W3XA_FEATCTRL_SHIFT)
#define EGA_W3XA_FEATCTRL_01    (0b01 << EGA_W3XA_FEATCTRL_SHIFT)
#define EGA_W3XA_FEATCTRL_10    (0b10 << EGA_W3XA_FEATCTRL_SHIFT)
#define EGA_W3XA_FEATCTRL_11    (0b11 << EGA_W3XA_FEATCTRL_SHIFT)

//
// 03BA/03DA R: Input Status Register One
//
// Bits 1,2 are from U35 (74LS367, SHT 9, when G2? asserted).
// Bits 0,3,4,5 are from the Attribute Controller (U24, SHT 8).
//

// Display Enable
#define EGA_R3XA_DISPENABLE_SHIFT     0
#define EGA_R3XA_DISPENABLE_MASK      (0b1 << EGA_R3XA_DISPENABLE_SHIFT)
#define EGA_R3XA_DISPENABLE_RETRACE   (0b0 << EGA_R3XA_DISPENABLE_SHIFT)
#define EGA_R3XA_DISPENABLE_ACTIVEVID (0b1 << EGA_R3XA_DISPENABLE_SHIFT)

// Light Pen Strobe
// Q line from U18 (74LS74, 2x1-bit D-latch, SHT 9)
// - Also outputs to LPEN STB/LPSTB into the CRTC (U8, SHT 3)
// ~LPSET (SHT 4) goes into U18 PR
// ~LPCLR (SHT 4) goes into U18 CLR
// +5V goes into U18 D
// The NOT of ~LPEN IN (P2, 1x6 Berg connector, SHT 4) goes into U18 clock
#define EGA_R3XA_LPENSTROBE_SHIFT 1
#define EGA_R3XA_LPENSTROBE_MASK  (0b1 << EGA_R3XA_LPENSTROBE_SHIFT)
#define EGA_R3XA_LPENSTROBE_OFF   (0b0 << EGA_R3XA_LPENSTROBE_SHIFT)
#define EGA_R3XA_LPENSTROBE_ON    (0b1 << EGA_R3XA_LPENSTROBE_SHIFT)

// Light Pen Switch
// From ~LPEN SW line on P2 (1x6 Berg connector, SHT 9)
#define EGA_R3XA_LPENSWITCH_SHIFT 2
#define EGA_R3XA_LPENSWITCH_MASK  (0b1 << EGA_R3XA_LPENSWITCH_SHIFT)
#define EGA_R3XA_LPENSWITCH_ON    (0b0 << EGA_R3XA_LPENSWITCH_SHIFT)
#define EGA_R3XA_LPENSWITCH_OFF   (0b1 << EGA_R3XA_LPENSWITCH_SHIFT)

// Vertical Retrace
#define EGA_R3XA_VRETRACE_SHIFT     3
#define EGA_R3XA_VRETRACE_MASK      (0b1 << EGA_R3XA_VRETRACE_SHIFT)
#define EGA_R3XA_VRETRACE_ACTIVEVID (0b0 << EGA_R3XA_VRETRACE_SHIFT)
#define EGA_R3XA_VRETRACE_VRETRACE  (0b1 << EGA_R3XA_VRETRACE_SHIFT)

// Diagnostic Usage
// From the IBM EGA doc:
// "The Color Plane Enable register controls the multiplexer for the video wiring."
// The mux address is AR12.4-5.
#define EGA_R3XA_DIAGOUT_SHIFT      4
#define EGA_R3XA_DIAGOUT_MASK       (0b1 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_00_PR0_PB0 (0b00 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_00_PR0_PB1 (0b01 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_00_PR1_PB0 (0b10 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_00_PR1_PB1 (0b11 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_01_SB0_PG0 (0b00 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_01_SB0_PG1 (0b01 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_01_SB1_PG0 (0b10 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_01_SB1_PG1 (0b11 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_10_SR0_SG0 (0b00 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_10_SR0_SG1 (0b01 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_10_SR1_SG0 (0b10 << EGA_R3XA_DIAGOUT_SHIFT)
#define EGA_R3XA_DIAGOUT_10_SR1_SG1 (0b11 << EGA_R3XA_DIAGOUT_SHIFT)

//
// 03C0 W: Attribute Controller registers
//

// Address parts
// The actual address
#define EGA_W3C0_ADDR_SHIFT 0
#define EGA_W3C0_ADDR_MASK  (0b11111 << EGA_W3C0_ADDR_SHIFT)
// Screen display vs Letting the palette be editable
#define EGA_W3C0_PALSRC_SHIFT   5
#define EGA_W3C0_PALSRC_MASK    (0b1 << EGA_W3C0_PALSRC_SHIFT)
#define EGA_W3C0_PALSRC_BLANK   (0b0 << EGA_W3C0_PALSRC_SHIFT)
#define EGA_W3C0_PALSRC_DISPLAY (0b1 << EGA_W3C0_PALSRC_SHIFT)

//
// 03C4 and 03C5 W: Sequencer registers
//

// Alpha (provides separate A14,A15 for planes 2 and 3)
#define EGA_SR04_ALPHA_SHIFT 0
#define EGA_SR04_ALPHA_MASK  (0b1 << EGA_SR04_ALPHA_SHIFT)
#define EGA_SR04_ALPHA_OFF   (0b0 << EGA_SR04_ALPHA_SHIFT)
#define EGA_SR04_ALPHA_ON    (0b1 << EGA_SR04_ALPHA_SHIFT)
// Extended Memory (lets A14,A15 be nonzero)
// NOTE: On the actual IBM EGA card, these are not address pins, they're 4 chip select pins.
#define EGA_SR04_EXTMEM_SHIFT 0
#define EGA_SR04_EXTMEM_MASK  (0b1 << EGA_SR04_EXTMEM_SHIFT)
#define EGA_SR04_EXTMEM_OFF   (0b0 << EGA_SR04_EXTMEM_SHIFT)
#define EGA_SR04_EXTMEM_ON    (0b1 << EGA_SR04_EXTMEM_SHIFT)
// Odd/Even (for plane write enables)
#define EGA_SR04_ODDEVEN_SHIFT 2
#define EGA_SR04_ODDEVEN_MASK  (0b1 << EGA_SR04_ODDEVEN_SHIFT)
#define EGA_SR04_ODDEVEN_ON    (0b0 << EGA_SR04_ODDEVEN_SHIFT)
#define EGA_SR04_ODDEVEN_OFF   (0b1 << EGA_SR04_ODDEVEN_SHIFT)

//
// 03CE and 03CF W: Graphics Controller registers
//

// GR03.0-2: Rotate Count (rotate right by this amount)
#define EGA_GR03_ROR_SHIFT 0
#define EGA_GR03_ROR_MASK  (0b111 << EGA_GR03_ROR_SHIFT)
// GR03.3-4: Function Select
#define EGA_GR03_FUNC_SHIFT 3
#define EGA_GR03_FUNC_MASK  (0b11 << EGA_GR03_FUNC_SHIFT)
#define EGA_GR03_FUNC_SET   (0b00 << EGA_GR03_FUNC_SHIFT)
#define EGA_GR03_FUNC_AND   (0b01 << EGA_GR03_FUNC_SHIFT)
#define EGA_GR03_FUNC_OR    (0b10 << EGA_GR03_FUNC_SHIFT)
#define EGA_GR03_FUNC_XOR   (0b11 << EGA_GR03_FUNC_SHIFT)

// GR05.0-1: Write Mode
#define EGA_GR05_WRITEMODE_SHIFT 0
#define EGA_GR05_WRITEMODE_MASK  (0b11 << EGA_GR05_WRITEMODE_SHIFT)
#define EGA_GR05_WRITEMODE_0     (0b00 << EGA_GR05_WRITEMODE_SHIFT)
#define EGA_GR05_WRITEMODE_1     (0b01 << EGA_GR05_WRITEMODE_SHIFT)
#define EGA_GR05_WRITEMODE_2     (0b10 << EGA_GR05_WRITEMODE_SHIFT)
// GR05.2: Test Condition
#define EGA_GR05_TESTCOND_SHIFT        2
#define EGA_GR05_TESTCOND_MASK         (0b1 << EGA_GR05_TESTCOND_SHIFT)
#define EGA_GR05_TESTCOND_NORMAL       (0b0 << EGA_GR05_TESTCOND_SHIFT)
#define EGA_GR05_TESTCOND_FLOATOUTPUTS (0b1 << EGA_GR05_TESTCOND_SHIFT)
// GR05.3: Read Mode
#define EGA_GR05_READMODE_SHIFT     3
#define EGA_GR05_READMODE_MASK      (0b1 << EGA_GR05_READMODE_SHIFT)
#define EGA_GR05_READMODE_0_SINGLE  (0b0 << EGA_GR05_READMODE_SHIFT)
#define EGA_GR05_READMODE_1_COMPARE (0b1 << EGA_GR05_READMODE_SHIFT)
// GR05.4: Odd/Even (for plane reads)
#define EGA_GR05_ODDEVEN_SHIFT 4
#define EGA_GR05_ODDEVEN_MASK  (0b1 << EGA_GR05_ODDEVEN_SHIFT)
#define EGA_GR05_ODDEVEN_OFF   (0b0 << EGA_GR05_ODDEVEN_SHIFT)
#define EGA_GR05_ODDEVEN_ON    (0b1 << EGA_GR05_ODDEVEN_SHIFT)
// GR05.5: Shift Register
// - 0: Fully planar.
// - 1: Each pair of planes is considered to be chunky.
#define EGA_GR05_SHIFTMODE_SHIFT  5
#define EGA_GR05_SHIFTMODE_MASK   (0b1 << EGA_GR05_SHIFTMODE_SHIFT)
#define EGA_GR05_SHIFTMODE_PLANAR (0b0 << EGA_GR05_SHIFTMODE_SHIFT)
#define EGA_GR05_SHIFTMODE_2X2    (0b1 << EGA_GR05_SHIFTMODE_SHIFT)

// GR06 GC #1 .0: Graphics Mode
#define EGA_GR06_0_GRAPHICS_SHIFT 0
#define EGA_GR06_0_GRAPHICS_MASK  (0b1 << EGA_GR06_0_GRAPHICS_SHIFT)
#define EGA_GR06_0_GRAPHICS_OFF   (0b0 << EGA_GR06_0_GRAPHICS_SHIFT)
#define EGA_GR06_0_GRAPHICS_ON    (0b1 << EGA_GR06_0_GRAPHICS_SHIFT)
// GR06 GC #1 .1: Odd/Even (for CPU VRAM address generation)
#define EGA_GR06_0_ODDEVEN_SHIFT 1
#define EGA_GR06_0_ODDEVEN_MASK  (0b1 << EGA_GR06_0_ODDEVEN_SHIFT)
#define EGA_GR06_0_ODDEVEN_OFF   (0b0 << EGA_GR06_0_ODDEVEN_SHIFT)
#define EGA_GR06_0_ODDEVEN_ON    (0b1 << EGA_GR06_0_ODDEVEN_SHIFT)
// GR06 GC #2 .0-1: Memory Map
#define EGA_GR06_1_MEMORYMAP_SHIFT     0
#define EGA_GR06_1_MEMORYMAP_MASK      (0b11 << EGA_GR06_1_MEMORYMAP_SHIFT)
#define EGA_GR06_1_MEMORYMAP_A000_128K (0b00 << EGA_GR06_1_MEMORYMAP_SHIFT)
#define EGA_GR06_1_MEMORYMAP_A000_64K  (0b01 << EGA_GR06_1_MEMORYMAP_SHIFT)
#define EGA_GR06_1_MEMORYMAP_B000_32K  (0b10 << EGA_GR06_1_MEMORYMAP_SHIFT)
#define EGA_GR06_1_MEMORYMAP_B800_32K  (0b11 << EGA_GR06_1_MEMORYMAP_SHIFT)

//
// 03B4/03D4 and 03B5/03D5 W: CRT Controller registers
//
// All registers are write-only EXCEPT where otherwise noted.
// Some writeable addresses are shared with different read-only registers!
//

#define EGA_CR00_MASK 0xFF
// CR00: Horizontal Total minus 2
#define EGA_CR00_HTOTAL_SHIFT    0
#define EGA_CR00_HTOTAL_ZERO     -2
#define EGA_CR00_HTOTAL_MASK     (((1 << 8) - 1) << EGA_CR00_HTOTAL_SHIFT)
#define EGA_CR00_HTOTAL_WRITE(x) EGA_MAKE_WRITE(EGA_CR00_HTOTAL, (x))
#define EGA_CR00_HTOTAL_READ(x)  EGA_MAKE_READ(EGA_CR00_HTOTAL, (x))

#define EGA_CR01_MASK            0xFF
// CR01: Horizontal Display Enable End minus 1
#define EGA_CR01_HDISPEND_SHIFT    0
#define EGA_CR01_HDISPEND_ZERO     -1
#define EGA_CR01_HDISPEND_MASK     (((1 << 8) - 1) << EGA_CR01_HDISPEND_SHIFT)
#define EGA_CR01_HDISPEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR01_HDISPEND, (x))
#define EGA_CR01_HDISPEND_READ(x)  EGA_MAKE_READ(EGA_CR01_HDISPEND, (x))

#define EGA_CR02_MASK              0xFF
// CR02: Start Horizontal Blanking
#define EGA_CR02_HBLANKBEG_SHIFT    0
#define EGA_CR02_HBLANKBEG_ZERO     0
#define EGA_CR02_HBLANKBEG_MASK     (((1 << 8) - 1) << EGA_CR02_HBLANKBEG_SHIFT)
#define EGA_CR02_HBLANKBEG_WRITE(x) EGA_MAKE_WRITE(EGA_CR02_HBLANKBEG, (x))
#define EGA_CR02_HBLANKBEG_READ(x)  EGA_MAKE_READ(EGA_CR02_HBLANKBEG, (x))

#define EGA_CR03_MASK               0x7F
// CR03.0-4: End Horizontal Blanking, low 5 bits
#define EGA_CR03_HBLANKEND_SHIFT    0
#define EGA_CR03_HBLANKEND_ZERO     0
#define EGA_CR03_HBLANKEND_MASK     (((1 << 5) - 1) << EGA_CR03_HBLANKEND_SHIFT)
#define EGA_CR03_HBLANKEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR03_HBLANKEND, (x))
#define EGA_CR03_HBLANKEND_READ(x)  EGA_MAKE_READ(EGA_CR03_HBLANKEND, (x))
// CR03.5-6: Display Enable Skew Control
// This basically moves the whole visible border right by CR03.5-6 character clocks.
#define EGA_CR03_HDISPSKEW_SHIFT    5
#define EGA_CR03_HDISPSKEW_ZERO     0
#define EGA_CR03_HDISPSKEW_MASK     (((1 << 2) - 1) << EGA_CR03_HDISPSKEW_SHIFT)
#define EGA_CR03_HDISPSKEW_WRITE(x) EGA_MAKE_WRITE(EGA_CR03_HDISPSKEW, (x))
#define EGA_CR03_HDISPSKEW_READ(x)  EGA_MAKE_READ(EGA_CR03_HDISPSKEW, (x))

#define EGA_CR04_MASK               0xFF
// CR04: Start Horizontal Retrace
#define EGA_CR04_HSYNCBEG_SHIFT    0
#define EGA_CR04_HSYNCBEG_ZERO     0
#define EGA_CR04_HSYNCBEG_MASK     (((1 << 8) - 1) << EGA_CR04_HSYNCBEG_SHIFT)
#define EGA_CR04_HSYNCBEG_WRITE(x) EGA_MAKE_WRITE(EGA_CR04_HSYNCBEG, (x))
#define EGA_CR04_HSYNCBEG_READ(x)  EGA_MAKE_READ(EGA_CR04_HSYNCBEG, (x))

#define EGA_CR05_MASK              0xFF
// CR05.0-4: End Horizontal Retrace, low 5 bits
#define EGA_CR05_HSYNCEND_SHIFT    0
#define EGA_CR05_HSYNCEND_ZERO     0
#define EGA_CR05_HSYNCEND_MASK     (((1 << 5) - 1) << EGA_CR05_HSYNCEND_SHIFT)
#define EGA_CR05_HSYNCEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR05_HSYNCEND, (x))
#define EGA_CR05_HSYNCEND_READ(x)  EGA_MAKE_READ(EGA_CR05_HSYNCEND, (x))
// CR05.5-6: Horizontal Retrace Delay
// This basically moves the whole display plus border ot the left by this many character clocks.
// WARNING: This can clash with HSYNC which can have a weird effect on some displays!
#define EGA_CR05_HSYNCSKEW_SHIFT    5
#define EGA_CR05_HSYNCSKEW_ZERO     0
#define EGA_CR05_HSYNCSKEW_MASK     (((1 << 2) - 1) << EGA_CR05_HSYNCSKEW_SHIFT)
#define EGA_CR05_HSYNCSKEW_WRITE(x) EGA_MAKE_WRITE(EGA_CR05_HSYNCSKEW, (x))
#define EGA_CR05_HSYNCSKEW_READ(x)  EGA_MAKE_READ(EGA_CR05_HSYNCSKEW, (x))
// CR05.7: Start Odd/Even Memory Address
// From the IBM EGA doc:
// "This bit controls whether the first CRT memory address output after a horizontal retrace begins with an even or an odd address."
#define EGA_CR05_HOEOFFS0_SHIFT 7
#define EGA_CR05_HOEOFFS0_ZERO  0
#define EGA_CR05_HOEOFFS0_MASK  (0b1 << EGA_CR05_HOEOFFS0_SHIFT)

#define EGA_CR06_MASK           0xFF
// CR06: Vertical Total, low 8 bits (bit 8 = CR07.0)
#define EGA_CR06_VTOTAL_SHIFT 0
#define EGA_CR06_VTOTAL_ZERO  0
#define EGA_CR06_VTOTAL_MASK  (((1 << 8) - 1) << EGA_CR06_VTOTAL_SHIFT)

#define EGA_CR07_MASK         0x1F
// CR07.0: Vertical Total bit 8 (low 8 bits = CR06)
#define EGA_CR07_VTOTAL_8_SHIFT 0
#define EGA_CR07_VTOTAL_8_ZERO  0
#define EGA_CR07_VTOTAL_8_MASK  (0b1 << EGA_CR07_VTOTAL_8_SHIFT)
// CR07.1: Vertical Display Enable End bit 8 (low 8 bits = CR12)
#define EGA_CR07_VDISPEND_8_SHIFT 1
#define EGA_CR07_VDISPEND_8_ZERO  0
#define EGA_CR07_VDISPEND_8_MASK  (0b1 << EGA_CR07_VDISPEND_8_SHIFT)
// CR07.2: Vertical Retrace Start bit 8 (low 8 bits = CR10 write)
#define EGA_CR07_VSYNCBEG_8_SHIFT 2
#define EGA_CR07_VSYNCBEG_8_ZERO  0
#define EGA_CR07_VSYNCBEG_8_MASK  (0b1 << EGA_CR07_VSYNCBEG_8_SHIFT)
// CR07.3: Start Vertical Blanking bit 8 (low 8 bits = CR15)
#define EGA_CR07_VBLANKBEG_8_SHIFT 3
#define EGA_CR07_VBLANKBEG_8_ZERO  0
#define EGA_CR07_VBLANKBEG_8_MASK  (0b1 << EGA_CR07_VBLANKBEG_8_SHIFT)
// CR07.4: Line Compare bit 8 (low 8 bits = CR18)
#define EGA_CR07_LINECOMPARE_8_SHIFT 4
#define EGA_CR07_LINECOMPARE_8_ZERO  0
#define EGA_CR07_LINECOMPARE_8_MASK  (0b1 << EGA_CR07_LINECOMPARE_8_SHIFT)
// Supposedly CR07.5 is bit 8 of the Cursor Location register (CR0A) but that's a 5-bit register...

#define EGA_CR08_MASK 0x1F
// CR08.0-4: Preset Row Scan (vertical scrolling)
// From the IBM EGA doc:
// "Starting Row Scan Count after a Vertical Retrace"
#define EGA_CR08_VFINESCROLL_SHIFT 0
#define EGA_CR08_VFINESCROLL_ZERO  0
#define EGA_CR08_VFINESCROLL_MASK  (((1 << 5) - 1) << EGA_CR08_VFINESCROLL_SHIFT)

#define EGA_CR09_MASK              0x1F
// CR09.0-4: Maximum Scan Line minus 1
// This is the character height in text modes.
// In graphics modes this allows for scan-doubling and other multipliers.
#define EGA_CR09_CHARHEIGHT_SHIFT 0
#define EGA_CR09_CHARHEIGHT_ZERO  -1
#define EGA_CR09_CHARHEIGHT_MASK  (((1 << 5) - 1) << EGA_CR09_CHARHEIGHT_SHIFT)

#define EGA_CR0A_MASK             0x1F
// CR0A.0-4: Cursor Start minus 1
#define EGA_CR0A_CURSORBEG_SHIFT 0
#define EGA_CR0A_CURSORBEG_ZERO  -1
#define EGA_CR0A_CURSORBEG_MASK  (((1 << 5) - 1) << EGA_CR0A_CURSORBEG_SHIFT)

#define EGA_CR0B_MASK            0x7F
// CR0B.0-4: Cursor End
#define EGA_CR0B_CURSOREND_SHIFT 0
#define EGA_CR0B_CURSOREND_ZERO  0
#define EGA_CR0B_CURSOREND_MASK  (((1 << 5) - 1) << EGA_CR0B_CURSOREND_SHIFT)
// CR0B.5-6: Cursor Skew
// This kinda moves the cursor to the right by this many character clocks.
// But if it's on the first column, it can potentially be duplicated? TODO: REAL HARDWARE NEEDED: CONFIRM THIS! --GM
#define EGA_CR0B_CURSORSKEW_SHIFT 2
#define EGA_CR0B_CURSORSKEW_ZERO  0
#define EGA_CR0B_CURSORSKEW_MASK  (((1 << 2) - 1) << EGA_CR0B_CURSORSKEW_SHIFT)

#define EGA_CR0C_MASK             0xFF
// CR0C RW: Start Address High
#define EGA_CR0C_STARTHI_SHIFT    0
#define EGA_CR0C_STARTHI_ZERO     0
#define EGA_CR0C_STARTHI_MASK     (((1 << 8) - 1) << EGA_CR0C_STARTHI_SHIFT)
#define EGA_CR0C_STARTHI_WRITE(x) EGA_MAKE_WRITE(EGA_CR0C_STARTHI, (x))
#define EGA_CR0C_STARTHI_READ(x)  EGA_MAKE_READ(EGA_CR0C_STARTHI, (x))

#define EGA_CR0D_MASK             0xFF
// CR0D RW: Start Address Low
#define EGA_CR0D_STARTLO_SHIFT    0
#define EGA_CR0D_STARTLO_ZERO     0
#define EGA_CR0D_STARTLO_MASK     (((1 << 8) - 1) << EGA_CR0D_STARTLO_SHIFT)
#define EGA_CR0D_STARTLO_WRITE(x) EGA_MAKE_WRITE(EGA_CR0D_STARTLO, (x))
#define EGA_CR0D_STARTLO_READ(x)  EGA_MAKE_READ(EGA_CR0D_STARTLO, (x))

#define EGA_CR0E_MASK             0xFF
// CR0E RW: Cursor Address High
#define EGA_CR0E_CURSORHI_SHIFT    0
#define EGA_CR0E_CURSORHI_ZERO     0
#define EGA_CR0E_CURSORHI_MASK     (((1 << 8) - 1) << EGA_CR0E_CURSORHI_SHIFT)
#define EGA_CR0E_CURSORHI_WRITE(x) EGA_MAKE_WRITE(EGA_CR0E_CURSORHI, (x))
#define EGA_CR0E_CURSORHI_READ(x)  EGA_MAKE_READ(EGA_CR0E_CURSORHI, (x))

#define EGA_CR0F_MASK              0xFF
// CR0F RW: Cursor Address Low
#define EGA_CR0F_CURSORLO_SHIFT    0
#define EGA_CR0F_CURSORLO_ZERO     0
#define EGA_CR0F_CURSORLO_MASK     (((1 << 8) - 1) << EGA_CR0F_CURSORLO_SHIFT)
#define EGA_CR0F_CURSORLO_WRITE(x) EGA_MAKE_WRITE(EGA_CR0F_CURSORLO, (x))
#define EGA_CR0F_CURSORLO_READ(x)  EGA_MAKE_READ(EGA_CR0F_CURSORLO, (x))

// CR10 W: Vertical Retrace Start, low 8 bits (bit 8 = CR07.2)

// CR10 R: Light Pen High

// CR11.0-3 W: Vertical Retrace End, low 4 bits
// CR11.4 W: Clear Vertical Interrupt (when 0)
// CR11.5 W: Enable Vertical Interrupt (when 0)

// CR11 R: Light Pen Low

// CR12: Vertical Display Enable End, low 8 bits (bit 8 = CR07.1)

// CR13: Offset (in words or dwords? TODO get this right --GM)
// This is the display pitch address.

// CR14.0-4: Underline Location minus 1

// CR15: Start Vertical Blanking, low 8 bits (bit 8 = CR07.3)

// CR16.0-4: End Vertical Blanking, low 5 bits

// CR17.0: Compatibility Mode Support
// When 0, this replaces CRTC address bit 13 with row scan count bit 0.
// Set to 0 for CGA graphics mode compatibility.

// CR17.1: Select Row Scan Counter
// When 0, this replaces CRTC address bit 14 with row scan count bit 0.
// IBM seems to insist on not explaining what this bit is for.
// But I will! Set this and CR17.0 to 0 for Hercules graphics mode compatibility.

// CR17.2: Horizontal Retrace Select
// When 1, vertical counters only increment on every 2nd horizontal retrace.
// When 0, vertical counters increment on every horizontal retrace.
// WARNING: This WILL require changes to your vertical timings in order to remain compatible!

// CR17.3: Count By Two
// When 1, the memory address only increments on every 2nd character clock.
// When 0, the memory address increments on every character clock.
// Set to 1 for Odd/Even mode.

// CR17.4: Output Control
// Set to 0 for normal operation.
// When 1, this floats all CRTC outputs.

// CR17.5: Address Wrap
// CR17.6 "Word Mode" must be set to 0 for this to have any effect.
// When 0, use memory address bit 13 for output address bit 0.
// When 1, use memory address bit 15 for output address bit 0.

// CR17.6: Word Mode
// When 0, all memory addresses outputs are shifted left by 1, and output address bit 0 is as per CR17.5.
// When 1, the memory address is used as-is for the output address.

// CR17.7: Hardware Reset
// Set to 1 for normal operation.
// When 0, horizontal and vertical retraces are forced to be deasserted.

// CR18: Line Compare, low 8 bits (bit 8 = CR07.4)
