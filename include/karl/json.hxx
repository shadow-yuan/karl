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

#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <exception>
#include <iostream>
#include <initializer_list>
#include <memory>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <vector>

namespace karl {

// ---------------------------  json exception  ---------------------------------
class exception : public std::exception {
public:
    const char* what() const noexcept override { return _e.what(); }
protected:
    exception(const char* message) : _e(message) {}
    static std::string name(const std::string& ename, const char* file, int line) {
        if (!file || line <= 0) {
            return "[json.exception." + ename +  "] ";
        }
#ifdef _WIN32
        const char* f = strrchr(file, '\\');
#else
        const char* f = strrchr(file, '/');
#endif
        if (!f) f = file;
        else f += 1;
        return "[json.exception." + ename + ":" + std::string(f) + " (" + std::to_string(line) + ")] ";
    }
private:
    std::runtime_error _e;
};

class invalid_iterator : public exception {
public:
    static invalid_iterator create(const std::string& message, const char* file = nullptr, int line = 0) {
        std::string w = exception::name("invalid_iterator", file, line) + message;
        return invalid_iterator(w.c_str());
    }

private:
    explicit invalid_iterator(const char* what_arg) : exception(what_arg) {}
};

class other_error : public exception {
public:
    static other_error create(const std::string& message, const char* file = nullptr, int line = 0) {
        std::string w = exception::name("other_error", file, line) + message;
        return other_error(w.c_str());
    }

private:
    explicit other_error(const char* what_arg) : exception(what_arg) {}
};

class out_of_range : public exception {
public:
    static out_of_range create(const std::string& message, const char* file = nullptr, int line = 0) {
        std::string w = exception::name("out_of_range", file, line) + message;
        return out_of_range(w.c_str());
    }

private:
    explicit out_of_range(const char* what_arg) : exception(what_arg) {}
};

class parse_error : public exception {
public:
    static parse_error create(const std::string& message, const char* file = nullptr, int line = 0) {
        std::string w = exception::name("parse_error", file, line) + message;
        return parse_error( w.c_str());
    }

private:
    explicit parse_error(const char* what_arg) : exception(what_arg) {}
};

class type_error : public exception {
public:
    static type_error create(const std::string& message, const char* file = nullptr, int line = 0) {
        std::string w = exception::name("type_error", file, line) + message;
        return type_error(w.c_str());
    }

private:
    explicit type_error(const char* what_arg) : exception(what_arg) {}
};

#define THROW_INVALID_INTERATOR(msg) \
    throw invalid_iterator::create((msg), __FILE__, __LINE__)

#define THROW_OTHER_ERROR(msg) \
    throw other_error::create((msg), __FILE__, __LINE__)

#define THROW_OUT_OF_RANGE(msg) \
    throw out_of_range::create((msg), __FILE__, __LINE__)

#define THROW_PARSE_ERROR(msg) \
    throw parse_error::create((msg), __FILE__, __LINE__)

#define THROW_TYPE_ERROR(msg) \
    throw type_error::create((msg), __FILE__, __LINE__)

// ---------------------------------------------------------------------------------

template <typename X, typename Y>
inline std::shared_ptr<X> As(const std::shared_ptr<Y>& ptr) {
    return std::dynamic_pointer_cast<X>(ptr);
}

template<class _Ty>
struct is_integer
    : std::false_type {};

template<>
struct is_integer<int8_t>
    : std::true_type {};

template<>
struct is_integer<uint8_t>
    : std::true_type {};

template<>
struct is_integer<int16_t>
    : std::true_type {};

template<>
struct is_integer<uint16_t>
    : std::true_type {};

template<>
struct is_integer<int32_t>
    : std::true_type {};

template<>
struct is_integer<uint32_t>
    : std::true_type {};

template<>
struct is_integer<int64_t>
    : std::true_type {};

template<>
struct is_integer<uint64_t>
    : std::true_type {};

template<>
struct is_integer<double>
    : std::false_type {};

template<>
struct is_integer<float>
    : std::false_type {};

template<>
struct is_integer<bool>
    : std::false_type {};

template<typename Ty>
struct check_integer_type
    : is_integer<typename std::remove_cv<Ty>::type> {};

// ---------------------------------------------------------------------------------

class json_value;
using sequence = std::vector<std::shared_ptr<json_value>>;
using array_iterator = sequence::iterator;

enum class value_type {
    kNull, kBoolean, kNumber, kString, kArray, kObject
};

// ---------------------------------------------------------------------------------

class key_value_pair final {
public:
    key_value_pair();
    ~key_value_pair();
    key_value_pair(const std::string& key, const char* value);
    key_value_pair(const std::string& key, const std::string& value);
    key_value_pair(const std::string& key, int8_t value);
    key_value_pair(const std::string& key, int16_t value);
    key_value_pair(const std::string& key, int32_t value);
    key_value_pair(const std::string& key, int64_t value);
    key_value_pair(const std::string& key, uint8_t value);
    key_value_pair(const std::string& key, uint16_t value);
    key_value_pair(const std::string& key, uint32_t value);
    key_value_pair(const std::string& key, uint64_t value);
    key_value_pair(const std::string& key, double value);
    key_value_pair(const std::string& key, float value);
    key_value_pair(const std::string& key, bool value);
    
