// Copyright (c) 2019 shadow-yuan. All rights reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "LICENSE");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// An easy to use c++ json library
// Version 1.0.0
// https://github.com/shadow-yuan/karl
//
// Authors: Shadow Yuan (shadow_yuan@qq.com)
//

#pragma once
#include "karl.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <vector>

namespace karl {
namespace cbor {

extern const bool is_little_endian;

extern const uint8_t null_code;
extern const uint8_t true_code;
extern const uint8_t false_code;

template<typename Ty>
std::vector<uint8_t> encode_number(const Ty number) {
    std::vector<uint8_t> vec(sizeof(Ty));
    memmove(vec.data(), &number, sizeof(Ty));
    if (is_little_endian && sizeof(Ty) != 1) {
        std::reverse(vec.begin(), vec.end());
    }
    return vec;
}

bool is_double_precision(double value);

std::vector<uint8_t> build_null();
std::vector<uint8_t> build_boolean(bool value);
std::vector<uint8_t> build_number_float(double value);
std::vector<uint8_t> build_number_signed(int64_t value);
std::vector<uint8_t> build_number_usigned(uint64_t value);
std::vector<uint8_t> build_string(const std::string& s);

// build array :
// step 1: write control byte and the array size
// vector = build_array_prefix(array.size())
// step 2: write each element
// for (auto item : array) {  
//     vector += build_value(item);
// }
//
std::vector<uint8_t> build_array_prefix(size_t array_size);

// build object :
// step 1: write control byte and the object size
// vector = build_object_prefix(obj.size())
// step 2: write each element
// for (auto kv : obj) {  
//     vector += build_key(kv.first);
//     vector += build_value(kv.second);
// }
//
std::vector<uint8_t> build_object_prefix(size_t obj_size);

// -------------------------------------------------------------

class Writer final {
public:
    Writer() : _index(0) {}
    ~Writer() = default;

    template<typename T>
    void write_number(const T value) {
        append_cbor_binary(encode_number(value));
    }

    void write_character(uint8_t b);
    void write_characters(const uint8_t* ptr, size_t size);

    const std::vector<uint8_t>& binary() const;
    Writer& operator += (const std::vector<uint8_t>& bin);
    void append_cbor_binary(const std::vector<uint8_t>& vec);

private:
    void check_space(size_t add_size);
    size_t _index;
    mutable std::vector<uint8_t> _vec;
};

// -------------------------------------------------------------

enum class MajorType : uint8_t {
    kUnsignedInteger,
    kNegativeInteger,
    kString,
    kArray,
    kObject,
    kFalse,
    kTrue,
    kNull,
    kFloat,
    kUnknown
};

class Reader final {
public:
    static const size_t INDEFINITE_LENGTH = size_t(-1);

    Reader(const uint8_t* ptr, size_t size);
    ~Reader() = default;

    bool read_next_type(MajorType& type);
    
    // skip one byte
    bool skip_a_character();

    size_t current_position() const;

    bool read_unsigned_number(uint64_t& value);
    bool read_negative_number(int64_t& value);
    bool read_float_number(double& value);

    bool read_string(std::string& s);
    bool read_array(std::shared_ptr<json_array>& obj);
    bool read_object(std::shared_ptr<json_object>& obj);


    // When the next type is simply encoded (such as null, false,
    // true), if you don't use the following function, you need
    // to skip one byte using the 'skip_a_character' function.
    bool read_false();
    bool read_true();
    bool read_null();

private:
    bool get_next_character(uint8_t& c);
    bool read_character(uint8_t& c);
    bool read_string(size_t len, std::string& s);
    bool read_array_size(size_t& size);
    bool read_object_size(size_t& size);
    bool parse_into_json_value(
        std::shared_ptr<json_value>& obj, size_t* transfer_bytes);

    template<typename Ty>
    bool decode_number(Ty& result) {
        std::vector<uint8_t> vec(sizeof(Ty));
        for (size_t i = 0; i < sizeof(Ty); i++) {
            uint8_t current = 0;
            if (!read_character(current)) {
                return false;
            }
            if (is_little_endian) {
                vec[sizeof(Ty) - i - 1] = current;
            } else {
                vec[i] = current;
            }
        }
        memmove(&result, vec.data(), sizeof(Ty));
        return true;
    }

    const uint8_t* _buff;
    const size_t _size;
    size_t _position;
};

std::shared_ptr<json_value>
 parse_into_arbitrary_json_object(const uint8_t* bin, size_t len, size_t* transfer_bytes);

}  // namespace cbor
}  // namespace karl
