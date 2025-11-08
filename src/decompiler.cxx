#include "decompiler.hxx"
#include <math.h>

namespace ed = ax::NodeEditor;

void str_opcode(const u32 *op, char *buff) {
    const u8 opcode = *op & 0xFF;
    const u8 modifier = (*op >> 0x8) & 0xFF;
    const u16 argument = *op >> 0x10;
    const s16 sargument = (s16) argument;
    switch (opcode) {
        case NOP:
            snprintf(buff, MAX_STR_OPCODE - 1, "NOP");
            break;
        case FUNC_TABLE_CALL:
            snprintf(buff, MAX_STR_OPCODE - 1, "SYSCALL [%01X][%03X]", argument >> 12, argument & 0x0FFF);
            break;
        case THROW:
            snprintf(buff, MAX_STR_OPCODE - 1, "THROW %02X", argument);
            break;
        case CALL:
            snprintf(buff, MAX_STR_OPCODE - 1, "CALL %+d", sargument);
            break;
        case EXTERNAL_CALL:
            snprintf(buff, MAX_STR_OPCODE - 1, "EXT CALL %+d", sargument);
            break;
        case RETURN:
            snprintf(buff, MAX_STR_OPCODE - 1, "%s (del %X)", modifier ? "RET" : "RET(END)", argument);
            break;
        case STACK_EXTEND:
            snprintf(buff, MAX_STR_OPCODE - 1, "STACK EXTEND %04X", argument);
            break;
        case COND_JMP:
            switch (modifier) {
                case 1:
                    snprintf(buff, MAX_STR_OPCODE - 1, "JUMPNZ %+d", sargument);
                    break;
                case 2:
                    snprintf(buff, MAX_STR_OPCODE - 1, "JUMPZ %+d", sargument);
                    break;
                case 3:
                    snprintf(buff, MAX_STR_OPCODE - 1, "JUMPZ (ERR) %+d", sargument);
                    break;
                default:
                    snprintf(buff, MAX_STR_OPCODE - 1, "UJUMP %+d", sargument);
                    break;
            }
            break;
        case SWITCH_CASE:
            snprintf(buff, MAX_STR_OPCODE - 1, "CASE %02X (JUMP %+d)", modifier, sargument);
            break;
        case PUSH_LOCAL:
            snprintf(buff, MAX_STR_OPCODE - 1, "PUSH LOCAL[%+d]", sargument);
            break;
        case POP_LOCAL:
            switch(modifier) {
                case 0:
                    snprintf(buff, MAX_STR_OPCODE - 1, "SET LOCAL[%+d]", argument);
                    break;
                case 1:
                    snprintf(buff, MAX_STR_OPCODE - 1, "ADD LOCAL[%+d]", sargument);
                    break;
                default:
                    snprintf(buff, MAX_STR_OPCODE - 1, "SUB LOCAL[%+d]", sargument);
                    break;
            }
            break;
        case STACK_DELETE:
            snprintf(buff, MAX_STR_OPCODE - 1, "STACK DEL %04X", argument);
            break;
        case PUSH_W:
            snprintf(buff, MAX_STR_OPCODE - 1, "PUSHW %04X", argument);
            break;
        case PUSH_DW:
            snprintf(buff, MAX_STR_OPCODE - 1, "PUSHDW %08lX (%+ld)", op[1], (s32)op[1]);
            break;
        case PUSH_RETURN:
            snprintf(buff, MAX_STR_OPCODE - 1, "PUSH RETURN");
            break;
        case PUSH_STR:
            snprintf(buff, MAX_STR_OPCODE - 1, "PUSH STR %04X", argument);
            break;
        case ADD:
            snprintf(buff, MAX_STR_OPCODE - 1, "ADD");
            break;
        case SUB:
            if (modifier)
                snprintf(buff, MAX_STR_OPCODE - 1, "SUB (a-b)");
            else
                snprintf(buff, MAX_STR_OPCODE - 1, "SUB (b-a)");
            break;
        case MULT:
            snprintf(buff, MAX_STR_OPCODE - 1, "MULT");
            break;
        case DIV:
            if (modifier)
                snprintf(buff, MAX_STR_OPCODE - 1, "DIV (a-b)");
            else
                snprintf(buff, MAX_STR_OPCODE - 1, "SIV (b-a)");
            break;
        case MOD:
            if (modifier)
                snprintf(buff, MAX_STR_OPCODE - 1, "MOD (a-b)");
            else
                snprintf(buff, MAX_STR_OPCODE - 1, "MOD (b-a)");
            break;
        case CMP:
            const char *operator_;
            switch(argument) {
                case 0: operator_ = "=="; break;
                case 1: operator_ = "!="; break;
                case 2: operator_ = ">"; break;
                case 3: operator_ = "<"; break;
                case 4: operator_ = ">="; break;
                case 5: operator_ = "<="; break;
                default: operator_ = "flags"; break;
            }
            if (modifier)
                snprintf(buff, MAX_STR_OPCODE - 1, "TEST (a %s b)", operator_);
            else
                snprintf(buff, MAX_STR_OPCODE - 1, "TEST (b %s a)", operator_);
            break;
        case AND:
            snprintf(buff, MAX_STR_OPCODE - 1, "AND");
            break;
        case OR:
            snprintf(buff, MAX_STR_OPCODE - 1, "OR");
            break;
        case XOR:
            snprintf(buff, MAX_STR_OPCODE - 1, "XOR");
            break;
        case BITWISE_NOT:
            snprintf(buff, MAX_STR_OPCODE - 1, "NOT(~)");
            break;
        case NEGATE:
            snprintf(buff, MAX_STR_OPCODE - 1, "NEGATE(-)");
            break;
        case LOGICAL_NOT:
            snprintf(buff, MAX_STR_OPCODE - 1, "NOT(!)");
            break;
        case SHIFT:
            const char *operator__;
            switch(argument) {
                case 1: operator__ = "ASHR"; break;
                case 2: operator__ = "SHR"; break; // logical (unsigned)
                default: operator__ = "SHL"; break;
            }
            if (modifier)
                snprintf(buff, MAX_STR_OPCODE - 1, "%s (a <> b)", operator__);
            else
                snprintf(buff, MAX_STR_OPCODE - 1, "%s (b <> a)", operator__);
            break;
        case LEA:
            snprintf(buff, MAX_STR_OPCODE - 1, "LEA %+d", sargument);
            break;
        default:
            snprintf(buff, MAX_STR_OPCODE - 1, "UNRECOGNIZED OPCODE %X", *op);
            break;
    }
}

