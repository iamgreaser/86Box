//
// NOTE: DO NOT EDIT ega_regs.h DIRECTLY.
// Edit ega_regs.gen.h for the template and schema.
// Edit ega_regs.gen.py for the generator.
// Run ega_regs.gen.py with a suitable version of Python 3 to generate ega_regs.h.
// (3.13 works, older versions probably also work as far back as maybe 3.6? Not tested though.)
//

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

// 3BX/3DX CRTC I/O Address (~MONO MONITOR)
// This provides in input into the I/O address decoder PROM (U34, SHT 4).
#define EGA_W3C2_IOBASE_SHIFT    0
#define EGA_W3C2_IOBASE_ZERO     0
#define EGA_W3C2_IOBASE_MASK     (((1 << 1) - 1) << 0)
#define EGA_W3C2_IOBASE_WRITE(x) EGA_MAKE_WRITE(EGA_W3C2_IOBASE, x)
#define EGA_W3C2_IOBASE_READ(x)  EGA_MAKE_READ(EGA_W3C2_IOBASE, x)
#define EGA_W3C2_IOBASE_3BX      ((0b0) << 0)
#define EGA_W3C2_IOBASE_3DX      ((0b1) << 0)

// Enable RAM (RAM ENABLE)
// This provides an input into the memory address decoder PROM (U48, SHT 4).
#define EGA_W3C2_RAMENABLE_SHIFT    1
#define EGA_W3C2_RAMENABLE_ZERO     0
#define EGA_W3C2_RAMENABLE_MASK     (((1 << 1) - 1) << 1)
#define EGA_W3C2_RAMENABLE_WRITE(x) EGA_MAKE_WRITE(EGA_W3C2_RAMENABLE, x)
#define EGA_W3C2_RAMENABLE_READ(x)  EGA_MAKE_READ(EGA_W3C2_RAMENABLE, x)
#define EGA_W3C2_RAMENABLE_OFF      ((0b0) << 1)
#define EGA_W3C2_RAMENABLE_ON       ((0b1) << 1)

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
#define EGA_W3C2_CLOCKSEL_SHIFT    2
#define EGA_W3C2_CLOCKSEL_ZERO     0
#define EGA_W3C2_CLOCKSEL_MASK     (((1 << 2) - 1) << 2)
#define EGA_W3C2_CLOCKSEL_WRITE(x) EGA_MAKE_WRITE(EGA_W3C2_CLOCKSEL, x)
#define EGA_W3C2_CLOCKSEL_READ(x)  EGA_MAKE_READ(EGA_W3C2_CLOCKSEL, x)
#define EGA_W3C2_CLOCKSEL_14MHZ    ((0b00) << 2)
#define EGA_W3C2_CLOCKSEL_16MHZ    ((0b01) << 2)
#define EGA_W3C2_CLOCKSEL_EXTOSC   ((0b10) << 2)

// Disable Internal Video Drivers (~INTERNAL)
// When high, this severs the outputs on U36 (74LS244 4x2=8 buffer, SHT 9).
// These go to DE-9 output connector J3 (SHT 9).
// The only output that isn't severed is pin 1 which is hard-wired to GND.
// The feature connector goes past U36 but before the resistors going to J3.
#define EGA_W3C2_VIDDRIVERS_SHIFT    4
#define EGA_W3C2_VIDDRIVERS_ZERO     0
#define EGA_W3C2_VIDDRIVERS_MASK     (((1 << 1) - 1) << 4)
#define EGA_W3C2_VIDDRIVERS_WRITE(x) EGA_MAKE_WRITE(EGA_W3C2_VIDDRIVERS, x)
#define EGA_W3C2_VIDDRIVERS_READ(x)  EGA_MAKE_READ(EGA_W3C2_VIDDRIVERS, x)
#define EGA_W3C2_VIDDRIVERS_ON       ((0b0) << 4)
#define EGA_W3C2_VIDDRIVERS_OFF      ((0b1) << 4)

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
#define EGA_W3C2_OEPAGE_SHIFT    5
#define EGA_W3C2_OEPAGE_ZERO     0
#define EGA_W3C2_OEPAGE_MASK     (((1 << 1) - 1) << 5)
#define EGA_W3C2_OEPAGE_WRITE(x) EGA_MAKE_WRITE(EGA_W3C2_OEPAGE, x)
#define EGA_W3C2_OEPAGE_READ(x)  EGA_MAKE_READ(EGA_W3C2_OEPAGE, x)
#define EGA_W3C2_OEPAGE_HI       ((0b0) << 5)
#define EGA_W3C2_OEPAGE_LO       ((0b1) << 5)

// Horizontal Retrace Polarity (H POL), bit 6
// Vertical Retrace Polarity (V POL), bit 7
// 0 is positive, 1 is negative.
// These XOR the HSYNC/VSYNC values from the CRTC (U8, 74LS86 4x 2-XOR, SHT 9, 2 pairs used).
// The numbers given are the typical expected number of scanlines.
// (EGA only cares about 200-line and 350-line screens.)
#define EGA_W3C2_POLARITY_SHIFT     6
#define EGA_W3C2_POLARITY_ZERO      0
#define EGA_W3C2_POLARITY_MASK      (((1 << 2) - 1) << 6)
#define EGA_W3C2_POLARITY_WRITE(x)  EGA_MAKE_WRITE(EGA_W3C2_POLARITY, x)
#define EGA_W3C2_POLARITY_READ(x)   EGA_MAKE_READ(EGA_W3C2_POLARITY, x)
#define EGA_W3C2_POLARITY_VP_HP_200 ((0b00) << 6)
#define EGA_W3C2_POLARITY_VP_HN_400 ((0b01) << 6)
#define EGA_W3C2_POLARITY_VN_HP_350 ((0b10) << 6)
#define EGA_W3C2_POLARITY_VN_HN_480 ((0b11) << 6)

// W3C2 end
#define EGA_W3C2_MASK 0xff

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
#define EGA_R3C2_SWITCHSENSE_SHIFT    4
#define EGA_R3C2_SWITCHSENSE_ZERO     0
#define EGA_R3C2_SWITCHSENSE_MASK     (((1 << 1) - 1) << 4)
#define EGA_R3C2_SWITCHSENSE_WRITE(x) EGA_MAKE_WRITE(EGA_R3C2_SWITCHSENSE, x)
#define EGA_R3C2_SWITCHSENSE_READ(x)  EGA_MAKE_READ(EGA_R3C2_SWITCHSENSE, x)
#define EGA_R3C2_SWITCHSENSE_ON       ((0b0) << 4)
#define EGA_R3C2_SWITCHSENSE_OFF      ((0b1) << 4)

