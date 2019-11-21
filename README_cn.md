# [karl](https://github.com/shadow-yuan/karl)

[![Build status](https://ci.appveyor.com/api/projects/status/85wbmr7o9cxcnq9q?svg=true)](https://ci.appveyor.com/project/shadow-yuan/karl)  [![Codacy Badge](https://api.codacy.com/project/badge/Grade/1a937e18634342d1a044d02e704c408c)](https://www.codacy.com/manual/shadow-yuan/karl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=shadow-yuan/karl&amp;utm_campaign=Badge_Grade)

[English version](README.md)

一个简单易用的 c++ json 库

## 使用方法

### 编译

有几种方法可以将 karl 添加到您的项目中。

#### CMake
推荐使用 CMake 进行 karl 库构建，需要 CMake 版本等于或高于 3.8。我们需要将 CMake 编译后的文件放在与源文件不同的目录中，因此，建立一个`build`目录并在其中运行 CMake。

```
  mkdir build
  cd build
  cmake ..
```

  如果你在 Unix 平台，接下来你可以直接使用`make`命令编译：

```
  make
```

  如果你在 Windows 平台，接下来需要使用`Visual Studio`打开解决方案文件`karl.sln`，然后编译 karl。推荐使用`Visual Studio 2015`或`Visual Studio 2017`。

#### Makefile

假如你安装了`gcc, gcc-c++`，你可以使用`Makefile`来编译 karl。

在源码目录运行如下命令编译 karl 静态库和一个示例程序：

```
  make all
```

#### 拷贝源码

只需要拷贝`src/cJSON.c/h, src/cbor.cc/h, src/karl.cc/h` 和 `include` 目录到你的项目即可使用。

### 头文件包含

不管是直接使用 karl 源码还是用静态库，你需要将`include`目录拷贝到一个固定目录（项目目录、系统目录或者其他地方），然后在`Visual Studio`或`Makefile`中指定头文件目录。

在源码中包含代码如下：

```c
  #include <karl/json.hxx>
  using Json = karl::json;
```

### 静态库链接

  在 Windows 平台，在项目属性中加入，也可以用如下代码链接指定库：
```c
  #pragma comment(lib, "karl.lib")
```

  在 Unix 平台需要在`Makefile`中添加相关代码，用`-L`指定库的位置，并添加`-lkarl`。

### 注意事项

1.不像别的 json 库有诸如 number、string、array 或 object 等对象，karl 库只对外提供一种对象类型，就是`json`类。它可能是 number、string，也可能是 arry 或者 object，视情况而定。

2.不要使用 json 对象的引用，因为在 karl 库内部使用智能指针共享原始对象，普通赋值即可达到引用效果。
  ```c
    Json& j = js["xxx"];    //  错误。
    Json j = js["xxx"];     //  正确，并且对 j 的修改会影响 js。
  ```

3.需要生成一个 json 对象的拷贝，请使用`json.copy`方法。
  ```c
    Json j = js.copy();     //  正确，对 j 的修改不会影响 js。
  ```

4.当获取数据的类型不正确，或是请求的数据不存在时，会抛出异常（`std::exception`）。请使用`try catch`来捕获异常确保流程的正常进行。

### 示例

下面是常规用法的示例：

1.解析 json 字符串
  ```c
    char obj[] = "{\"one\":true, \"number\":[1, 2, 3, 4]}";
    Json js = Json::parse(obj);
    
    std::string s = "[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]";
    Json j = Json::parse(s);
  ```

2.序列化成 std::string 对象
  ```c
    std::string s = "[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]";
    Json j = Json::parse(s);
    std::string str = j.dump();
  ```

3.赋值构建 json array 对象
  ```c
    Json j;
    j[1] = 2;
    j[2] = "ss";
    j[0] = 1;
  ```

4.赋值构建 json object 对象
  ```c
    Json j;
    j["one"] = false;
    j["two"] = 2;
    j["three"] = "hello world";
  ```

5.Json array 对象追加元素
  ```c
    Json j;
    j[1] = 2;
    j[0] = 1;
    Json jj;
    jj.push_back(j);
  ```

6.Json 对象获取值
  ```c
    Json j;
    j["one"] = true;
    j["two"] = 2.0f;
    j["three"] = "hello world";
    bool one = j["one"].get<bool>();
    int two = j["two"].get<int>();
  ```

7.Json 对象多层取值
  ```c
    Json j = Json::parse("[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]");
    bool one = j[0]["one"].get<bool>();
    int two = j[0]["two"].get<int>();
  ```

8.Json 对象迭代
  ```c
    Json j = Json::parse("[{\"one\":true,\"two\":2,\"three\":\"hello world\"}]");
    for (auto item : j) {
        std::cout << item.dump() << std::endl;
    }
  ```

9.使用键值对构造 json 对象
  ```c
    Json j = { { "one", nullptr },{ "two", true },{ "three", 10086 },{ "str", "World" } };
  ```

10.使用 vector 构造 json 对象
  ```c
    std::vector<int> vec{ 1,2,3,4,5 };
    Json j = vec;
  ```

更详细的示例代码请查阅`example/main.cc`。

### 其他

在开发过程中，我对一些细节功能进行了取舍，但不会影响 karl 在使用上的便捷性。代码中也可能有未被发现的问题，如果您有什么疑问，请联系我（shadow_yuan@qq.com）。