const char *str_decomp_error(int err) {
    switch (err) {
        case DERROR_INFINITE_LOOP:
            return "Infinite Loop";
        case DERROR_JUMP_OUT_OF_BOUNDS:
            return "Out of bounds jump";
        case DERROR_BAD_COMMAND:
            return "Unreconized opcode";
        case 0:
            return "No Error";
        default:
            return "Unrecognized decompilation error";
    }
}

// TODO: Is this a hash function? Can we replace it with a normal one like crc32fast()? - torph
u64 get_pin_index(bool is_input, u32 node_index, u16 connection_num) {
    return (((u64)is_input) | (((u64)connection_num) << 1) | (((u64)node_index) << 17)) ^ (u64)0x12345678ABCDEF;
}

bool is_pin_index_for_node(u64 pin_index, u32 node_index) {
    return ((pin_index ^ 0x12345678ABCDEF) >> 17) == node_index;
}

char *str_stackvar(int stackpos, char *buff) {
    snprintf(buff, MAX_STR_OPCODE - 1, "stackvar %d", stackpos);
    return buff;
}

int get_num_var_in_from_opcode(u32 instr) {
    const u8 opcode = instr & 0xFF;
    const u8 modifier = (instr >> 0x8) & 0xFF;
    const u16 argument = instr >> 0x10;
    switch (opcode) {
        case NOP:
        case THROW:
        case STACK_EXTEND:
        case PUSH_LOCAL:
        case PUSH_W:
        case PUSH_DW:
        case PUSH_STR:
        case LEA:
            // all of these do not pop anything from the stack
            return 0;
        case FUNC_TABLE_CALL:
        case CALL: // If the called function cleans up properly these work
        case EXTERNAL_CALL:
            // all of these are function calls
            return modifier;
        case RETURN: // we do it like this because the things this pops are for cleanup only
            // thus, 
            if (modifier)
                return 1;
            else
                return 0;
        case COND_JMP:
        case BITWISE_NOT:
        case NEGATE:
        case LOGICAL_NOT:
        case SHIFT:
        case POP_LOCAL:
            // simple 1-param
            return 1;
        case SWITCH_CASE:
            return 0; // we'll have to figure something out for this (switch)
        case PUSH_RETURN:
            return 0; // This will probably be handled somewhere else
        case ADD:
        case SUB:
        case MULT:
        case DIV:
        case MOD:
        case CMP:
        case AND:
        case OR:
        case XOR:
            // simple 2-parameter
            return 2;
        case STACK_DELETE:
            // maybe change because this does cleanup
            return argument;
        default:
            return -1;
    }
}

