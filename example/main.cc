#include <iostream>
#include "karl/json.hxx"
using Json = karl::json;

void test_json_object_parse() {
    char json[] = "{\"one\":true, \"number\":[1, 2, 3, 4]}";
    std::cout << "test_json_parse => " << std::endl;
    Json js = Json::parse(json);
    std::cout << "json: " << json << std::endl;
    std::cout << "dump: " << js.dump() << std::endl;

    if (js["one"].get<bool>() && js["number"].size() == 4 &&
        js["number"][2].get<int>() == 3) {
        std::cout << "test_json_parse success" << std::endl;
    } else {
        std::cout << "test_json_parse failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_array_parse() {
    std::cout << "test_json_array_parse => " << std::endl;
    std::string s = "[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]";
    Json j = Json::parse(s);
    std::string j_dump = j.dump();
    std::cout << j_dump << std::endl;
    if (j[0]["one"].get<bool>() && j[0]["two"].get<int>() == 2 &&
        j[0]["three"].get<std::string>() == "hello world") {
        std::cout << "test_json_array_parse success" << std::endl;
    } else {
        std::cout << "test_json_array_parse failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_long_string() {
    char buff[] = "{\r\n\t\"status\":\"success!\",\r\n\t\"data\":{\r\n\t\t\"class\":\"bank\",\r\n\t\t\"tag\":\"\",\r\n\t\t\"platform\":\"ALL\",\r\n\t\t\"session\":\"fee89343d998407a0dbc7e9b2ca16abf\",\r\n\t\t\"user_detail\":{\r\n\t\t\t\"push_type\":\"personal\",\r\n\t\t\t\"users\":{\r\n\t\t\t\t\"phone_nums\":[],\r\n\t\t\t\t\"registers\":{\r\n\t\t\t\t\t\"xiaomi\":[],\r\n\t\t\t\t\t\"huawei\":[],\r\n\t\t\t\t\t\"oppo\":[{\"regId\":\"CN_5f0921dd45c766e76ba3af2fb006394f\",\"mobilestyle\":\"android_V4.0.3_OPPO_OPPO A79kt\"}],\r\n\t\t\t\t\t\"vivo\":[{\"regId\":\"15693019830451459242007\",\"mobilestyle\":\"android_V4.0.2_vivo_V1818A\"}]\r\n\t\t\t\t}\r\n\t\t\t}\r\n\t\t},\r\n\t\t\"android_data\":{\r\n\t\t\t\"title\":\"2 days\",\r\n\t\t\t\"content\":\"click>>\",\r\n\t\t\t\"data_type\":\"detail\",\r\n\t\t\t\"id\":4188\r\n\t\t},\r\n\t\t\"ios_data\":{\r\n\t\t\t\"title\":\"2 days\",\r\n\t\t\t\"content\":\"click>>\",\r\n\t\t\t\"data_type\":\"detail\",\r\n\t\t\t\"id\":4188\r\n\t\t}\r\n\t}\r\n}\r\n";
    std::cout << "test_json_long_string => " << std::endl;
    std::cout << "json: " << buff << std::endl;
    Json js = Json::parse(buff);
    std::string js_dump = js.dump(2);
    std::cout << js_dump << std::endl;
    std::cout << "test_json_long_string success" << std::endl;
    std::cout << " ---------------- " << std::endl;
}

void test_json_array() {
    std::cout << "test_json_array => " << std::endl;

    Json j;
    j[1] = 2;
    j[2] = "ss";
    j[0] = 1;
    auto str = j.dump();
    std::cout << "dump: " << str << std::endl;
    if (str != "[1,2,\"ss\"]") {
        std::cout << "test_json_array failed" << std::endl;
    } else {
        std::cout << "test_json_array success" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_object() {
    std::cout << "test_json_object => " << std::endl;

    Json j;
    j["one"] = false;
    j["two"] = 2;
    j["three"] = "hello world";

    auto str = j.dump();
    std::cout << "dump: " << str << std::endl;

    if (j["one"].get<bool>() == false && j["two"].get<int>() == 2 &&
        j["three"].get<std::string>() == "hello world") {
        std::cout << "test_json_object success" << std::endl;
    } else {
        std::cout << "test_json_object failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_arry_push_back() {
    std::cout << "test_json_arry_push_back => " << std::endl;

    Json j;
    j["one"] = true;
    j["two"] = 2;
    j["three"] = "hello world";
    Json jj;
    std::cout << "before push_back: " << j.dump() << std::endl;
    jj.push_back(j);
    auto str = jj.dump();
    std::cout << "after push_back: " << str << std::endl;

    if (jj[0]["one"].get<bool>()) {
        if (jj[0]["two"].get<int>() == 2) {
            if (jj[0]["three"].get<std::string>() == "hello world") {
                std::cout << "test_json_arry_push_back success" << std::endl;
                std::cout << " ---------------- " << std::endl;
                return;
            }
        }
    }
    std::cout << "test_json_arry_push_back failed" << std::endl;
    std::cout << " ---------------- " << std::endl;
}

void test_json_get_func() {
    std::cout << "test_json_get_func => " << std::endl;

    Json j;
    j["one"] = true;
    j["two"] = 2.0f;
    j["three"] = "hello world";

    std::cout << "before get: " << j.dump() << std::endl;

    bool one = j["one"].get<bool>();
    int two = j["two"].get<int>();
    std::string three = j["three"].get<std::string>();

    std::cout << "after get: " << std::endl
        << "one: " << one
        << ", two: " << two
        << ", three: " << three
        << std::endl;

    if (one && two == 2 && three == "hello world") {
        std::cout << "test_json_get_func success" << std::endl;
    } else {
        std::cout << "test_json_get_func failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_multi_layer_get() {
    std::cout << "test_json_multi_layer_get => " << std::endl;
    Json test = Json::parse("{\"test\": [{\"one\":true,\"two\":2,\"three\":\"hello world\"}]}");
    Json j = test["test"];
    std::cout << "dump: " << j.dump() << std::endl;

    bool one = j[0]["one"].get<bool>();
    int two = j[0]["two"].get<int>();
    std::string three = j[0]["three"].get<std::string>();

    std::cout << "after get: " << std::endl
        << "one: " << one
        << ", two: " << two
        << ", three: " << three
        << std::endl;

    if (one && two == 2 && three == "hello world") {
        std::cout << "test_json_multi_layer_get success" << std::endl;
    } else {
        std::cout << "test_json_multi_layer_get failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_obj_assignment() {
    std::cout << "test_json_obj_assignment => " << std::endl;
    Json t;
    t["one"] = 2;
    t["w"] = "ok";
    Json j;
    j["test"] = t;

    for (const auto i : j) {
        std::cout << i.dump() << std::endl;
    }

    std::string s = j.dump();
    std::cout << "dump: " << s << std::endl;

    if (!j["x"].is_null()) {
        std::cout << "test_json_obj_assignment failed" << std::endl;
        std::cout << " ---------------- " << std::endl;
        return;
    }
    if (j["test"]["one"].get() == 2 &&
        j["test"]["w"].get<std::string>() == "ok") {
        std::cout << "test_json_obj_assignment success" << std::endl;
    } else {
        std::cout << "test_json_obj_assignment failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_multi_layer_assignment() {
    std::cout << "test_json_multi_layer_assignment => " << std::endl;
    Json t;
    t[1]["one"] = 2;
    std::string s = t.dump();
    std::cout << "dump: " << s << std::endl;
    if (s == "[null,{\"one\":2}]") {
        std::cout << "test_json_multi_layer_assignment success" << std::endl;
    } else {
        std::cout << "test_json_multi_layer_assignment failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_const_iterator() {
    std::cout << "test_json_const_iterator => " << std::endl;
    std::string s1;
    std::string s2;
    Json j;
    for (int i = 0; i < 3; i++) {
        Json jj;
        jj["peer_id"] = i;
        jj["ms"] = std::to_string(i + 10);
        j.push_back(jj);
        if (i == 0) {
            s1 = jj.dump();
        }
    }
    s2 = j.dump();
    std::cout << s2 << std::endl;
    Json mj;
    mj.push_back(j);
    mj.push_back(j);
    mj.push_back(j);
    std::cout << mj.dump() << std::endl;

    bool ret = false;
    int i = 0;

    for (const auto k : mj) {
        Json t = k[0];
        std::string td = t.dump();
        std::string kd = k.dump();
        std::cout << td << std::endl;;
        std::cout << kd << std::endl;;

        if (td != s1 || kd != s2) {
            goto __exit;
        }
    }

    for (const auto m : j) {
        int peer_id = m["peer_id"].get<int>();
        std::string ms = m["ms"].get<std::string>();
        std::cout << "peer_id: " << peer_id
            << ", ms: " << ms << std::endl;
        if (peer_id != i || ms != std::to_string(i + 10)) {
            goto __exit;
        }
        i++;
    }
    ret = true;
__exit:
    if (ret) {
        std::cout << "test_json_const_iterator success" << std::endl;
    } else {
        std::cout << "test_json_const_iterator failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}


void test_json_reference_iterator() {
    std::cout << "test_json_reference_iterator => " << std::endl;

    Json j;
    for (int i = 0; i < 3; i++) {
        Json jj;
        jj["peer_id"] = i;
        jj["ms"] = std::to_string(i + 10);
        j.push_back(jj);
    }

    int x = 101;
    for (auto& k : j) {
        k["peer_id"] = x++;
        k["test"] = false;
        k["ms"] = "Helloworld";
    }

    bool ret = false;
    int y = 101;
    for (auto it = j.begin(); it != j.end(); ++it) {
        Json k = *it;
        int peer_id = k["peer_id"].get<int>();
        std::string ms = k["ms"].get<std::string>();
        bool test = k["test"].get<bool>();
        std::cout << "peer_id: " << peer_id
            << ", ms: " << ms
            << ", test: " << test << std::endl;

        if (peer_id != y ||
            test || ms != std::string("Helloworld")) {
            goto __exit;
        }
        y++;
    }
    ret = true;
__exit:
    if (ret) {
        std::cout << "test_json_reference_iterator success" << std::endl;
    } else {
        std::cout << "test_json_reference_iterator failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_initializer_list_construct() {
    std::cout << "test_json_initializer_list_construct => " << std::endl;
    Json j = { { "one", nullptr },{ "two", true },{ "three", 10086 },{ "str", "World" } };
    std::string j_dump = j.dump();
    std::cout << j_dump << std::endl;

    if (j["one"].is_null() &&
        j["two"].get<bool>() &&
        j["three"].get() == 10086 &&
        j["str"].get<std::string>() == "World") {
        std::cout << "test_json_initializer_list_construct success" << std::endl;
    } else {
        std::cout << "test_json_initializer_list_construct failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_initializer_list_vector_construct() {
    std::cout << "test_json_initializer_list_vector_construct => " << std::endl;
    std::vector<int> vec{ 1,2,3,4,5 };
    Json j = vec;
    std::string j_dump = j.dump();
    std::cout << j_dump << std::endl;

    if (j.is_array() && j[2].get<int>() == 3) {
        std::cout << "test_json_initializer_list_vector_construct success" << std::endl;
    } else {
        std::cout << "test_json_initializer_list_vector_construct failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}


void test_json_object_deep_copy() {
    std::cout << "test_json_object_deep_copy => " << std::endl;
    char str[] = "[1, null, 2, 3, 4]";
    Json js = Json::parse(str);
    Json bak = js.copy();

    js[1] = 5;
    std::cout << js.dump() << std::endl;
    std::cout << bak.dump() << std::endl;
    if (js[1].get<int>() == 5 && bak[1].is_null()) {
        std::cout << "test_json_object_deep_copy success" << std::endl;
    } else {
        std::cout << "test_json_object_deep_copy failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_cbor_encode_and_decode() {
    char buff[] = "{\r\n\t\"status\":\"success!\",\r\n\t\"data\":{\r\n\t\t\"class\":\"bank\",\r\n\t\t\"tag\":\"\",\r\n\t\t\"platform\":\"ALL\",\r\n\t\t\"session\":\"fee89343d998407a0dbc7e9b2ca16abf\",\r\n\t\t\"user_detail\":{\r\n\t\t\t\"push_type\":\"personal\",\r\n\t\t\t\"users\":{\r\n\t\t\t\t\"phone_nums\":[],\r\n\t\t\t\t\"registers\":{\r\n\t\t\t\t\t\"xiaomi\":[],\r\n\t\t\t\t\t\"huawei\":[],\r\n\t\t\t\t\t\"oppo\":[{\"regId\":\"CN_5f0921dd45c766e76ba3af2fb006394f\",\"mobilestyle\":\"android_V4.0.3_OPPO_OPPO A79kt\"}],\r\n\t\t\t\t\t\"vivo\":[{\"regId\":\"15693019830451459242007\",\"mobilestyle\":\"android_V4.0.2_vivo_V1818A\"}]\r\n\t\t\t\t}\r\n\t\t\t}\r\n\t\t},\r\n\t\t\"android_data\":{\r\n\t\t\t\"title\":\"2 days\",\r\n\t\t\t\"content\":\"click>>\",\r\n\t\t\t\"data_type\":\"detail\",\r\n\t\t\t\"id\":4188\r\n\t\t},\r\n\t\t\"ios_data\":{\r\n\t\t\t\"title\":\"2 days\",\r\n\t\t\t\"content\":\"click>>\",\r\n\t\t\t\"data_type\":\"detail\",\r\n\t\t\t\"id\":4188\r\n\t\t}\r\n\t}\r\n}\r\n";
    std::cout << "test_json_cbor_encode_and_decode => " << std::endl;

    Json js = Json::parse(buff);
    std::vector<uint8_t> cbor = js.to_cbor();
    Json njs = Json::from_cbor(cbor);

    std::string status1 = js["status"].get<std::string>();
    std::string status2 = njs["status"].get<std::string>();
    std::string data1 = js["data"]["class"].get<std::string>();
    std::string data2 = njs["data"]["class"].get<std::string>();

    if (status1 == status2 && data1 == data2) {
        std::cout << "test_json_cbor_encode_and_decode success" << std::endl;
    } else {
        std::cout << njs.dump(2) << std::endl;
        std::cout << "test_json_cbor_encode_and_decode failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

void test_json_parse_nlohmann_cbor_data() {
    // data from nlohmann::cbor
    unsigned char data[471] = {
        0xA2, 0x64, 0x64, 0x61, 0x74, 0x61, 0xA7, 0x6C, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x5F,
        0x64, 0x61, 0x74, 0x61, 0xA4, 0x67, 0x63, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x67, 0x63, 0x6C,
        0x69, 0x63, 0x6B, 0x3E, 0x3E, 0x69, 0x64, 0x61, 0x74, 0x61, 0x5F, 0x74, 0x79, 0x70, 0x65, 0x66,
        0x64, 0x65, 0x74, 0x61, 0x69, 0x6C, 0x62, 0x69, 0x64, 0x19, 0x10, 0x5C, 0x65, 0x74, 0x69, 0x74,
        0x6C, 0x65, 0x66, 0x32, 0x20, 0x64, 0x61, 0x79, 0x73, 0x65, 0x63, 0x6C, 0x61, 0x73, 0x73, 0x64,
        0x62, 0x61, 0x6E, 0x6B, 0x68, 0x69, 0x6F, 0x73, 0x5F, 0x64, 0x61, 0x74, 0x61, 0xA4, 0x67, 0x63,
        0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x67, 0x63, 0x6C, 0x69, 0x63, 0x6B, 0x3E, 0x3E, 0x69, 0x64,
        0x61, 0x74, 0x61, 0x5F, 0x74, 0x79, 0x70, 0x65, 0x66, 0x64, 0x65, 0x74, 0x61, 0x69, 0x6C, 0x62,
        0x69, 0x64, 0x19, 0x10, 0x5C, 0x65, 0x74, 0x69, 0x74, 0x6C, 0x65, 0x66, 0x32, 0x20, 0x64, 0x61,
        0x79, 0x73, 0x68, 0x70, 0x6C, 0x61, 0x74, 0x66, 0x6F, 0x72, 0x6D, 0x63, 0x41, 0x4C, 0x4C, 0x67,
        0x73, 0x65, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x78, 0x20, 0x66, 0x65, 0x65, 0x38, 0x39, 0x33, 0x34,
        0x33, 0x64, 0x39, 0x39, 0x38, 0x34, 0x30, 0x37, 0x61, 0x30, 0x64, 0x62, 0x63, 0x37, 0x65, 0x39,
        0x62, 0x32, 0x63, 0x61, 0x31, 0x36, 0x61, 0x62, 0x66, 0x63, 0x74, 0x61, 0x67, 0x60, 0x6B, 0x75,
        0x73, 0x65, 0x72, 0x5F, 0x64, 0x65, 0x74, 0x61, 0x69, 0x6C, 0xA2, 0x69, 0x70, 0x75, 0x73, 0x68,
        0x5F, 0x74, 0x79, 0x70, 0x65, 0x68, 0x70, 0x65, 0x72, 0x73, 0x6F, 0x6E, 0x61, 0x6C, 0x65, 0x75,
        0x73, 0x65, 0x72, 0x73, 0xA2, 0x6A, 0x70, 0x68, 0x6F, 0x6E, 0x65, 0x5F, 0x6E, 0x75, 0x6D, 0x73,
        0x80, 0x69, 0x72, 0x65, 0x67, 0x69, 0x73, 0x74, 0x65, 0x72, 0x73, 0xA4, 0x66, 0x68, 0x75, 0x61,
        0x77, 0x65, 0x69, 0x80, 0x64, 0x6F, 0x70, 0x70, 0x6F, 0x81, 0xA2, 0x6B, 0x6D, 0x6F, 0x62, 0x69,
        0x6C, 0x65, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x78, 0x1E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64,
        0x5F, 0x56, 0x34, 0x2E, 0x30, 0x2E, 0x33, 0x5F, 0x4F, 0x50, 0x50, 0x4F, 0x5F, 0x4F, 0x50, 0x50,
        0x4F, 0x20, 0x41, 0x37, 0x39, 0x6B, 0x74, 0x65, 0x72, 0x65, 0x67, 0x49, 0x64, 0x78, 0x23, 0x43,
        0x4E, 0x5F, 0x35, 0x66, 0x30, 0x39, 0x32, 0x31, 0x64, 0x64, 0x34, 0x35, 0x63, 0x37, 0x36, 0x36,
        0x65, 0x37, 0x36, 0x62, 0x61, 0x33, 0x61, 0x66, 0x32, 0x66, 0x62, 0x30, 0x30, 0x36, 0x33, 0x39,
        0x34, 0x66, 0x64, 0x76, 0x69, 0x76, 0x6F, 0x81, 0xA2, 0x6B, 0x6D, 0x6F, 0x62, 0x69, 0x6C, 0x65,
        0x73, 0x74, 0x79, 0x6C, 0x65, 0x78, 0x1A, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x5F, 0x56,
        0x34, 0x2E, 0x30, 0x2E, 0x32, 0x5F, 0x76, 0x69, 0x76, 0x6F, 0x5F, 0x56, 0x31, 0x38, 0x31, 0x38,
        0x41, 0x65, 0x72, 0x65, 0x67, 0x49, 0x64, 0x77, 0x31, 0x35, 0x36, 0x39, 0x33, 0x30, 0x31, 0x39,
        0x38, 0x33, 0x30, 0x34, 0x35, 0x31, 0x34, 0x35, 0x39, 0x32, 0x34, 0x32, 0x30, 0x30, 0x37, 0x66,
        0x78, 0x69, 0x61, 0x6F, 0x6D, 0x69, 0x80, 0x66, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x68, 0x73,
        0x75, 0x63, 0x63, 0x65, 0x73, 0x73, 0x21
    };
    std::cout << "test_json_parse_nlohmann_cbor_data => " << std::endl;
    size_t transfer_bytes = 0;
    std::vector<uint8_t> obj(data, data + sizeof(data));
    Json jj = Json::from_cbor(obj, &transfer_bytes);
    std::cout << jj.dump(2) << std::endl;
    if (transfer_bytes == sizeof(data)) {
        std::cout << "test_json_parse_nlohmann_cbor_data success" << std::endl;
    } else {
        std::cout << "test_json_parse_nlohmann_cbor_data failed" << std::endl;
    }
    std::cout << " ---------------- " << std::endl;
}

int main(int argc, char* argv[]) {
    test_json_object_parse();
    test_json_array_parse();
    test_json_long_string();
    test_json_array();
    test_json_object();
    test_json_arry_push_back();
    test_json_get_func();
    test_json_multi_layer_get();
    test_json_obj_assignment();
    test_json_multi_layer_assignment();
    test_json_const_iterator();
    test_json_reference_iterator();
    test_json_initializer_list_construct();
    test_json_initializer_list_vector_construct();
    test_json_object_deep_copy();
    test_json_cbor_encode_and_decode();
    test_json_parse_nlohmann_cbor_data();
    getchar();
    return 0;
}
