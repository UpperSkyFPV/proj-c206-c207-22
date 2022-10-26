#include "file.hpp"

namespace uppr::file {

std::string read_file_text(const std::string &filename) {
    std::ifstream in{filename, std::ios::in | std::ios::binary};

    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(contents.data(), contents.size());
        in.close();

        return contents;
    }

    return "";
}
} // namespace uppr::file