    key_value_pair(const key_value_pair& oth);
    key_value_pair& operator= (const key_value_pair& rhs);

    key_value_pair(const std::string& key, const std::vector<int8_t>& value);
    key_value_pair(const std::string& key, const std::vector<int16_t>& value);
    key_value_pair(const std::string& key, const std::vector<int32_t>& value);
    key_value_pair(const std::string& key, const std::vector<int64_t>& value);

    key_value_pair(const std::string& key, const std::vector<uint8_t>& value);
    key_value_pair(const std::string& key, const std::vector<uint16_t>& value);
    key_value_pair(const std::string& key, const std::vector<uint32_t>& value);
    key_value_pair(const std::string& key, const std::vector<uint64_t>& value);

    key_value_pair(const std::string& key, const std::vector<std::string>& value);
    key_value_pair(const std::string& key, const std::vector<double>& value);
    key_value_pair(const std::string& key, const std::vector<bool>& value);

    const std::string& key() const;
    std::shared_ptr<json_value> value() const;

private:
    std::string _key;
    std::shared_ptr<json_value> _value;
};

// ---------------------------  json  ---------------------------------

class json_iterator;
class json final {
    friend json_iterator;
public:
    static json parse(const std::string& data);
    static json parse(const char* ptr, size_t size);
    static json parse(std::istream* stream);
    static json from_cbor(const uint8_t* ptr, size_t len, size_t* tranfer_bytes = nullptr);
    static json from_cbor(const std::vector<uint8_t>& bin, size_t* tranfer_bytes = nullptr);
    static std::vector<uint8_t> to_cbor(json js);
    static json array();
    static json object();
    static const char* type_name(value_type ty);

    json(std::shared_ptr<json_value> item);
    json();
    ~json() = default;
    json(const json& j);
    json& operator= (const json& j);

    json(std::initializer_list<key_value_pair> init);

    std::string dump(int indent = -1) const;
    value_type get_type() const;
    bool empty() const;
    json copy() const;
    std::vector<uint8_t> to_cbor() const;

    bool is_null() const;
    bool is_boolean() const;
    bool is_number() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;
    bool is_structured() const;
    bool has_key(const std::string& key) const;

    // remove the specified key-value pair from JsonObject
    void erase(const std::string& key);

    // remove the entry of the specified index from JsonArray
    void erase(size_t index);

    json operator[] (size_t index);
    json operator[] (const std::string& key);

    const json operator[] (size_t index) const;
    const json operator[] (const std::string& key) const;

    void clear();

    size_t size();

    void push_back(json j);
    inline void emplace_back(json&& j) {
        push_back(j);
    }

