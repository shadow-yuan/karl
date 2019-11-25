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

#include "karl.h"
#include <assert.h>
#include <math.h>
#include <limits>
#include <utility>
#include <sstream>
#include "cJSON.h"
#include "cbor.h"

namespace karl {
namespace {
const char NULL_STR[] = "null";
const char EMPTY_OBJ[] = "{}";
const char TRUE_STR[] = "true";
const char FALSE_STR[] = "false";
const char INDENT_CHAR = ' ';
}  // namespace

// ---------------------------  json_null members  ---------------------------------

value_type json_null::type() const { return value_type::kNull; }
std::string json_null::dump() const { return NULL_STR; }
std::string json_null::dump(int indent, int prefix) const { return NULL_STR; }
std::string json_null::value() const { return NULL_STR; }
std::shared_ptr<json_value> json_null::copy() const { return New<json_null>(); }
std::vector<uint8_t> json_null::to_cbor() const {
    return cbor::build_null();
}

bool json_null::empty() const { return false; }

// ---------------------------  json_number members  ---------------------------------

value_type json_boolean::type() const { return value_type::kBoolean; }
std::string json_boolean::dump() const {
    return (_value) ? std::string(TRUE_STR) : std::string(FALSE_STR);
}
std::string json_boolean::dump(int indent, int prefix) const {
    return (_value) ? std::string(TRUE_STR) : std::string(FALSE_STR);
}
json_boolean& json_boolean::operator= (bool value) {
    _value = value; return *this;
}
bool json_boolean::empty() const { return false; }

std::shared_ptr<json_value> json_boolean::copy() const {
    return New<json_boolean>(_value);
}
std::vector<uint8_t> json_boolean::to_cbor() const {
    return cbor::build_boolean(_value);
}

// ---------------------------  json_number members  ---------------------------------

std::shared_ptr<json_value> json_number::create(double value) {
    number_check number;
    number.ddd = value;
    number.u64 = static_cast<uint64_t>(value);

    auto obj = New<json_number>();
    obj->_value = number;

    if (number.ddd > 0) {
        auto integer = floor(number.ddd);
        if (number.ddd - integer >= std::numeric_limits<double>::epsilon()) {
            obj->_value_type = number_type::kFloat;
            return obj;
        }
        // arriving here means it's not a float

    } else if (number.ddd < 0) {
        auto integer = ceil(number.ddd);  // eg: -2 = ceil(-2.6);
        if (integer - number.ddd >= std::numeric_limits<double>::epsilon()) {
            obj->_value_type = number_type::kFloat;
            return obj;
        }
        // arriving here means it's not a float

    } else {
        obj->_value_type = number_type::kUnsigned;
        return obj;
    }

    obj->_value_type = (number.i64 < 0) ? number_type::kSigned : number_type::kUnsigned;
    return obj;
}

json_number::json_number(int8_t value)   { set_value(static_cast<int64_t>(value));}
json_number::json_number(int16_t value)  { set_value(static_cast<int64_t>(value)); }
json_number::json_number(int32_t value)  { set_value(static_cast<int64_t>(value)); }
json_number::json_number(int64_t value)  { set_value(value);}
json_number::json_number(uint8_t value)  { set_value(static_cast<uint64_t>(value));}
json_number::json_number(uint16_t value) { set_value(static_cast<uint64_t>(value));}
json_number::json_number(uint32_t value) { set_value(static_cast<uint64_t>(value));}
json_number::json_number(uint64_t value) { set_value(value);}
json_number::json_number(float value)    { set_value(static_cast<double>(value));}
json_number::json_number(double value)   { set_value(value);}

json_number::json_number(const json_number& oth)
    : _value_type(oth._value_type), _value(oth._value) {}

json_number& json_number::operator=(const json_number& rhs) {
    if (this != &rhs) {
        _value_type = rhs._value_type;
        _value = rhs._value;
    }
    return *this;
}

json_number& json_number::operator=(int8_t value) {
    set_value(static_cast<int64_t>(value));
    return *this;
}

json_number& json_number::operator=(int16_t value) {
    set_value(static_cast<int64_t>(value));
    return *this;
}

json_number& json_number::operator=(int32_t value) {
    set_value(static_cast<int64_t>(value));
    return *this;
}

json_number& json_number::operator=(int64_t value) {
    set_value(value);
    return *this;
}

json_number& json_number::operator=(uint8_t value) {
    set_value(static_cast<uint64_t>(value));
    return *this;
}

json_number& json_number::operator=(uint16_t value) {
    set_value(static_cast<uint64_t>(value));
    return *this;
}

json_number& json_number::operator=(uint32_t value) {
    set_value(static_cast<uint64_t>(value));
    return *this;
}

json_number& json_number::operator=(uint64_t value) {
    set_value(value);
    return *this;
}

json_number& json_number::operator=(float value) {
    set_value(static_cast<double>(value));
    return *this;
}

json_number& json_number::operator=(double value) {
    set_value(value);
    return *this;
}

json_number::operator int8_t  () const { return static_cast<int8_t>(_value.i32); }
json_number::operator int16_t () const { return static_cast<int16_t>(_value.i32); }
json_number::operator int32_t () const { return _value.i32; }
json_number::operator int64_t () const { return _value.i64; }
json_number::operator uint8_t () const { return static_cast<uint8_t>(_value.u32); }
json_number::operator uint16_t() const { return static_cast<uint16_t>(_value.u32); }
json_number::operator uint32_t() const { return _value.u32; }
json_number::operator uint64_t() const { return _value.u64; }
json_number::operator double()   const { return _value.ddd; }
json_number::operator float()    const { return static_cast<float>(_value.ddd); }

std::string json_number::dump() const {
    switch (_value_type) {
    case number_type::kSigned:
        return std::to_string(_value.i64);
    case number_type::kUnsigned:
        return std::to_string(_value.u64);
    default:
        return std::to_string(_value.ddd);
    }
}

std::string json_number::dump(int indent, int prefix) const {
    return this->dump();
}

std::shared_ptr<json_value> json_number::copy() const {
    auto obj = New<json_number>();
    obj->_value = _value;
    obj->_value_type = _value_type;
    return obj;
}

std::vector<uint8_t> json_number::to_cbor() const {
    if (_value_type == number_type::kSigned) {
        return cbor::build_number_signed(_value.i64);
    }
    if (_value_type == number_type::kUnsigned) {
        return cbor::build_number_usigned(_value.u64);
    }
    return cbor::build_number_float(_value.ddd);
}

void json_number::set_value(int64_t value) {
    _value_type = (value < 0) ? number_type::kSigned : number_type::kUnsigned;
    _value.u64 = static_cast<uint64_t>(value);
    _value.ddd = static_cast<double>(value);
}

void json_number::set_value(uint64_t value) {
    _value_type = number_type::kUnsigned;
    _value.ddd = static_cast<double>(value);
    _value.u64 = value;
}

void json_number::set_value(double value) {
    _value_type = number_type::kFloat;
    _value.ddd = value;
    _value.u64 = static_cast<uint64_t>(value);
}

bool json_number::is_float() const {
    return (_value_type == number_type::kFloat);
}
bool json_number::is_unsigned() const {
    return (_value_type == number_type::kUnsigned);
}
bool json_number::is_signed() const {
    return (_value_type == number_type::kSigned);
}

// ---------------------------  json_string members  ---------------------------------

json_string::json_string(const char* value) : _value(value) {}
json_string::json_string(const std::string& value) : _value(value) {}

std::string json_string::dump() const {
    std::stringstream ss;
    ss << "\"" << _value << "\"";
    return ss.str();
}

std::string json_string::dump(int indent, int prefix) const {
    std::stringstream ss;
    ss << "\"" << _value << "\"";
    return ss.str();
}

std::shared_ptr<json_value> json_string::copy() const {
    return New<json_string>(_value);
}

std::vector<uint8_t> json_string::to_cbor() const {
    return cbor::build_string(_value);
}

json_string& json_string::operator=(const std::string& value) {
    _value = value;
    return *this;
}

// ---------------------------  json_array members  ---------------------------------

std::shared_ptr<json_value> json_array::GetAt(size_t i) {
    if (i >= _seq.size()) {
        return nullptr;
    }
    return _seq[i];
}

bool json_array::SetAt(size_t i, std::shared_ptr<json_value> element) {
    if (i >= _seq.size()) {
        _seq.resize(i + 1);
    }
    _seq[i] = element;
    return true;
}

std::shared_ptr<json_value>& json_array::operator[](size_t i) {
    if (i >= _seq.size()) {
        _seq.resize(i + 1);
    }
    return _seq[i];
}

const std::shared_ptr<json_value>& json_array::operator[](size_t i) const {
    return _seq[i];
}

bool json_array::insert(size_t i, std::shared_ptr<json_value> element) {
    if (i > _seq.size()) {
        _seq.resize(i);
    }
    auto iter = _seq.begin();
    std::advance(iter, i);
    _seq.insert(iter, element);
    return true;
}

void json_array::append(std::shared_ptr<json_value> element) {
    _seq.push_back(element);
}

void json_array::resize(size_t size) {
    _seq.resize(size);
}

void json_array::clear() {
    _seq.clear();
}

size_t json_array::size() const {
    return _seq.size();
}

void json_array::erase(size_t index) {
    if (index < _seq.size()) {
        _seq.erase(_seq.begin() + index);
    }
}

array_iterator json_array::begin() {
    return _seq.begin();
}

array_iterator json_array::end() {
    return _seq.end();
}

std::string json_array::dump() const {
    std::stringstream ss;
    ss << "[";

    size_t size = _seq.size();
    size_t index = 0;
    for (const std::shared_ptr<json_value>& iter : _seq) {
        ss << ((iter) ? iter->dump() : std::string(NULL_STR));
        if (++index != size) {
            ss << ",";
        }
    }
    ss << "]";
    return ss.str();
}

std::string json_array::dump(int indent, int prefix) const {
    std::stringstream ss;
    if (_seq.empty()) {
        ss << "[]";
        return ss.str();
    }
    ss << "[";

    size_t size = _seq.size();
    size_t index = 0;
    for (const std::shared_ptr<json_value>& iter : _seq) {
        if (index == 0) {
            if (iter->type() == value_type::kArray || iter->type() == value_type::kObject) {
                ss << /*"\n" <<*/ std::string(prefix + indent, INDENT_CHAR);
            }
        }
        ss << "\n" << std::string(prefix + indent, INDENT_CHAR);
        ss << ((iter) ? iter->dump(indent, prefix + indent) : std::string(NULL_STR));
        
        if (++index != size) {
            ss << ",";
        } else {
            ss << "\n";
        }
    }
    if (prefix) {
        ss << std::string(prefix, INDENT_CHAR);
    }
    ss << "]";
    return ss.str();
}

std::shared_ptr<json_value> json_array::copy() const {
    auto obj = New<json_array>();
    if (!_seq.empty()) {
        obj->resize(_seq.size());
        for (size_t i = 0; i < _seq.size(); i++) {
            if (_seq[i]) {
                obj->SetAt(i, _seq[i]->copy());
            }
        }
    }
    return obj;
}

std::vector<uint8_t> json_array::to_cbor() const {
    cbor::Writer obj;
    obj += cbor::build_array_prefix(_seq.size());
    for (size_t i = 0; i < _seq.size(); i++) {
        if (_seq[i]) {
            obj += _seq[i]->to_cbor();
        } else {
            obj.write_character(cbor::null_code);
        }
    }
    return obj.binary();
}

// ---------------------------  json_object members  ---------------------------------

bool json_object::has_key(const std::string& key) const {
    auto it = _map.find(key);
    if (it == _map.end()) {
        return false;
    }
    return true;
}

std::shared_ptr<json_value> json_object::get_value(const std::string& key) const {
    auto it = _map.find(key);
    if (it == _map.end()) {
        return nullptr;
    }
    return it->second;
}

void json_object::set_value(const std::string& key, std::shared_ptr<json_value> element) {
    _map[key] = element;
}

void json_object::clear() {
    _map.clear();
}

json_object::iterator json_object::begin() {
    return _map.begin();
}

json_object::iterator json_object::end() {
    return _map.end();
}

void json_object::erase(const std::string& key) {
    _map.erase(key);
}

size_t json_object::size() const {
    return _map.size();
}

std::string json_object::dump() const {
    std::stringstream ss;
    ss << "{";
    size_t size = _map.size();
    size_t index = 0;
    for (const std::pair<const std::string, std::shared_ptr<json_value>>& iter : _map) {
        ss << "\"" << iter.first << "\":";
        ss << ((iter.second) ? iter.second->dump() : std::string(NULL_STR));
        if (++index != size) {
            ss << ",";
        }
    }
    ss << "}";
    return ss.str();
}

std::string json_object::dump(int indent, int prefix) const {
    std::stringstream ss;
    if (_map.empty()) {
        ss << "{}";
        return ss.str();
    }
    ss << "{\n";
    size_t size = _map.size();
    size_t index = 0;
    for (const std::pair<const std::string, std::shared_ptr<json_value>>& iter : _map) {
        ss << std::string(prefix + indent, INDENT_CHAR);
        ss << "\"" << iter.first << "\": ";

        if (iter.second) {
            ss << iter.second->dump(indent, prefix + indent);
        } else {
            ss << std::string(NULL_STR);
        }
        if (++index != size) {
            ss << ",\n";
        } else {
            ss << "\n";
        }
    }
    if (prefix) {
        ss << std::string(prefix, INDENT_CHAR);
    }
    ss << "}";
    return ss.str();
}

std::shared_ptr<json_value> json_object::copy() const {
    auto obj = New<json_object>();
    if (!_map.empty()) {
        for (auto it = _map.begin(); it != _map.end(); ++it) {
            if (it->second) {
                obj->set_value(it->first, it->second->copy());
            } else {
                obj->set_value(it->first, it->second);
            }
        }
    }
    return obj;
}

std::vector<uint8_t> json_object::to_cbor() const {
    cbor::Writer obj;
    obj += cbor::build_object_prefix(_map.size());
    for (auto it = _map.begin(); it != _map.end(); ++it) {
        obj += cbor::build_string(it->first);
        if (it->second) {
            obj += it->second->to_cbor();
        } else {
            obj.write_character(cbor::null_code);
        }
    }
    return obj.binary();
}

// ---------------------------  key_value_pair members  ---------------------------------

key_value_pair::key_value_pair() {}
key_value_pair::~key_value_pair() {}
key_value_pair::key_value_pair(const std::string& key, const char* value)
    : _key(key) {
    if (value) {
        _value = New<json_string>(value);
    } else {
        _value = New<json_null>();
    }
}

key_value_pair::key_value_pair(const std::string& key, const std::string& value)
    : _key(key), _value(New<json_string>(value)){}

key_value_pair::key_value_pair(const std::string& key, int8_t value) 
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, int16_t value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, int32_t value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, int64_t value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, uint8_t value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, uint16_t value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, uint32_t value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, uint64_t value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, double value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, float value)
    : _key(key), _value(New<json_number>(value)) {}