// Feature Code (SHT 10)
// Received from FEAT 0, FEAT 1 (pins 20 and 21 respectively on J4, SHT 10)
#define EGA_R3C2_FEATCODE_SHIFT    5
#define EGA_R3C2_FEATCODE_ZERO     0
#define EGA_R3C2_FEATCODE_MASK     (((1 << 2) - 1) << 5)
#define EGA_R3C2_FEATCODE_WRITE(x) EGA_MAKE_WRITE(EGA_R3C2_FEATCODE, x)
#define EGA_R3C2_FEATCODE_READ(x)  EGA_MAKE_READ(EGA_R3C2_FEATCODE, x)
#define EGA_R3C2_FEATCODE_00       ((0b00) << 5)
#define EGA_R3C2_FEATCODE_01       ((0b01) << 5)
#define EGA_R3C2_FEATCODE_10       ((0b10) << 5)
#define EGA_R3C2_FEATCODE_11       ((0b11) << 5)

// CRT Interrupt (from CRTC output, U8, SHT 3)
// This goes directly to IRQ2 (which cascades to IRQ9 on AT and up).
#define EGA_R3C2_CRTINT_SHIFT     7
#define EGA_R3C2_CRTINT_ZERO      0
#define EGA_R3C2_CRTINT_MASK      (((1 << 1) - 1) << 7)
#define EGA_R3C2_CRTINT_WRITE(x)  EGA_MAKE_WRITE(EGA_R3C2_CRTINT, x)
#define EGA_R3C2_CRTINT_READ(x)   EGA_MAKE_READ(EGA_R3C2_CRTINT, x)
#define EGA_R3C2_CRTINT_ACTIVEVID ((0b0) << 7)
#define EGA_R3C2_CRTINT_VRETRACE  ((0b1) << 7)

// R3C2 end
#define EGA_R3C2_MASK 0xf0

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
#define EGA_W3XA_FEATCTRL_SHIFT    0
#define EGA_W3XA_FEATCTRL_ZERO     0
#define EGA_W3XA_FEATCTRL_MASK     (((1 << 2) - 1) << 0)
#define EGA_W3XA_FEATCTRL_WRITE(x) EGA_MAKE_WRITE(EGA_W3XA_FEATCTRL, x)
#define EGA_W3XA_FEATCTRL_READ(x)  EGA_MAKE_READ(EGA_W3XA_FEATCTRL, x)
#define EGA_W3XA_FEATCTRL_00       ((0b00) << 0)
#define EGA_W3XA_FEATCTRL_01       ((0b01) << 0)
#define EGA_W3XA_FEATCTRL_10       ((0b10) << 0)
#define EGA_W3XA_FEATCTRL_11       ((0b11) << 0)

// W3XA end
#define EGA_W3XA_MASK 0x03

//
// 03BA/03DA R: Input Status Register One
//
// Bits 1,2 are from U35 (74LS367, SHT 9, when G2? asserted).
// Bits 0,3,4,5 are from the Attribute Controller (U24, SHT 8).
//

// Display Enable
#define EGA_R3XA_DISPENABLE_SHIFT     0
#define EGA_R3XA_DISPENABLE_ZERO      0
#define EGA_R3XA_DISPENABLE_MASK      (((1 << 1) - 1) << 0)
#define EGA_R3XA_DISPENABLE_WRITE(x)  EGA_MAKE_WRITE(EGA_R3XA_DISPENABLE, x)
#define EGA_R3XA_DISPENABLE_READ(x)   EGA_MAKE_READ(EGA_R3XA_DISPENABLE, x)
#define EGA_R3XA_DISPENABLE_RETRACE   ((0b0) << 0)
#define EGA_R3XA_DISPENABLE_ACTIVEVID ((0b1) << 0)

// Light Pen Strobe
// Q line from U18 (74LS74, 2x1-bit D-latch, SHT 9)
// - Also outputs to LPEN STB/LPSTB into the CRTC (U8, SHT 3)
// ~LPSET (SHT 4) goes into U18 PR
// ~LPCLR (SHT 4) goes into U18 CLR
// +5V goes into U18 D
// The NOT of ~LPEN IN (P2, 1x6 Berg connector, SHT 4) goes into U18 clock
#define EGA_R3XA_LPENSTROBE_SHIFT    1
#define EGA_R3XA_LPENSTROBE_ZERO     0
#define EGA_R3XA_LPENSTROBE_MASK     (((1 << 1) - 1) << 1)
#define EGA_R3XA_LPENSTROBE_WRITE(x) EGA_MAKE_WRITE(EGA_R3XA_LPENSTROBE, x)
#define EGA_R3XA_LPENSTROBE_READ(x)  EGA_MAKE_READ(EGA_R3XA_LPENSTROBE, x)
#define EGA_R3XA_LPENSTROBE_OFF      ((0b0) << 1)
#define EGA_R3XA_LPENSTROBE_ON       ((0b1) << 1)

// Light Pen Switch
// From ~LPEN SW line on P2 (1x6 Berg connector, SHT 9)
#define EGA_R3XA_LPENSWITCH_SHIFT    2
#define EGA_R3XA_LPENSWITCH_ZERO     0
#define EGA_R3XA_LPENSWITCH_MASK     (((1 << 1) - 1) << 2)
#define EGA_R3XA_LPENSWITCH_WRITE(x) EGA_MAKE_WRITE(EGA_R3XA_LPENSWITCH, x)
#define EGA_R3XA_LPENSWITCH_READ(x)  EGA_MAKE_READ(EGA_R3XA_LPENSWITCH, x)
#define EGA_R3XA_LPENSWITCH_ON       ((0b0) << 2)
#define EGA_R3XA_LPENSWITCH_OFF      ((0b1) << 2)

// Vertical Retrace
#define EGA_R3XA_VRETRACE_SHIFT     3
#define EGA_R3XA_VRETRACE_ZERO      0
#define EGA_R3XA_VRETRACE_MASK      (((1 << 1) - 1) << 3)
#define EGA_R3XA_VRETRACE_WRITE(x)  EGA_MAKE_WRITE(EGA_R3XA_VRETRACE, x)
#define EGA_R3XA_VRETRACE_READ(x)   EGA_MAKE_READ(EGA_R3XA_VRETRACE, x)
#define EGA_R3XA_VRETRACE_RETRACE   ((0b0) << 3)
#define EGA_R3XA_VRETRACE_ACTIVEVID ((0b1) << 3)

