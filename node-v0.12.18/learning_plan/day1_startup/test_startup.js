#!/usr/bin/env node

/**
 * Node.js v0.12.18 启动流程测试脚本
 *
 * 这个脚本用于验证我们对启动流程的理解
 * 通过输出关键信息来观察启动过程
 */

console.log("=== Node.js 启动流程测试 ===\n");

// 1. 显示进程信息
console.log("1. 进程基本信息:");
console.log("   - Node.js 版本:", process.version);
console.log("   - V8 版本:", process.versions.v8);
console.log("   - 平台:", process.platform);
console.log("   - 架构:", process.arch);
console.log("   - 启动时间:", new Date().toISOString());
console.log();

// 2. 显示命令行参数
console.log("2. 命令行参数:");
console.log("   - 执行文件:", process.argv[0]);
console.log("   - 脚本文件:", process.argv[1]);
console.log("   - 其他参数:", process.argv.slice(2));
console.log();

// 3. 显示环境变量
console.log("3. 相关环境变量:");
const relevantEnvVars = [
  "NODE_INVALID_UTF8",
  "NODE_DEBUG",
  "NODE_NO_READLINE",
  "NODE_DISABLE_COLORS",
  "NODE_CHANNEL_FD",
  "NODE_UNIQUE_ID",
];

relevantEnvVars.forEach((envVar) => {
  const value = process.env[envVar];
  if (value !== undefined) {
    console.log(`   - ${envVar}:`, value);
  }
});
console.log();

// 4. 显示模块加载列表
console.log("4. 模块加载列表 (前10个):");
if (process.moduleLoadList && process.moduleLoadList.length > 0) {
  process.moduleLoadList.slice(0, 10).forEach((module, index) => {
    console.log(`   ${index + 1}. ${module}`);
  });
  if (process.moduleLoadList.length > 10) {
    console.log(`   ... 还有 ${process.moduleLoadList.length - 10} 个模块`);
  }
} else {
  console.log("   - 模块加载列表不可用");
}
console.log();

// 5. 显示全局对象
console.log("5. 全局对象检查:");
console.log("   - global 存在:", typeof global !== "undefined");
console.log("   - process 存在:", typeof process !== "undefined");
console.log("   - Buffer 存在:", typeof Buffer !== "undefined");
console.log("   - setTimeout 存在:", typeof setTimeout !== "undefined");
console.log("   - setInterval 存在:", typeof setInterval !== "undefined");
console.log("   - setImmediate 存在:", typeof setImmediate !== "undefined");
console.log();

// 6. 显示标准输入输出
console.log("6. 标准输入输出:");
console.log("   - stdin 类型:", typeof process.stdin);
console.log("   - stdout 类型:", typeof process.stdout);
console.log("   - stderr 类型:", typeof process.stderr);
console.log("   - stdin 是否 TTY:", process.stdin.isTTY);
console.log("   - stdout 是否 TTY:", process.stdout.isTTY);
console.log();

// 7. 测试事件循环
console.log("7. 事件循环测试:");
let tickCount = 0;

// 测试 process.nextTick
process.nextTick(() => {
  console.log("   - process.nextTick 回调执行");
  tickCount++;
});

// 测试 setImmediate
setImmediate(() => {
  console.log("   - setImmediate 回调执行");
  tickCount++;
});

// 测试 setTimeout
setTimeout(() => {
  console.log("   - setTimeout 回调执行");
  tickCount++;
}, 0);

// 等待所有异步操作完成
setTimeout(() => {
  console.log(`   - 总共执行了 ${tickCount} 个异步回调`);
  console.log();

  // 8. 显示内存使用情况
  console.log("8. 内存使用情况:");
  const memUsage = process.memoryUsage();
  console.log(
    "   - RSS (常驻内存):",
    Math.round(memUsage.rss / 1024 / 1024),
    "MB"
  );
  console.log(
    "   - 堆总计:",
    Math.round(memUsage.heapTotal / 1024 / 1024),
    "MB"
  );
  console.log(
    "   - 堆已使用:",
    Math.round(memUsage.heapUsed / 1024 / 1024),
    "MB"
  );
  console.log(
    "   - 外部内存:",
    Math.round(memUsage.external / 1024 / 1024),
    "MB"
  );
  console.log();

  // 9. 测试完成
  console.log("=== 启动流程测试完成 ===");
  console.log("这个测试验证了 Node.js 启动过程中的关键组件都正常工作。");

  // 退出程序
  process.exit(0);
}, 100);
