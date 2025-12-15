#pragma once
#include <string>
#include <common/int.h>
#include <common/vfile.h>
#include "ssb.h"

struct fileclass {
    std::string filename;
    u8* data = nullptr;
    u32 size = 0;

    bool load(const char* path) noexcept;

    /// @brief Verify that the loaded file is valid (usually checks magic value)
    virtual bool load_verify() const noexcept {
        return true;
    }

    void unload() noexcept;
    bool save(const char* path) const noexcept;

    /// Allows derived classes to be cast to vfile
    explicit operator vfile() const noexcept {
        return vfile_open(data, size);
    }

    ~fileclass() noexcept;
};



struct ssb_file : fileclass {
    ssb_header* get_header();
    u32* get_bytecode();
    ssb_func_entry* func_table();
    u32 num_functions();
    char* string_pool();

    bool load_verify() const noexcept override;

    // Used for editing strings in the string pool
    std::string string_pool_input;

    // The location in the string pool we're currently editing, if any
    char* string_pool_edit_dest = nullptr;

    // TODO: Add ability to expand the file, so we can do arbitrary length text edits
};