// Diagnostic Usage
// From the IBM EGA doc:
// "The Color Plane Enable register controls the multiplexer for the video wiring."
// The mux address is AR12.4-5.
#define EGA_R3XA_DIAGOUT0_SHIFT    4
#define EGA_R3XA_DIAGOUT0_ZERO     0
#define EGA_R3XA_DIAGOUT0_MASK     (((1 << 1) - 1) << 4)
#define EGA_R3XA_DIAGOUT0_WRITE(x) EGA_MAKE_WRITE(EGA_R3XA_DIAGOUT0, x)
#define EGA_R3XA_DIAGOUT0_READ(x)  EGA_MAKE_READ(EGA_R3XA_DIAGOUT0, x)
#define EGA_R3XA_DIAGOUT0_00_PB0   ((0b0) << 4)
#define EGA_R3XA_DIAGOUT0_00_PB1   ((0b1) << 4)
#define EGA_R3XA_DIAGOUT0_01_PG0   ((0b0) << 4)
#define EGA_R3XA_DIAGOUT0_01_PG1   ((0b1) << 4)
#define EGA_R3XA_DIAGOUT0_10_SG0   ((0b0) << 4)
#define EGA_R3XA_DIAGOUT0_10_SG1   ((0b1) << 4)
#define EGA_R3XA_DIAGOUT1_SHIFT    5
#define EGA_R3XA_DIAGOUT1_ZERO     0
#define EGA_R3XA_DIAGOUT1_MASK     (((1 << 1) - 1) << 5)
#define EGA_R3XA_DIAGOUT1_WRITE(x) EGA_MAKE_WRITE(EGA_R3XA_DIAGOUT1, x)
#define EGA_R3XA_DIAGOUT1_READ(x)  EGA_MAKE_READ(EGA_R3XA_DIAGOUT1, x)
#define EGA_R3XA_DIAGOUT1_00_PR0   ((0b0) << 5)
#define EGA_R3XA_DIAGOUT1_00_PR1   ((0b1) << 5)
#define EGA_R3XA_DIAGOUT1_01_SB0   ((0b0) << 5)
#define EGA_R3XA_DIAGOUT1_01_SB1   ((0b1) << 5)
#define EGA_R3XA_DIAGOUT1_10_SR0   ((0b0) << 5)
#define EGA_R3XA_DIAGOUT1_10_SR1   ((0b1) << 5)

// R3XA end
#define EGA_R3XA_MASK 0x3f

//
// 03C0 W: Attribute Controller registers
//

// Address parts
// The actual address
#define EGA_W3C0_ADDR_SHIFT    0
#define EGA_W3C0_ADDR_ZERO     0
#define EGA_W3C0_ADDR_MASK     (((1 << 5) - 1) << 0)
#define EGA_W3C0_ADDR_WRITE(x) EGA_MAKE_WRITE(EGA_W3C0_ADDR, x)
#define EGA_W3C0_ADDR_READ(x)  EGA_MAKE_READ(EGA_W3C0_ADDR, x)
// Screen display vs Letting the palette be editable
#define EGA_W3C0_PALSRC_SHIFT    5
#define EGA_W3C0_PALSRC_ZERO     0
#define EGA_W3C0_PALSRC_MASK     (((1 << 1) - 1) << 5)
#define EGA_W3C0_PALSRC_WRITE(x) EGA_MAKE_WRITE(EGA_W3C0_PALSRC, x)
#define EGA_W3C0_PALSRC_READ(x)  EGA_MAKE_READ(EGA_W3C0_PALSRC, x)
#define EGA_W3C0_PALSRC_BLANK    ((0b0) << 5)
#define EGA_W3C0_PALSRC_DISPLAY  ((0b1) << 5)

// W3C0 end
#define EGA_W3C0_MASK 0x3f

//
// 03C4 and 03C5 W: Sequencer registers
//

// Alpha (provides separate A14,A15 for planes 2 and 3)
#define EGA_SR04_ALPHA_SHIFT    0
#define EGA_SR04_ALPHA_ZERO     0
#define EGA_SR04_ALPHA_MASK     (((1 << 1) - 1) << 0)
#define EGA_SR04_ALPHA_WRITE(x) EGA_MAKE_WRITE(EGA_SR04_ALPHA, x)
#define EGA_SR04_ALPHA_READ(x)  EGA_MAKE_READ(EGA_SR04_ALPHA, x)
#define EGA_SR04_ALPHA_OFF      ((0b0) << 0)
#define EGA_SR04_ALPHA_ON       ((0b1) << 0)
// Extended Memory (lets A14,A15 be nonzero)
// NOTE: On the actual IBM EGA card, these are not address pins, they're 4 chip select pins.
#define EGA_SR04_EXTMEM_SHIFT    1
#define EGA_SR04_EXTMEM_ZERO     0
#define EGA_SR04_EXTMEM_MASK     (((1 << 1) - 1) << 1)
#define EGA_SR04_EXTMEM_WRITE(x) EGA_MAKE_WRITE(EGA_SR04_EXTMEM, x)
#define EGA_SR04_EXTMEM_READ(x)  EGA_MAKE_READ(EGA_SR04_EXTMEM, x)
#define EGA_SR04_EXTMEM_OFF      ((0b0) << 1)
#define EGA_SR04_EXTMEM_ON       ((0b1) << 1)
// Odd/Even (for plane write enables)
#define EGA_SR04_ODDEVEN_SHIFT    2
#define EGA_SR04_ODDEVEN_ZERO     0
#define EGA_SR04_ODDEVEN_MASK     (((1 << 1) - 1) << 2)
#define EGA_SR04_ODDEVEN_WRITE(x) EGA_MAKE_WRITE(EGA_SR04_ODDEVEN, x)
#define EGA_SR04_ODDEVEN_READ(x)  EGA_MAKE_READ(EGA_SR04_ODDEVEN, x)
#define EGA_SR04_ODDEVEN_ON       ((0b0) << 2)
#define EGA_SR04_ODDEVEN_OFF      ((0b1) << 2)
// SR04 end
#define EGA_SR04_MASK 0x07

//
// 03CE and 03CF W: Graphics Controller registers
//

// GR03.0-2: Rotate Count (rotate right by this amount)
#define EGA_GR03_ROR_SHIFT    0
#define EGA_GR03_ROR_ZERO     0
#define EGA_GR03_ROR_MASK     (((1 << 3) - 1) << 0)
#define EGA_GR03_ROR_WRITE(x) EGA_MAKE_WRITE(EGA_GR03_ROR, x)
#define EGA_GR03_ROR_READ(x)  EGA_MAKE_READ(EGA_GR03_ROR, x)
// GR03.3-4: Function Select
#define EGA_GR03_FUNC_SHIFT    3
#define EGA_GR03_FUNC_ZERO     0
#define EGA_GR03_FUNC_MASK     (((1 << 2) - 1) << 3)
#define EGA_GR03_FUNC_WRITE(x) EGA_MAKE_WRITE(EGA_GR03_FUNC, x)
#define EGA_GR03_FUNC_READ(x)  EGA_MAKE_READ(EGA_GR03_FUNC, x)
#define EGA_GR03_FUNC_SET      ((0b00) << 3)
#define EGA_GR03_FUNC_AND      ((0b01) << 3)
#define EGA_GR03_FUNC_OR       ((0b10) << 3)
#define EGA_GR03_FUNC_XOR      ((0b11) << 3)
// GR03 end
#define EGA_GR03_MASK 0x1f