    inline json& operator= (int8_t v)   { return assign(static_cast<int64_t>(v)); }
    inline json& operator= (int16_t v)  { return assign(static_cast<int64_t>(v)); }
    inline json& operator= (int32_t v)  { return assign(static_cast<int64_t>(v)); }
    inline json& operator= (int64_t v)  { return assign(v); }
    inline json& operator= (uint8_t v)  { return assign(static_cast<uint64_t>(v)); }
    inline json& operator= (uint16_t v) { return assign(static_cast<uint64_t>(v)); }
    inline json& operator= (uint32_t v) { return assign(static_cast<uint64_t>(v)); }
    inline json& operator= (uint64_t v) { return assign(v); }
    inline json& operator= (float v)    { return assign(static_cast<double>(v)); }
    inline json& operator= (double v)   { return assign(v); }

    inline json& operator= (const char* ptr) {
        return this->assign(ptr ? std::string(ptr) : std::string());
    }
    inline json& operator= (const std::string& s) {
        return this->assign(s);
    }
    inline json& operator= (std::string&& s) {
        return this->assign(s);
    }
    inline json& operator= (bool v) {
        return this->assign(v);
    }

    template<typename T = int>
    inline T get() const {
        if (check_integer_type<T>::value) {
            return static_cast<T>(to_uint64());
        }
        THROW_TYPE_ERROR("The type of json::get should be integer or double, but is " +
            std::string(typeid(typename std::remove_cv<T>::type).name()));
    }

    // iterator
    json_iterator begin();
    json_iterator end();

    // construct from array
    template<typename Ty>
    json(const std::vector<Ty>& value) {
        if (check_integer_type<Ty>::value) {
            key_value_pair jkv("temp", value);
            _data = jkv.value();
            _depth = 0;
            return;
        }
        THROW_TYPE_ERROR("The type of json constructed by vector should be integer or double, but is " +
            std::string(typeid(typename std::remove_cv<Ty>::type).name()));
    }

private:
    std::shared_ptr<json_value> current_value() const;
    void fill_current_value(std::shared_ptr<json_value> obj);
    const char* current_type() const;

    std::string to_string() const;
    uint64_t to_uint64() const;
    double to_double() const;
    bool to_bool() const;

    json& assign(const std::string& s);
    json& assign(uint64_t v);
    json& assign(int64_t v);
    json& assign(double v);
    json& assign(bool v);
    json& set_json_number(json_value* number);

    int _depth;
    std::shared_ptr<json_value> _data;
    std::shared_ptr<size_t> _index;     // for array
    std::shared_ptr<std::string> _key;  // for object
};

template<>
inline std::string json::get<std::string>() const {
    return to_string();
}
template<>
inline bool json::get<bool>() const {
    return to_bool();
}
template<>
inline double json::get<double>() const {
    return to_double();
}
template<>
inline float json::get<float>() const {
    return static_cast<float>(to_double());
}

template<>
inline json::json(const std::vector<std::string>& value) {
    key_value_pair jkv("temp", value);
    _data = jkv.value();
    _depth = 0;
}

template<>
inline json::json(const std::vector<double>& value) {
    key_value_pair jkv("temp", value);
    _data = jkv.value();
    _depth = 0;
}

template<>
inline json::json(const std::vector<bool>& value) {
    key_value_pair jkv("temp", value);
    _data = jkv.value();
    _depth = 0;
}

// ------------ json iterator ------------

class json_iterator final {
    friend json;
public:
    json_iterator();
    json_iterator(const json_iterator& iter);
    json_iterator& operator= (const json_iterator& iter);
    ~json_iterator();

    bool operator == (const json_iterator& oth) {
        return equal(oth);
    }
    bool operator != (const json_iterator& oth) {
        return !equal(oth);
    }
    // ---- prefix ----
    json_iterator& operator++ () {
        increment();
        return *this;
    }
    // ---- suffix ----
    json_iterator operator++(int) {
        json_iterator tmp = *this;
        increment();
        return tmp;
    }
    json& operator* ();
    json* operator-> ();

private:
    bool equal(const json_iterator& rhs) const;
    void increment();
    std::shared_ptr<json> _js_obj;
    std::shared_ptr<json_value> _value;
    array_iterator _iter;
};
}  // namespace karl
