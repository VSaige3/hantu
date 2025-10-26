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
    EXTERNAL_CALL = 0x04, // Not well understood, probably calls a function in another file idk
    RETURN = 0x05,
    STACK_EXTEND = 0x06,
    COND_JMP = 0x07,
    SWITCH_CASE = 0x08,
    PUSH_LOCAL = 0x09,
    POP_LOCAL = 0x0A,
    STACK_DELETE = 0x0D,
    PUSH_W = 0x0E,
    PUSH_DW = 0x0F,
    PUSH_RETURN = 0x10,
    PUSH_STR = 0x11,
    ADD = 0x12,
    SUB = 0x13,
    MULT = 0x14,
    DIV = 0x15,
    MOD = 0x16,
    CMP = 0x17,
    AND = 0x18, // bitwise operations below
    OR = 0x19,
    XOR = 0x1A,
    BITWISE_NOT = 0x1B, // ~x
    NEGATE = 0x1C, // -x
    LOGICAL_NOT = 0x1D, // !x
    SHIFT = 0x1E, // variety of bit shifts
    LEA = 0x1F, // pointer to an offset from this instruction (for now assume it always gets data)
} ssb_opcode;


typedef struct {
    u32 text1;
    u32 text2;
    u32 func_offset;
} ssb_func_entry;
static_assert(sizeof(ssb_func_entry) == 0xC, "Wrong SSB function entry size!");

decoded_text decode_text(ssb_func_entry val);


#ifdef __cplusplus
}
#endif