// GR05.0-1: Write Mode
#define EGA_GR05_WRITEMODE_SHIFT    0
#define EGA_GR05_WRITEMODE_ZERO     0
#define EGA_GR05_WRITEMODE_MASK     (((1 << 2) - 1) << 0)
#define EGA_GR05_WRITEMODE_WRITE(x) EGA_MAKE_WRITE(EGA_GR05_WRITEMODE, x)
#define EGA_GR05_WRITEMODE_READ(x)  EGA_MAKE_READ(EGA_GR05_WRITEMODE, x)
#define EGA_GR05_WRITEMODE_0        ((0b00) << 0)
#define EGA_GR05_WRITEMODE_1        ((0b01) << 0)
#define EGA_GR05_WRITEMODE_2        ((0b10) << 0)
// GR05.2: Test Condition
#define EGA_GR05_TESTCOND_SHIFT        2
#define EGA_GR05_TESTCOND_ZERO         0
#define EGA_GR05_TESTCOND_MASK         (((1 << 1) - 1) << 2)
#define EGA_GR05_TESTCOND_WRITE(x)     EGA_MAKE_WRITE(EGA_GR05_TESTCOND, x)
#define EGA_GR05_TESTCOND_READ(x)      EGA_MAKE_READ(EGA_GR05_TESTCOND, x)
#define EGA_GR05_TESTCOND_NORMAL       ((0b0) << 2)
#define EGA_GR05_TESTCOND_FLOATOUTPUTS ((0b1) << 2)
// GR05.3: Read Mode
#define EGA_GR05_READMODE_SHIFT     3
#define EGA_GR05_READMODE_ZERO      0
#define EGA_GR05_READMODE_MASK      (((1 << 1) - 1) << 3)
#define EGA_GR05_READMODE_WRITE(x)  EGA_MAKE_WRITE(EGA_GR05_READMODE, x)
#define EGA_GR05_READMODE_READ(x)   EGA_MAKE_READ(EGA_GR05_READMODE, x)
#define EGA_GR05_READMODE_0_SINGLE  ((0b0) << 3)
#define EGA_GR05_READMODE_1_COMPARE ((0b1) << 3)
// GR05.4: Odd/Even (for plane reads)
#define EGA_GR05_ODDEVEN_SHIFT    4
#define EGA_GR05_ODDEVEN_ZERO     0
#define EGA_GR05_ODDEVEN_MASK     (((1 << 1) - 1) << 4)
#define EGA_GR05_ODDEVEN_WRITE(x) EGA_MAKE_WRITE(EGA_GR05_ODDEVEN, x)
#define EGA_GR05_ODDEVEN_READ(x)  EGA_MAKE_READ(EGA_GR05_ODDEVEN, x)
#define EGA_GR05_ODDEVEN_OFF      ((0b0) << 4)
#define EGA_GR05_ODDEVEN_ON       ((0b1) << 4)
// GR05.5: Shift Register
// - 0: Fully planar.
// - 1: Each pair of planes is considered to be chunky.
#define EGA_GR05_SHIFTMODE_SHIFT    5
#define EGA_GR05_SHIFTMODE_ZERO     0
#define EGA_GR05_SHIFTMODE_MASK     (((1 << 1) - 1) << 5)
#define EGA_GR05_SHIFTMODE_WRITE(x) EGA_MAKE_WRITE(EGA_GR05_SHIFTMODE, x)
#define EGA_GR05_SHIFTMODE_READ(x)  EGA_MAKE_READ(EGA_GR05_SHIFTMODE, x)
#define EGA_GR05_SHIFTMODE_PLANAR   ((0b0) << 5)
#define EGA_GR05_SHIFTMODE_2X2      ((0b1) << 5)
// GR05 end
#define EGA_GR05_MASK 0x3f

// GR06 GC #1 .0: Graphics Mode
#define EGA_GR06_0_GRAPHICS_SHIFT    0
#define EGA_GR06_0_GRAPHICS_ZERO     0
#define EGA_GR06_0_GRAPHICS_MASK     (((1 << 1) - 1) << 0)
#define EGA_GR06_0_GRAPHICS_WRITE(x) EGA_MAKE_WRITE(EGA_GR06_0_GRAPHICS, x)
#define EGA_GR06_0_GRAPHICS_READ(x)  EGA_MAKE_READ(EGA_GR06_0_GRAPHICS, x)
#define EGA_GR06_0_GRAPHICS_OFF      ((0b0) << 0)
#define EGA_GR06_0_GRAPHICS_ON       ((0b1) << 0)
// GR06 GC #1 .1: Odd/Even (for CPU VRAM address generation)
#define EGA_GR06_0_ODDEVEN_SHIFT    1
#define EGA_GR06_0_ODDEVEN_ZERO     0
#define EGA_GR06_0_ODDEVEN_MASK     (((1 << 1) - 1) << 1)
#define EGA_GR06_0_ODDEVEN_WRITE(x) EGA_MAKE_WRITE(EGA_GR06_0_ODDEVEN, x)
#define EGA_GR06_0_ODDEVEN_READ(x)  EGA_MAKE_READ(EGA_GR06_0_ODDEVEN, x)
#define EGA_GR06_0_ODDEVEN_OFF      ((0b0) << 1)
#define EGA_GR06_0_ODDEVEN_ON       ((0b1) << 1)
// GR06_0 end
#define EGA_GR06_0_MASK 0x03
// GR06 GC #2 .0-1: Memory Map
#define EGA_GR06_1_MEMORYMAP_SHIFT     0
#define EGA_GR06_1_MEMORYMAP_ZERO      0
#define EGA_GR06_1_MEMORYMAP_MASK      (((1 << 2) - 1) << 0)
#define EGA_GR06_1_MEMORYMAP_WRITE(x)  EGA_MAKE_WRITE(EGA_GR06_1_MEMORYMAP, x)
#define EGA_GR06_1_MEMORYMAP_READ(x)   EGA_MAKE_READ(EGA_GR06_1_MEMORYMAP, x)
#define EGA_GR06_1_MEMORYMAP_A000_128K ((0b00) << 0)
#define EGA_GR06_1_MEMORYMAP_A000_64K  ((0b01) << 0)
#define EGA_GR06_1_MEMORYMAP_B000_32K  ((0b10) << 0)
#define EGA_GR06_1_MEMORYMAP_B800_32K  ((0b11) << 0)
// GR06_1 end
#define EGA_GR06_1_MASK 0x03

//
// 03B4/03D4 and 03B5/03D5 W: CRT Controller registers
//
// All registers are write-only EXCEPT where otherwise noted.
// Some writeable addresses are shared with different read-only registers!
//

// CR00: Horizontal Total minus 2
#define EGA_CR00_HTOTAL_SHIFT    0
#define EGA_CR00_HTOTAL_ZERO     -2
#define EGA_CR00_HTOTAL_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR00_HTOTAL_WRITE(x) EGA_MAKE_WRITE(EGA_CR00_HTOTAL, x)
#define EGA_CR00_HTOTAL_READ(x)  EGA_MAKE_READ(EGA_CR00_HTOTAL, x)
// CR00 end
#define EGA_CR00_MASK 0xff

// CR01: Horizontal Display Enable End minus 1
#define EGA_CR01_HDISPEND_SHIFT    0
#define EGA_CR01_HDISPEND_ZERO     -1
#define EGA_CR01_HDISPEND_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR01_HDISPEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR01_HDISPEND, x)
#define EGA_CR01_HDISPEND_READ(x)  EGA_MAKE_READ(EGA_CR01_HDISPEND, x)
// CR01 end
#define EGA_CR01_MASK 0xff

