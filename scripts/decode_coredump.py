#!/usr/bin/env python3
"""Decode an ESP32 coredump while bypassing app SHA mismatch."""
import sys
from esp_coredump import CoreDump
from esp_coredump.corefile import loader as ld
from esp_coredump.corefile.elf import ElfFile
from construct import Int32ul


def patched_extract(self, exe_name=None, e_machine=ld.ESPCoreDumpElfFile.EM_XTENSA):
    """Write core ELF as-is and optionally attach chip revision note."""
    with open(self.core_elf_file, "wb") as fw:
        fw.write(self.core_src.data)
    if self.chip_rev is not None:
        try:
            core_elf = ld.ESPCoreDumpElfFile(self.core_elf_file, e_machine=e_machine)
            note = self._build_note_section("ESP_CHIP_REV", ElfFile.PT_ESP_INFO, Int32ul.build(self.chip_rev))
            core_elf.add_segment(0, note, ElfFile.PT_NOTE, 0)
            core_elf.dump(self.core_elf_file)
        except Exception:
            pass


def main():
    if len(sys.argv) != 3:
        print("Usage: decode_coredump.py <core.bin> <program.elf>")
        sys.exit(1)

    core_path, elf_path = sys.argv[1], sys.argv[2]
    ld.EspCoreDumpLoader._extract_elf_corefile = patched_extract

    gdb_path = "/Users/leo/.platformio/packages/toolchain-xtensa-esp32s3/bin/xtensa-esp32s3-elf-gdb"
    cd = CoreDump(core=core_path, prog=elf_path, core_format="raw", gdb=gdb_path)
    cd.info_corefile()


if __name__ == "__main__":
    main()
