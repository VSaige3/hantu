#pragma once
#include <layer.hxx>
#include <imgui_node_editor.h>
#include <vector>
#include <stack>

#include "fileclass.hxx"
#include "ssb.h"
#define MAX_STR_OPCODE 0x30

#define DERROR_INFINITE_LOOP 1
#define DERROR_JUMP_OUT_OF_BOUNDS 2
#define DERROR_BAD_COMMAND 3
#define DERROR_WRONG_NUM_ARGUMENTS 4

typedef enum {
    EMPTY_NODE,         // node that can't accept any connections (pointer is to the string of its value)
    FUNCTIONAL_NODE,    // anything representing a bit of bytecode (pointer is to the instructions)
    STRING_NODE,        // string area pointer
    STACKVAR_NODE,      // local stack node; this is a signed long representing its position on the stack
    DATA_NODE           // data pointer node; pointer is to the data idk
} DECOMP_NODE_TYPE;

typedef enum {
    TYPE_FLOAT,
    TYPE_INT,
    TYPE_COLOR,
    TYPE_POINTER,
    TYPE_STRING,
    TYPE_CONTROL_FLOW,
} NODE_VALUE_TYPE;

typedef union {
    char *empty_node_name;
    u32 *instr_pointer;
    char *str_pointer;
    int local_stackpos;
    void *data_ptr;
} decomp_node_data_t;


// types of nodes:
//  functional (nodes representing bytecode)
//      NOP (sequential flow)
//      (binary) operator [ADD | SUB | ... | XOR] (sequential flow) (parameter * parameter -> result)
//      (unary) operator [BITWISE_NOT | ... | SHIFT] (sequential flow) (parameter -> result)
//      RETURN (functional -> ()) (() -> ())
//      COND_JMP (functional -> (functional * functional)) (parameter -> ())
//      SWITCH (functional -> functional list) (parameter -> ())
//      
//  variable (nodes representing semi-permanent variables) (not including ones pushed onto stack, we assume those only live as long as they are needed)
//      local (created on local stack)
//      data (nodes representing data in the ssb)
//  string (not always strings, but read-only)


// there's kinda an implicit "stack" between many of these
// types of relations between nodes:
//  sequential flow: functional -> functional option
//  forked flow: functional -> (functional * functional)
//  parameter: [functional | local | string] -> functional (note that these always go through the stack)
//  pointer: data -> functional (? do we ever take pointers to functions?)
//  result: functional -> variable

// process for decompilation:
//  For each opcode:
//      add to the symbolic stack
//      register it as type "functional"
//      if it points to a piece of data, also register that
//      if it does a jump of some sort, switch processing to there
//          if the destination of the jump is already registered, point it there

// Next Generation -> Deep space 9

typedef struct {
    const char *name;
    NODE_VALUE_TYPE type;
} node_value_in;

typedef struct {
    u32 id;
    u32 in_index;
    u16 connection_num_in;
    u32 out_index;
    u16 connection_num_out;
    NODE_VALUE_TYPE type;
} link_info;

u64 get_pin_index(bool is_input, u32 node_index, u16 connection_num);

class DecompNode {
    private:
        DECOMP_NODE_TYPE node_type;
        char *get_title_text();
        void generate_title_text();
        char *title_text;
        std::vector<node_value_in*> input_names;
        bool visible_control_in; 
        u64 cached_instr;
        s32 command_size; // size in ints
        bool _has_branch;
        s32 branch_offset;
    public:
        DecompNode(decomp_node_data_t data, DECOMP_NODE_TYPE node_type);
        bool render(u32);
        bool is_same_node(decomp_node_data_t other, DECOMP_NODE_TYPE node_type);
        int get_num_var_in();   // number of stack variables this accepts
        static DecompNode * const from_instr(u32 * const instr_pointer);
        s32 get_command_size(); // size in ints
        bool has_branch();
        s32 get_branch_offset();
        u32 branched_node_index;
        u32 next_node_index;
        decomp_node_data_t data;
};

class FunctionDecompiler {
    private:
        u32 entry_offset;
        ssb_file *file;
        std::vector<DecompNode *> nodes;
        std::vector<u32> visual_levels; // y positions
        std::vector<u32> visual_distance; // x positions
        std::vector<link_info*> links;
        int decomp_error;   
        bool push_back_nodes(int params_hint);
        bool push_back_nodes_recursive(int param_hint, u32 last_node_id, u32 last_link_id);
        bool create_node_layout(int params_hint);
        bool recursive_layout_nodes(u32, u32, u32);
        bool process_stack(int params_hint);
        bool get_matching_node(decomp_node_data_t other, DECOMP_NODE_TYPE node_type, u32 *index_out);
    public:
        FunctionDecompiler(u32 entry_offset, ssb_file *file);
        bool decompile(int params_hint);
        int get_last_error();
        bool render_all_nodes(u32 *);
        void space_nodes(u32, ImVec2&);
        ImVec2 bounds;
};

char *str_opcode(u32 *op, char *buff);
const char *str_decomp_error(int err);