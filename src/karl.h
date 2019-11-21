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
#include <unordered_map>
#include "karl/json.hxx"

namespace karl {

// ---------------------------------------------------------------------------------

template <typename T, typename... Args>
inline std::shared_ptr<T> New(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

class json_value {
public:
    virtual ~json_value() = default;
    virtual value_type type() const = 0;
    virtual std::string dump() const = 0;
    virtual std::string dump(int indent, int prefix) const = 0;
    virtual bool empty() const = 0;
    virtual std::shared_ptr<json_value> copy() const = 0;
    virtual std::vector<uint8_t> to_cbor() const = 0;
};

class json_null : public json_value {
public:
    json_null() {}
    ~json_null() {}

    value_type type() const override;
    std::string dump() const override;
    std::string dump(int, int) const override;
    bool empty() const override;
    std::string value() const;
    std::shared_ptr<json_value> copy() const override;
    std::vector<uint8_t> to_cbor() const override;
};

class json_boolean : public json_value {
public:
    explicit json_boolean(bool value) : _value(value) {}
    json_boolean() {}
    ~json_boolean() {}

    value_type type() const override;
    inline bool value() const { return _value; }
    operator bool() const { return _value; }
    std::string dump() const override;
    std::string dump(int, int) const override;
    json_boolean& operator= (bool value);
    bool empty() const override;
    std::shared_ptr<json_value> copy() const override;
    std::vector<uint8_t> to_cbor() const override;

private:
    bool _value;
};

class json_number : public json_value {
public:
    enum class number_type {
        kUnsigned, kSigned, kFloat
    };

    static std::shared_ptr<json_value> create(double value);
    json_number() : _value_type(number_type::kUnsigned), _value() {}
    json_number(int8_t value);
    json_number(int16_t value);
    json_number(int32_t value);
    json_number(int64_t value);
    json_number(uint8_t value);
    json_number(uint16_t value);
    json_number(uint32_t value);
    json_number(uint64_t value);
    json_number(float value);
    json_number(double value);
    json_number(const json_number&);
    json_number& operator= (const json_number&);

    json_number& operator= (int8_t);
    json_number& operator= (int16_t);
    json_number& operator= (int32_t);
    json_number& operator= (int64_t);
    json_number& operator= (uint8_t);
    json_number& operator= (uint16_t);
    json_number& operator= (uint32_t);
    json_number& operator= (uint64_t);
    json_number& operator= (float);
    json_number& operator= (double);

    operator int8_t() const;
    operator int16_t() const;
    operator int32_t() const;
    operator int64_t() const;
    operator uint8_t() const;
    operator uint16_t() const;
    operator uint32_t() const;
    operator uint64_t() const;
    operator double() const;
    operator float() const;

    bool is_float() const ;
    bool is_unsigned() const;
    bool is_signed() const;

    value_type type() const override { return value_type::kNumber; }
    std::string dump() const override;
    std::string dump(int, int) const override;
    bool empty() const override { return false; }
    void clear() { _value.clear(); }
    std::shared_ptr<json_value> copy() const override;
    std::vector<uint8_t> to_cbor() const override;
    void set_value(int64_t value);
    void set_value(uint64_t value);
    void set_value(double value);

private:
    number_type _value_type;
    struct number_check {
        union {
            int32_t i32;
            int64_t i64;
            uint32_t u32;
            uint64_t u64;
        };
        double ddd;
        number_check() : ddd(0), u64(0) {}
        void clear() { ddd = 0; u64 = 0; }
    } _value;
};

class json_string : public json_value {
public:
    json_string() {}
    json_string(const char* value);
    json_string(const std::string& value);
    value_type type() const override { return value_type::kString; }
    const std::string& value() const { return _value; }
    operator std::string() const { return _value; }
    json_string& operator= (const std::string& value);
    std::string dump() const override;
    std::string dump(int, int) const override;
    void clear() { _value.clear(); }
    bool empty() const override {
        return _value.empty();
    }
    std::shared_ptr<json_value> copy() const override;
    std::vector<uint8_t> to_cbor() const override;

private:
    std::string _value;
};

class json_array : public json_value {
public:
    json_array() {}

    std::shared_ptr<json_value> GetAt(size_t i);
    bool SetAt(size_t i, std::shared_ptr<json_value> element);

    std::shared_ptr<json_value>& operator[] (size_t i);
    const std::shared_ptr<json_value>& operator[] (size_t i) const;

    bool insert(size_t i, std::shared_ptr<json_value> element);
    void append(std::shared_ptr<json_value> element);
    void resize(size_t size);
    void clear();
    size_t size() const;
    void erase(size_t index);

    array_iterator begin();
    array_iterator end();

    value_type type() const override { return value_type::kArray; }
    std::string dump() const override;
    std::string dump(int, int) const override;
    bool empty() const override {
        return _seq.empty();
    }
    std::shared_ptr<json_value> copy() const override;
    std::vector<uint8_t> to_cbor() const override;

private:
    sequence _seq;
};

class json_object : public json_value {
public:
    using object = std::unordered_map<std::string, std::shared_ptr<json_value>>;
    using iterator = object::iterator;

    json_object() = default;
    bool has_key(const std::string& key) const;
    std::shared_ptr<json_value> get_value(const std::string& key) const;
    void set_value(const std::string& key, std::shared_ptr<json_value> element);
    void clear();

    iterator begin();
    iterator end();
    void erase(const std::string& key);

    size_t size() const;

    value_type type() const override { return value_type::kObject; }
    std::string dump() const override;
    std::string dump(int indent, int) const override;
    bool empty() const override {
        return _map.empty();
    }
    std::shared_ptr<json_value> copy() const override;
    std::vector<uint8_t> to_cbor() const override;

private:
    object _map;
};
}  // namespace karl