// CR02: Start Horizontal Blanking
#define EGA_CR02_HBLANKBEG_SHIFT    0
#define EGA_CR02_HBLANKBEG_ZERO     0
#define EGA_CR02_HBLANKBEG_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR02_HBLANKBEG_WRITE(x) EGA_MAKE_WRITE(EGA_CR02_HBLANKBEG, x)
#define EGA_CR02_HBLANKBEG_READ(x)  EGA_MAKE_READ(EGA_CR02_HBLANKBEG, x)
// CR02 end
#define EGA_CR02_MASK 0xff

// CR03.0-4: End Horizontal Blanking, low 5 bits
#define EGA_CR03_HBLANKEND_SHIFT    0
#define EGA_CR03_HBLANKEND_ZERO     0
#define EGA_CR03_HBLANKEND_MASK     (((1 << 5) - 1) << 0)
#define EGA_CR03_HBLANKEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR03_HBLANKEND, x)
#define EGA_CR03_HBLANKEND_READ(x)  EGA_MAKE_READ(EGA_CR03_HBLANKEND, x)
// CR03.5-6: Display Enable Skew Control
// This basically moves the whole visible border right by CR03.5-6 character clocks.
#define EGA_CR03_HDISPSKEW_SHIFT    5
#define EGA_CR03_HDISPSKEW_ZERO     0
#define EGA_CR03_HDISPSKEW_MASK     (((1 << 2) - 1) << 5)
#define EGA_CR03_HDISPSKEW_WRITE(x) EGA_MAKE_WRITE(EGA_CR03_HDISPSKEW, x)
#define EGA_CR03_HDISPSKEW_READ(x)  EGA_MAKE_READ(EGA_CR03_HDISPSKEW, x)
// CR03 end
#define EGA_CR03_MASK 0x7f

// CR04: Start Horizontal Retrace
#define EGA_CR04_HSYNCBEG_SHIFT    0
#define EGA_CR04_HSYNCBEG_ZERO     0
#define EGA_CR04_HSYNCBEG_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR04_HSYNCBEG_WRITE(x) EGA_MAKE_WRITE(EGA_CR04_HSYNCBEG, x)
#define EGA_CR04_HSYNCBEG_READ(x)  EGA_MAKE_READ(EGA_CR04_HSYNCBEG, x)
// CR04 end
#define EGA_CR04_MASK 0xff

// CR05.0-4: End Horizontal Retrace, low 5 bits
#define EGA_CR05_HSYNCEND_SHIFT    0
#define EGA_CR05_HSYNCEND_ZERO     0
#define EGA_CR05_HSYNCEND_MASK     (((1 << 5) - 1) << 0)
#define EGA_CR05_HSYNCEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR05_HSYNCEND, x)
#define EGA_CR05_HSYNCEND_READ(x)  EGA_MAKE_READ(EGA_CR05_HSYNCEND, x)
// CR05.5-6: Horizontal Retrace Delay
// This basically moves the whole display plus border ot the left by this many character clocks.
// WARNING: This can clash with HSYNC which can have a weird effect on some displays!
#define EGA_CR05_HSYNCSKEW_SHIFT    5
#define EGA_CR05_HSYNCSKEW_ZERO     0
#define EGA_CR05_HSYNCSKEW_MASK     (((1 << 2) - 1) << 5)
#define EGA_CR05_HSYNCSKEW_WRITE(x) EGA_MAKE_WRITE(EGA_CR05_HSYNCSKEW, x)
#define EGA_CR05_HSYNCSKEW_READ(x)  EGA_MAKE_READ(EGA_CR05_HSYNCSKEW, x)
// CR05.7: Start Odd/Even Memory Address
// From the IBM EGA doc:
// "This bit controls whether the first CRT memory address output after a horizontal retrace begins with an even or an odd address."
#define EGA_CR05_HOEOFFS0_SHIFT    7
#define EGA_CR05_HOEOFFS0_ZERO     0
#define EGA_CR05_HOEOFFS0_MASK     (((1 << 1) - 1) << 7)
#define EGA_CR05_HOEOFFS0_WRITE(x) EGA_MAKE_WRITE(EGA_CR05_HOEOFFS0, x)
#define EGA_CR05_HOEOFFS0_READ(x)  EGA_MAKE_READ(EGA_CR05_HOEOFFS0, x)
// CR05 end
#define EGA_CR05_MASK 0xff

// CR06: Vertical Total, low 8 bits (bit 8 = CR07.0)
#define EGA_CR06_VTOTAL_SHIFT    0
#define EGA_CR06_VTOTAL_ZERO     0
#define EGA_CR06_VTOTAL_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR06_VTOTAL_WRITE(x) EGA_MAKE_WRITE(EGA_CR06_VTOTAL, x)
#define EGA_CR06_VTOTAL_READ(x)  EGA_MAKE_READ(EGA_CR06_VTOTAL, x)
// CR06 end
#define EGA_CR06_MASK 0xff

// CR07.0: Vertical Total bit 8 (low 8 bits = CR06)
#define EGA_CR07_VTOTAL_8_SHIFT    0
#define EGA_CR07_VTOTAL_8_ZERO     0
#define EGA_CR07_VTOTAL_8_MASK     (((1 << 1) - 1) << 0)
#define EGA_CR07_VTOTAL_8_WRITE(x) EGA_MAKE_WRITE(EGA_CR07_VTOTAL_8, x)
#define EGA_CR07_VTOTAL_8_READ(x)  EGA_MAKE_READ(EGA_CR07_VTOTAL_8, x)
// CR07.1: Vertical Display Enable End bit 8 (low 8 bits = CR12)
#define EGA_CR07_VDISPEND_8_SHIFT    1
#define EGA_CR07_VDISPEND_8_ZERO     0
#define EGA_CR07_VDISPEND_8_MASK     (((1 << 1) - 1) << 1)
#define EGA_CR07_VDISPEND_8_WRITE(x) EGA_MAKE_WRITE(EGA_CR07_VDISPEND_8, x)
#define EGA_CR07_VDISPEND_8_READ(x)  EGA_MAKE_READ(EGA_CR07_VDISPEND_8, x)
// CR07.2: Vertical Retrace Start bit 8 (low 8 bits = CR10 write)
#define EGA_CR07_VSYNCBEG_8_SHIFT    2
#define EGA_CR07_VSYNCBEG_8_ZERO     0
#define EGA_CR07_VSYNCBEG_8_MASK     (((1 << 1) - 1) << 2)
#define EGA_CR07_VSYNCBEG_8_WRITE(x) EGA_MAKE_WRITE(EGA_CR07_VSYNCBEG_8, x)
#define EGA_CR07_VSYNCBEG_8_READ(x)  EGA_MAKE_READ(EGA_CR07_VSYNCBEG_8, x)
// CR07.3: Start Vertical Blanking bit 8 (low 8 bits = CR15)
#define EGA_CR07_VBLANKBEG_8_SHIFT    3
#define EGA_CR07_VBLANKBEG_8_ZERO     0
#define EGA_CR07_VBLANKBEG_8_MASK     (((1 << 1) - 1) << 3)
#define EGA_CR07_VBLANKBEG_8_WRITE(x) EGA_MAKE_WRITE(EGA_CR07_VBLANKBEG_8, x)
#define EGA_CR07_VBLANKBEG_8_READ(x)  EGA_MAKE_READ(EGA_CR07_VBLANKBEG_8, x)
// CR07.4: Line Compare bit 8 (low 8 bits = CR18)
#define EGA_CR07_LINECOMPARE_8_SHIFT    4
#define EGA_CR07_LINECOMPARE_8_ZERO     0
#define EGA_CR07_LINECOMPARE_8_MASK     (((1 << 1) - 1) << 4)
#define EGA_CR07_LINECOMPARE_8_WRITE(x) EGA_MAKE_WRITE(EGA_CR07_LINECOMPARE_8, x)
#define EGA_CR07_LINECOMPARE_8_READ(x)  EGA_MAKE_READ(EGA_CR07_LINECOMPARE_8, x)
// Supposedly CR07.5 is bit 8 of the Cursor Location register (CR0A) but that's a 5-bit register...
// CR07 end
#define EGA_CR07_MASK 0x1f

