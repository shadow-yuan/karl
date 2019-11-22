# [karl](https://github.com/shadow-yuan/karl)

[![Build status](https://ci.appveyor.com/api/projects/status/85wbmr7o9cxcnq9q?svg=true)](https://ci.appveyor.com/project/shadow-yuan/karl)  [![Coverity Scan Build Status](https://scan.coverity.com/projects/19884/badge.svg)](https://scan.coverity.com/projects/shadow-yuan-karl)  [![Codacy Badge](https://api.codacy.com/project/badge/Grade/1a937e18634342d1a044d02e704c408c)](https://www.codacy.com/manual/shadow-yuan/karl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=shadow-yuan/karl&amp;utm_campaign=Badge_Grade)  [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/shadow-yuan/karl.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/shadow-yuan/karl/context:cpp)

[中文版](README_cn.md)

An easy to use c++ json library

## Usage

### Building

There are several ways to incorporate karl into your project.

#### CMake
It is recommended to use CMake for karl library build, which requires CMake version equal to or higher than 3.8. We need to put the CMake compiled file in a different directory than the source file, so create a `build` directory and run CMake inside it.

```c
  mkdir build
  cd build
  cmake ..
```

  If you are on a Unix platform, you can compile directly with the `make` command:

```c
  make
```

  If you are on the Windows platform, you will need to open the solution file `karl.sln` with `Visual Studio` and compile the karl. It is recommended to use `Visual Studio 2015` or `Visual Studio 2017`.

#### Makefile

If you installed `gcc, gcc-c++`, you can use `Makefile` to compile karl:

Run this command in the directory with the source code and it will automatically compile static library and an example program.

```c
  make all
```

#### copying the source

Just copy the `src/cJSON.c/h, src/cbor.cc/h, src/karl.cc/h` and `include` directory to your project and start using it.

### Including

Whether you use the karl source directly or the static library, you need to copy the `include` directory to a fixed directory (project directory, system directory or some other place), and then specify the header file directory in `Visual Studio` or `Makefile`.
You can include karl like this:

```c
  #include <karl/json.hxx>
  using Json = karl::json;
```

### Linking library
  If on a Windows platform, add in the project properties, you can also use the following code to link the specified library:
```c
  #pragma comment(lib, "karl.lib")
```
  If on a Unix platform. You Need to add the relevant code in `Makefile`, specify the location of the library with `-L`, and add `-lkarl`.

### Precautions

1.Unlike other json libraries that have objects such as number, string, array, or object, the kal library only provides an object type, the `json` class. It could be number, string, or arry or object, as the case may be.

2.Do not use references to json objects, because smart pointers are used inside the karl library to share the original object, and normal assignments can achieve reference effects.
  ```c
    Json& j = js["xxx"];    //  error.
    Json j = js["xxx"];     //  right, and the modification of j will affect js.
  ```

3.Need to generate a copy of the json object, please use the `json.copy` method.
  ```c
    Json j = js.copy();
  ```

4.An exception (`std::exception`) is thrown when the type of the acquired data is incorrect or the requested data does not exist. Please use `try catch` to catch exceptions to ensure the normal flow of the process.

### Example

The following is an example of general usage:

1.Parse the json string
  ```c
    char obj[] = "{\"one\":true, \"number\":[1, 2, 3, 4]}";
    Json js = Json::parse(obj);
    
    std::string s = "[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]";
    Json j = Json::parse(s);
  ```
  
2.Serialized to a std::string object
  ```c
    std::string s = "[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]";
    Json j = Json::parse(s);
    std::string str = j.dump();
  ```
  
3.Assignment build json array object
  ```c
    Json j;
    j[1] = 2;
    j[2] = "ss";
    j[0] = 1;
  ```
  
4.Assignment build json object object
  ```c
    Json j;
    j["one"] = false;
    j["two"] = 2;
    j["three"] = "hello world";
  ```
  
5.Json array object append element
  ```c
    Json j;
    j[1] = 2;
    j[0] = 1;
    Json jj;
    jj.push_back(j);
  ```
  
6.Json object gets the value
  ```c
    Json j;
    j["one"] = true;
    j["two"] = 2.0f;
    j["three"] = "hello world";
    bool one = j["one"].get<bool>();
    int two = j["two"].get<int>();
  ```
  
7.Json object multi-level gets the value
  ```c
    Json j = Json::parse("[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]");
    bool one = j[0]["one"].get<bool>();
    int two = j[0]["two"].get<int>();
  ```
  
8.Json object iteration
  ```c
    Json j = Json::parse("[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]");
    for (auto item : j) {
        std::cout << item.dump() << std::endl;
    }
  ```
  
9.Construct a json object with a key-value pair
  ```c
    Json j = { { "one", nullptr },{ "two", true },{ "three", 10086 },{ "str", "World" } };   
  ```
 
10.Construct a json object with a vector
  ```c
    std::vector<int> vec{ 1,2,3,4,5 };
    Json j = vec;
  ```

See `example/main.cc` for more detailed sample code.

### Other

During the development process, I made some choices about the details, but it does not affect the convenience of the use of the karl. There may also be undiscovered issues in the code. If you have any questions, please contact me (shadow_yuan@qq.com).
