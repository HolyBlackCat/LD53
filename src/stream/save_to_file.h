#pragma once

#include <cstdio>
#include <cstdint>
#include <iterator>
#include <string>
#include <memory>
#include <utility>

#include "macros/finally.h"
#include "program/errors.h"
#include "stream/better_fopen.h"
#include "stream/utils.h"
#include "strings/format.h"
#include "utils/archive.h"

namespace Stream
{
    enum class SaveMode
    {
        binary,
        append_binary,
        text,
        append_text,
    };
    using enum SaveMode;

    [[nodiscard]] inline const char *SaveModeStringRepresentation(SaveMode mode)
    {
        switch (mode)
        {
            case binary:        return "wb";
            case append_binary: return "ab";
            case text:          return "w";
            case append_text:   return "a";
        }

        ASSERT(false, "Invalid file save mode.");
        return SaveModeStringRepresentation(binary);
    }


    // Saves a block of memory to a file. Throws on failure.
    inline void SaveFile(std::string file_name, const std::uint8_t *begin, const std::uint8_t *end, SaveMode mode = binary)
    {
        FILE *file = better_fopen(file_name.c_str(), SaveModeStringRepresentation(mode));
        if (!file)
            throw std::runtime_error(FMT("Unable to open file `{}` for writing.", file_name));
        FINALLY{std::fclose(file);};
        if (!std::fwrite(begin, end - begin, 1, file))
            throw std::runtime_error(FMT("Unable to write to file `{}`.", file_name));
    }

    // Saves a block of memory to a file. Throws on failure.
    inline void SaveFile(std::string file_name, const char *begin, const char *end, SaveMode mode = binary)
    {
        SaveFile(std::move(file_name), reinterpret_cast<const std::uint8_t *>(begin), reinterpret_cast<const std::uint8_t *>(end), mode);
    }

    // Saves a container to a file. Throws on failure.
    template <impl::FlatByteContainer T>
    void SaveFile(std::string file_name, const T &container, SaveMode mode = binary)
    {
        const std::uint8_t *ptr = reinterpret_cast<const std::uint8_t *>(std::data(container));
        SaveFile(std::move(file_name), ptr, ptr + std::size(container), mode);
    }


    // Saves a block of memory to a file, in a compressed form (see `archive.h` for details). Throws on failure.
    inline void SaveFileCompressed(std::string file_name, const std::uint8_t *begin, const std::uint8_t *end)
    {
        auto buffer_size = Archive::MaxCompressedSize(begin, end);
        auto buffer = std::make_unique<std::uint8_t[]>(buffer_size);
        auto compressed_end = Archive::Compress(begin, end, buffer.get(), buffer.get() + buffer_size);
        SaveFile(file_name, buffer.get(), compressed_end);
    }

    // Saves a block of memory to a file, in a compressed form (see `archive.h` for details). Throws on failure.
    inline void SaveFileCompressed(std::string file_name, const char *begin, const char *end)
    {
        SaveFileCompressed(std::move(file_name), reinterpret_cast<const std::uint8_t *>(begin), reinterpret_cast<const std::uint8_t *>(end));
    }

    // Saves a container to a file, in a compressed form (see `archive.h` for details). Throws on failure.
    template <impl::FlatByteContainer T>
    void SaveFileCompressed(std::string file_name, const T &container)
    {
        const std::uint8_t *ptr = reinterpret_cast<const std::uint8_t *>(std::data(container));
        SaveFileCompressed(std::move(file_name), ptr, ptr + std::size(container));
    }
}
