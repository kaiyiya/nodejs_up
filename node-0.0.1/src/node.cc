#include "node.h"

#include "net.h"
#include "file.h"
#include "http.h"
#include "timer.h"
#include "constants.h"

#include "natives.h" 

#include <stdio.h>
#include <assert.h>

#include <string>
#include <list>
#include <map>

using namespace v8;
using namespace node;
using namespace std;

static int exit_code = 0;

ObjectWrap::~ObjectWrap ( )
{
  handle_->SetInternalField(0, Undefined());
  handle_.Dispose();
  handle_.Clear(); 
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
  attach_count_ += 1;// 增加附加计数器
}

void
ObjectWrap::Detach ()
{
  if (attach_count_ > 0)
    attach_count_ -= 1;//减少引用计数器

  if(weak_ && attach_count_ == 0) {
    V8::AdjustAmountOfExternalAllocatedMemory(-size());
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
  v8::V8::AdjustAmountOfExternalAllocatedMemory(obj->size());//通知v8外部内存使用情况
}

// Extracts a C string from a V8 Utf8Value.
const char*
ToCString(const v8::String::Utf8Value& value)
{
  return *value ? *value : "<string conversion failed>";
}

void
ReportException(v8::TryCatch* try_catch)
{
  v8::HandleScope handle_scope;
  v8::String::Utf8Value exception(try_catch->Exception());
  const char* exception_string = ToCString(exception);
  v8::Handle<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    printf("%s\n", exception_string);
  } else {
    message->PrintCurrentStackTrace(stdout);

    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(message->GetScriptResourceName());
    const char* filename_string = ToCString(filename);
    int linenum = message->GetLineNumber();
    printf("%s:%i: %s\n", filename_string, linenum, exception_string);
    // Print line of source code.
    v8::String::Utf8Value sourceline(message->GetSourceLine());
    const char* sourceline_string = ToCString(sourceline);
    printf("%s\n", sourceline_string);
    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn();
    for (int i = 0; i < start; i++) {
      printf(" ");
    }
    int end = message->GetEndColumn();
    for (int i = start; i < end; i++) {
      printf("^");
    }
    printf("\n");
  }
}

// Executes a string within the current v8 context.
Handle<Value>
ExecuteString(v8::Handle<v8::String> source,
              v8::Handle<v8::Value> filename)
{
  HandleScope scope;
  TryCatch try_catch;

  Handle<Script> script = Script::Compile(source, filename);
  if (script.IsEmpty()) {
    ReportException(&try_catch);
    ::exit(1);
  }

  Handle<Value> result = script->Run();
  if (result.IsEmpty()) {
    ReportException(&try_catch);
    ::exit(1);
  }

  return scope.Close(result);
}

NODE_METHOD(node_exit)
{
  int r = 0;
  if (args.Length() > 0) 
    r = args[0]->IntegerValue();
  ::exit(r);
  return Undefined(); 
}

NODE_METHOD(compile) 
{
  if (args.Length() < 2) 
    return Undefined();

  HandleScope scope;

  Local<String> source = args[0]->ToString();
  Local<String> filename = args[1]->ToString();

  Handle<Value> result = ExecuteString(source, filename);
  
  return scope.Close(result);
}

NODE_METHOD(debug) 
{
  if (args.Length() < 1) 
    return Undefined();
  HandleScope scope;
  String::Utf8Value msg(args[0]->ToString());
  fprintf(stderr, "DEBUG: %s\n", *msg);
  return Undefined();
}

static void
OnFatalError (const char* location, const char* message)
{

#define FATAL_ERROR "\033[1;31mV8 FATAL ERROR.\033[m"
  if (location)
    fprintf(stderr, FATAL_ERROR " %s %s\n", location, message);
  else 
    fprintf(stderr, FATAL_ERROR " %s\n", message);

  ::exit(1);
}


void
node::FatalException (TryCatch &try_catch)
{
  ReportException(&try_catch);
  ::exit(1);
}

static ev_async eio_watcher;

