/*============================================================================

  compat_elf.h 

  Definition of the EFL header, as in the usual elf.h, but without
  the external dependencies 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

// ELF architecture type for X64
#define EM_X86_64 62

#define ELF_GEN_OFFSET 0x1000

typedef struct ELFHeader
    {
    char magic[4];
    char width;
    char endian;
    char elf_version1;
    char undefined[9];

    int16_t e_type;
    int16_t e_machine;
    int32_t e_version;
    int32_t e_entry;
    int32_t e_phoff;
    int32_t e_shoff;
    int32_t e_flags;
    int16_t e_ehsize;
    int16_t e_phentsize;
    int16_t e_phnum;
    int16_t e_shentsize;
    int16_t e_shnum;
    int16_t e_shstrndx;
    } ELFHeader;