DecompNode* DecompNode::from_instr(u32* instr_pointer) {
    decomp_node_data_t d = {};
    d.instr_pointer = instr_pointer;
    DecompNode *node = new DecompNode(d, FUNCTIONAL_NODE);
    const u32 instr = *instr_pointer;
    const u8 opcode = instr & 0xFF;
    const u8 modifier = (instr >> 0x8) & 0xFF;
    const u16 argument = instr >> 0x10;
    node->command_size = 1;
    node->_has_branch = false;

    switch (opcode) {
        case NOP:
        case THROW:
        case STACK_EXTEND:
        case PUSH_LOCAL:
        case POP_LOCAL:
        case STACK_DELETE:
        case PUSH_W:
        case PUSH_RETURN:
        case PUSH_STR:
        case ADD: 
        case SUB: 
        case MULT:
        case DIV:
        case MOD:
        case CMP:
        case AND:
        case OR:
        case XOR:
        case BITWISE_NOT:
        case NEGATE:
        case LOGICAL_NOT:
        case SHIFT:
        case LEA:
            break;
        case FUNC_TABLE_CALL:
            // They don't have names yet, but we can infer the number of params from the modifier
            node->_has_branch = false;
            for (u8 i = 0; i < modifier; i ++)
                node->input_names.push_back(new node_value_in{"Input", TYPE_INT});
            break;
        case CALL:
            for (u8 i = 0; i < modifier; i ++)
                node->input_names.push_back(new node_value_in{"Input", TYPE_INT});
            // TODO: handle the function call
            break; 
        case EXTERNAL_CALL:
            for (u8 i = 0; i < modifier; i ++)
                node->input_names.push_back(new node_value_in{"Input", TYPE_INT});
            break;
        case RETURN:
            node->command_size = -1;
            if (modifier)
                node->input_names.push_back(new node_value_in{"Return", TYPE_INT});
            break;
        case COND_JMP:
            node->_has_branch = true;
            node->branch_offset = (s16)argument;
            break; 
        case SWITCH_CASE:
            node->_has_branch = true;
            node->branch_offset = (s16)argument;
            break;
        case PUSH_DW:
            node->command_size = 2;
            break; 
        default:
            return NULL;
    }
    return node;
}

DecompNode::DecompNode(decomp_node_data_t data, DECOMP_NODE_TYPE node_type) {
    this->data = data;
    this->node_type = node_type;
}


void DecompNode::generate_title_text() {
    switch (this->node_type) {
        case EMPTY_NODE:
            this->title_text = this->data.empty_node_name;
            break;
        case FUNCTIONAL_NODE:
            this->title_text = (char *)malloc(MAX_STR_OPCODE);
            str_opcode(this->data.instr_pointer, this->title_text);
            break;
        case STRING_NODE:
            this->title_text = this->data.str_pointer;
            break;
        case STACKVAR_NODE:
            this->title_text = (char *)malloc(MAX_STR_OPCODE);
            str_stackvar(this->data.local_stackpos, this->title_text);
            break;
    }
}

char *DecompNode::get_title_text() {
    if (!this->title_text) {
        this->generate_title_text();
    }
    return this->title_text;
}

bool DecompNode::render(u32 node_index) {
    const u64 pin_index_in = get_pin_index(true, node_index, 0);
    const u64 pin_index_out = get_pin_index(false, node_index, 0);
    const u64 pin_index_branch = get_pin_index(false, node_index, 1);
    // u32 pin_index_in = node_index + 1;
    // u32 pin_index_out = ~(node_index + 1);
    // printf("Title %s\n", this->get_title_text());
    ed::BeginNode(node_index);
        ImGui::Text(this->get_title_text());
        ed::BeginPin(pin_index_in, ed::PinKind::Input);
            ImGui::Text(">");
        ed::EndPin();
        ImGui::SameLine(200);
        if (command_size > 0) {
            ed::BeginPin(pin_index_out, ed::PinKind::Output);
                ImGui::Text(">");
            ed::EndPin();
        }
        if (_has_branch) {
            ed::BeginPin(pin_index_branch, ed::PinKind::Output);
                ImGui::Text("Branch");
            ed::EndPin();
        }
    ed::EndNode();
    return true;
}

