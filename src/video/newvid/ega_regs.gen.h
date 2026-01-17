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
!startreg R3C2

// Switch Sense (SW1 muxed through U15 (74LS153 4-to-1 mux), SHT 9)
// Selected via EGA_W3C2_CLOCKSEL_* - write to 03C2.2-3:
//   - 00 = SW1 switch 1 (0 = closed)
//   - 01 = SW1 switch 2 (0 = closed)
//   - 10 = SW1 switch 3 (0 = closed)
//   - 11 = SW1 switch 4 (0 = closed)
!field SWITCHSENSE 4 1 0 ON=0b0 OFF=0b1

// Feature Code (SHT 10)
// Received from FEAT 0, FEAT 1 (pins 20 and 21 respectively on J4, SHT 10)
!field FEATCODE 5 2 0 00=0b00 01=0b01 10=0b10 11=0b11

// CRT Interrupt (from CRTC output, U8, SHT 3)
// This goes directly to IRQ2 (which cascades to IRQ9 on AT and up).
!field CRTINT 7 1 0 ACTIVEVID=0b0 VRETRACE=0b1

!endreg R3C2

//
// 03BA/03DA W: Feature Control Register
//
!startreg W3XA

// Feature Control Bits
// Sets FC0, FC1 (pins 19 and 17 respectively on J4, SHT 10)
//
// On boot (original IBM EGA), the following happens:
// - On both 3BA and 3DA, set FC1:FC0=01; then read 3C2
// - On both 3BA and 3DA, set FC1:FC0=10; then read 3C2
//
!field FEATCTRL 0 2 0 00=0b00 01=0b01 10=0b10 11=0b11

!endreg W3XA

//
// 03BA/03DA R: Input Status Register One
//
// Bits 1,2 are from U35 (74LS367, SHT 9, when G2? asserted).
// Bits 0,3,4,5 are from the Attribute Controller (U24, SHT 8).
//
!startreg R3XA

// Display Enable
!field DISPENABLE 0 1 0 RETRACE=0b0 ACTIVEVID=0b1

// Light Pen Strobe
// Q line from U18 (74LS74, 2x1-bit D-latch, SHT 9)
// - Also outputs to LPEN STB/LPSTB into the CRTC (U8, SHT 3)
// ~LPSET (SHT 4) goes into U18 PR
// ~LPCLR (SHT 4) goes into U18 CLR
// +5V goes into U18 D
// The NOT of ~LPEN IN (P2, 1x6 Berg connector, SHT 4) goes into U18 clock
!field LPENSTROBE 1 1 0 OFF=0b0 ON=0b1

// Light Pen Switch
// From ~LPEN SW line on P2 (1x6 Berg connector, SHT 9)
!field LPENSWITCH 2 1 0 ON=0b0 OFF=0b1

// Vertical Retrace
!field VRETRACE 3 1 0 RETRACE=0b0 ACTIVEVID=0b1

// Diagnostic Usage
// From the IBM EGA doc:
// "The Color Plane Enable register controls the multiplexer for the video wiring."
// The mux address is AR12.4-5.
!field DIAGOUT0 4 1 0 00_PB0=0b0 00_PB1=0b1 01_PG0=0b0 01_PG1=0b1 10_SG0=0b0 10_SG1=0b1
!field DIAGOUT1 5 1 0 00_PR0=0b0 00_PR1=0b1 01_SB0=0b0 01_SB1=0b1 10_SR0=0b0 10_SR1=0b1

!endreg R3XA

//
// 03C0 W: Attribute Controller registers
//
!startreg W3C0

// Address parts
// The actual address
!field ADDR 0 5 0
// Screen display vs Letting the palette be editable
!field PALSRC 5 1 0 BLANK=0b0 DISPLAY=0b1

!endreg W3C0

!startreg AC10
// AC10.0: Graphics/Alphanumeric Mode
// When 1:
// - Low 4 bits of attribute input are our pixel data.
// - High 4 bits of attribute input are from raw plane 1, but ignored for this.
// When 0:
// - All 8 bits of attribute input, assummed to be from raw plane 1, are attribute data.
// - Raw plane 2 input is our character data.
// - Those 8 bits feed a 4x 2-to-1 MUX of the background and foreground attribute data.
!field GRAPHICS 0 1 0 OFF=0b0 ON=0b1
// AC10.1: Monochrome Display/Color Display
// This affects text-mode attributes and graphics-mode blinking.
!field MONOATTR 1 1 0 OFF=0b0 ON=0b1
// AC10.2: Enable Line Graphics Character Codes
// Used for 9-dot text mode.
// When 1, if raw plane 0 data is from 0xC0 to 0xDF, the 8th dot is duplicated into the 9th dot.
!field 9DOTLINES 2 1 0 OFF=0b0 ON=0b1
// AC10.3: Enable Blink/Select Background Intensity
!field ATTR7 3 1 0 BGINTENSITY=0b0 BLINK=0b1
!endreg AC10

