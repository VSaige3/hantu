#include "fileclass.hxx"
#include <stdio.h>
#include <stdlib.h>
#include <common/file.h>
#include <common/logging.h>

bool fileclass::load(const char* path) noexcept {
    if (data) {
        unload();
    }

    size = file_size(path);
    if (!size) {
        return false;
    }

    data = file_load(path);
    if (!data) {
        return false;
    }

    if (!load_verify()) {
        unload();
        return false;
    }

    filename = path;
    return true;
}

void fileclass::unload() noexcept {
    free(data);
    data = nullptr;
    size = 0;
}

bool fileclass::save(const char* path) const noexcept {
    FILE* f = fopen(path, "wb");
    if (!f) {
        LOG_MSG(error, "Couldn't open new file \"%s\"", path);
        return false;
    }

    bool result = true;
    if (fwrite(data, size, 1, f) != 1) {
        LOG_MSG(error, "Failed to save the entire %d bytes to new file \"%s\"\n", size, path);
        result = false;
    }

    fclose(f);
    return result;
}

fileclass::~fileclass() noexcept {
    free(data);
}


ssb_header* ssb_file::get_header() {
    return (ssb_header*)data;
}

u32* ssb_file::get_bytecode() {
    auto* header = (ssb_header*)data;
    return &header->instructions[0];
}

ssb_func_entry* ssb_file::func_table() {
    const auto* header = (ssb_header*)data;
    return (ssb_func_entry*)(data + header->func_table_addr);
}

u32 ssb_file::num_functions() {
    const auto func_ptr = (intptr_t)func_table();
    const auto string_ptr = (intptr_t)string_pool();
    return std::abs(string_ptr - func_ptr) / sizeof(ssb_func_entry);
}

char* ssb_file::string_pool() {
    const auto* header = (ssb_header*)data;
    return (char*)(data + header->string_pool_ptr);
}

bool ssb_file::load_verify() const noexcept {
    const auto* header = (ssb_header*)data;
    bool passed = true;
    passed &= header->header_size == sizeof(*header);
    passed &= header->string_pool_ptr == header->string_pool_ptr2;

    return passed;
}