bool DecompNode::is_same_node(decomp_node_data_t other, DECOMP_NODE_TYPE type) const {
    return other.data_ptr == this->data.data_ptr && this->node_type == type;
}

int DecompNode::get_num_var_in() const {
    if (this->node_type != FUNCTIONAL_NODE) {
        return 0;
    }
    return get_num_var_in_from_opcode(*this->data.instr_pointer);
}

s32 DecompNode::get_command_size() const {
    return command_size;
}
bool DecompNode::has_branch() const {
    return _has_branch;
}
s32 DecompNode::get_branch_offset() const {
    return branch_offset;
}

FunctionDecompiler::FunctionDecompiler(u32 entry_offset, ssb_file *file) {
    this->entry_offset = entry_offset;
    this->file = file;
}


bool FunctionDecompiler::get_matching_node(decomp_node_data_t other, DECOMP_NODE_TYPE node_type, u32 *index_out) {
    for (u32 index = 0; index < nodes.size(); index ++) {
        if (this->nodes[index]->is_same_node(other, node_type)) {
            if (index_out) *index_out = index;
            return true;
        }
    }
    return false;   
}

bool FunctionDecompiler::push_back_nodes_recursive(int param_hint, u32 last_node_id, u32 last_link_id) {
    if (last_node_id >= this->nodes.size()) {
        this->decomp_error = DERROR_JUMP_OUT_OF_BOUNDS;
        return false;
    }
    bool good = true;
    DecompNode *last_node = nodes[last_node_id];
    decomp_node_data_t data = {};
    const bool nonterminal = last_node->get_command_size() > 0;
    const bool branches = last_node->has_branch();
    if (nonterminal) {
        u32 *next_instr_pointer = &last_node->data.instr_pointer[last_node->get_command_size()];
        data.instr_pointer = next_instr_pointer;
        u32 index = 0;
        bool matching = get_matching_node(data, FUNCTIONAL_NODE, &index);
        if (!matching) {
            index = nodes.size();
            DecompNode *new_node = DecompNode::from_instr(next_instr_pointer);
            if (!new_node) {
                decomp_error = DERROR_BAD_COMMAND;
                printf("Bad commmand %X encountered\n", *next_instr_pointer);
                return false;
            }
            nodes.push_back(new_node);
            good &= this->push_back_nodes_recursive(param_hint, index, links.size());
        }
        links.push_back(new link_info{(u32)links.size(), last_node_id, 0, index, 0, TYPE_CONTROL_FLOW});
        last_node->next_node_index = index;
    }
    if (branches) {
        u32 *branch_instr = &last_node->data.instr_pointer[last_node->get_branch_offset() + 1];
        data.instr_pointer = branch_instr;
        u32 index = 0;
        bool matching = get_matching_node(data, FUNCTIONAL_NODE, &index);
        if (!matching) {
            index = nodes.size();
            nodes.push_back(DecompNode::from_instr(branch_instr));
            good &= this->push_back_nodes_recursive(param_hint, index, links.size()+2);
        }
        links.push_back(new link_info{(u32)links.size(), last_node_id, 1, index, 0, TYPE_CONTROL_FLOW});
        last_node->branched_node_index = index;
    }
    // std::vector<s32> &branches = this->nodes[last_node_id]->branches;
    // for (u16 i = 0; i < branches.size(); i ++) {
    //     u32 new_level = visual_levels[last_node_id] + i;
    //     u32 new_distance = visual_distance[last_node_id] + 1;
    //     s32 offset = branches[i];
    //     u32 *instr_ptr = &this->nodes[last_node_id]->data.instr_pointer[offset + 1];
    //     decomp_node_data_t data;
    //     data.instr_pointer = instr_ptr;
    //     bool found_node = false;
    //     u32 index;
    //     for (index = 0; index < this->nodes.size(); index ++) {
    //         if (this->nodes[index]->is_same_node(data, FUNCTIONAL_NODE)) {
    //             if (offset > 0)
    //                 visual_levels[index] = MAX(new_level, visual_levels[index]);
                
    //             visual_distance[index] = MIN(new_distance, visual_distance[index]);
    //             this->links.push_back(new link_info{++last_link_id, last_node_id, i, index, 0, TYPE_CONTROL_FLOW});
    //             found_node = true;
    //             break;
    //         }
    //     }
    //     // if (!found_node && offset < 0) {
    //     //     printf("Infinite loop? offset is %d\n", offset);
    //     //     this->decomp_error = DERROR_INFINITE_LOOP;
    //     //     return false;
    //     // }
    //     if (!found_node) {
    //         index = this->nodes.size();
    //         this->nodes.push_back(DecompNode::from_instr(instr_ptr));
    //         this->links.push_back(new link_info{++last_link_id, last_node_id, i, index, 0, TYPE_CONTROL_FLOW});
    //         visual_levels.push_back(new_level);
    //         visual_distance.push_back(new_distance);
    //         good = this->push_back_nodes_recursive(param_hint, index, last_link_id);
    //     }
    // }
    return good;
}

