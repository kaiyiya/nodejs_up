function log(str) {
  console.log(str);
}
function alert(str) {
  console.log(str);
}
// 字符串会被拆分为单字符数组
console.log(Array.from("Matt")); // ["M", "a", "t", "t"]
// 可以使用from()将集合和映射转换为一个新数组
const m = new Map().set(1, 2).set(3, 4);
const s = new Set().add(1).add(2).add(3).add(4);
console.log(Array.from(m)); // [[1, 2], [3, 4]]
console.log(Array.from(s)); // [1, 2, 3, 4]
// Array.from()对现有数组执行浅复制
const a1 = [1, 2, 3, 4];
const a2 = Array.from(a1);
console.log(a1); // [1, 2, 3, 4]
console.log(a1 === a2); // false
// 可以使用任何可迭代对象
const iter = {
  *[Symbol.iterator]() {
    yield 1;
    yield 2;
    yield 3;
    yield 4;
  },
};
console.log(Array.from(iter)); // [1, 2, 3, 4]
// arguments对象可以被轻松地转换为数组
function getArgsArray() {
  return Array.from(arguments);
}
console.log(getArgsArray(1, 2, 3, 4, 5)); // [1, 2, 3, 4]
// from()也能转换带有必要属性的自定义对象
const arrayLikeObject = {
  0: 1,
  1: 2,
  2: 3,
  3: 4,
  4: 10,
  length: 5,
};

console.log(Array.from(arrayLikeObject)); // [1, 2, 3, 4]
const c1 = [1, 2, 3, 4];
const c2 = Array.from(c1, (x) => x ** 2);
const c3 = Array.from(
  c1,
  function (x) {
    return x ** this.exponent;
  },
  { exponent: 3 }
);
console.log(c2); // [1, 4, 9, 16]
console.log(c3); // [1, 8, 27, 64]

console.log(Array.of(1, 2, 3, 4)); // [1, 2, 3, 4]
console.log(Array.of(undefined)); // [undefined]
// 数组空位
const option = [, , , , ,]; // 创建包含5 个元素的数组
console.log(option.length); // 5
console.log(option); // [,,,,,]

const option1 = [1, , , , 5];
for (const option of option1) {
  console.log(option === undefined);
}
// false
// true
// true
// true
// false
const a = Array.from([, , ,]); // 使用ES6 的Array.from()创建的包含3 个空位的数组
for (const val of a) {
  console.log(val === undefined);
}
// true
// true
// true
console.log(Array.of(...[, , ,])); // [undefined, undefined, undefined]
for (const [index, value] of option1.entries()) {
  console.log(index + ":" + value);
}
// 1
// undefined
// undefined
// undefined
// 5
const options2 = [1, , , , 5];
// map()会跳过空位置
console.log(options2.map(() => 6)); // [6, undefined, undefined, undefined, 6]
// join()视空位置为空字符串
console.log(options2.join("-")); // "1----5"
// 数组索引
let colors = ["red", "blue", "green"]; // 定义一个字符串数组
log(colors[0]); // 显示第一项
colors[2] = "black"; // 修改第三项
colors[3] = "brown"; // 添加第四项
let colors1 = ["red", "blue", "green"]; // 创建一个包含3 个字符串的数组
let names = []; // 创建一个空数组
log(colors1.length); //3
log(names.length); //0
let colors2 = ["red", "blue", "green"]; // 创建一个包含3 个字符串的数组
colors2.length = 2;
alert(colors2[2]); //undefined
let colors3 = ["red", "blue", "green"]; // 创建一个包含3 个字符串的数组
colors3[colors3.length] = "black"; // 添加一种颜色（位置3）
colors3[colors3.length] = "brown"; // 再添加一种颜色（位置4）
log(colors3); //[ 'red', 'blue', 'green', 'black', 'brown' ]
let colors4 = ["red", "blue", "green"]; // 创建一个包含3 个字符串的数组
colors4[99] = "black"; // 添加一种颜色（位置99）
alert(colors4.length); // 100
let arr = new Array(4294967295);
log(arr.length); //4294967295
// 检测数组
if (arr instanceof Array) {
  log("操作数组");
}
if (Array.isArray(arr)) {
  log("操作数组");
}
// 迭代器方法
const a3 = ["foo", "bar", "baz", "qux"];
// 因为这些方法都返回迭代器，所以可以将它们的内容
// 通过Array.from()直接转换为数组实例
const aKeys = Array.from(a3.keys());
const aValues = Array.from(a3.values());
const aEntries = Array.from(a3.entries());
console.log(aKeys); // [0, 1, 2, 3]
console.log(aValues); // ["foo", "bar", "baz", "qux"]
console.log(aEntries); // [[0, "foo"], [1, "bar"], [2, "baz"], [3, "qux"]]
const a4 = ["foo", "bar", "baz", "qux"];
for (const [idx, element] of a4.entries()) {
  alert(idx);
  alert(element);
}
// 0
// foo
// 1
// bar
// 2
// baz
// 3
// qux
const zeroes = [0, 0, 0, 0, 0];
// 用5 填充整个数组
zeroes.fill(5);
console.log(zeroes); // [5, 5, 5, 5, 5]
zeroes.fill(0); // 重置
// 用6 填充索引大于等于3 的元素
zeroes.fill(6, 3);
console.log(zeroes); // [0, 0, 0, 6, 6]
zeroes.fill(0); // 重置
// 用7 填充索引大于等于1 且小于3 的元素
zeroes.fill(7, 1, 3);
console.log(zeroes); // [0, 7, 7, 0, 0];
zeroes.fill(0); // 重置
// 用8 填充索引大于等于1 且小于4 的元素
// (-4 + zeroes.length = 1)
// (-1 + zeroes.length = 4)
zeroes.fill(8, -4, -1); //0 0 0 0 0
console.log(zeroes); // [0, 8, 8, 8, 0];
zeroes.fill(0);
// 索引过低，忽略
zeroes.fill(1, -10, -6);
console.log(zeroes); // [0, 0, 0, 0, 0]
// 索引过高，忽略
zeroes.fill(1, 10, 15);
console.log(zeroes); // [0, 0, 0, 0, 0]
// 索引反向，忽略
zeroes.fill(2, 4, 2);
console.log(zeroes); // [0, 0, 0, 0, 0]
// 索引部分可用，填充可用部分
zeroes.fill(4, 3, 10);
console.log(zeroes); // [0, 0, 0, 4, 4]
let ints,
  reset = () => (ints = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);
