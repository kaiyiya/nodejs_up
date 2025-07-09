function alert(str) {
  console.log(str);
}
const buf = new ArrayBuffer(16); // 在内存中分配16 字节
alert(buf.byteLength); // 16
const buf1 = new ArrayBuffer(16);
const buf2 = buf1.slice(4, 12);
alert(buf2.byteLength); // 8
// DataView默认使用整个ArrayBuffer
const fullDataView = new DataView(buf);
alert(fullDataView.byteOffset); // 0
alert(fullDataView.byteLength); // 16
alert(fullDataView.buffer === buf); // true
// 构造函数接收一个可选的字节偏移量和字节长度
//    byteOffset=0 表示视图从缓冲起点开始
//    byteLength=8 限制视图为前8 个字节
const firstHalfDataView = new DataView(buf, 0, 8);
alert(firstHalfDataView.byteOffset); // 0
alert(firstHalfDataView.byteLength); // 8
alert(firstHalfDataView.buffer === buf); // true
// 如果不指定，则DataView会使用剩余的缓冲
//    byteOffset=8 表示视图从缓冲的第9 个字节开始
//    byteLength未指定，默认为剩余缓冲
const secondHalfDataView = new DataView(buf, 8);
alert(secondHalfDataView.byteOffset); // 8
alert(secondHalfDataView.byteLength); // 8
alert(secondHalfDataView.buffer === buf); // true
// 在内存中分配两个字节并声明一个DataView
const buf11 = new ArrayBuffer(2);
const view = new DataView(buf11);
// 说明整个缓冲确实所有二进制位都是0
// 检查第一个和第二个字符
alert(view.getInt8(0)); // 0
alert(view.getInt8(1)); // 0
// 检查整个缓冲
alert(view.getInt16(0)); // 0
// 将整个缓冲都设置为1
// 255 的二进制表示是11111111（2^8-1）
view.setUint8(0, 255);
// DataView会自动将数据转换为特定的ElementType
// 255 的十六进制表示是0xFF
view.setUint8(1, 0xff);
// 现在，缓冲里都是1 了
// 如果把它当成二补数的有符号整数，则应该是-1
alert(view.getInt16(0)); // -1
