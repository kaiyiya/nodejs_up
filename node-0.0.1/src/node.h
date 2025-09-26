#ifndef node_h
#define node_h

#include <ev.h>
#include <eio.h>
#include <v8.h>

namespace node {

#define NODE_SYMBOL(name) v8::String::NewSymbol(name)
#define NODE_METHOD(name) v8::Handle<v8::Value> name (const v8::Arguments& args)
#define NODE_SET_METHOD(obj, name, callback) \
  obj->Set(NODE_SYMBOL(name), v8::FunctionTemplate::New(callback)->GetFunction())
#define NODE_UNWRAP(type, value) static_cast<type*>(node::ObjectWrap::Unwrap(value))
#define NODE_DEFINE_CONSTANT(target, constant) \
  (target)->Set(v8::String::NewSymbol(#constant), v8::Integer::New(constant))

#define NODE_SET_PROTOTYPE_METHOD(templ, name, callback)                  \
do {                                                                      \
  Local<Signature> __callback##_SIG = Signature::New(templ);              \
  Local<FunctionTemplate> __callback##_TEM =                              \
    FunctionTemplate::New(callback, Handle<Value>() , __callback##_SIG ); \
  templ->PrototypeTemplate()->Set(NODE_SYMBOL(name), __callback##_TEM);   \
} while(0)

enum encoding {ASCII, UTF8, RAW};
enum encoding ParseEncoding (v8::Handle<v8::Value> encoding_v);
void FatalException (v8::TryCatch &try_catch); 
void eio_warmup (void); // call this before creating a new eio event.

class ObjectWrap {//管理v8生命周期和内存分配
public:
  ObjectWrap (v8::Handle<v8::Object> handle);//构造函数生命,接受一个v8句柄,建立c++和js对象的关联
  virtual ~ObjectWrap ( );//虚析构函数,支持多态删除,确保派生类能够正常析构

  virtual size_t size (void) = 0;//纯虚函数,要求派生类必须实现,用于返回对象的内存大小,便于v8进行内存管理

  /* This must be called after each new ObjectWrap creation! */
  static void InformV8ofAllocation (node::ObjectWrap *obj);//通知v8引擎关于对象的内存分配情况,用于v8的垃圾回收机制

protected:
  static void* Unwrap (v8::Handle<v8::Object> handle);//从v8对象句柄中获取对应的c++指针
  v8::Persistent<v8::Object> handle_;//持久句柄,保持v8对象的引用

  /* Attach() marks the object as being attached to an event loop.
   * Attached objects will not be garbage collected, even if
   * all references are lost.
   */
  void Attach();//标记对象已经附加到事件循环,防止垃圾被回收
  /* Detach() marks an object as detached from the event loop.  This is its
   * default state.  When an object with a "weak" reference changes from
   * attached to detached state it will be freed. Be careful not to access
   * the object after making this call as it might be gone!
   * (A "weak reference" is v8 terminology for an object who only has a
   * persistant handle.)
   * 翻译:
   * 默认状态。当一个具有"弱引用"的对象从附加状态变为分离状态时，
   * 它将被释放。进行此调用后要小心不要访问该对象，因为它可能已经不存在了！
   * （"弱引用"是v8术语，指仅具有持久句柄的对象。）
   */
  void Detach();//标记对象已经从事件循环中分离,可能被垃圾回收(分离状态)

private:
  static void MakeWeak (v8::Persistent<v8::Value> _, void *data);//处理v8的弱引用机制,只有弱引用可以被回收
  int attach_count_;//附加计数器,跟踪对象被附加到事件循环的次数
  bool weak_;//标记对象是否处于弱引用的状态
};

} // namespace node
#endif // node_h