static void 
node_eio_cb (EV_P_ ev_async *w, int revents)
{
  int r = eio_poll();
  /* returns 0 if all requests were handled, -1 if not, or the value of EIO_FINISH if != 0 */

  // XXX is this check too heavy? 
  //  it require three locks in eio
  //  what's the better way?
  if (eio_nreqs () == 0 && eio_nready() == 0 && eio_npending() == 0) 
    ev_async_stop(EV_DEFAULT_UC_ w);
}

static void
eio_want_poll (void)
{
  ev_async_send(EV_DEFAULT_UC_ &eio_watcher); 
}

void
node::eio_warmup (void)
{
  ev_async_start(EV_DEFAULT_UC_ &eio_watcher);
}

enum encoding
node::ParseEncoding (Handle<Value> encoding_v)
{
  HandleScope scope;

  if (!encoding_v->IsString())
    return RAW;

  String::Utf8Value encoding(encoding_v->ToString());

  if(strcasecmp(*encoding, "utf8") == 0) {
    return UTF8;
  } else if (strcasecmp(*encoding, "ascii") == 0) {
    return ASCII;
  } else {
    return RAW;
  }
}

int
main (int argc, char *argv[]) 
{
  ev_default_loop(EVFLAG_AUTO); // initialize the default ev loop.

  // start eio thread pool
  ev_async_init(&eio_watcher, node_eio_cb);
  eio_init(eio_want_poll, NULL);

  V8::SetFlagsFromCommandLine(&argc, argv, true);
  V8::Initialize();

  if(argc < 2)  {
    fprintf(stderr, "No script was specified.\n");
    return 1;
  }

  string filename(argv[1]);

  HandleScope handle_scope;

  Persistent<Context> context = Context::New(NULL, ObjectTemplate::New());
  Context::Scope context_scope(context);
  V8::SetFatalErrorHandler(OnFatalError);

  Local<Object> g = Context::GetCurrent()->Global();

  V8::PauseProfiler(); // to be resumed in Connection::on_read

  Local<Object> node = Object::New();
  g->Set(String::New("node"), node);

  NODE_SET_METHOD(node, "compile", compile); // internal 
  NODE_SET_METHOD(node, "debug", debug);
  NODE_SET_METHOD(node, "exit", node_exit);

  Local<Array> arguments = Array::New(argc);
  for (int i = 0; i < argc; i++) {
    Local<String> arg = String::New(argv[i]);
    arguments->Set(Integer::New(i), arg);
  }
  g->Set(String::New("ARGV"), arguments);


  // BUILT-IN MODULES
  Timer::Initialize(node);

  Local<Object> constants = Object::New();
  node->Set(String::New("constants"), constants);
  DefineConstants(constants);

  Local<Object> fs = Object::New();
  node->Set(String::New("fs"), fs);
  File::Initialize(fs);

  Local<Object> tcp = Object::New();
  node->Set(String::New("tcp"), tcp);
  Acceptor::Initialize(tcp);
  Connection::Initialize(tcp);

  Local<Object> http = Object::New();
  node->Set(String::New("http"), http);
  HTTPServer::Initialize(http);
  HTTPConnection::Initialize(http);

  // NATIVE JAVASCRIPT MODULES
  TryCatch try_catch;

  ExecuteString(String::New(native_http), String::New("http.js"));
  if (try_catch.HasCaught()) goto native_js_error; 

  ExecuteString(String::New(native_file), String::New("file.js"));
  if (try_catch.HasCaught()) goto native_js_error; 

  ExecuteString(String::New(native_node), String::New("node.js"));
  if (try_catch.HasCaught()) goto native_js_error; 

  ev_loop(EV_DEFAULT_UC_ 0);

  context.Dispose();
  // The following line when uncommented causes an error.
  // To reproduce do this:
  // > node --prof test-http_simple.js 
  // 
  // > curl http://localhost:8000/quit/
  //
  //V8::Dispose();

  return exit_code;

native_js_error:
  ReportException(&try_catch);
  return 1;
}
