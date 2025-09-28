#include "node.h"//核心头文件

#include "net.h"//网络模块
#include "file.h"//文件系统模块
#include "http.h"//http模块
#include "timer.h"//定时器模块
#include "constants.h"//常量模块

#include "natives.h" //内置模块

#include <stdio.h>//标准输入输出
#include <assert.h>//断言

#include <string>//字符串
#include <list>//链表
#include <map>//映射

using namespace v8;//V8引擎命名空间
using namespace node;//Node.js命名空间
using namespace std;//标准库命名空间

static int exit_code = 0;//程序退出码

ObjectWrap::~ObjectWrap ( )
{
  // 析构函数：清理V8对象关联
  handle_->SetInternalField(0, Undefined());//清空内部字段
  handle_.Dispose();//释放持久句柄
  handle_.Clear(); //清除句柄引用
}

ObjectWrap::ObjectWrap (Handle<Object> handle)//构造函数
{
  // TODO throw exception if it's already set
  HandleScope scope;// 创建一个句柄作用域，用于管理V8对象的生命周期
  handle_ = Persistent<Object>::New(handle); // 将传入的handle对象创建为持久化引用，防止被V8垃圾回收机制回收
  Handle<External> external = External::New(this); // 创建一个External对象，包装this指针
  handle_->SetInternalField(0, external); // 将External对象存储在handle对象的第0个内部字段中，用于后续获取C++对象指针
  handle_.MakeWeak(this, ObjectWrap::MakeWeak); // 设置弱引用回调，当V8垃圾回收时会调用ObjectWrap::MakeWeak函数
  attach_count_ = 0; // 初始化附加计数器为0，用于跟踪对象的引用计数
  weak_ = false; // 初始化弱引用标志为false，表示对象当前不处于弱引用状态
}

void
ObjectWrap::Attach ()
{
  // 附加对象到事件循环，防止被垃圾回收
  attach_count_ += 1;// 增加附加计数器
}

void
ObjectWrap::Detach ()
{
  // 从事件循环中分离对象，允许被垃圾回收
  if (attach_count_ > 0)
    attach_count_ -= 1;//减少引用计数器

  // 当对象处于弱引用状态且C++引用计数为0时，删除对象
  if(weak_ && attach_count_ == 0) {
    V8::AdjustAmountOfExternalAllocatedMemory(-size());//调整V8外部内存统计
    delete this;//当对象处于弱引用状态并且引用计数为0的时候,执行清理并且删除对象实例
  }
  // 当v8垃圾回收器发现js对象不再被引用的时候,会调用MakeWeak进行回调,这时候会把弱引用标识设置为true
  // attach_count_跟踪的是c++层面的引用技术,并不是js层面的,它通过Attach()和Detach()手动管理
  // 这里有两套引用计数系统,js引用是由v8管理,决定是否进入弱引用状态,c++引用是由attch_count_手动管理
  // 决定是否真正删除对象,这样避免悬空指针的问题
}

// 这段代码是Node.js中 ObjectWrap::Unwrap 函数的实现，功能是从V8对象中提取C++对象指针：
// 1. 检查传入的handle是否为空，为空则返回NULL
// 2. 验证对象是否有内部字段，没有则返回NULL
// 3. 获取第一个内部字段的值
// 4. 将值转换为External类型并返回其存储的C++对象指针
// 主要用于JavaScript对象与C++对象之间的相互转换。
void*
ObjectWrap::Unwrap (Handle<Object> handle)
{
  HandleScope scope;
  if (handle.IsEmpty()) { 
    fprintf(stderr, "Node: Tried to unwrap empty object.\n");
    return NULL;
  }
  if ( handle->InternalFieldCount() == 0) {
    fprintf(stderr, "Node: Tried to unwrap object without internal fields.\n");
    return NULL;
  }
  Local<Value> value = handle->GetInternalField(0);
  if (value.IsEmpty()) {
    fprintf(stderr, "Tried to unwrap object with empty internal field.\n");
    return NULL;
  }
  Handle<External> field = Handle<External>::Cast(value);
  return field->Value();
}