reset();
// 从ints中复制索引0 开始的内容，插入到索引5 开始的位置
// 在源索引或目标索引到达数组边界时停止
ints.copyWithin(5);
console.log(ints); // [0, 1, 2, 3, 4, 0, 1, 2, 3, 4]
reset();
// 从ints中复制索引5 开始的内容，插入到索引0 开始的位置
ints.copyWithin(0, 5);
console.log(ints); // [5, 6, 7, 8, 9, 5, 6, 7, 8, 9]
reset();
// 从ints中复制索引0 开始到索引3 结束的内容
// 插入到索引4 开始的位置
ints.copyWithin(4, 0, 3);
alert(ints); // [0, 1, 2, 3, 0, 1, 2, 7, 8, 9]
reset();
// JavaScript引擎在插值前会完整复制范围内的值
// 因此复制期间不存在重写的风险
ints.copyWithin(2, 0, 6);
alert(ints); // [0, 1, 0, 1, 2, 3, 4, 5, 8, 9]
reset();
// 支持负索引值，与fill()相对于数组末尾计算正向索引的过程是一样的
ints.copyWithin(-4, -7, -3); //0 1 2 [3 4 5 6 7) 8 9
alert(ints); // [0, 1, 2, 3, 4, 5, 3, 4, 5, 6]
let colo = ["red", "blue", "green"]; // 创建一个包含3 个字符串的数组
alert(colo.toString()); // red,blue,green
alert(colo.valueOf()); // red,blue,green
alert(colo); // red,blue,green
let person1 = {
  toLocaleString() {
    return "Nikolaos";
  },
  toString() {
    return "Nicholas";
  },
};
let person2 = {
  toLocaleString() {
    return "Grigorios";
  },
  toString() {
    return "Greg";
  },
};
let people = [person1, person2];
alert(people); // Nicholas, Greg
alert(people.toString()); // Nicholas,Greg
alert(people.toLocaleString()); // Nikolaos,Grigorios
alert(colors.join(", ")); // red, green, blue
alert(colors.join("||")); // red||green||blue
// 注意 如果数组中某一项是null或undefined，则在join()、toLocaleString()、toString()和valueOf()返回的结果中会以空字符串表示。
// 栈方法
let colorsA = new Array(); // 创建一个数组
let countA = colorsA.push("red", "green"); // 推入两项
alert(countA); // 2
countA = colorsA.push("black"); // 再推入一项
alert(countA); // 3
let item = colorsA.pop(); // 取得最后一项
alert(item); // black
alert(colorsA.length); // 2
// 队列方法
let colorsB = new Array(); // 创建一个数组
let countB = colorsB.push("red", "green"); // 推入两项
alert(countB); // 2
countB = colorsB.push("black"); // 再推入一项
alert(countB); // 3
let itemB = colorsB.shift(); // 取得第一项
alert(itemB); // red
alert(colorsB.length); // 2
// 排序方法
let values = [1, 2, "13", "15", 4];
values.reverse();
alert(values); // 5,4,3,2,1
let values1 = [0, 1, 5, 10, 15];
values1.sort();
alert(values1); // 0,1,10,15,5
function compare(value1, value2) {
  if (value1 < value2) {
    return -1;
  } else if (value1 > value2) {
    return 1;
  } else {
    return 0;
  }
}
let values2 = [0, 111, 5, 10, 15];
values2.sort(compare);
alert(values2); // 0,1,5,10,15
// 简写
values2.sort((a, b) => (a < b ? 1 : a > b ? -1 : 0));
alert(values2); // 15,10,5,1,0
// 操作方法
let colorsC = ["red", "green", "blue"];
let colorsD = colorsC.concat("yellow", ["black", "brown"]);
console.log(colorsC); // ["red", "green", "blue"]
console.log(colorsD); // ["red", "green", "blue", "yellow", "black", "brown"]
// =======
let colorsQ = ["red", "green", "blue"];
let newColors = ["black", "brown"];
let moreNewColors = {
  [Symbol.isConcatSpreadable]: true,
  length: 2,
  0: "pink",
  1: "cyan",
};
newColors[Symbol.isConcatSpreadable] = false;
// 强制不打平数组
let colorsE = colorsQ.concat("yellow", newColors);
// 强制打平类数组对象
let colorsF = colorsQ.concat(moreNewColors);
console.log(colorsQ); // ["red", "green", "blue"]
console.log(colorsE); // ["red", "green", "blue", "yellow", ["black", "brown"]]
console.log(colorsF); // ["red", "green", "blue", "pink", "cyan"]




