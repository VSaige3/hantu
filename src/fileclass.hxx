#pragma once
#include <string>
#include <common/int.h>
#include <common/vfile.h>

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