void
ObjectWrap::MakeWeak (Persistent<Value> _, void *data)
{
  // 在v8中,弱引用是指不会阻止对象被垃圾回收的引用,在objectWrap类中,弱引用状态是当MakeWeak被调用的时候,对象进入弱引用状态
  // 弱持久句柄会被垃圾回收,对象被删除,普通持久句柄不会被垃圾回收,当js对象被回收的时候会触发回调
  ObjectWrap *obj = static_cast<ObjectWrap*> (data);//强转类型,将void指针(无类型指针)转换为objectwrap类指针
  obj->weak_ = true;//该对象目前处于弱引用状态
  if (obj->attach_count_ == 0)
    delete obj;//如果该指针的附加计数器为0,删除对象
}

void
ObjectWrap::InformV8ofAllocation (ObjectWrap *obj)
{
  // 通知V8引擎外部内存分配情况，用于垃圾回收决策
  v8::V8::AdjustAmountOfExternalAllocatedMemory(obj->size());//通知v8外部内存使用情况
}

// Extracts a C string from a V8 Utf8Value.
const char*
ToCString(const v8::String::Utf8Value& value)
{//转换为C字符串
  // 从V8 UTF-8字符串值中提取C字符串，转换失败时返回错误信息
  return *value ? *value : "<string conversion failed>";
}

void
ReportException(v8::TryCatch* try_catch)
{//报告异常
  v8::HandleScope handle_scope;//创建一个句柄作用域，用于管理V8对象的生命周期
  v8::String::Utf8Value exception(try_catch->Exception());//js字符串(v8内部表示)
  const char* exception_string = ToCString(exception);//转换为C字符串
  v8::Handle<v8::Message> message = try_catch->Message();//获取消息
  if (message.IsEmpty()) {
    // 如果V8没有提供任何关于这个错误的信息，只是打印异常
    printf("%s\n", exception_string);
  } else {
    message->PrintCurrentStackTrace(stdout);
    // 打印(文件名):(行号): (消息).
    v8::String::Utf8Value filename(message->GetScriptResourceName());
    // 获取文件名
    const char* filename_string = ToCString(filename);
    // 获取行号
    int linenum = message->GetLineNumber();
    // 打印(文件名):(行号): (消息).
    printf("%s:%i: %s\n", filename_string, linenum, exception_string);
    // 打印源代码行
    v8::String::Utf8Value sourceline(message->GetSourceLine());
    const char* sourceline_string = ToCString(sourceline);
    printf("%s\n", sourceline_string);
    // 打印波浪线(GetUnderline已弃用).
    int start = message->GetStartColumn();
    // 打印空格
    for (int i = 0; i < start; i++) {
      printf(" ");
    }
    // 获取结束列
    int end = message->GetEndColumn();
    // 打印^
    for (int i = start; i < end; i++) {
      printf("^");
    }
    // 打印换行符
    printf("\n");
  }
}

// Executes a string within the current v8 context.
Handle<Value>
ExecuteString(v8::Handle<v8::String> source,
              v8::Handle<v8::Value> filename)
{// 执行字符串
  HandleScope scope;//创建一个句柄作用域，用于管理V8对象的生命周期
  TryCatch try_catch;//创建一个TryCatch对象，用于捕获异常

  Handle<Script> script = Script::Compile(source, filename);//编译字符串
  if (script.IsEmpty()) {
      // 编译失败的可能原因：
      // 1. 语法错误
      // 2. 内存不足
      // 3. 文件读取错误
      // 4. 编码问题
    ReportException(&try_catch);
    ::exit(1);
  }

  Handle<Value> result = script->Run();
  if (result.IsEmpty()) {
    ReportException(&try_catch);
    ::exit(1);
  }

  return scope.Close(result);//关闭句柄，返回结果
}

NODE_METHOD(node_exit)
{//退出程序
  int r = 0;//初始化返回值为0
  if (args.Length() > 0) 
    r = args[0]->IntegerValue();//如果提供了参数,将该参数作为退出码
  ::exit(r);//退出程序
  return Undefined(); 
}

NODE_METHOD(compile) 
{
  // 检查参数数量，需要至少2个参数：源代码和文件名
  if (args.Length() < 2) 
    return Undefined();  // 参数不足时返回 undefined

  // 创建句柄作用域，管理V8对象的生命周期
  HandleScope scope;

  // 将第一个参数转换为字符串（JavaScript源代码）
  Local<String> source = args[0]->ToString();
  // 将第二个参数转换为字符串（文件名，用于错误报告）
  Local<String> filename = args[1]->ToString();

  // 调用ExecuteString函数编译并执行JavaScript代码
  Handle<Value> result = ExecuteString(source, filename);
  
  // 返回执行结果，并关闭作用域
  return scope.Close(result);
}

