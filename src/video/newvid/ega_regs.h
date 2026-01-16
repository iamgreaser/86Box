//
// 03C2 W: Miscellaneous Output Register
//
// U37, 74LS273 8-bit D-latch, SHT 9
//

// 3BX/3DX CRTC I/O Address (~MONO MONITOR)
// This provides in input into the I/O address decoder PROM (U34, SHT 4).
#define EGA_W3C2_IOBASE_SHIFT 0
#define EGA_W3C2_IOBASE_MASK  (0b1 << EGA_W3C2_IOBASE_SHIFT)
#define EGA_W3C2_IOBASE_3BX   (0b0 << EGA_W3C2_IOBASE_SHIFT)
#define EGA_W3C2_IOBASE_3DX   (0b1 << EGA_W3C2_IOBASE_SHIFT)

// Enable RAM (RAM ENABLE)
// This provides an input into the memory address decoder PROM (U48, SHT 4).
#define EGA_W3C2_RAMENABLE_SHIFT 1
#define EGA_W3C2_RAMENABLE_MASK  (0b1 << EGA_W3C2_RAMENABLE_SHIFT)
#define EGA_W3C2_RAMENABLE_OFF   (0b0 << EGA_W3C2_RAMENABLE_SHIFT)
#define EGA_W3C2_RAMENABLE_ON    (0b1 << EGA_W3C2_RAMENABLE_SHIFT)

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
#define EGA_W3C2_CLOCKSEL_SHIFT  2
#define EGA_W3C2_CLOCKSEL_MASK   (0b11 << EGA_W3C2_CLOCKSEL_SHIFT)
#define EGA_W3C2_CLOCKSEL_14MHZ  (0b00 << EGA_W3C2_CLOCKSEL_SHIFT)
#define EGA_W3C2_CLOCKSEL_16MHZ  (0b01 << EGA_W3C2_CLOCKSEL_SHIFT)
#define EGA_W3C2_CLOCKSEL_EXTOSC (0b10 << EGA_W3C2_CLOCKSEL_SHIFT)

// Disable Internal Video Drivers (~INTERNAL)
// When high, this severs the outputs on U36 (74LS244 4x2=8 buffer, SHT 9).
// These go to DE-9 output connector J3 (SHT 9).
// The only output that isn't severed is pin 1 which is hard-wired to GND.
// The feature connector goes past U36 but before the resistors going to J3.
#define EGA_W3C2_VIDDRIVERS_SHIFT 4
#define EGA_W3C2_VIDDRIVERS_MASK  (0b1 << EGA_W3C2_VIDDRIVERS_SHIFT)
#define EGA_W3C2_VIDDRIVERS_ON    (0b0 << EGA_W3C2_VIDDRIVERS_SHIFT)
#define EGA_W3C2_VIDDRIVERS_OFF   (0b1 << EGA_W3C2_VIDDRIVERS_SHIFT)

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
#define EGA_W3C2_OEPAGE_SHIFT 5
#define EGA_W3C2_OEPAGE_MASK  (0b1 << EGA_W3C2_OEPAGE_SHIFT)
#define EGA_W3C2_OEPAGE_HI    (0b0 << EGA_W3C2_OEPAGE_SHIFT)
#define EGA_W3C2_OEPAGE_LO    (0b1 << EGA_W3C2_OEPAGE_SHIFT)

// Horizontal Retrace Polarity (H POL), bit 6
// Vertical Retrace Polarity (V POL), bit 7
// 0 is positive, 1 is negative.
// These XOR the HSYNC/VSYNC values from the CRTC (U8, 74LS86 4x 2-XOR, SHT 9, 2 pairs used).
// The numbers given are the typical expected number of scanlines.
// (EGA only cares about 200-line and 350-line screens.)
#define EGA_W3C2_POLARITY_SHIFT     6
#define EGA_W3C2_POLARITY_MASK      (0b11 << EGA_W3C2_POLARITY_SHIFT)
#define EGA_W3C2_POLARITY_VP_HP_200 (0b00 << EGA_W3C2_POLARITY_SHIFT)
#define EGA_W3C2_POLARITY_VP_HN_400 (0b01 << EGA_W3C2_POLARITY_SHIFT)
#define EGA_W3C2_POLARITY_VN_HP_350 (0b10 << EGA_W3C2_POLARITY_SHIFT)
#define EGA_W3C2_POLARITY_VN_HN_480 (0b11 << EGA_W3C2_POLARITY_SHIFT)

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
#define EGA_W3C0_ADDR_SHIFT     0
#define EGA_W3C0_ADDR_MASK      (0b11111 << EGA_W3C0_ADDR_SHIFT)
// Screen display vs Letting the palette be editable
#define EGA_W3C0_PALSRC_SHIFT   5
#define EGA_W3C0_PALSRC_MASK    (0b1 << EGA_W3C0_PALSRC_SHIFT)
#define EGA_W3C0_PALSRC_BLANK   (0b0 << EGA_W3C0_PALSRC_SHIFT)
#define EGA_W3C0_PALSRC_DISPLAY (0b1 << EGA_W3C0_PALSRC_SHIFT)

//
// 03C4 and 03C5 W: Sequencer registers
//

// Alpha (provides separate A14,A15 for planes 2 and 3)
#define EGA_SR04_ALPHA_SHIFT   0
#define EGA_SR04_ALPHA_MASK    (0b1 << EGA_SR04_ALPHA_SHIFT)
#define EGA_SR04_ALPHA_OFF     (0b0 << EGA_SR04_ALPHA_SHIFT)
#define EGA_SR04_ALPHA_ON      (0b1 << EGA_SR04_ALPHA_SHIFT)
// Extended Memory (lets A14,A15 be nonzero)
// NOTE: On the actual IBM EGA card, these are not address pins, they're 4 chip select pins.
#define EGA_SR04_EXTMEM_SHIFT   0
#define EGA_SR04_EXTMEM_MASK    (0b1 << EGA_SR04_EXTMEM_SHIFT)
#define EGA_SR04_EXTMEM_OFF     (0b0 << EGA_SR04_EXTMEM_SHIFT)
#define EGA_SR04_EXTMEM_ON      (0b1 << EGA_SR04_EXTMEM_SHIFT)
// Odd/Even (for plane write enables)
#define EGA_SR04_ODDEVEN_SHIFT   2
#define EGA_SR04_ODDEVEN_MASK    (0b1 << EGA_SR04_ODDEVEN_SHIFT)
#define EGA_SR04_ODDEVEN_ON      (0b0 << EGA_SR04_ODDEVEN_SHIFT)
#define EGA_SR04_ODDEVEN_OFF     (0b1 << EGA_SR04_ODDEVEN_SHIFT)

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
#define EGA_GR05_ODDEVEN_SHIFT     4
#define EGA_GR05_ODDEVEN_MASK      (0b1 << EGA_GR05_ODDEVEN_SHIFT)
#define EGA_GR05_ODDEVEN_OFF       (0b0 << EGA_GR05_ODDEVEN_SHIFT)
#define EGA_GR05_ODDEVEN_ON        (0b1 << EGA_GR05_ODDEVEN_SHIFT)
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
