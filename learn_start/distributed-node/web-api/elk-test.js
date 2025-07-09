const net = require('net');
const { faker } = require('@faker-js/faker'); // 用于生成模拟数据

// 配置参数 (根据实际情况调整)
const HOST = '192.168.126.6'; // Logstash服务地址
const PORT = 4560;        // 对应Logstash TCP端口
const SEND_INTERVAL = 2000; // 发送间隔(毫秒)

// 创建TCP客户端
const client = new net.Socket();

client.connect(PORT, HOST, () => {
    console.log(`已连接到 ${HOST}:${PORT}`);
    console.log('开始发送模拟日志 (按Ctrl+C停止)...\n');
});

// 错误处理
client.on('error', (err) => {
    console.error('连接错误:', err.message);
});

// 生成模拟日志函数
function generateLog() {
    return JSON.stringify({
        timestamp: new Date().toISOString(),
        level: ['DEBUG', 'INFO', 'WARN', 'ERROR'][Math.floor(Math.random() * 4)],
        message: faker.lorem.sentence(),
        service: 'test-service',
        user: {
            id: faker.string.uuid(),
            name: faker.person.fullName(),
            ip: faker.internet.ipv4()
        },
        data: {
            value: faker.number.int({ min: 0, max: 100 }),
            duration: faker.number.float({ min: 0.1, max: 5.0, precision: 0.1 })
        }
    });
}

// 定期发送日志
setInterval(() => {
    const logEntry = generateLog();
    console.log(`发送日志: ${logEntry}`);

    // 发送日志 + 换行符 (Logstash需要分行处理)
    client.write(logEntry + '\n');
}, SEND_INTERVAL);

// 优雅退出
process.on('SIGINT', () => {
    console.log('\n停止发送日志');
    client.end();
    process.exit();
});