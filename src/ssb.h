#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/// @file ssb.h
/// @brief Structures and helper functions for SSB files
/// @author Vu
/// @author Torphedo

#include <assert.h>
#include <common/int.h>
#include "pd_common.h"

// These structures could be out of date. If you use this file as reference for
// your own code, double-check them against the latest information at:
// https://phantomdust.miraheze.org/wiki/File_Formats/SSB

// Data in this file comes mostly from the wiki page, and most of that SSB
// research comes from Vu. Thanks for the help :)

typedef struct {
    u32 header_size;
    u32 func_table_addr;
    u32 string_pool_ptr; // This is always repeated twice for some reason.
    u32 string_pool_ptr2;
    u32 pad;
    u16 unk1;
    u16 unk2; // Often similar to the first
    u32 unk3;
    u32 pad2;
    u32 instructions[]; // Instruction opcodes are 32-bit
} ssb_header;
static_assert(sizeof(ssb_header) == 0x20, "Wrong SSB header size!");

enum {
    SSB_STACK_SIZE = 0x200,
    SSB_REG_COUNT = 4,
};

typedef enum {
    NOP = 0x00,
    FUNC_TABLE_CALL = 0x01,
    THROW = 0x02,
    CALL = 0x03,
    UNK_NATIVE = 0x04,
    RETURN = 0x05,
    STACK_PUSH = 0x06,
    COND_JMP = 0x07,
    COND_JMP_POP = 0x08,
    IDX_PUSH = 0x09,
} ssb_opcode;

typedef struct {
    u32 text1;
    u32 text2;
    u32 func_offset;
} ssb_func_entry;
static_assert(sizeof(ssb_func_entry) == 0xC, "Wrong SSB function entry size!");

#ifdef __cplusplus
}
#endif
