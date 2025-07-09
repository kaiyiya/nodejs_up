const express = require('express');
const axios = require('axios');
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
    localServiceName: 'service-a'
});

const app = express();
const port = 3000;

// 添加 Zipkin 中间件
app.use(zipkinMiddleware({ tracer }));

// 调用 Service B 的端点
async function callServiceB(traceId) {
    try {
        // 创建子 span
        const childSpan = tracer.createChildId();
        tracer.setId(childSpan);

        console.log(`[Service-A] Calling Service-B (Trace ID: ${traceId})`);

        // 添加追踪头
        const headers = {};
        tracer.inject(childSpan, 'HTTP_HEADERS', headers);

        const response = await axios.get('http://service-b:4000/api', { headers });
        return response.data;
    } catch (error) {
        console.error('Error calling Service-B:', error.message);
        throw error;
    }
}

// 主端点
app.get('/start', async (req, res) => {
    // 获取当前追踪ID
    const traceId = req.traceId.traceId;

    try {
        // 记录自定义标签
        tracer.recordBinary('user.id', 'user-12345');
        tracer.recordBinary('request.path', req.path);

        const result = await callServiceB(traceId);

        // 添加自定义注释
        tracer.recordAnnotation('Service-B call completed');

        res.json({
            traceId,
            message: 'Request completed',
            serviceBResponse: result
        });
    } catch (error) {
        tracer.recordBinary('error', error.message);
        res.status(500).json({ error: 'Internal Server Error' });
    }
});

app.listen(port, () => {
    console.log(`Service A running on http://localhost:${port}`);
});