NODE_METHOD(debug) 
{
  // 检查参数数量，需要至少1个参数：调试消息
  if (args.Length() < 1) 
    return Undefined();  // 参数不足时返回 undefined

  // 创建句柄作用域，管理V8对象的生命周期
  HandleScope scope;
  
  // 将第一个参数转换为UTF-8字符串（调试消息）
  String::Utf8Value msg(args[0]->ToString());
  
  // 将调试消息输出到标准错误流，格式为 "DEBUG: 消息内容"
  fprintf(stderr, "DEBUG: %s\n", *msg);
  
  // 返回 undefined（调试函数通常不需要返回值）
  return Undefined();
}

static void
OnFatalError (const char* location, const char* message)
{
  // 定义致命错误的前缀，使用ANSI颜色代码显示红色文本
  // \033[1;31m 表示粗体红色，\033[m 表示重置颜色
#define FATAL_ERROR "\033[1;31mV8 FATAL ERROR.\033[m"
  
  // 如果有位置信息，输出位置和错误消息
  if (location)
    fprintf(stderr, FATAL_ERROR " %s %s\n", location, message);
  // 如果没有位置信息，只输出错误消息
  else 
    fprintf(stderr, FATAL_ERROR " %s\n", message);

  // 致命错误发生后，立即退出程序，退出码为1表示异常退出
  ::exit(1);
}


void
node::FatalException (TryCatch &try_catch)
{
  // 报告异常
  ReportException(&try_catch);
  // 退出程序，退出码为1表示异常退出
  ::exit(1);
}

static ev_async eio_watcher;//事件循环

static void 
node_eio_cb (EV_P_ ev_async *w, int revents)//
{//事件循环回调函数
  int r = eio_poll();
  /* returns 0 if all requests were handled, -1 if not, or the value of EIO_FINISH if != 0 */
  // 如果所有请求都被处理,返回0,如果未处理,返回-1,如果!=0,返回EIO_FINISH

  // XXX is this check too heavy? 
  //  it require three locks in eio
  //  what's the better way? 每次都要检查三把锁,有没有更好的实现方式?
  // 检查是否所有请求都被处理,如果所有请求都被处理,停止事件循环
  if (eio_nreqs () == 0 && eio_nready() == 0 && eio_npending() == 0) 
    // 停止事件循环
    ev_async_stop(EV_DEFAULT_UC_ w);
}

static void
eio_want_poll (void)
{
  // 异步I/O需要轮询时的回调函数，通知事件循环有新的I/O操作完成
  ev_async_send(EV_DEFAULT_UC_ &eio_watcher); 
}

void
node::eio_warmup (void)
{
  // 预热异步I/O系统，启动异步观察器
  ev_async_start(EV_DEFAULT_UC_ &eio_watcher);
}

enum encoding
node::ParseEncoding (Handle<Value> encoding_v)
{
  // 解析字符串编码类型，将JavaScript字符串转换为C++枚举
  HandleScope scope;

  // 检查参数是否为字符串类型
  if (!encoding_v->IsString())
    return RAW;  // 非字符串类型，返回原始编码

  // 将V8字符串转换为UTF-8字符串
  String::Utf8Value encoding(encoding_v->ToString());

  // 根据字符串内容确定编码类型
  if(strcasecmp(*encoding, "utf8") == 0) {
    return UTF8;   // UTF-8编码
  } else if (strcasecmp(*encoding, "ascii") == 0) {
    return ASCII;  // ASCII编码
  } else {
    return RAW;    // 原始编码
  }
}

