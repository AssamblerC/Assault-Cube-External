#pragma once
#include <string>
#include <vector>

inline const std::string& GetObfusString() {
    static std::string author_str;
    if (author_str.empty()) {
        const unsigned char author_data[] = { 0x16, 0x11, 0x49, 0x32, 0x5e, 0x1a, 0x12, 0x40, 0x4, 0x19, 0xb, 0x1c };

        const std::string key = "this-is-funny-test-69";

        for (size_t i = 0; i < sizeof(author_data); ++i) {
            author_str += author_data[i] ^ key[i % key.length()];
        }
    }
    return author_str;
}