// CR08.0-4: Preset Row Scan (vertical scrolling)
// From the IBM EGA doc:
// "Starting Row Scan Count after a Vertical Retrace"
#define EGA_CR08_VFINESCROLL_SHIFT    0
#define EGA_CR08_VFINESCROLL_ZERO     0
#define EGA_CR08_VFINESCROLL_MASK     (((1 << 5) - 1) << 0)
#define EGA_CR08_VFINESCROLL_WRITE(x) EGA_MAKE_WRITE(EGA_CR08_VFINESCROLL, x)
#define EGA_CR08_VFINESCROLL_READ(x)  EGA_MAKE_READ(EGA_CR08_VFINESCROLL, x)
// CR08 end
#define EGA_CR08_MASK 0x1f

// CR09.0-4: Maximum Scan Line minus 1
// This is the character height in text modes.
// In graphics modes this allows for scan-doubling and other multipliers.
#define EGA_CR09_CHARHEIGHT_SHIFT    0
#define EGA_CR09_CHARHEIGHT_ZERO     -1
#define EGA_CR09_CHARHEIGHT_MASK     (((1 << 5) - 1) << 0)
#define EGA_CR09_CHARHEIGHT_WRITE(x) EGA_MAKE_WRITE(EGA_CR09_CHARHEIGHT, x)
#define EGA_CR09_CHARHEIGHT_READ(x)  EGA_MAKE_READ(EGA_CR09_CHARHEIGHT, x)
// CR09 end
#define EGA_CR09_MASK 0x1f

// CR0A.0-4: Cursor Start minus 1
#define EGA_CR0A_CURSORBEG_SHIFT    0
#define EGA_CR0A_CURSORBEG_ZERO     -1
#define EGA_CR0A_CURSORBEG_MASK     (((1 << 5) - 1) << 0)
#define EGA_CR0A_CURSORBEG_WRITE(x) EGA_MAKE_WRITE(EGA_CR0A_CURSORBEG, x)
#define EGA_CR0A_CURSORBEG_READ(x)  EGA_MAKE_READ(EGA_CR0A_CURSORBEG, x)
// CR0A end
#define EGA_CR0A_MASK 0x1f

// CR0B.0-4: Cursor End
#define EGA_CR0B_CURSOREND_SHIFT    0
#define EGA_CR0B_CURSOREND_ZERO     0
#define EGA_CR0B_CURSOREND_MASK     (((1 << 5) - 1) << 0)
#define EGA_CR0B_CURSOREND_WRITE(x) EGA_MAKE_WRITE(EGA_CR0B_CURSOREND, x)
#define EGA_CR0B_CURSOREND_READ(x)  EGA_MAKE_READ(EGA_CR0B_CURSOREND, x)
// CR0B.5-6: Cursor Skew
// This kinda moves the cursor to the right by this many character clocks.
// But if it's on the first column, it can potentially be duplicated? TODO: REAL HARDWARE NEEDED: CONFIRM THIS! --GM
#define EGA_CR0B_CURSORSKEW_SHIFT    5
#define EGA_CR0B_CURSORSKEW_ZERO     0
#define EGA_CR0B_CURSORSKEW_MASK     (((1 << 2) - 1) << 5)
#define EGA_CR0B_CURSORSKEW_WRITE(x) EGA_MAKE_WRITE(EGA_CR0B_CURSORSKEW, x)
#define EGA_CR0B_CURSORSKEW_READ(x)  EGA_MAKE_READ(EGA_CR0B_CURSORSKEW, x)
// CR0B end
#define EGA_CR0B_MASK 0x7f

// CR0C RW: Start Address High
#define EGA_CR0C_STARTHI_SHIFT    0
#define EGA_CR0C_STARTHI_ZERO     0
#define EGA_CR0C_STARTHI_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR0C_STARTHI_WRITE(x) EGA_MAKE_WRITE(EGA_CR0C_STARTHI, x)
#define EGA_CR0C_STARTHI_READ(x)  EGA_MAKE_READ(EGA_CR0C_STARTHI, x)
// CR0C end
#define EGA_CR0C_MASK 0xff

// CR0D RW: Start Address Low
#define EGA_CR0D_STARTLO_SHIFT    0
#define EGA_CR0D_STARTLO_ZERO     0
#define EGA_CR0D_STARTLO_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR0D_STARTLO_WRITE(x) EGA_MAKE_WRITE(EGA_CR0D_STARTLO, x)
#define EGA_CR0D_STARTLO_READ(x)  EGA_MAKE_READ(EGA_CR0D_STARTLO, x)
// CR0D end
#define EGA_CR0D_MASK 0xff

// CR0E RW: Cursor Address High
#define EGA_CR0E_CURSORHI_SHIFT    0
#define EGA_CR0E_CURSORHI_ZERO     0
#define EGA_CR0E_CURSORHI_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR0E_CURSORHI_WRITE(x) EGA_MAKE_WRITE(EGA_CR0E_CURSORHI, x)
#define EGA_CR0E_CURSORHI_READ(x)  EGA_MAKE_READ(EGA_CR0E_CURSORHI, x)
// CR0E end
#define EGA_CR0E_MASK 0xff

// CR0F RW: Cursor Address Low
#define EGA_CR0F_CURSORLO_SHIFT    0
#define EGA_CR0F_CURSORLO_ZERO     0
#define EGA_CR0F_CURSORLO_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR0F_CURSORLO_WRITE(x) EGA_MAKE_WRITE(EGA_CR0F_CURSORLO, x)
#define EGA_CR0F_CURSORLO_READ(x)  EGA_MAKE_READ(EGA_CR0F_CURSORLO, x)
// CR0F end
#define EGA_CR0F_MASK 0xff

