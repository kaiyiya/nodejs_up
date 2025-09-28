#include "node.h"
#include "constants.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

using namespace v8;
using namespace node;

void
node::DefineConstants (Handle<Object> target)
{
  NODE_DEFINE_CONSTANT(target, RAW);//原始数据
  NODE_DEFINE_CONSTANT(target, UTF8);//UTF编码
  NODE_DEFINE_CONSTANT(target, ASCII);//ASCII编码

  NODE_DEFINE_CONSTANT(target, STDIN_FILENO);//标准输入
  NODE_DEFINE_CONSTANT(target, STDOUT_FILENO);//标准输出
  NODE_DEFINE_CONSTANT(target, STDERR_FILENO);//标准错误

  // file access modes
  NODE_DEFINE_CONSTANT(target, O_RDONLY);//只读
  NODE_DEFINE_CONSTANT(target, O_WRONLY);//只写
  NODE_DEFINE_CONSTANT(target, O_RDWR);//读写

#ifdef O_CREAT
  NODE_DEFINE_CONSTANT(target, O_CREAT);//创建
#endif

#ifdef O_EXCL
  NODE_DEFINE_CONSTANT(target, O_EXCL);//独占
#endif

#ifdef O_NOCTTY
  NODE_DEFINE_CONSTANT(target, O_NOCTTY);//不分配控制终端
#endif

#ifdef O_TRUNC
  NODE_DEFINE_CONSTANT(target, O_TRUNC);//截断
#endif

#ifdef O_APPEND
  NODE_DEFINE_CONSTANT(target, O_APPEND);//追加
#endif

#ifdef O_DIRECTORY
  NODE_DEFINE_CONSTANT(target, O_DIRECTORY);//目录
#endif

#ifdef O_EXCL
  NODE_DEFINE_CONSTANT(target, O_EXCL);//独占
#endif

#ifdef O_NOFOLLOW
  NODE_DEFINE_CONSTANT(target, O_NOFOLLOW);//不跟随
#endif

#ifdef O_SYNC
  NODE_DEFINE_CONSTANT(target, O_SYNC);//同步
#endif

#ifdef S_IRWXU
  NODE_DEFINE_CONSTANT(target, S_IRWXU);//所有者
#endif


#ifdef S_IRUSR
  NODE_DEFINE_CONSTANT(target, S_IRUSR);//所有者读
#endif

#ifdef S_IWUSR
  NODE_DEFINE_CONSTANT(target, S_IWUSR);//所有者写
#endif

#ifdef S_IXUSR
  NODE_DEFINE_CONSTANT(target, S_IXUSR);//所有者执行
#endif


#ifdef S_IRWXG
  NODE_DEFINE_CONSTANT(target, S_IRWXG);//组
#endif


#ifdef S_IRGRP
  NODE_DEFINE_CONSTANT(target, S_IRGRP);//组读
#endif

#ifdef S_IWGRP
  NODE_DEFINE_CONSTANT(target, S_IWGRP);//组写      
#endif

#ifdef S_IXGRP
  NODE_DEFINE_CONSTANT(target, S_IXGRP);//组执行
#endif


#ifdef S_IRWXO
  NODE_DEFINE_CONSTANT(target, S_IRWXO);//其他用户读写执行
#endif


#ifdef S_IROTH
  NODE_DEFINE_CONSTANT(target, S_IROTH);//其他用户读
#endif

#ifdef S_IWOTH
  NODE_DEFINE_CONSTANT(target, S_IWOTH);//其他用户写
#endif

#ifdef S_IXOTH
  NODE_DEFINE_CONSTANT(target, S_IXOTH);//其他用户执行
#endif

#ifdef E2BIG
  NODE_DEFINE_CONSTANT(target, E2BIG);//参数列表太长
#endif

#ifdef EACCES
  NODE_DEFINE_CONSTANT(target, EACCES);//权限被拒绝
#endif

#ifdef EADDRINUSE
  NODE_DEFINE_CONSTANT(target, EADDRINUSE);//地址已在使用
#endif

#ifdef EADDRNOTAVAIL
  NODE_DEFINE_CONSTANT(target, EADDRNOTAVAIL);//地址不可用
#endif

#ifdef EAFNOSUPPORT
  NODE_DEFINE_CONSTANT(target, EAFNOSUPPORT);//地址族不支持
#endif

#ifdef EAGAIN
  NODE_DEFINE_CONSTANT(target, EAGAIN);//资源暂时不可用
#endif

#ifdef EALREADY
  NODE_DEFINE_CONSTANT(target, EALREADY);//连接已在进行中
#endif

#ifdef EBADF
  NODE_DEFINE_CONSTANT(target, EBADF);//错误的文件描述符
#endif

#ifdef EBADMSG
  NODE_DEFINE_CONSTANT(target, EBADMSG);//错误的消息
#endif

#ifdef EBUSY
  NODE_DEFINE_CONSTANT(target, EBUSY);//设备或资源忙
#endif

#ifdef ECANCELED
  NODE_DEFINE_CONSTANT(target, ECANCELED);//操作被取消
#endif

#ifdef ECHILD
  NODE_DEFINE_CONSTANT(target, ECHILD);//没有子进程
#endif

#ifdef ECONNABORTED
  NODE_DEFINE_CONSTANT(target, ECONNABORTED);//连接被中止
#endif

#ifdef ECONNREFUSED
  NODE_DEFINE_CONSTANT(target, ECONNREFUSED);//连接被拒绝
#endif

#ifdef ECONNRESET
  NODE_DEFINE_CONSTANT(target, ECONNRESET);//连接被重置
#endif

#ifdef EDEADLK
  NODE_DEFINE_CONSTANT(target, EDEADLK);//资源死锁避免
#endif

#ifdef EDESTADDRREQ
  NODE_DEFINE_CONSTANT(target, EDESTADDRREQ);//需要目标地址
#endif

#ifdef EDOM
  NODE_DEFINE_CONSTANT(target, EDOM);//数学参数超出函数定义域
#endif

#ifdef EDQUOT
  NODE_DEFINE_CONSTANT(target, EDQUOT);//磁盘配额超出
#endif

#ifdef EEXIST
  NODE_DEFINE_CONSTANT(target, EEXIST);//文件已存在
#endif

#ifdef EFAULT
  NODE_DEFINE_CONSTANT(target, EFAULT);//错误的地址
#endif

#ifdef EFBIG
  NODE_DEFINE_CONSTANT(target, EFBIG);//文件过大
#endif

#ifdef EHOSTUNREACH
  NODE_DEFINE_CONSTANT(target, EHOSTUNREACH);//主机不可达
#endif

#ifdef EIDRM
  NODE_DEFINE_CONSTANT(target, EIDRM);//标识符被删除
#endif

#ifdef EILSEQ
  NODE_DEFINE_CONSTANT(target, EILSEQ);//非法的字节序列
#endif

#ifdef EINPROGRESS
  NODE_DEFINE_CONSTANT(target, EINPROGRESS);//操作正在进行中
#endif

#ifdef EINTR
  NODE_DEFINE_CONSTANT(target, EINTR);//被信号中断
#endif

#ifdef EINVAL
  NODE_DEFINE_CONSTANT(target, EINVAL);//无效参数
#endif

#ifdef EIO
  NODE_DEFINE_CONSTANT(target, EIO);//输入输出错误
#endif

#ifdef EISCONN
  NODE_DEFINE_CONSTANT(target, EISCONN);//套接字已连接
#endif

#ifdef EISDIR
  NODE_DEFINE_CONSTANT(target, EISDIR);//是目录
#endif

#ifdef ELOOP
  NODE_DEFINE_CONSTANT(target, ELOOP);//符号链接层数过多
#endif

#ifdef EMFILE
  NODE_DEFINE_CONSTANT(target, EMFILE);//打开的文件过多
#endif

#ifdef EMLINK
  NODE_DEFINE_CONSTANT(target, EMLINK);//链接过多
#endif

#ifdef EMSGSIZE
  NODE_DEFINE_CONSTANT(target, EMSGSIZE);//消息过大
#endif

#ifdef EMULTIHOP
  NODE_DEFINE_CONSTANT(target, EMULTIHOP);//多跳尝试
#endif

#ifdef ENAMETOOLONG
  NODE_DEFINE_CONSTANT(target, ENAMETOOLONG);//文件名过长
#endif

#ifdef ENETDOWN
  NODE_DEFINE_CONSTANT(target, ENETDOWN);//网络不可用
#endif

#ifdef ENETRESET
  NODE_DEFINE_CONSTANT(target, ENETRESET);//网络连接重置
#endif

#ifdef ENETUNREACH
  NODE_DEFINE_CONSTANT(target, ENETUNREACH);//网络不可达
#endif

#ifdef ENFILE
  NODE_DEFINE_CONSTANT(target, ENFILE);//系统打开的文件过多
#endif

#ifdef ENOBUFS
  NODE_DEFINE_CONSTANT(target, ENOBUFS);//缓冲区空间不足
#endif

#ifdef ENODATA
  NODE_DEFINE_CONSTANT(target, ENODATA);//没有可用的数据
#endif

#ifdef ENODEV
  NODE_DEFINE_CONSTANT(target, ENODEV);//没有该设备
#endif

#ifdef ENOENT
  NODE_DEFINE_CONSTANT(target, ENOENT);//文件或目录不存在
#endif

#ifdef ENOEXEC
  NODE_DEFINE_CONSTANT(target, ENOEXEC);//执行格式错误
#endif

#ifdef ENOLCK
  NODE_DEFINE_CONSTANT(target, ENOLCK);//没有可用的锁
#endif

#ifdef ENOLINK
  NODE_DEFINE_CONSTANT(target, ENOLINK);//链接已被切断
#endif

#ifdef ENOMEM
  NODE_DEFINE_CONSTANT(target, ENOMEM);//内存不足
#endif

#ifdef ENOMSG
  NODE_DEFINE_CONSTANT(target, ENOMSG);//没有所需类型的消息
#endif

#ifdef ENOPROTOOPT
  NODE_DEFINE_CONSTANT(target, ENOPROTOOPT);//协议选项不可用
#endif

#ifdef ENOSPC
  NODE_DEFINE_CONSTANT(target, ENOSPC);//设备上没有空间
#endif

#ifdef ENOSR
  NODE_DEFINE_CONSTANT(target, ENOSR);//没有可用的流资源
#endif

#ifdef ENOSTR
  NODE_DEFINE_CONSTANT(target, ENOSTR);//不是流设备
#endif

#ifdef ENOSYS
  NODE_DEFINE_CONSTANT(target, ENOSYS);//功能未实现
#endif

#ifdef ENOTCONN
  NODE_DEFINE_CONSTANT(target, ENOTCONN);//套接字未连接
#endif

#ifdef ENOTDIR
  NODE_DEFINE_CONSTANT(target, ENOTDIR);//不是目录
#endif

#ifdef ENOTEMPTY
  NODE_DEFINE_CONSTANT(target, ENOTEMPTY);//目录非空
#endif

#ifdef ENOTSOCK
  NODE_DEFINE_CONSTANT(target, ENOTSOCK);//不是套接字
#endif

#ifdef ENOTSUP
  NODE_DEFINE_CONSTANT(target, ENOTSUP);//操作不支持
#endif

#ifdef ENOTTY
  NODE_DEFINE_CONSTANT(target, ENOTTY);//不适当的IO控制操作
#endif

#ifdef ENXIO
  NODE_DEFINE_CONSTANT(target, ENXIO);//设备或地址不存在
#endif

#ifdef EOPNOTSUPP
  NODE_DEFINE_CONSTANT(target, EOPNOTSUPP);//操作不支持
#endif

#ifdef EOVERFLOW
  NODE_DEFINE_CONSTANT(target, EOVERFLOW);//值溢出
#endif

#ifdef EPERM
  NODE_DEFINE_CONSTANT(target, EPERM);//操作不被允许
#endif

#ifdef EPIPE
  NODE_DEFINE_CONSTANT(target, EPIPE);//管道破裂
#endif

#ifdef EPROTO
  NODE_DEFINE_CONSTANT(target, EPROTO);//协议错误
#endif

#ifdef EPROTONOSUPPORT
  NODE_DEFINE_CONSTANT(target, EPROTONOSUPPORT);//协议不支持
#endif

#ifdef EPROTOTYPE
  NODE_DEFINE_CONSTANT(target, EPROTOTYPE);//套接字类型不支持协议
#endif

#ifdef ERANGE
  NODE_DEFINE_CONSTANT(target, ERANGE);//结果超出范围
#endif

#ifdef EROFS
  NODE_DEFINE_CONSTANT(target, EROFS);//只读文件系统
#endif

#ifdef ESPIPE
  NODE_DEFINE_CONSTANT(target, ESPIPE);//非法寻址
#endif

#ifdef ESRCH
  NODE_DEFINE_CONSTANT(target, ESRCH);//没有该进程
#endif

#ifdef ESTALE
  NODE_DEFINE_CONSTANT(target, ESTALE);//文件句柄过时
#endif

#ifdef ETIME
  NODE_DEFINE_CONSTANT(target, ETIME);//计时器过期
#endif

#ifdef ETIMEDOUT
  NODE_DEFINE_CONSTANT(target, ETIMEDOUT);//连接超时
#endif

#ifdef ETXTBSY
  NODE_DEFINE_CONSTANT(target, ETXTBSY);//文本文件忙
#endif

#ifdef EWOULDBLOCK
  NODE_DEFINE_CONSTANT(target, EWOULDBLOCK);//操作会阻塞
#endif

#ifdef EXDEV
  NODE_DEFINE_CONSTANT(target, EXDEV);//跨设备链接
#endif

}

