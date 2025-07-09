function alert(str) {
  console.log(str);
}
// slice
let colors = ["red", "green", "blue", "yellow", "purple"];
let colors2 = colors.slice(1);
let colors3 = colors.slice(1, 4);
alert(colors2); // green, blue, yellow, purple
alert(colors3); // green, blue, yellow
// splice
let colors1 = ["red", "green", "blue"];
let removed = colors1.splice(0, 1); // 删除第一项
alert(colors1); // green, blue
alert(removed); // red，只有一个元素的数组
removed = colors1.splice(1, 0, "yellow", "orange"); // 在位置1 插入两个元素
alert(colors1); // green, yellow, orange, blue
alert(removed); // 空数组
removed = colors1.splice(1, 1, "red", "purple"); // 插入两个值，删除一个元素
alert(colors1); // green,red,purple,orange,blue
alert(removed); // yellow，只有一个元素的数组
//  搜索和位置方法
let numbers = [1, 2, 3, 4, 5, 4, 3, 2, 1];
alert(numbers.indexOf(4)); // 3
alert(numbers.lastIndexOf(4)); // 5
alert(numbers.includes(4)); // true
alert(numbers.indexOf(4, 4)); // 5
alert(numbers.lastIndexOf(4, 4)); // 3
alert(numbers.includes(4, 7)); // false
let person = { name: "Nicholas" };
let people = [{ name: "Nicholas" }];
let morePeople = [person];
alert(people.indexOf(person)); // -1
alert(morePeople.indexOf(person)); // 0
alert(people.includes(person)); // false
alert(morePeople.includes(person)); // true

const people1 = [
  {
    name: "Matt",
    age: 27,
  },
  {
    name: "Nicholas",
    age: 29,
  },
];
alert(people1.find((element, index, array) => element.age < 28));
// {name: "Matt", age: 27}
alert(people1.findIndex((element, index, array) => element.age < 28));
// 0
const evens = [2, 4, 6];
// 找到匹配后，永远不会检查数组的最后一个元素
evens.find((element, index, array) => {
  console.log(element);
  console.log(index);
  console.log(array);
  return element === 4;
});
// 2
// 0
// [2, 4, 6]
// 4
// 1
// [2, 4, 6]
// 迭代方法
let numbers1 = [1, 2, 3, 4, 5, 4, 3, 2, 1];
let everyResult = numbers1.every((item, index, array) => item > 2);
alert(everyResult); // false
let someResult = numbers1.some((item, index, array) => item > 2);
alert(someResult); // true
let filterResult = numbers1.filter((item, index, array) => item > 2);
alert(filterResult); // 3,4,5,4,3
let mapResult = numbers1.map((item, index, array) => item * 2);
alert(mapResult); // 2,4,6,8,10,8,6,4,2
numbers1.forEach((item, index, array) => {});
// 归并
let values = [1, 2, 3, 4, 5];
let sum = values.reduce((prev, cur, index, array) => prev + cur);
alert(sum); // 15
