#!/usr/bin/env python3

from typing import IO, Optional

"""
Lines beginning with a ! sign are preprocessed by this.

!boardprefix name
    Applies a name+"_" prefix to every register.
    MUST appear exactly once, before any register definitions.
!startreg name
    Starts a register definition, which applies a name+"_" prefix to every field.
    MUST appear before any field definitions.
    MUST NOT have two in a row without an !endreg in between.
!endreg name
    Finishes a register definition.
    MUST appear exactly once after a given !startreg
!field name shift bits zero {name=bitpattern)
    Defines a register field.
    "shift" is the number of bits to shift it left by.
    "bits" is the number of bits the field has.
    "zero" is the value which represents zero. It may be out of range.
    The remaining values are for generating constant named values.
"""

import json


def main() -> None:
    with open("ega_regs.gen.h", "r") as infp:
        with open("ega_regs.h", "w") as outfp:
            p = Processor(outfp=outfp)
            for line in infp:
                p.process_line(line.rstrip("\r\n"))
            p.finish()


class Processor:
    def __init__(self, *, outfp: IO[str]) -> None:
        self.outfp = outfp

        self.this_board: Optional[str] = None
        self.this_register: Optional[str] = None
        self.this_register_mask: int = 0x00

    def process_line(self, line: str) -> None:
        # If it doesn't start with "!", this is a verbatim line.
        if not line.startswith("!"):
            self.write_line(line)
            return

        #
        L = line.rstrip(" ").split(" ")
        if L[0] == "!boardprefix":
            (name,) = L[1:]
            if self.this_board is not None:
                raise Exception(
                    f"!boardprefix already defined as {self.this_board!r}, attempting to redefine as {name!r}"
                )
            else:
                self.this_board = name

        elif L[0] == "!startreg":
            (reg,) = L[1:]
            if self.this_board is None:
                raise Exception(f"!startreg {reg!r} with no !boardprefix")
            elif self.this_register is not None:
                raise Exception(
                    f"Expected !endreg {self.this_register}, got !startreg {reg} instead"
                )
            else:
                self.this_register = reg

        elif L[0] == "!endreg":
            (reg,) = L[1:]

            if self.this_board is None:
                raise Exception(f"!endreg {reg} with no !boardprefix")
            elif self.this_register is None:
                raise Exception(f"!endreg {reg} with no matching !startreg")
            elif reg != self.this_register:
                raise Exception(
                    f"Expected !endreg {self.this_register}, got !endreg {reg} instead"
                )
            else:
                self.write_line(f"// {self.this_register} end")
                self.write_line(
                    f"#define {self.this_board}_{self.this_register}_MASK 0x{self.this_register_mask:02x}"
                )
                self.this_register = None
                self.this_register_mask = 0x00

        elif L[0] == "!field":
            L = L[1:]
            name, shift_str, bits_str, zero_str = L[:4]
            L = L[4:]
            if self.this_board is None:
                raise Exception(f"!field {name} with no !boardprefix")
            if self.this_register is None:
                raise Exception(f"!field {name} with no !startreg")
            shift: int = int(shift_str)
            bits: int = int(bits_str)
            zero: int = int(zero_str)

            # Compute mask
            mask = ((1 << bits) - 1) << shift
            if (mask & self.this_register_mask) != 0:
                raise Exception(f"Field {name!r} mask overlaps previous fields")
            if mask > 0xFF:
                raise Exception(f"Field {name!r} mask exceeds 8-bit limit")
            self.this_register_mask |= mask

            # Compute length for alignment
            align_len = 8
            for valpair in L:
                k, sep, v = valpair.partition("=")
                if sep == "":
                    raise Exception(f"Expected key=value pair, got {valpair!r} instead")
                align_len = max(align_len, len(k))

            # Write common header
            align_main = " " * (align_len - 8)
            nprefix = f"{self.this_board}_{self.this_register}_{name}"  # name prefix
            self.write_line(f"#define {nprefix}_SHIFT   {align_main} {shift}")
            self.write_line(f"#define {nprefix}_ZERO    {align_main} {zero}")
            self.write_line(
                f"#define {nprefix}_MASK    {align_main} (((1 << {bits}) - 1) << {shift})"
            )
            self.write_line(
                f"#define {nprefix}_WRITE(x){align_main} {self.this_board}_MAKE_WRITE({nprefix}, x)"
            )
            self.write_line(
                f"#define {nprefix}_READ(x) {align_main} {self.this_board}_MAKE_READ({nprefix}, x)"
            )

            # Write field values
            for valpair in L:
                k, sep, v = valpair.partition("=")
                if sep == "":
                    raise Exception(f"Expected key=value pair, got {valpair!r} instead")
                kalign = " " * (align_len - len(k))
                self.write_line(f"#define {nprefix}_{k}{kalign} (({v}) << {shift})")

        else:
            raise Exception(f"Invalid preprocessor command {L[0]!r}")

    def finish(self) -> None:
        if self.this_register is not None:
            raise Exception(f"Expected !endreg {self.this_register}, got EOF instead")

    def write_line(self, line: str) -> None:
        self.outfp.write(line + "\n")


if __name__ == "__main__":
    main()
