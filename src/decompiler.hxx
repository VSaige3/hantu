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
        DECOMP_NODE_TYPE node_type = EMPTY_NODE;
        char *title_text = nullptr;
        std::vector<node_value_in*> input_names;
        bool visible_control_in = false;
        u64 cached_instr = 0;
        s32 command_size = 0; // size in ints
        bool _has_branch = false;
        s32 branch_offset = 0;

        char *get_title_text();
        void generate_title_text();
    public:
        u32 branched_node_index = 0;
        u32 next_node_index = 0;
        decomp_node_data_t data = {};

        /// @brief Render a node in the editor.
        ///
        /// @param node_index ID to use in ImGui
        /// @return Always true
        bool render(u32 node_index);

        /// @brief Compare 2 nodes.
        bool is_same_node(decomp_node_data_t other, DECOMP_NODE_TYPE type) const;
        // TODO: ^ Can we make this an operator==()?

        // number of stack variables this accepts
        int get_num_var_in() const;

        /// @brief Size (in units of 4 bytes) of the instruction(s) this node was created from
        s32 get_command_size() const;

        /// @brief Whether this node has a branch instruction
        bool has_branch() const;

        /// @brief The offset of the branch target, if applicable
        s32 get_branch_offset() const;

        DecompNode(decomp_node_data_t data, DECOMP_NODE_TYPE node_type);
        static DecompNode* from_instr(u32* instr_pointer);
};

class FunctionDecompiler {
    private:
        u32 entry_offset;
        std::vector<DecompNode *> nodes;
        std::vector<u32> visual_levels; // y positions
        std::vector<u32> visual_distance; // x positions
        std::vector<link_info*> links;

        // TODO: Can we have methods return an error directly?
        int decomp_error = 0;

        bool push_back_nodes(int params_hint, u32* bytecode);
        bool push_back_nodes_recursive(int param_hint, u32 last_node_id, u32 last_link_id);
        bool create_node_layout();
        bool recursive_layout_nodes(u32 last_index, u32 new_level, u32 new_distance);
        bool process_stack(int params_hint);
        bool get_matching_node(decomp_node_data_t other, DECOMP_NODE_TYPE node_type, u32 *index_out) const;
    public:
        FunctionDecompiler(u32 entry_offset);
        bool decompile(int params_hint, u32* bytecode);
        int get_last_error();
        bool render_all_nodes(u32 starting_id);
        void space_nodes(u32 starting_id, ImVec2 initial_pos);
        ImVec2 bounds;
};

void str_opcode(const u32 *op, char *buff);
const char *str_decomp_error(int err);