bool FunctionDecompiler::push_back_nodes(int params_hint) {
    this->nodes.push_back(DecompNode::from_instr(&this->file->get_bytecode()[this->entry_offset - 8]));
    bool success = push_back_nodes_recursive(params_hint, 0, 0);
    return success;
}

bool FunctionDecompiler::process_stack(int params_hint) {
    return true;
}

bool create_link_from_info(link_info *info, u32 id_offset) {
    return ed::Link(info->id, get_pin_index(false, info->in_index + id_offset, info->connection_num_in), get_pin_index(true, info->out_index + id_offset, info->connection_num_out));
}

bool FunctionDecompiler::render_all_nodes(u32 starting_id) {
    u32 id = starting_id;
    // printf("Rendering id starts %d\n", id);
    for (DecompNode *node : nodes) {
        if (!node->render(id)) return false;
        id ++;
    }
    
    for (link_info *info : links) {
        create_link_from_info(info, starting_id);
    }
    return true;
}

bool FunctionDecompiler::recursive_layout_nodes(u32 last_index, u32 new_level, u32 new_distance) {
    // u32 new_level = visual_levels[last_index] + 1;
    // u32 new_distance = visual_distance[last_index] + 1;
    u32 index = last_index;
    while (true) {
        // printf("Laying out %d, size of nodes %ld\n", index, nodes.size());
        DecompNode *node = nodes[index];
        if (visual_levels[index] == 0) {
            visual_levels[index] = new_level;
            visual_distance[index] = new_distance;
        }
        if (node->has_branch() && node->get_branch_offset() > 0) {
            recursive_layout_nodes(node->branched_node_index, new_level + 1, new_distance + 1);
        }
        if (node->get_command_size() < 0) break;
        new_distance ++;
        index = node->next_node_index;
    }
    return true;
}

bool FunctionDecompiler::create_node_layout(int params_hint) {
    visual_distance.resize(nodes.size(), 0);
    visual_levels.resize(nodes.size(), 0);
    return recursive_layout_nodes(0, 0, 0);
    // now every (forward) branch go to the branch and set all following to a higher level
    // if there is another later branch that jumps to it, put it there ig?
}

bool FunctionDecompiler::decompile(int params_hint) {
    decomp_error = 0;
    links.clear();
    nodes.clear();
    visual_distance.clear();
    visual_levels.clear();
    bool result = push_back_nodes(params_hint);
    result &= create_node_layout(params_hint);
    result &= process_stack(params_hint);
    return result;
}


int FunctionDecompiler::get_last_error() {
    int last_err = decomp_error;
    decomp_error = 0;
    return last_err;
}


void FunctionDecompiler::space_nodes(u32 starting_id, ImVec2 initial_pos) {
    float max_x = 0.0f;
    float max_y = 0.0f;
    const float init_x = initial_pos.x;
    const float init_y = initial_pos.y;
    for (u32 i = 0; i < nodes.size(); i ++) {
        const float x = float(visual_levels[i]) * 400.0f;
        const float y = float(visual_distance[i]) * 100.0f; // + ((float)visual_levels[i]) * 10.0f;
        
        // float x = i * 400.0f;
        // float y = 0;
        max_x = MAX(max_x, x);
        max_y = MAX(max_y, y);
        ed::SetNodePosition(starting_id + i, ImVec2(init_x + x, init_y + y));
    }
    // printf("Size: %.2f, %.2f\n", max_x + 200.0f, max_y);
    bounds.x = max_x;
    bounds.y = max_y;
}
