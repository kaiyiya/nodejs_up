/**
 * V8 String 类使用示例
 * 
 * 这个示例展示了如何在 C++ 中使用 V8 的 String 类
 * 注意：这是演示代码，实际编译需要 V8 开发环境
 */

#include "v8.h"
#include <iostream>
#include <string>

using namespace v8;

class V8StringDemo {
public:
    static void RunDemo() {
        // 1. 创建 V8 平台和隔离器
        V8::InitializePlatform(V8::CreateDefaultPlatform());
        V8::Initialize();
        
        Isolate* isolate = Isolate::New();
        Isolate::Scope isolate_scope(isolate);
        HandleScope handle_scope(isolate);
        Local<Context> context = Context::New(isolate);
        Context::Scope context_scope(context);
        
        std::cout << "=== V8 String 类演示 ===\n\n";
        
        // 2. 创建 JavaScript 字符串的不同方式
        DemoStringCreation(isolate);
        
        // 3. 字符串操作演示
        DemoStringOperations(isolate);
        
        // 4. 编码转换演示
        DemoEncodingConversion(isolate);
        
        // 5. 清理
        isolate->Dispose();
        V8::Dispose();
        V8::ShutdownPlatform();
        
        std::cout << "\n=== 演示完成 ===\n";
    }
    
private:
    static void DemoStringCreation(Isolate* isolate) {
        std::cout << "1. 字符串创建演示:\n";
        
        // 从 C 字符串创建
        Local<String> str1 = String::NewFromUtf8(isolate, "Hello World");
        std::cout << "   从 C 字符串创建: " << StringToCString(str1) << "\n";
        
        // 从 UTF-16 数据创建
        uint16_t utf16_data[] = {0x0048, 0x0065, 0x006C, 0x006C, 0x006F}; // "Hello"
        Local<String> str2 = String::NewFromTwoByte(isolate, utf16_data, 
                                                   NewStringType::kNormalString, 5);
        std::cout << "   从 UTF-16 创建: " << StringToCString(str2) << "\n";
        
        // 创建空字符串
        Local<String> empty_str = String::NewFromUtf8(isolate, "");
        std::cout << "   空字符串长度: " << empty_str->Length() << "\n";
        
        std::cout << "\n";
    }
    
    static void DemoStringOperations(Isolate* isolate) {
        std::cout << "2. 字符串操作演示:\n";
        
        Local<String> str = String::NewFromUtf8(isolate, "Hello World");
        
        // 基本属性
        std::cout << "   字符串: " << StringToCString(str) << "\n";
        std::cout << "   长度: " << str->Length() << "\n";
        std::cout << "   UTF-8 长度: " << str->Utf8Length() << "\n";
        std::cout << "   是否单字节: " << (str->IsOneByte() ? "是" : "否") << "\n";
        
        // 字符串比较
        Local<String> str2 = String::NewFromUtf8(isolate, "Hello World");
        std::cout << "   字符串相等: " << (str->Equals(str2) ? "是" : "否") << "\n";
        
        // 字符串连接
        Local<String> prefix = String::NewFromUtf8(isolate, "Prefix: ");
        Local<String> concatenated = String::Concat(prefix, str);
        std::cout << "   连接结果: " << StringToCString(concatenated) << "\n";
        
        std::cout << "\n";
    }
    
    static void DemoEncodingConversion(Isolate* isolate) {
        std::cout << "3. 编码转换演示:\n";
        
        // 创建包含 Unicode 字符的字符串
        Local<String> unicode_str = String::NewFromUtf8(isolate, "你好世界");
        
        std::cout << "   Unicode 字符串: " << StringToCString(unicode_str) << "\n";
        std::cout << "   字符数: " << unicode_str->Length() << "\n";
        std::cout << "   UTF-8 字节数: " << unicode_str->Utf8Length() << "\n";
        
        // 转换为 UTF-8
        int utf8_len = unicode_str->Utf8Length();
        char* utf8_buffer = new char[utf8_len + 1];
        unicode_str->WriteUtf8(utf8_buffer, utf8_len);
        utf8_buffer[utf8_len] = '\0';
        
        std::cout << "   UTF-8 编码: ";
        for (int i = 0; i < utf8_len; i++) {
            std::cout << "0x" << std::hex << (unsigned char)utf8_buffer[i] << " ";
        }
        std::cout << std::dec << "\n";
        
        delete[] utf8_buffer;
        std::cout << "\n";
    }
    
    // 辅助函数：将 V8 String 转换为 C++ 字符串
    static std::string StringToCString(Local<String> str) {
        String::Utf8Value utf8_value(str);
        return std::string(*utf8_value);
    }
};

// 主函数
int main() {
    try {
        V8StringDemo::RunDemo();
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

/*
编译说明（需要 V8 开发环境）:
g++ -std=c++11 -I/path/to/v8/include -L/path/to/v8/lib \
    v8_string_demo.cc -lv8 -lv8_libplatform -o v8_string_demo

运行:
./v8_string_demo
*/