key_value_pair::key_value_pair(const std::string& key, bool value)
    : _key(key), _value(New<json_boolean>(value)) {}

key_value_pair::key_value_pair(const key_value_pair& oth)
    : _key(oth._key), _value(oth._value) {}

key_value_pair& key_value_pair::operator= (const key_value_pair& rhs) {
    if (this != &rhs) {
        _key = rhs._key;
        _value = rhs._value;
    }
    return *this;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<int8_t>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<int16_t>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<int32_t>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<int64_t>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<uint8_t>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<uint16_t>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<uint32_t>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<uint64_t>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<std::string>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_string>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<double>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_number>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

key_value_pair::key_value_pair(const std::string& key, const std::vector<bool>& value) {
    auto obj = New<json_array>();
    obj->resize(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        auto item = New<json_boolean>(value[i]);
        obj->SetAt(i, item);
    }
    _value = obj;
    _key = key;
}

const std::string& key_value_pair::key() const { return _key; }
std::shared_ptr<json_value> key_value_pair::value() const { return _value; }

//////////////////////////////////////////////////////////////////////////
// ---------------------------  global functions  ---------------------------------

std::shared_ptr<json_value> internal_parse_object(cJSON* js);
std::shared_ptr<json_value> internal_parse_array(cJSON* js);

std::shared_ptr<json_value> internal_parse_array(cJSON* ptr) {
    assert(ptr && ptr->type == cJSON_Array);
    std::shared_ptr<json_array> obj = New<json_array>();
    cJSON* child = ptr->child;

    while (child) {
        std::shared_ptr<json_value> value;
        switch (child->type) {
        case cJSON_False:
            value = New<json_boolean>(false);
            break;
        case cJSON_True:
            value = New<json_boolean>(true);
            break;
        case cJSON_NULL:
            value = New<json_null>();
            break;
        case cJSON_Number:
            value = json_number::create(child->valuedouble);
            break;
        case cJSON_String:
            value = New<json_string>(child->valuestring);
            break;
        case cJSON_Array:
            value = internal_parse_array(child);
            break;
        case cJSON_Object:
            value = internal_parse_object(child);
            break;
        }
        assert(value);

        obj->append(value);
        child = child->next;
    }
    return obj;
}

std::shared_ptr<json_value> internal_parse_object(cJSON* ptr) {
    assert(ptr && ptr->type == cJSON_Object);
    std::shared_ptr<json_object> obj = New<json_object>();
    cJSON* child = ptr->child;
    while (child) {
        std::shared_ptr<json_value> value;
        switch (child->type) {
        case cJSON_False:
            value = New<json_boolean>(false);
            break;
        case cJSON_True:
            value = New<json_boolean>(true);
            break;
        case cJSON_NULL:
            value = New<json_null>();
            break;
        case cJSON_Number:
            value = json_number::create(child->valuedouble);
            break;
        case cJSON_String:
            value = New<json_string>(child->valuestring);
            break;
        case cJSON_Array:
            value = internal_parse_array(child);
            break;
        case cJSON_Object:
            value = internal_parse_object(child);
            break;
        }
        assert(value);

        obj->set_value(child->string, value);
        child = child->next;
    }
    return obj;
}

// ---------------------------  json static members  ---------------------------------

json json::parse(const std::string& data) {
    cJSON* js = cJSON_Parse(data.c_str());
    if (!js) {
        return json();
    }
    if (js->type == cJSON_Array) {
        json obj(internal_parse_array(js));
        cJSON_Delete(js);
        return obj;
    }
    if (js->type == cJSON_Object) {
        json obj(internal_parse_object(js));
        cJSON_Delete(js);
        return obj;
    }
    THROW_PARSE_ERROR("the data is not json array or json object");
}

json json::parse(const char* ptr, size_t size) {
    if (ptr == nullptr || size == 0) {
        return json();
    }
    std::string data(ptr, size);
    return parse(data);
}

json json::parse(std::istream* stream) {
    if (!stream) {
        return json();
    }
    std::stringstream ss;
    ss << stream->rdbuf();
    return parse(ss.str());
}

json json::from_cbor(const uint8_t* ptr, size_t len, size_t* tranfer_bytes) {
    if (len == 0) {
        return json();
    }
    size_t parse_bytes = 0;
    auto obj = cbor::parse_into_arbitrary_json_object(ptr, len, &parse_bytes);
    if (tranfer_bytes) {
        *tranfer_bytes = parse_bytes;
    }
    return json(obj);
}

json json::from_cbor(const std::vector<uint8_t>& bin, size_t* tranfer_bytes) {
    if (bin.empty()){
        return json();
    }
    size_t parse_bytes = 0;
    auto obj = cbor::parse_into_arbitrary_json_object(bin.data(), bin.size(), &parse_bytes);
    if (tranfer_bytes) {
        *tranfer_bytes = parse_bytes;
    }
    return json(obj);
}

std::vector<uint8_t> json::to_cbor(json js) {
    return js.to_cbor();
}

json json::array() {
    return json(New<json_array>());
}

json json::object() {
    return json(New<json_object>());
}

// ---------------------------  json members  ---------------------------------

json::json(std::shared_ptr<json_value> item)
    : _depth(0), _data(item) {}

json::json() : _depth(0) {}

json::json(const json& j)
    : _depth(j._depth)
    , _data(j._data)
    , _index(j._index)
    , _key(j._key) {}

json::json(std::initializer_list<key_value_pair> init)
    : _depth(0) {
    auto obj = New<json_object>();
    for (auto it = init.begin(); it != init.end(); ++it) {
        obj->set_value(it->key(), it->value());
    }
    _data = std::move(obj);
}

json& json::operator=(const json& j) {
    auto value = j.current_value();
    fill_current_value(value);
    return *this;
}

std::string json::dump(int indent) const {
    auto obj = current_value();
    if (!obj) {
        return _depth ? std::string(NULL_STR) : std::string(EMPTY_OBJ);
    }
    if (indent < 0) {
        return obj->dump();
    }
    return obj->dump(indent, 0);
}

bool json::empty() const {
    auto obj = current_value();
    if (!obj) {
        return true;
    }
    return obj->empty();
}
json json::copy() const {
    auto obj = current_value();
    if (!obj) {
        return json();
    }
    json js(obj->copy());
    return js;
}

std::vector<uint8_t> json::to_cbor() const {
    auto obj = current_value();
    if (!obj) {
        return std::vector<uint8_t>();
    }
    return obj->to_cbor();
}

bool json::is_null() const {
    return get_type() == value_type::kNull;
}

bool json::is_boolean() const {
    return get_type() == value_type::kBoolean;
}

bool json::is_number() const {
    return get_type() == value_type::kNumber;
}

bool json::is_string() const {
    return get_type() == value_type::kString;
}

bool json::is_array() const {
    return get_type() == value_type::kArray;
}

bool json::is_object() const {
    return get_type() == value_type::kObject;
}

bool json::is_structured() const {
    auto type = get_type();
    return  (type == value_type::kArray) || (type == value_type::kObject);
}

bool json::has_key(const std::string& key) const {
    auto value = current_value();
    if (value && value->type() == value_type::kObject) {
        auto obj = As<json_object>(value);
        return obj->has_key(key);
    }
    return false;
}

void json::erase(const std::string& key) {
    auto value = current_value();
    if (value && value->type() == value_type::kObject) {
        auto obj = As<json_object>(value);
        obj->erase(key);
    }
}

void json::erase(size_t idx) {
    auto value = current_value();
    if (value && value->type() == value_type::kArray) {
        auto obj = As<json_array>(value);
        obj->erase(idx);
    }
}

json json::operator[](size_t index) {
    if (_depth != 0) {
        json js;
        js._data = current_value();
        if (!js._data) {
            auto obj = New<json_array>();
            fill_current_value(obj);
            js._data = obj;
        }
        js._index = New<size_t>(index);
        js._depth = _depth + 1;
        return js;
    }

    if (!_data || _data->type() == value_type::kNull) {
        json js;
        _data = New<json_array>();
        js._data = _data;
        js._index = New<size_t>(index);
        js._depth = _depth + 1;
        return js;
    } else if (_data->type() == value_type::kArray) {
        json js;
        js._data = _data;
        js._index = New<size_t>(index);
        js._depth = _depth + 1;
        return js;
    }

    THROW_OTHER_ERROR("cannot use operator[] with a size_t argument with " + std::string(current_type()));
}

json json::operator[](const std::string& key) {
    if (_depth != 0) {
        json js;
        js._data = current_value();
        if (!js._data) {
            auto obj = New<json_object>();
            fill_current_value(obj);
            js._data = obj;
        }
        js._key = New<std::string>(key);
        js._depth = _depth + 1;
        return js;
    } 
    
    if (!_data || _data->type() == value_type::kNull) {
        json js;
        _data = New<json_object>();
        js._data = _data;
        js._key = New<std::string>(key);
        js._depth = _depth + 1;
        return js;
    } else if (_data->type() == value_type::kObject) {
        json js;
        js._data = _data;
        js._key = New<std::string>(key);
        js._depth = _depth + 1;
        return js;
    }
    
    THROW_OTHER_ERROR("cannot use operator[] with a string argument with " + std::string(current_type()));
}

const json json::operator[](size_t index) const {
    if (is_array()) {
        json js;
        js._data = current_value();
        js._index = New<size_t>(index);
        js._depth = _depth + 1;
        return js;
    }

    THROW_TYPE_ERROR("cannot use operator[] with a size_t argument with "
        + std::string(current_type()) + std::string(", current depth = ") + std::to_string(_depth));
}

const json json::operator[](const std::string& key) const {
    if (is_object()) {
        json js;
        js._data = current_value();
        js._key = New<std::string>(key);
        js._depth = _depth + 1;
        return js;
    }

    THROW_TYPE_ERROR("cannot use operator[] with a string argument with "
        + std::string(current_type()) + std::string(", current depth = ") + std::to_string(_depth));
}

void json::clear() {
    _depth = 0;
    _data.reset();
    _index.reset();
    _key.reset();
}

size_t json::size() {
    auto obj = current_value();
    if (obj) {
        if (obj->type() == value_type::kArray) {
            auto vec = As<json_array>(obj);
            return vec->size();
        }
        if (obj->type() == value_type::kObject) {
            auto map = As<json_object>(obj);
            return map->size();
        }
    }
    return 0;
}

json& json::assign(const std::string& s) {
    if (_index) {
        auto vec = As<json_array>(_data);
        auto old = vec->GetAt(*_index);
        if (!old || old->type() != value_type::kString) {
            old = New<json_string>(s);
            vec->SetAt(*_index, old);
        } else {
            auto obj = As<json_string>(old);
            obj ->operator= (s);
        }
        return *this;
    }
    if (_key) {
        auto map = As<json_object>(_data);
        auto old = map->get_value(*_key);
        if (!old || old->type() != value_type::kString) {
            old = New<json_string>(s);
            map->set_value(*_key, old);
        } else {
            auto obj = As<json_string>(old);
            obj ->operator= (s);
        }
        return *this;
    }
    if (!_data || _data->type() != value_type::kString) {
        _data = New<json_string>(s);
    } else {
        auto obj = As<json_string>(_data);
        obj ->operator= (s);
    }
    return *this;
}

json& json::assign(uint64_t value) {
    json_number number(value);
    return set_json_number(&number);
}

json& json::assign(int64_t value) {
    json_number number(value);
    return set_json_number(&number);
}

json& json::assign(double value) {
    json_number number(value);
    return set_json_number(&number);
}

json& json::set_json_number(json_value* value) {
    json_number* number = dynamic_cast<json_number*>(value);
    if (_index) {
        auto vec = As<json_array>(_data);
        auto old = vec->GetAt(*_index);
        if (!old || old->type() != value_type::kNumber) {
            old = New<json_number>(*number);
            vec->SetAt(*_index, old);
        } else {
            auto obj = As<json_number>(old);
            obj->operator= (*number);
        }
        return *this;
    }
    if (_key) {
        auto map = As<json_object>(_data);
        auto old = map->get_value(*_key);
        if (!old || old->type() != value_type::kNumber) {
            old = New<json_number>(*number);
            map->set_value(*_key, old);
        } else {
            auto obj = As<json_number>(old);
            obj->operator= (*number);
        }
        return *this;
    }
    if (!_data || _data->type() != value_type::kNumber) {
        _data = New<json_number>(*number);
    } else {
        auto obj = As<json_number>(_data);
        obj->operator= (*number);
    }
    return *this;
}

json& json::assign(bool v) {
    if (_index) {
        auto vec = As<json_array>(_data);
        auto old = vec->GetAt(*_index);
        if (!old || old->type() != value_type::kBoolean) {
            old = New<json_boolean>(v);
            vec->SetAt(*_index, old);
        } else {
            auto obj = As<json_boolean>(old);
            obj ->operator= (v);
        }
        return *this;
    }
    if (_key) {
        auto map = As<json_object>(_data);
        auto old = map->get_value(*_key);
        if (!old || old->type() != value_type::kBoolean) {
            old = New<json_boolean>(v);
            map->set_value(*_key, old);
        } else {
            auto obj = As<json_boolean>(old);
            obj ->operator= (v);
        }
        return *this;
    }
    if (!_data || _data->type() != value_type::kBoolean) {
        _data = New<json_boolean>(v);
    } else {
        auto obj = As<json_boolean>(_data);
        obj ->operator= (v);
    }
    return *this;
}

void json::push_back(json j) {
    if (_depth == 0) {
        if (!_data || _data->type() == value_type::kNull) {
            auto obj = New<json_array>();
            obj->append(j.current_value());
            _data = obj;
            return;
        } else if (_data->type() == value_type::kArray) {
            auto obj = As<json_array>(_data);
            obj->append(j.current_value());
            return;
        }
    } else {
        if (_index) {
            auto obj = As<json_array>(_data);
            auto real = obj->GetAt(*_index);
            if (!real) {
                auto vec = New<json_array>();
                obj->SetAt(*_index, vec);
                vec->append(j.current_value());
            } else {
                auto vec = As<json_array>(real);
                vec->append(j.current_value());
            }
            return;
        }

        if (_key) {
            auto obj = As<json_object>(_data);
            auto real = obj->get_value(*_key);
            if (!real) {
                auto vec = New<json_array>();
                obj->set_value(*_key, vec);
                vec->append(j.current_value());
            } else {
                auto vec = As<json_array>(real);
                vec->append(j.current_value());
            }
            return;
        }
        value_type type = get_type();
        if (type == value_type::kNull) {
            auto obj = New<json_array>();
            _data = obj;
            obj->append(j._data);
            return;
        }
        if (type == value_type::kArray) {
            auto obj = As<json_array>(_data);
            obj->append(j._data);
            return;
        }
    }

    THROW_TYPE_ERROR("cannot use push_back() with " + std::string(current_type()));
}

const char* json::type_name(value_type ty) {
    static const char* type_str[] = {
        NULL_STR, "boolean", "number", "string", "array", "object"
    };
    return type_str[static_cast<int>(ty)];
}

value_type json::get_type() const {
    auto obj = current_value();
    if (!obj) {
        return value_type::kNull;
    }
    return obj->type();
}

std::shared_ptr<json_value> json::current_value() const {
    if (_depth == 0) {
        return _data;
    }
    if (_index) {
        auto obj = As<json_array>(_data);
        return obj->GetAt(*_index);
    }
    if (_key) {
        auto obj = As<json_object>(_data);
        return obj->get_value(*_key);
    }
    THROW_OTHER_ERROR("cann't get current value, depth = " + std::to_string(_depth));
}

const char* json::current_type() const {
    return type_name(get_type());
}

std::string json::to_string() const {
    auto obj = current_value();
    if (!obj || obj->type() != value_type::kString) {
        THROW_TYPE_ERROR("type must be string, but is " + std::string(current_type()));
    }
    return *As<json_string>(obj);
}

bool json::to_bool() const {
    auto obj = current_value();
    if (!obj || obj->type() != value_type::kBoolean) {
        THROW_TYPE_ERROR("type must be boolean, but is " + std::string(current_type()));
    }
    return *As<json_boolean>(obj);
}

uint64_t json::to_uint64() const {
    auto obj = current_value();
    if (!obj || obj->type() != value_type::kNumber) {
        THROW_TYPE_ERROR("type must be number, but is " + std::string(current_type()));
    }
    return *As<json_number>(obj);
}

int64_t json::to_int64() const {
    auto obj = current_value();
    if (!obj || obj->type() != value_type::kNumber) {
        THROW_TYPE_ERROR("type must be number, but is " + std::string(current_type()));
    }
    return *As<json_number>(obj);
}

double json::to_double() const {
    auto obj = current_value();
    if (!obj || obj->type() != value_type::kNumber) {
        THROW_TYPE_ERROR("type must be number, but is " + std::string(current_type()));
    }
    return *As<json_number>(obj);
}

void json::fill_current_value(std::shared_ptr<json_value> obj) {
    if (_depth == 0) {
        _data = obj;
        return;
    }

    if (_index) {
        auto vec = As<json_array>(_data);
        vec->SetAt(*_index, obj);
        return;
    }
    if (_key) {
        auto map = As<json_object>(_data);
        map->set_value(*_key, obj);
        return;
    }
    THROW_OTHER_ERROR("cann't fill current value, depth = " + std::to_string(_depth));
}

json_iterator json::begin() {
    json_iterator it;
    auto obj = current_value();
    if (obj) {
        if (obj->type() == value_type::kArray) {
            auto vec = As<json_array>(obj);
            if (vec->empty()) {
                return it;
            }
            it._iter = vec->begin();
            it._js_obj = New<json>();
            it._js_obj->_data = obj;
            it._js_obj->_depth = 1;
            it._js_obj->_index = New<size_t>(0);
        } else {
            it._js_obj = New<json>();
            it._js_obj->_data = obj;
        }
        it._value = obj;
    }
    return it;
}

json_iterator json::end() {
    return json_iterator();
}

// ---------------------------  json_iterator members  ---------------------------------

json_iterator::json_iterator() {}
json_iterator::~json_iterator() {}

json_iterator::json_iterator(const json_iterator& iter)
    : _js_obj(iter._js_obj)
    , _value(iter._value)
    , _iter(iter._iter) {}

json_iterator& json_iterator::operator= (const json_iterator& iter) {
    if (this != &iter) {
        _js_obj = iter._js_obj;
        _value = iter._value;
        _iter = iter._iter;
    }
    return *this;
}

json& json_iterator::operator*() {
    if (_js_obj) {
        return *_js_obj;
    }
    THROW_INVALID_INTERATOR("cannot use * operator, because already reach the end");
}

json* json_iterator::operator->() {
    if (_js_obj) {
        return _js_obj.get();
    }
    THROW_INVALID_INTERATOR("cannot use -> operator, because already reach the end");
}

bool json_iterator::equal(const json_iterator& rhs) const {
    if (this == &rhs) {
        return true;
    }
    if (!_js_obj && !rhs._js_obj) {
        return true;
    }
    if ((!_js_obj && rhs._js_obj) || (_js_obj && !rhs._js_obj)) {
        return false;
    }
    if (_value->type() != rhs._value->type()) {
        return false;
    }
    if (_iter == rhs._iter) {
        return true;
    }
    return false;
}

void json_iterator::increment() {
    if (!_js_obj || !_value) {
        THROW_INVALID_INTERATOR("cannot use increment for an invalid iterator");
    }
    if (_value->type() == value_type::kArray) {
        auto obj = As<json_array>(_value);
        if (++_iter != obj->end()) {
            (*_js_obj->_index) += 1;
            return;
        }
    }
    // reach the end
    _js_obj.reset();
    _value.reset();
}
}  // namespace karl