!startreg AC11
// AC11.0-5: Overscan Color
!field BORDER 0 6 0
!endreg AC11

!startreg AC12
// AC12.0-3: Enable Color Plane
!field PLANEMASK 0 4 0
// AC12.4-5: Video Status MUX
// Determines the sources of the values of 3BA/3DA.4-5.
!field DIAGMUX 4 2 0 PRPB=0b00 SBPG=0b01 SRSG=0b10
!endreg AC12

!startreg AC13
// These are for "Horizontal Pel Panning".
// Effectively this is equivalent to:
// - 4 8-bit SIPO shift registers
// - All shift register outputs go to their own 8-to-1 MUX (AC13.0-2)
// - Each 8-to-1 MUX goes to 1 input of a 2-to-1 MUX (AC13.3)
// - The other 2-to-1 MUX input is the inputs to the shift registers
// - The 2-to-1 MUX outputs form the resulting colour palette index.
//
// AC13.0-2: Horizontal pel panning shift register output select
!field DELAY 0 3 -1
// AC13.3: Bypass horizontal pel panning shift registers
!field BYPASS 3 1 0 OFF=0b0 ON=0b1
!endreg AC13

//
// 03C4 and 03C5 W: Sequencer registers
//

!startreg SR00
// SR00.0: Asynchronous Reset
// 0 = halt, 1 = go
// From the IBM EGA doc:
// "Resetting the sequencer with this bit can cause data loss in the dynamic RAMs."
!field ASYNCRST 0 1 0 RESET=0b0 NORMAL=0b1
// SR00.1: Synchronous Reset
// 0 = halt, 1 = go
// From the IBM EGA doc:
// "Reset the sequencer with this bit before changing the Clocking Mode Register [(SR01)], if memory contents are to be preserved."
!field SYNCRST 1 1 0 RESET=0b0 NORMAL=0b1
!endreg SR00

!startreg SR01
// SR01.0: 8/9 Dot Clocks
// Number of dot clocks per character clock.
!field CHARCLK 0 1 0 9DOT=0b0 8DOT=0b1
// SR01.1: Bandwidth
// Every 4 character clocks, there are 5 opportunities to access memory.
// This field determines how many are reserved for the video card.
// From the IBM EGA doc:
// "All high resolution modes must provide the CRTC with 4 out of 5 memory cycles in order to refresh the display image."
// High resolution means the dot clock isn't being divided by 2.
!field BANDWIDTH 1 1 0 CPU_3_IN_5=0b0 CPU_1_IN_5=0b1 CRTC_4_IN_5=0b0 CRTC_2_IN_5=0b1
// SR01.2: Shift Load
// How many character clocks do we need before loading the next data into the shift registers?
// TODO: REAL HARDWARE NEEDED: How does this behave when the number of char clocks per scanline is odd? --GM
!field SHIFTLOAD0 2 1 0 NORMAL=0b0 DIV2=0b1
// SR01.3: Dot Clock
// Clock divisor for generating the dot clock.
!field DOTCLK 3 1 0 NORMAL=0b0 DIV2=0b1
!endreg SR01

!startreg SR02
// SR02.0-3: Map Mask
// Setting a bit to 1 enables writes for its corresponding plane.
!field MAPMASK 0 4 0
!endreg SR02

!startreg SR03
// SR03.0-1: Character Map Select B
// Selects bits A14,A15 to be used for fetching the font when attr bit 3 is 1.
!field MAPB0 0 2 0
// SR03.2-3: Character Map Select A
// Selects bits A14,A15 to be used for fetching the font when attr bit 3 is 0.
!field MAPA0 2 2 0
!endreg SR03

!startreg SR04
// SR04.0: Alpha (provides separate A14,A15 for planes 2 and 3)
!field GRAPHICS 0 1 0 ON=0b0 OFF=0b1
// SR04.1: Extended Memory (lets A14,A15 be nonzero)
// NOTE: On the actual IBM EGA card, these are not address pins, they're 4 chip select pins.
!field EXTMEM 1 1 0 OFF=0b0 ON=0b1
// SR04.2: Odd/Even (for plane write enables)
!field ODDEVEN 2 1 0 ON=0b0 OFF=0b1
!endreg SR04

//
// 03CE and 03CF W: Graphics Controller registers
//

!startreg GR03
// GR03.0-2: Rotate Count (rotate right by this amount)
!field ROR 0 3 0
// GR03.3-4: Function Select
!field FUNC 3 2 0 SET=0b00 AND=0b01 OR=0b10 XOR=0b11
!endreg GR03

