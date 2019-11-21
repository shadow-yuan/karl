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

// A c++ simple json library
// Version 1.0.0
// https://github.com/shadow-yuan/karl
//
// Authors: Shadow Yuan (shadow_yuan@qq.com)
//

#include "cbor.h"
#include <algorithm>
#include <cmath>
#include <limits>

// https://www.rfc-editor.org/rfc/rfc7049.txt

namespace karl {
namespace {
constexpr bool little_endianess(int num = 1) noexcept {
    return (*reinterpret_cast<char*>(&num) == 1);
}
}  // namespace
namespace cbor {
const bool is_little_endian = little_endianess();

/*
    +-----------------+-------------------------------------------------+
    | 0xf4            | False                                           |
    |                 |                                                 |
    | 0xf5            | True                                            |
    |                 |                                                 |
    | 0xf6            | Null                                            |
    |                 |                                                 |
    | 0xf7            | Undefined                                       |
    |                 |                                                 |
    | 0xf8            | (simple value, one byte follows)                |
    |                 |                                                 |
    | 0xf9            | Half-Precision Float (two-byte IEEE 754)        |
    |                 |                                                 |
    | 0xfa            | Single-Precision Float (four-byte IEEE 754)     |
    |                 |                                                 |
    | 0xfb            | Double-Precision Float (eight-byte IEEE 754)    |
    |                 |                                                 |
    | 0xff            | "break" stop code                               |
    +-----------------+-------------------------------------------------+
*/

const uint8_t null_code = 0xf6;
const uint8_t true_code = 0xf5;
const uint8_t false_code = 0xf4;

const uint8_t half_precision_prefix = 0xf9;
const uint8_t single_precision_prefix = 0xfa;
const uint8_t double_precision_prefix = 0xfb;
const uint8_t break_stop_code = 0xff;

bool is_double_precision(double value) {
    if (value > 0) {
        auto integer = floor(value);
        if (value - integer >= std::numeric_limits<double>::epsilon()) {
            if (value - integer < std::numeric_limits<float>::epsilon()) {
                return true;
            }
            if (value <= std::numeric_limits<float>::max()) {
                return false;
            }
            return true;
        }
        return false;
    }
    
    if (value < 0) {
        auto integer = ceil(value);  // eg: -2 = ceil(-2.6);
        if (integer - value >= std::numeric_limits<double>::epsilon()) {
            if (integer - value < std::numeric_limits<float>::epsilon()) {
                return true;
            }
            if (value >= -std::numeric_limits<float>::max()) {
                return false;
            }
            return true;
        }
    }
    return false;
}

// -------------------------------------------------------------

void Writer::write_character(uint8_t b) {
    check_space(1);
    _vec[_index++] = b;
}

void Writer::write_characters(const uint8_t* ptr, size_t size) {
    check_space(size);
    std::copy(ptr, ptr + size, _vec.begin() + _index);
    _index += size;
}

const std::vector<uint8_t>& Writer::binary() const {
    _vec.resize(_index);
    return _vec;
}

Writer& Writer::operator+=(const std::vector<uint8_t>& bin) {
    append_cbor_binary(bin);
    return *this;
}

void Writer::append_cbor_binary(const std::vector<uint8_t>& obj) {
    check_space(obj.size());
    std::copy(obj.begin(), obj.end(), _vec.begin() + _index);
    _index += obj.size();
}

void Writer::check_space(size_t add_size) {
    if (_vec.size() < _index + add_size) {
        auto need_space = static_cast<double>(_index + add_size);
        auto count = ceil(need_space / 512.0);
        _vec.resize(static_cast<size_t>(count) * 512);
    }
}

// -------------------------------------------------------------

std::vector<uint8_t> build_null() {
    return{ null_code };
}

std::vector<uint8_t> build_boolean(bool value) {
    if (value) {
        return{ true_code };
    }
    return{ false_code };
}

std::vector<uint8_t> build_number_signed(int64_t value) {
    Writer obj;
    if (value >= 0) {
        /*
         * Major type 0: an unsigned integer. 0b000_00000 => 0x00
         * The 5-bit additional information
         * is either the integer itself (for additional information values 0
         * through 23) or the length of additional data. Additional
         * information 24 means the value is represented in an additional
         * uint8_t, 25 means a uint16_t, 26 means a uint32_t, and 27 means a
         * uint64_t.
         */
        if (value <= 0x17) {
            obj.write_number(static_cast<uint8_t>(value));
        } else if (value <= std::numeric_limits<uint8_t>::max()) {
            obj.write_character(0x18);  // 0x18 means uint8_t
            obj.write_number(static_cast<uint8_t>(value));
        } else if (value <= std::numeric_limits<uint16_t>::max()) {
            obj.write_character(0x19);  // 0x19 means uint16_t
            obj.write_number(static_cast<uint16_t>(value));
        } else if (value <= std::numeric_limits<uint32_t>::max()) {
            obj.write_character(0x1A);  // 0x1a means uint32_t
            obj.write_number(static_cast<uint32_t>(value));
        } else {
            obj.write_character(0x1B);  // 0x1b means uint64_t
            obj.write_number(static_cast<uint64_t>(value));
        }
    } else {
        // Major type 1: a negative integer 0b001_00000 => 0x20
        const auto positive_number = -1 - value;
        if (value >= -24) {  // 0-23
            obj.write_number(static_cast<uint8_t>(0x20 + positive_number));
        } else if (positive_number <= std::numeric_limits<uint8_t>::max()) {
            obj.write_character(0x38);  // 0x20 + 0x18 
            obj.write_number(static_cast<uint8_t>(positive_number));
        } else if (positive_number <= std::numeric_limits<uint16_t>::max()) {
            obj.write_character(0x39);  // 0x20 + 0x19 
            obj.write_number(static_cast<uint16_t>(positive_number));
        } else if (positive_number <= std::numeric_limits<uint32_t>::max()) {
            obj.write_character(0x3A);  // 0x20 + 0x1a 
            obj.write_number(static_cast<uint32_t>(positive_number));
        } else {
            obj.write_character(0x3B);  // 0x20 + 0x1b 
            obj.write_number(static_cast<uint64_t>(positive_number));
        }
    }
    return obj.binary();
}

std::vector<uint8_t> build_number_usigned(uint64_t value) {
    Writer obj;
    // Major type 0: an unsigned integer. 0b000_00000 => 0x00
    if (value <= 0x17) {
        obj.write_number(static_cast<uint8_t>(value));
    } else if (value <= std::numeric_limits<uint8_t>::max()) {
        obj.write_character(0x18);
        obj.write_number(static_cast<uint8_t>(value));
    } else if (value <= std::numeric_limits<uint16_t>::max()) {
        obj.write_character(0x19);
        obj.write_number(static_cast<uint16_t>(value));
    } else if (value <= std::numeric_limits<uint32_t>::max()) {
        obj.write_character(0x1A);
        obj.write_number(static_cast<uint32_t>(value));
    } else {
        obj.write_character(0x1B);
        obj.write_number(static_cast<uint64_t>(value));
    }
    return obj.binary();
}

std::vector<uint8_t> build_number_float(double value) {
    Writer obj;
    if (is_double_precision(value)) {
        obj.write_character(double_precision_prefix);
        obj.write_number(value);
    } else {
        obj.write_character(single_precision_prefix);
        obj.write_number(static_cast<float>(value));
    }
    return obj.binary();
}

std::vector<uint8_t> build_string(const std::string& str) {
    Writer obj;
    // Major type 3: a text string, specifically a string of Unicode
    // characters that is encoded as UTF - 8[RFC3629]. 0b011_00000 => 0x60

    // step 1: write control byte and the string length
    const auto size = str.size();
    if (size <= 0x17) {
        obj.write_number(static_cast<uint8_t>(0x60 + size));
    } else if (size <= std::numeric_limits<uint8_t>::max()) {
        obj.write_character(0x78);
        obj.write_number(static_cast<uint8_t>(size));
    } else if (size <= std::numeric_limits<uint16_t>::max()) {
        obj.write_character(0x79);
        obj.write_number(static_cast<uint16_t>(size));
    } else if (size <= std::numeric_limits<uint32_t>::max()) {
        obj.write_character(0x7A);
        obj.write_number(static_cast<uint32_t>(size));
    } else if (size <= std::numeric_limits<uint64_t>::max()) {
        obj.write_character(0x7B);
        obj.write_number(static_cast<uint64_t>(size));
    }

    // step 2: write the string
    obj.write_characters(
        reinterpret_cast<const uint8_t*>(str.c_str()), str.size());

    return obj.binary();
}

std::vector<uint8_t> build_array_prefix(size_t array_size) {
    Writer obj;
    // Major type 4: an array of data items. 0b100_00000 => 0x80
    if (array_size <= 0x17) {
        obj.write_number(static_cast<uint8_t>(0x80 + array_size));
    } else if (array_size <= std::numeric_limits<uint8_t>::max()) {
        obj.write_character(0x98);
        obj.write_number(static_cast<uint8_t>(array_size));
    } else if (array_size <= std::numeric_limits<uint16_t>::max()) {
        obj.write_character(0x99);
        obj.write_number(static_cast<uint16_t>(array_size));
    } else if (array_size <= std::numeric_limits<uint32_t>::max()) {
        obj.write_character(0x9A);
        obj.write_number(static_cast<uint32_t>(array_size));
    } else if (array_size <= std::numeric_limits<uint64_t>::max()) {
        obj.write_character(0x9B);
        obj.write_number(static_cast<uint64_t>(array_size));
    }
    return obj.binary();
}

std::vector<uint8_t> build_object_prefix(size_t obj_size) {
    Writer obj;
    // Major type 5: a map of pairs of data items. 0b101_00000 = > 0xa0
    if (obj_size <= 0x17) {
        obj.write_number(static_cast<uint8_t>(0xA0 + obj_size));
    } else if (obj_size <= std::numeric_limits<uint8_t>::max()) {
        obj.write_character(0xB8);
        obj.write_number(static_cast<uint8_t>(obj_size));
    } else if (obj_size <= std::numeric_limits<uint16_t>::max()) {
        obj.write_character(0xB9);
        obj.write_number(static_cast<uint16_t>(obj_size));
    } else if (obj_size <= std::numeric_limits<uint32_t>::max()) {
        obj.write_character(0xBA);
        obj.write_number(static_cast<uint32_t>(obj_size));
    } else if (obj_size <= std::numeric_limits<uint64_t>::max()) {
        obj.write_character(0xBB);
        obj.write_number(static_cast<uint64_t>(obj_size));
    }
    return obj.binary();
}

// -------------------------------------------------------------

namespace {
class InternalTypeTable {
public:
    InternalTypeTable();
    ~InternalTypeTable() = default;
    inline MajorType get(uint8_t character) {
        return _table[character];
    }
private:
    std::vector<MajorType> _table;
};

InternalTypeTable::InternalTypeTable()
    : _table(0xff, MajorType::kUnknown) {
    
    // UNSIGNED INTEGER
    for (size_t i = 0; i <= 0x1B; i++) {
        _table[i] = MajorType::kUnsignedInteger;
    }

    // NEGATIVE INTEGER
    for (size_t i = 0x20; i <= 0x3B; i++) {
        _table[i] = MajorType::kNegativeInteger;
    }

    // STRING
    for (size_t i = 0x60; i <= 0x7B; i++) {
        _table[i] = MajorType::kString;
    }
    _table[0x7F] = MajorType::kString;

    // ARRAY
    for (size_t i = 0x80; i <= 0x9B; i++) {
        _table[i] = MajorType::kArray;
    }
    _table[0x9F] = MajorType::kArray;

    // OBJECT
    for (size_t i = 0xA0; i <= 0xBB; i++) {
        _table[i] = MajorType::kObject;
    }
    _table[0xBF] = MajorType::kObject;

    // SIMPLE CODE
    _table[false_code] = MajorType::kFalse;
    _table[true_code] = MajorType::kTrue;
    _table[null_code] = MajorType::kNull;

    // FLOAT
    _table[half_precision_prefix] = MajorType::kFloat;
    _table[single_precision_prefix] = MajorType::kFloat;
    _table[double_precision_prefix] = MajorType::kFloat;
}

InternalTypeTable global_table;
}  // namespace

Reader::Reader(const uint8_t* ptr, size_t size)
    : _buff(ptr), _size(size) ,_position(0) {}

bool Reader::read_next_type(MajorType& type) {
    uint8_t current = 0;
    if (!get_next_character(current)) {
        return false;
    }

    type = global_table.get(current);
    return type != MajorType::kUnknown;
}

bool Reader::skip_a_character() {
    if (_position < _size) {
        _position++;
        return true;
    }
    return false;
}

size_t Reader::current_position() const {
    return _position;
}

bool Reader::read_string(std::string& s) {
    uint8_t current = 0; s.clear();
    if (!read_character(current)) {
        return false;
    }

    if (current >= 0x60 && current <= 0x77) {
        return read_string(current & 0x1F, s);
    }

    if (current == 0x78) {
        uint8_t len = 0;
        return decode_number(len) && read_string(len, s);
    }
    if (current == 0x79) {
        uint16_t len = 0;
        return decode_number(len) && read_string(len, s);
    }
    if (current == 0x7A) {
        uint32_t len = 0;
        return decode_number(len) && read_string(len, s);
    }
    if (current == 0x7B) {
        uint64_t len = 0;
        return decode_number(len) && read_string(static_cast<size_t>(len), s);
    }
    if (current == 0x7F) {  // indefinite length
        while (_buff[_position] != break_stop_code) {
            std::string chunk;
            if (!read_string(chunk)) {
                return false;
            }
            s.append(chunk);
        }
        return true;
    }
    return false;
}

bool Reader::read_array(std::shared_ptr<json_array>& obj) {
    size_t count = 0;
    if (!read_array_size(count)) {
        return false;
    }
    obj = New<json_array>();
    if (count == INDEFINITE_LENGTH) {
        while (_buff[_position] != break_stop_code) {
            size_t parse_bytes = 0;
            std::shared_ptr<json_value> value;
            if (!parse_into_json_value(value, &parse_bytes)) {
                return false;
            }
            obj->append(value);
            _position += parse_bytes;
        }
        _position += 1;
    } else {
        obj->resize(count);
        for (size_t i = 0; i < count; i++) {
            size_t parse_bytes = 0;
            std::shared_ptr<json_value> value;
            if (!parse_into_json_value(value, &parse_bytes)) {
                return false;
            }
            obj->SetAt(i, value);
            _position += parse_bytes;
        }
    }
    return true;
}

bool Reader::read_object(std::shared_ptr<json_object>& obj) {
    size_t count = 0;
    if (!read_object_size(count)) {
        return false;
    }

    obj = New<json_object>();
    if (count == INDEFINITE_LENGTH) {
        while (_buff[_position] != break_stop_code) {
            std::string key;
            if (!read_string(key)) {
                return false;
            }

            size_t transfer_bytes = 0;
            std::shared_ptr<json_value> value;
            if (!parse_into_json_value(value, &transfer_bytes)) {
                return false;
            }
            obj->set_value(key, value);
            _position += transfer_bytes;
        }
        _position += 1;
    } else {
        for (size_t i = 0; i < count; i++) {
            std::string key;
            if (!read_string(key)) {
                return false;
            }
            size_t transfer_bytes = 0;
            std::shared_ptr<json_value> value;
            if (!parse_into_json_value(value, &transfer_bytes)) {
                return false;
            }
            obj->set_value(key, value);
            _position += transfer_bytes;
        }
    }
    return true;
}

bool Reader::get_next_character(uint8_t& c) {
    if (_position >= _size) {
        return false;
    }
    c = _buff[_position];
    return true;
}

bool Reader::read_character(uint8_t& c) {
    if (_position < _size) {
        c = _buff[_position++];
        return true;
    }
    return false;
}

bool Reader::read_string(size_t len, std::string& s) {
    if (_position + len > _size) {
        return false;
    }
    s.resize(len);
    for (size_t i = 0; i < len; i++) {
        s[i] = _buff[_position++];
    }
    return true;
}

bool Reader::read_array_size(size_t& size) {
    uint8_t current = 0; size = 0;
    if (!read_character(current)) {
        return false;
    }
    if (current >= 0x80 && current <= 0x97) {
        size = static_cast<size_t>(current & 0x1F);
        return true;
    }
    if (current == 0x98) {
        uint8_t number;
        if (decode_number(number)) {
            size = static_cast<size_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0x99) {
        uint16_t number;
        if (decode_number(number)) {
            size = static_cast<size_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0x9A) {
        uint32_t number;
        if (decode_number(number)) {
            size = static_cast<size_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0x9B) {
        uint64_t number;
        if (decode_number(number)) {
            size = static_cast<size_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0x9F) {
        size = INDEFINITE_LENGTH;
        return true;
    }
    return false;
}

bool Reader::read_object_size(size_t& size) {
    uint8_t current = 0; size = 0;
    if (!read_character(current)) {
        return false;
    }

    if (current >= 0xA0 && current <= 0xB7) {
        size = static_cast<size_t>(current & 0x1F);
        return true;
    }
    if (current == 0xB8) {
        uint8_t number;
        if (decode_number(number)) {
            size = static_cast<size_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0xB9) {
        uint16_t number;
        if (decode_number(number)) {
            size = static_cast<size_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0xBA) {
        uint32_t number;
        if (decode_number(number)) {
            size = static_cast<size_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0xBB) {
        uint64_t number;
        if (decode_number(number)) {
            size = static_cast<size_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0xBF) {
        size = INDEFINITE_LENGTH;
        return true;
    }
    return false;
}

bool Reader::parse_into_json_value(std::shared_ptr<json_value>& obj, size_t* transfer_bytes) {
    Reader rder(_buff + _position, _size - _position);
    MajorType type;
    if (!read_next_type(type) || type == MajorType::kUnknown) {
        return false;
    }

    switch (type) {
    case MajorType::kUnsignedInteger:
    {
        uint64_t number = 0;
        if (!rder.read_unsigned_number(number)) {
            return false;
        }
        obj = New<json_number>(number);
    }
        break;
    case MajorType::kNegativeInteger:
    {
        int64_t number = 0;
        if (!rder.read_negative_number(number)) {
            return false;
        }
        obj = New<json_number>(number);
    }
        break;
    case MajorType::kString:
    {
        std::string s;
        if (!rder.read_string(s)) {
            return false;
        }
        obj = New<json_string>(s);
    }
        break;
    case MajorType::kArray:
    {
        std::shared_ptr<json_array> vec;
        if (!rder.read_array(vec)) {
            return false;
        }
        obj = vec;
    }
        break;
    case MajorType::kObject:
    {
        std::shared_ptr<json_object> sub;
        if (!rder.read_object(sub)) {
            return false;
        }
        obj = sub;
    }
        break;
    case MajorType::kFalse:
        obj = New<json_boolean>(false);
        skip_a_character();
        break;
    case MajorType::kTrue:
        obj = New<json_boolean>(true);
        skip_a_character();
        break;
    case MajorType::kNull:
        obj = New<json_null>();
        skip_a_character();
        break;
    case MajorType::kFloat:
    {
        double number = 0;
        if (!rder.read_float_number(number)) {
            return false;
        }
        obj = New<json_number>(number);
    }
        break;
    }
    *transfer_bytes = rder.current_position();
    return true;
}

bool Reader::read_false() {
    uint8_t c = 0;
    if (read_character(c) && c == false_code) {
        return true;
    }
    return false;
}

bool Reader::read_true() {
    uint8_t c = 0;
    if (read_character(c) && c == true_code) {
        return true;
    }
    return false;
}

bool Reader::read_null() {
    uint8_t c = 0;
    if (read_character(c) && c == null_code) {
        return true;
    }
    return false;
}

bool Reader::read_unsigned_number(uint64_t& value) {
    uint8_t current = 0; value = 0;
    if (!read_character(current)) {
        return false;
    }

    if (current <= 0x17) {
        value = static_cast<uint64_t>(current);
        return true;
    }

    if (current == 0x18) {
        uint8_t number = 0;
        if (decode_number(number)) {
            value = static_cast<uint64_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0x19) {
        uint16_t number = 0;
        if (decode_number(number)) {
            value = static_cast<uint64_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0x1A) {
        uint32_t number = 0;
        if (decode_number(number)) {
            value = static_cast<uint64_t>(number);
            return true;
        }
        return false;
    }
    if (current == 0x1B) {
        uint64_t number = 0;
        if (decode_number(number)) {
            value = number;
            return true;
        }
    }
    return false;
}

bool Reader::read_negative_number(int64_t& value) {
    uint8_t current = 0; value = 0;
    if (!read_character(current)) {
        return false;
    }

    if (current >= 0x20 && current <= 0x37) {
        value = (static_cast<int8_t>(0x20 - 1 - current));
        return true;
    }

    if (current == 0x38) {
        uint8_t number = 0;
        if (decode_number(number)) {
            value = static_cast<int64_t>(-1) - number;
            return true;
        }
        return false;
    }
    if (current == 0x39) {
        uint16_t number = 0;
        if (decode_number(number)) {
            value = static_cast<int64_t>(-1) - number;
            return true;
        }
        return false;
    }
    if (current == 0x3A) {
        uint32_t number = 0;
        if (decode_number(number)) {
            value = static_cast<int64_t>(-1) - number;
            return true;
        }
        return false;
    }
    if (current == 0x3B) {
        uint64_t number = 0;
        if (decode_number(number)) {
            value = static_cast<int64_t>(-1) - static_cast<int64_t>(number);
            return true;
        }
    }
    return false;
}

bool Reader::read_float_number(double& value) {
    uint8_t current = 0; value = 0;
    if (!read_character(current)) {
        return false;
    }

    if (current == half_precision_prefix) {  // Half-Precision Float (two-byte IEEE 754)
        if (_position + 2 > _size) {
            return false;
        }

        const auto byte1 = _buff[_position++];
        const auto byte2 = _buff[_position++];

        // code from RFC 7049, Appendix D, Figure 3:
        // As half-precision floating-point numbers were only added
        // to IEEE 754 in 2008, today's programming platforms often
        // still only have limited support for them. It is very
        // easy to include at least decoding support for them even
        // without such support. An example of a small decoder for
        // half-precision floating-point numbers in the C language
        // is shown in Fig. 3.
        const int half = (byte1 << 8) + byte2;
        const double val = [&half] () -> double {
            const int exp = (half >> 10) & 0x1F;
            const int mant = half & 0x3FF;

            switch (exp) {
            case 0:
                return std::ldexp(mant, -24);
            case 31:
                return (mant == 0)
                    ? std::numeric_limits<double>::infinity()
                    : std::numeric_limits<double>::quiet_NaN();
            default:
                return std::ldexp(mant + 1024, exp - 25);
            }
        }();

        value = ((half & 0x8000) != 0) ? (-val) : (val);
        return true;
    }

    if (current == single_precision_prefix) {  // Single-Precision Float (four-byte IEEE 754)
        float number = 0;
        if (decode_number(number)) {
            value = number;
            return true;
        }
        return false;
    }

    if (current == double_precision_prefix) {  // Double-Precision Float (eight-byte IEEE 754)
        double number = 0;
        if (decode_number(number)) {
            value = number;
            return true;
        }
    }
    return false;
}

// -------------------------------------------------------------

std::shared_ptr<json_value>
parse_into_arbitrary_json_object(const uint8_t* bin, size_t len, size_t* transfer_bytes) {
    std::shared_ptr<json_value> obj;
    Reader rder(bin, len);

    MajorType type;
    if (!rder.read_next_type(type) || type == MajorType::kUnknown) {
        goto __exit;
    }

    switch (type) {
    case MajorType::kUnsignedInteger:
    {
        uint64_t number = 0;
        if (!rder.read_unsigned_number(number)) {
            goto __exit;
        }
        obj = New<json_number>(number);
    }
    	break;
    case MajorType::kNegativeInteger:
    {
        int64_t number = 0;
        if (!rder.read_negative_number(number)) {
            goto __exit;
        }
        obj = New<json_number>(number);
    }
        break;
    case MajorType::kString:
    {
        std::string s;
        if (!rder.read_string(s)) {
            goto __exit;
        }
        obj = New<json_string>(s);
    }
        break;
    case MajorType::kArray:
    {
        std::shared_ptr<json_array> vec;
        if (!rder.read_array(vec)) {
            goto __exit;
        }
        obj = vec;
    }
        break;
    case MajorType::kObject:
    {
        std::shared_ptr<json_object> sub;
        if (!rder.read_object(sub)) {
            goto __exit;
        }
        obj = sub;
    }
        break;
    case MajorType::kFalse:
        obj = New<json_boolean>(false);
        rder.skip_a_character();
        break;
    case MajorType::kTrue:
        obj = New<json_boolean>(true);
        rder.skip_a_character();
        break;
    case MajorType::kNull:
        obj = New<json_null>();
        rder.skip_a_character();
        break;
    case MajorType::kFloat:
    {
        double number = 0;
        if (!rder.read_float_number(number)) {
            goto __exit;
        }
        obj = New<json_number>(number);
    }
        break;
    }
__exit:
    if (transfer_bytes) {
        *transfer_bytes = rder.current_position();
    }
    return obj;
}
}  // namespace cbor
}  // namespace karl
