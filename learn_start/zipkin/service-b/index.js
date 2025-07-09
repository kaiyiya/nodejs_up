const express = require('express');
const { Tracer, BatchRecorder, ExplicitContext } = require('zipkin');
const { HttpLogger } = require('zipkin-transport-http');
const zipkinMiddleware = require('zipkin-instrumentation-express').expressMiddleware;

// 创建 Zipkin 追踪器
const tracer = new Tracer({
    ctxImpl: new ExplicitContext(),
    recorder: new BatchRecorder({
        logger: new HttpLogger({
            endpoint: 'http://zipkin:9411/api/v2/spans'
        })
    }),
    localServiceName: 'service-b'
});

const app = express();
const port = 4000;

// 添加 Zipkin 中间件
app.use(zipkinMiddleware({ tracer }));

// 模拟数据库调用
function mockDatabaseCall() {
    return new Promise((resolve) => {
        setTimeout(() => {
            resolve({ data: 'DB result', items: [1, 2, 3] });
        }, 150);
    });
}

// API 端点
app.get('/api', async (req, res) => {
    const traceId = req.traceId.traceId;

    try {
        console.log(`[Service-B] Received request (Trace ID: ${traceId})`);

        // 记录自定义标签
        tracer.recordBinary('request.source', 'service-a');

        // 模拟数据库调用
        const dbResult = await mockDatabaseCall();

        // 添加自定义注释
        tracer.recordAnnotation('Database operation completed');

        res.json({
            service: 'service-b',
            traceId,
            timestamp: new Date().toISOString(),
            dbResult
        });
    } catch (error) {
        tracer.recordBinary('error', error.message);
        res.status(500).json({ error: 'Internal Server Error' });
    }
});

app.listen(port, () => {
    console.log(`Service B running on http://localhost:${port}`);
});