int
main (int argc, char *argv[]) 
{
  // ========== 第一阶段：事件循环初始化 ==========
  // 初始化默认的事件循环，使用自动检测的后端（epoll/kqueue/select等）
  ev_default_loop(EVFLAG_AUTO);

  // ========== 第二阶段：异步I/O系统初始化 ==========
  // 初始化异步I/O观察器，用于处理异步I/O操作的回调
  ev_async_init(&eio_watcher, node_eio_cb);
  // 初始化异步I/O线程池，eio_want_poll是回调函数
  eio_init(eio_want_poll, NULL);

  // ========== 第三阶段：V8引擎初始化 ==========
  // 从命令行参数设置V8引擎标志
  V8::SetFlagsFromCommandLine(&argc, argv, true);
  // 初始化V8 JavaScript引擎
  V8::Initialize();

  // ========== 第四阶段：参数验证 ==========
  // 检查是否提供了要执行的JavaScript脚本文件
  if(argc < 2)  {
    fprintf(stderr, "No script was specified.\n");
    return 1;  // 没有脚本文件，退出程序
  }

  // 获取要执行的JavaScript文件名
  string filename(argv[1]);

  // ========== 第五阶段：V8上下文创建 ==========
  // 创建句柄作用域，管理V8对象的生命周期
  HandleScope handle_scope;

  // 创建V8 JavaScript上下文，这是JavaScript代码执行的环境
  Persistent<Context> context = Context::New(NULL, ObjectTemplate::New());
  // 设置当前上下文作用域
  Context::Scope context_scope(context);
  // 设置V8引擎的致命错误处理器
  V8::SetFatalErrorHandler(OnFatalError);

  // 获取全局对象，这是JavaScript代码中的全局作用域
  Local<Object> g = Context::GetCurrent()->Global();

  // 暂停V8性能分析器，稍后在Connection::on_read中恢复
  V8::PauseProfiler();

  // ========== 第六阶段：Node.js对象创建 ==========
  // 创建node对象，这是Node.js的主要API对象
  Local<Object> node = Object::New();
  // 将node对象添加到全局作用域
  g->Set(String::New("node"), node);

  // 注册Node.js内置方法
  NODE_SET_METHOD(node, "compile", compile);  // 编译JavaScript代码
  NODE_SET_METHOD(node, "debug", debug);     // 调试输出
  NODE_SET_METHOD(node, "exit", node_exit);   // 退出程序

  // ========== 第七阶段：命令行参数处理 ==========
  // 创建参数数组，将C++的argv转换为JavaScript数组
  Local<Array> arguments = Array::New(argc);
  for (int i = 0; i < argc; i++) {
    Local<String> arg = String::New(argv[i]);
    arguments->Set(Integer::New(i), arg);
  }
  // 将参数数组添加到全局作用域，JavaScript代码可以通过ARGV访问
  g->Set(String::New("ARGV"), arguments);

  // ========== 第八阶段：内置模块初始化 ==========
  // 初始化定时器模块
  Timer::Initialize(node);

  // 初始化常量模块
  Local<Object> constants = Object::New();
  node->Set(String::New("constants"), constants);
  DefineConstants(constants);  // 定义所有系统常量

  // 初始化文件系统模块
  Local<Object> fs = Object::New();
  node->Set(String::New("fs"), fs);
  File::Initialize(fs);

  // 初始化TCP网络模块
  Local<Object> tcp = Object::New();
  node->Set(String::New("tcp"), tcp);
  Acceptor::Initialize(tcp);    // TCP服务器
  Connection::Initialize(tcp);  // TCP连接

  // 初始化HTTP模块
  Local<Object> http = Object::New();
  node->Set(String::New("http"), http);
  HTTPServer::Initialize(http);     // HTTP服务器
  HTTPConnection::Initialize(http);  // HTTP连接

  // ========== 第九阶段：原生JavaScript模块加载 ==========
  // 创建异常捕获器，用于处理JavaScript执行错误
  TryCatch try_catch;

  // 执行HTTP模块的JavaScript代码
  ExecuteString(String::New(native_http), String::New("http.js"));
  if (try_catch.HasCaught()) goto native_js_error; 

  // 执行文件系统模块的JavaScript代码
  ExecuteString(String::New(native_file), String::New("file.js"));
  if (try_catch.HasCaught()) goto native_js_error; 

  // 执行核心Node.js模块的JavaScript代码
  ExecuteString(String::New(native_node), String::New("node.js"));
  if (try_catch.HasCaught()) goto native_js_error; 

  // ========== 第十阶段：事件循环启动 ==========
  // 启动事件循环，这是Node.js的核心，处理所有异步操作
  ev_loop(EV_DEFAULT_UC_ 0);

  // ========== 第十一阶段：清理和退出 ==========
  // 释放V8上下文
  context.Dispose();
  
  // 注释掉的V8清理代码，因为会导致错误
  // 重现错误的方法：
  // > node --prof test-http_simple.js 
  // > curl http://localhost:8000/quit/
  //V8::Dispose();

  // 返回程序退出码
  return exit_code;

// ========== 错误处理 ==========
native_js_error:
  // 如果JavaScript模块加载失败，报告异常并退出
  ReportException(&try_catch);
  return 1;
}