// CR10 W: Vertical Retrace Start, low 8 bits (bit 8 = CR07.2)
#define EGA_CR10_VSYNCBEG_SHIFT    0
#define EGA_CR10_VSYNCBEG_ZERO     0
#define EGA_CR10_VSYNCBEG_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR10_VSYNCBEG_WRITE(x) EGA_MAKE_WRITE(EGA_CR10_VSYNCBEG, x)
#define EGA_CR10_VSYNCBEG_READ(x)  EGA_MAKE_READ(EGA_CR10_VSYNCBEG, x)
// CR10 end
#define EGA_CR10_MASK 0xff

// CR10 R: Light Pen High
#define EGA_CR10R_LPENHI_SHIFT    0
#define EGA_CR10R_LPENHI_ZERO     0
#define EGA_CR10R_LPENHI_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR10R_LPENHI_WRITE(x) EGA_MAKE_WRITE(EGA_CR10R_LPENHI, x)
#define EGA_CR10R_LPENHI_READ(x)  EGA_MAKE_READ(EGA_CR10R_LPENHI, x)
// CR10R end
#define EGA_CR10R_MASK 0xff

// CR11.0-3 W: Vertical Retrace End, low 4 bits
#define EGA_CR11_VSYNCEND_SHIFT    0
#define EGA_CR11_VSYNCEND_ZERO     0
#define EGA_CR11_VSYNCEND_MASK     (((1 << 4) - 1) << 0)
#define EGA_CR11_VSYNCEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR11_VSYNCEND, x)
#define EGA_CR11_VSYNCEND_READ(x)  EGA_MAKE_READ(EGA_CR11_VSYNCEND, x)
// CR11.4 W: Clear Vertical Interrupt (when 0)
#define EGA_CR11_CLEARVINT_SHIFT    4
#define EGA_CR11_CLEARVINT_ZERO     0
#define EGA_CR11_CLEARVINT_MASK     (((1 << 1) - 1) << 4)
#define EGA_CR11_CLEARVINT_WRITE(x) EGA_MAKE_WRITE(EGA_CR11_CLEARVINT, x)
#define EGA_CR11_CLEARVINT_READ(x)  EGA_MAKE_READ(EGA_CR11_CLEARVINT, x)
#define EGA_CR11_CLEARVINT_ON       ((0b0) << 4)
#define EGA_CR11_CLEARVINT_OFF      ((0b1) << 4)
// CR11.5 W: Enable Vertical Interrupt (when 0)
#define EGA_CR11_ENABLEVINT_SHIFT    5
#define EGA_CR11_ENABLEVINT_ZERO     0
#define EGA_CR11_ENABLEVINT_MASK     (((1 << 1) - 1) << 5)
#define EGA_CR11_ENABLEVINT_WRITE(x) EGA_MAKE_WRITE(EGA_CR11_ENABLEVINT, x)
#define EGA_CR11_ENABLEVINT_READ(x)  EGA_MAKE_READ(EGA_CR11_ENABLEVINT, x)
#define EGA_CR11_ENABLEVINT_ON       ((0b0) << 5)
#define EGA_CR11_ENABLEVINT_OFF      ((0b1) << 5)
// CR11 end
#define EGA_CR11_MASK 0x3f

// CR11 R: Light Pen Low
#define EGA_CR11R_LPENLO_SHIFT    0
#define EGA_CR11R_LPENLO_ZERO     0
#define EGA_CR11R_LPENLO_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR11R_LPENLO_WRITE(x) EGA_MAKE_WRITE(EGA_CR11R_LPENLO, x)
#define EGA_CR11R_LPENLO_READ(x)  EGA_MAKE_READ(EGA_CR11R_LPENLO, x)
// CR11R end
#define EGA_CR11R_MASK 0xff

// CR12: Vertical Display Enable End, low 8 bits (bit 8 = CR07.1)
#define EGA_CR12_VDISPEND_SHIFT    0
#define EGA_CR12_VDISPEND_ZERO     0
#define EGA_CR12_VDISPEND_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR12_VDISPEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR12_VDISPEND, x)
#define EGA_CR12_VDISPEND_READ(x)  EGA_MAKE_READ(EGA_CR12_VDISPEND, x)
// CR12 end
#define EGA_CR12_MASK 0xff

// CR13: Offset (in words or dwords? TODO get this right --GM)
// This is the display pitch address.
#define EGA_CR13_OFFSET_SHIFT    0
#define EGA_CR13_OFFSET_ZERO     0
#define EGA_CR13_OFFSET_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR13_OFFSET_WRITE(x) EGA_MAKE_WRITE(EGA_CR13_OFFSET, x)
#define EGA_CR13_OFFSET_READ(x)  EGA_MAKE_READ(EGA_CR13_OFFSET, x)
// CR13 end
#define EGA_CR13_MASK 0xff

// CR14.0-4: Underline Location minus 1
#define EGA_CR14_UNDERLINE_SHIFT    0
#define EGA_CR14_UNDERLINE_ZERO     -1
#define EGA_CR14_UNDERLINE_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR14_UNDERLINE_WRITE(x) EGA_MAKE_WRITE(EGA_CR14_UNDERLINE, x)
#define EGA_CR14_UNDERLINE_READ(x)  EGA_MAKE_READ(EGA_CR14_UNDERLINE, x)
// CR14 end
#define EGA_CR14_MASK 0xff

// CR15: Start Vertical Blanking, low 8 bits (bit 8 = CR07.3)
#define EGA_CR15_VBLANKBEG_SHIFT    0
#define EGA_CR15_VBLANKBEG_ZERO     0
#define EGA_CR15_VBLANKBEG_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR15_VBLANKBEG_WRITE(x) EGA_MAKE_WRITE(EGA_CR15_VBLANKBEG, x)
#define EGA_CR15_VBLANKBEG_READ(x)  EGA_MAKE_READ(EGA_CR15_VBLANKBEG, x)
// CR15 end
#define EGA_CR15_MASK 0xff

// CR16.0-4: End Vertical Blanking, low 5 bits
#define EGA_CR16_VBLANKEND_SHIFT    0
#define EGA_CR16_VBLANKEND_ZERO     0
#define EGA_CR16_VBLANKEND_MASK     (((1 << 5) - 1) << 0)
#define EGA_CR16_VBLANKEND_WRITE(x) EGA_MAKE_WRITE(EGA_CR16_VBLANKEND, x)
#define EGA_CR16_VBLANKEND_READ(x)  EGA_MAKE_READ(EGA_CR16_VBLANKEND, x)
// CR16 end
#define EGA_CR16_MASK 0x1f

// CR17.0: Compatibility Mode Support
// When 0, this replaces CRTC address bit 13 with row scan count bit 0.
// Set to 0 for CGA graphics mode compatibility.
#define EGA_CR17_A13_SHIFT    0
#define EGA_CR17_A13_ZERO     0
#define EGA_CR17_A13_MASK     (((1 << 1) - 1) << 0)
#define EGA_CR17_A13_WRITE(x) EGA_MAKE_WRITE(EGA_CR17_A13, x)
#define EGA_CR17_A13_READ(x)  EGA_MAKE_READ(EGA_CR17_A13, x)
#define EGA_CR17_A13_ROW0     ((0b0, ) << 0)
#define EGA_CR17_A13_NORMAL   ((0b1) << 0)