!startreg GR05
// GR05.0-1: Write Mode
!field WRITEMODE 0 2 0 0=0b00 1=0b01 2=0b10
// GR05.2: Test Condition
!field TESTCOND 2 1 0 NORMAL=0b0 FLOATOUTPUTS=0b1
// GR05.3: Read Mode
!field READMODE 3 1 0 0_SINGLE=0b0 1_COMPARE=0b1
// GR05.4: Odd/Even (for plane reads)
!field ODDEVEN 4 1 0 OFF=0b0 ON=0b1
// GR05.5: Shift Register
// - 0: Fully planar.
// - 1: Each pair of planes is considered to be chunky.
!field SHIFTMODE 5 1 0 PLANAR=0b0 2X2=0b1
!endreg GR05

!startreg GR06_0
// GR06 GC #1 .0: Graphics Mode
!field GRAPHICS 0 1 0 OFF=0b0 ON=0b1
// GR06 GC #1 .1: Odd/Even (for CPU VRAM address generation)
!field ODDEVEN 1 1 0 OFF=0b0 ON=0b1
!endreg GR06_0
!startreg GR06_1
// GR06 GC #2 .0-1: Memory Map
!field MEMORYMAP 0 2 0 A000_128K=0b00 A000_64K=0b01 B000_32K=0b10 B800_32K=0b11
!endreg GR06_1

//
// 03B4/03D4 and 03B5/03D5 W: CRT Controller registers
//
// All registers are write-only EXCEPT where otherwise noted.
// Some writeable addresses are shared with different read-only registers!
//

!startreg CR00
// CR00: Horizontal Total minus 2
!field HTOTAL 0 8 -2
!endreg CR00

!startreg CR01
// CR01: Horizontal Display Enable End minus 1
!field HDISPEND 0 8 -1
!endreg CR01

!startreg CR02
// CR02: Start Horizontal Blanking
!field HBLANKBEG 0 8 0
!endreg CR02

!startreg CR03
// CR03.0-4: End Horizontal Blanking, low 5 bits
!field HBLANKEND 0 5 0
// CR03.5-6: Display Enable Skew Control
// This basically moves the whole visible border right by CR03.5-6 character clocks.
!field HDISPSKEW 5 2 0
!endreg CR03

!startreg CR04
// CR04: Start Horizontal Retrace
!field HSYNCBEG 0 8 0
!endreg CR04

!startreg CR05
// CR05.0-4: End Horizontal Retrace, low 5 bits
!field HSYNCEND 0 5 0
// CR05.5-6: Horizontal Retrace Delay
// This basically moves the whole display plus border ot the left by this many character clocks.
// WARNING: This can clash with HSYNC which can have a weird effect on some displays!
!field HSYNCSKEW 5 2 0
// CR05.7: Start Odd/Even Memory Address
// From the IBM EGA doc:
// "This bit controls whether the first CRT memory address output after a horizontal retrace begins with an even or an odd address."
!field HOEOFFS0 7 1 0
!endreg CR05

!startreg CR06
// CR06: Vertical Total, low 8 bits (bit 8 = CR07.0)
!field VTOTAL 0 8 0
!endreg CR06

!startreg CR07
// CR07.0: Vertical Total bit 8 (low 8 bits = CR06)
!field VTOTAL_8 0 1 0
// CR07.1: Vertical Display Enable End bit 8 (low 8 bits = CR12)
!field VDISPEND_8 1 1 0
// CR07.2: Vertical Retrace Start bit 8 (low 8 bits = CR10 write)
!field VSYNCBEG_8 2 1 0
// CR07.3: Start Vertical Blanking bit 8 (low 8 bits = CR15)
!field VBLANKBEG_8 3 1 0
// CR07.4: Line Compare bit 8 (low 8 bits = CR18)
!field LINECOMPARE_8 4 1 0
// Supposedly CR07.5 is bit 8 of the Cursor Location register (CR0A) but that's a 5-bit register...
!endreg CR07

!startreg CR08
// CR08.0-4: Preset Row Scan (vertical scrolling)
// From the IBM EGA doc:
// "Starting Row Scan Count after a Vertical Retrace"
!field VFINESCROLL 0 5 0
!endreg CR08

!startreg CR09
// CR09.0-4: Maximum Scan Line minus 1
// This is the character height in text modes.
// In graphics modes this allows for scan-doubling and other multipliers.
!field CHARHEIGHT 0 5 -1
!endreg CR09

!startreg CR0A
// CR0A.0-4: Cursor Start minus 1
!field CURSORBEG 0 5 -1
!endreg CR0A