// CR17.1: Select Row Scan Counter
// When 0, this replaces CRTC address bit 14 with row scan count bit 0.
// IBM seems to insist on not explaining what this bit is for.
// But I will! Set this and CR17.0 to 0 for Hercules graphics mode compatibility.
#define EGA_CR17_A14_SHIFT    1
#define EGA_CR17_A14_ZERO     0
#define EGA_CR17_A14_MASK     (((1 << 1) - 1) << 1)
#define EGA_CR17_A14_WRITE(x) EGA_MAKE_WRITE(EGA_CR17_A14, x)
#define EGA_CR17_A14_READ(x)  EGA_MAKE_READ(EGA_CR17_A14, x)
#define EGA_CR17_A14_ROW1     ((0b0, ) << 1)
#define EGA_CR17_A14_NORMAL   ((0b1) << 1)

// CR17.2: Horizontal Retrace Select
// When 1, vertical counters only increment on every 2nd horizontal retrace.
// When 0, vertical counters increment on every horizontal retrace.
// WARNING: This WILL require changes to your vertical timings in order to remain compatible!
#define EGA_CR17_VDIVIDE_SHIFT    2
#define EGA_CR17_VDIVIDE_ZERO     0
#define EGA_CR17_VDIVIDE_MASK     (((1 << 1) - 1) << 2)
#define EGA_CR17_VDIVIDE_WRITE(x) EGA_MAKE_WRITE(EGA_CR17_VDIVIDE, x)
#define EGA_CR17_VDIVIDE_READ(x)  EGA_MAKE_READ(EGA_CR17_VDIVIDE, x)
#define EGA_CR17_VDIVIDE_NORMAL   ((0b0, ) << 2)
#define EGA_CR17_VDIVIDE_DIV2     ((0b1) << 2)

// CR17.3: Count By Two
// When 1, the memory address only increments on every 2nd character clock.
// When 0, the memory address increments on every character clock.
// Set to 1 for Odd/Even mode.
#define EGA_CR17_ADDRDIVIDE0_SHIFT    3
#define EGA_CR17_ADDRDIVIDE0_ZERO     0
#define EGA_CR17_ADDRDIVIDE0_MASK     (((1 << 1) - 1) << 3)
#define EGA_CR17_ADDRDIVIDE0_WRITE(x) EGA_MAKE_WRITE(EGA_CR17_ADDRDIVIDE0, x)
#define EGA_CR17_ADDRDIVIDE0_READ(x)  EGA_MAKE_READ(EGA_CR17_ADDRDIVIDE0, x)
#define EGA_CR17_ADDRDIVIDE0_NORMAL   ((0b0, ) << 3)
#define EGA_CR17_ADDRDIVIDE0_DIV2     ((0b1) << 3)

// CR17.4: Output Control
// Set to 0 for normal operation.
// When 1, this floats all CRTC outputs.
#define EGA_CR17_OUTCTRL_SHIFT        4
#define EGA_CR17_OUTCTRL_ZERO         0
#define EGA_CR17_OUTCTRL_MASK         (((1 << 1) - 1) << 4)
#define EGA_CR17_OUTCTRL_WRITE(x)     EGA_MAKE_WRITE(EGA_CR17_OUTCTRL, x)
#define EGA_CR17_OUTCTRL_READ(x)      EGA_MAKE_READ(EGA_CR17_OUTCTRL, x)
#define EGA_CR17_OUTCTRL_NORMAL       ((0b0, ) << 4)
#define EGA_CR17_OUTCTRL_FLOATOUTPUTS ((0b1) << 4)

// CR17.5: Address Wrap
// CR17.6 "Word Mode" must be set to 0 for this to have any effect.
// When 0, use memory address bit 13 for output address bit 0.
// When 1, use memory address bit 15 for output address bit 0.
#define EGA_CR17_WRAPBIT_SHIFT    5
#define EGA_CR17_WRAPBIT_ZERO     0
#define EGA_CR17_WRAPBIT_MASK     (((1 << 1) - 1) << 5)
#define EGA_CR17_WRAPBIT_WRITE(x) EGA_MAKE_WRITE(EGA_CR17_WRAPBIT, x)
#define EGA_CR17_WRAPBIT_READ(x)  EGA_MAKE_READ(EGA_CR17_WRAPBIT, x)
#define EGA_CR17_WRAPBIT_A13      ((0b0, ) << 5)
#define EGA_CR17_WRAPBIT_A15      ((0b1) << 5)

// CR17.6: Word Mode
// When 0, all memory addresses outputs are shifted left by 1, and output address bit 0 is as per CR17.5.
// When 1, the memory address is used as-is for the output address.
#define EGA_CR17_ADDRSHIFT0_SHIFT    6
#define EGA_CR17_ADDRSHIFT0_ZERO     0
#define EGA_CR17_ADDRSHIFT0_MASK     (((1 << 1) - 1) << 6)
#define EGA_CR17_ADDRSHIFT0_WRITE(x) EGA_MAKE_WRITE(EGA_CR17_ADDRSHIFT0, x)
#define EGA_CR17_ADDRSHIFT0_READ(x)  EGA_MAKE_READ(EGA_CR17_ADDRSHIFT0, x)
#define EGA_CR17_ADDRSHIFT0_1        ((0b0, ) << 6)
#define EGA_CR17_ADDRSHIFT0_0        ((0b1) << 6)

// CR17.7: Hardware Reset
// Set to 1 for normal operation.
// When 0, horizontal and vertical retraces are forced to be deasserted.
#define EGA_CR17_RESET_SHIFT     7
#define EGA_CR17_RESET_ZERO      0
#define EGA_CR17_RESET_MASK      (((1 << 1) - 1) << 7)
#define EGA_CR17_RESET_WRITE(x)  EGA_MAKE_WRITE(EGA_CR17_RESET, x)
#define EGA_CR17_RESET_READ(x)   EGA_MAKE_READ(EGA_CR17_RESET, x)
#define EGA_CR17_RESET_RESETTING ((0b0, ) << 7)
#define EGA_CR17_RESET_NORMAL    ((0b1) << 7)
// CR17 end
#define EGA_CR17_MASK 0xff

// CR18: Line Compare, low 8 bits (bit 8 = CR07.4)
#define EGA_CR18_LINECOMPARE_SHIFT    0
#define EGA_CR18_LINECOMPARE_ZERO     0
#define EGA_CR18_LINECOMPARE_MASK     (((1 << 8) - 1) << 0)
#define EGA_CR18_LINECOMPARE_WRITE(x) EGA_MAKE_WRITE(EGA_CR18_LINECOMPARE, x)
#define EGA_CR18_LINECOMPARE_READ(x)  EGA_MAKE_READ(EGA_CR18_LINECOMPARE, x)
// CR18 end
#define EGA_CR18_MASK 0xff