!startreg CR0B
// CR0B.0-4: Cursor End
!field CURSOREND 0 5 0
// CR0B.5-6: Cursor Skew
// This kinda moves the cursor to the right by this many character clocks.
// But if it's on the first column, it can potentially be duplicated? TODO: REAL HARDWARE NEEDED: CONFIRM THIS! --GM
!field CURSORSKEW 5 2 0
!endreg CR0B

!startreg CR0C
// CR0C RW: Start Address High
!field STARTHI 0 8 0
!endreg CR0C

!startreg CR0D
// CR0D RW: Start Address Low
!field STARTLO 0 8 0
!endreg CR0D

!startreg CR0E
// CR0E RW: Cursor Address High
!field CURSORHI 0 8 0
!endreg CR0E

!startreg CR0F
// CR0F RW: Cursor Address Low
!field CURSORLO 0 8 0
!endreg CR0F

!startreg CR10
// CR10 W: Vertical Retrace Start, low 8 bits (bit 8 = CR07.2)
!field VSYNCBEG 0 8 0
!endreg CR10

!startreg CR10R
// CR10 R: Light Pen High
!field LPENHI 0 8 0
!endreg CR10R

!startreg CR11
// CR11.0-3 W: Vertical Retrace End, low 4 bits
!field VSYNCEND 0 4 0
// CR11.4 W: Clear Vertical Interrupt (when 0)
!field CLEARVINT 4 1 0 ON=0b0 OFF=0b1
// CR11.5 W: Enable Vertical Interrupt (when 0)
!field ENABLEVINT 5 1 0 ON=0b0 OFF=0b1
!endreg CR11

!startreg CR11R
// CR11 R: Light Pen Low
!field LPENLO 0 8 0
!endreg CR11R

!startreg CR12
// CR12: Vertical Display Enable End, low 8 bits (bit 8 = CR07.1)
!field VDISPEND 0 8 0
!endreg CR12

!startreg CR13
// CR13: Offset (in words or dwords? TODO get this right --GM)
// This is the display pitch address.
!field OFFSET 0 8 0
!endreg CR13

!startreg CR14
// CR14.0-4: Underline Location minus 1
!field UNDERLINE 0 8 -1
!endreg CR14

!startreg CR15
// CR15: Start Vertical Blanking, low 8 bits (bit 8 = CR07.3)
!field VBLANKBEG 0 8 0
!endreg CR15

!startreg CR16
// CR16.0-4: End Vertical Blanking, low 5 bits
!field VBLANKEND 0 5 0
!endreg CR16

!startreg CR17
// CR17.0: Compatibility Mode Support
// When 0, this replaces CRTC address bit 13 with row scan count bit 0.
// Set to 0 for CGA graphics mode compatibility.
!field A13 0 1 0 ROW0=0b0 NORMAL=0b1

// CR17.1: Select Row Scan Counter
// When 0, this replaces CRTC address bit 14 with row scan count bit 0.
// IBM seems to insist on not explaining what this bit is for.
// But I will! Set this and CR17.0 to 0 for Hercules graphics mode compatibility.
!field A14 1 1 0 ROW1=0b0 NORMAL=0b1

// CR17.2: Horizontal Retrace Select
// When 1, vertical counters only increment on every 2nd horizontal retrace.
// When 0, vertical counters increment on every horizontal retrace.
// WARNING: This WILL require changes to your vertical timings in order to remain compatible!
!field VDIVIDE 2 1 0 NORMAL=0b0 DIV2=0b1

// CR17.3: Count By Two
// When 1, the memory address only increments on every 2nd character clock.
// When 0, the memory address increments on every character clock.
// Set to 1 for Odd/Even mode.
!field ADDRDIVIDE0 3 1 0 NORMAL=0b0 DIV2=0b1

// CR17.4: Output Control
// Set to 0 for normal operation.
// When 1, this floats all CRTC outputs.
!field OUTCTRL 4 1 0 NORMAL=0b0 FLOATOUTPUTS=0b1

// CR17.5: Address Wrap
// CR17.6 "Word Mode" must be set to 0 for this to have any effect.
// When 0, use memory address bit 13 for output address bit 0.
// When 1, use memory address bit 15 for output address bit 0.
!field WRAPBIT 5 1 0 A13=0b0 A15=0b1

// CR17.6: Word Mode
// When 0, all memory addresses outputs are shifted left by 1, and output address bit 0 is as per CR17.5.
// When 1, the memory address is used as-is for the output address.
!field ADDRSHIFT0 6 1 0 1=0b0 0=0b1

// CR17.7: Hardware Reset
// Set to 1 for normal operation.
// When 0, horizontal and vertical retraces are forced to be deasserted.
!field RESET 7 1 0 RESETTING=0b0 NORMAL=0b1
!endreg CR17

!startreg CR18
// CR18: Line Compare, low 8 bits (bit 8 = CR07.4)
!field LINECOMPARE 0 8 0
!endreg CR18
