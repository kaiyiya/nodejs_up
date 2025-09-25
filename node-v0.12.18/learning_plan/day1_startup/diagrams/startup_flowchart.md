# Node.js v0.12.18 启动流程图

## 主启动流程图

```mermaid
graph TD
    A[程序启动] --> B{操作系统}
    B -->|Windows| C[wmain函数<br/>处理宽字符参数]
    B -->|Unix/Linux| D[main函数<br/>直接启动]
    
    C --> E[node::Start函数]
    D --> E
    
    E --> F[环境准备阶段]
    F --> F1[设置UTF-8标志]
    F --> F2[安装信号处理器]
    F --> F3[处理命令行参数]
    
    F1 --> G[Init函数调用]
    F2 --> G
    F3 --> G
    
    G --> G1[记录启动时间]
    G --> G2[禁用stdio继承]
    G --> G3[初始化异步调试]
    G --> G4[设置V8选项]
    G --> G5[解析命令行参数]
    
    G1 --> H[V8引擎初始化]
    G2 --> H
    G3 --> H
    G4 --> H
    G5 --> H
    
    H --> H1[设置熵源]
    H --> H2[V8::Initialize]
    H --> H3[创建V8隔离器]
    
    H1 --> I[环境创建阶段]
    H2 --> I
    H3 --> I
    
    I --> I1[创建V8上下文]
    I --> I2[创建Environment对象]
    I --> I3[设置微任务处理]
    I --> I4[初始化libuv句柄]
    
    I1 --> J[调试器支持]
    I2 --> J
    I3 --> J
    I4 --> J
    
    J --> J1{启用调试器?}
    J1 -->|是| J2[StartDebug]
    J1 -->|否| K[LoadEnvironment]
    J2 --> K
    
    K --> K1[设置V8错误处理]
    K --> K2[执行node.js文件]
    K --> K3[创建全局对象]
    K --> K4[调用启动函数]
    
    K1 --> L[JavaScript启动]
    K2 --> L
    K3 --> L
    K4 --> L
    
    L --> L1[设置进程对象]
    L --> L2[配置全局变量]
    L --> L3[设置定时器]
    L --> L4[配置控制台]
    L --> L5[设置错误处理]
    L --> L6[配置标准I/O]
    L --> L7[设置信号处理]
    
    L1 --> M{运行模式判断}
    L2 --> M
    L3 --> M
    L4 --> M
    L5 --> M
    L6 --> M
    L7 --> M
    
    M -->|脚本文件| M1[Module.runMain]
    M -->|REPL模式| M2[启动REPL]
    M -->|调试模式| M3[启动调试器]
    M -->|eval模式| M4[执行代码]
    
    M1 --> N[事件循环启动]
    M2 --> N
    M3 --> N
    M4 --> N
    
    N --> N1[uv_run循环]
    N --> N2[处理I/O事件]
    N --> N3[执行回调函数]
    N --> N4[处理微任务]
    
    N1 --> O{循环继续?}
    N2 --> O
    N3 --> O
    N4 --> O
    
    O -->|是| N1
    O -->|否| P[退出处理]
    
    P --> P1[EmitBeforeExit]
    P --> P2[EmitExit]
    P --> P3[RunAtExit]
    P --> P4[环境清理]
    P --> P5[V8清理]
    
    P1 --> Q[程序结束]
    P2 --> Q
    P3 --> Q
    P4 --> Q
    P5 --> Q
```

## 详细组件交互图

```mermaid
graph LR
    subgraph "C++层"
        A[main/wmain] --> B[node::Start]
        B --> C[Init函数]
        B --> D[V8::Initialize]
        B --> E[CreateEnvironment]
        B --> F[LoadEnvironment]
        B --> G[uv_run循环]
    end
    
    subgraph "V8引擎"
        D --> H[V8隔离器]
        H --> I[V8上下文]
        I --> J[全局对象]
    end
    
    subgraph "libuv"
        K[事件循环] --> L[I/O句柄]
        L --> M[异步操作]
    end
    
    subgraph "Environment"
        E --> N[环境对象]
        N --> O[进程绑定]
        N --> P[模块系统]
    end
    
    subgraph "JavaScript层"
        F --> Q[node.js脚本]
        Q --> R[启动函数]
        R --> S[模块加载]
        S --> T[用户代码]
    end
    
    B --> K
    E --> N
    F --> Q
    G --> K
    H --> I
    N --> O
    Q --> R
```

## 关键函数调用时序图

```mermaid
sequenceDiagram
    participant Main as main/wmain
    participant Start as node::Start
    participant Init as Init
    participant V8 as V8引擎
    participant Create as CreateEnvironment
    participant Load as LoadEnvironment
    participant JS as JavaScript环境
    participant UV as libuv循环
    
    Main->>Start: 调用启动函数
    Start->>Start: 环境准备
    Start->>Init: 初始化参数
    Init->>Init: 解析命令行
    Init->>V8: 设置选项
    Start->>V8: 初始化V8
    V8-->>Start: 初始化完成
    Start->>Create: 创建环境
    Create->>Create: 设置V8上下文
    Create->>Create: 初始化句柄
    Create-->>Start: 环境创建完成
    Start->>Load: 加载JavaScript环境
    Load->>Load: 设置错误处理
    Load->>JS: 执行node.js
    JS->>JS: 启动函数
    JS-->>Load: 启动完成
    Load-->>Start: 加载完成
    Start->>UV: 启动事件循环
    UV->>UV: 处理I/O事件
    UV->>JS: 执行回调
    JS-->>UV: 回调完成
    UV-->>Start: 循环结束
    Start->>Start: 清理资源
    Start-->>Main: 程序结束
```

## 内存和对象管理图

```mermaid
graph TD
    subgraph "V8对象管理"
        A[V8隔离器] --> B[HandleScope]
        B --> C[V8上下文]
        C --> D[全局对象]
        D --> E[内置模块]
        E --> F[用户模块]
    end
    
    subgraph "Environment对象"
        G[Environment实例] --> H[事件循环引用]
        G --> I[进程绑定]
        G --> J[模块缓存]
        G --> K[定时器管理]
    end
    
    subgraph "libuv句柄"
        L[事件循环] --> M[I/O句柄]
        M --> N[定时器句柄]
        M --> O[异步句柄]
        M --> P[信号句柄]
    end
    
    A --> G
    G --> L
    C --> I
    H --> M
```

## 错误处理流程

```mermaid
graph TD
    A[错误发生] --> B{错误类型}
    B -->|JavaScript错误| C[V8错误处理]
    B -->|系统错误| D[libuv错误处理]
    B -->|致命错误| E[致命错误处理]
    
    C --> C1[TryCatch捕获]
    C --> C2[错误信息格式化]
    C --> C3[堆栈跟踪]
    
    D --> D1[errno处理]
    D --> D2[系统调用错误]
    
    E --> E1[OnFatalError]
    E --> E2[进程退出]
    
    C1 --> F[错误报告]
    C2 --> F
    C3 --> F
    D1 --> F
    D2 --> F
    
    F --> G{错误被处理?}
    G -->|是| H[继续执行]
    G -->|否| I[进程退出]
    
    E1 --> I
    E2 --> I
```

这个流程图展示了 Node.js v0.12.18 的完整启动过程，从程序入口到事件循环运行的全过程。每个阶段都有详细的说明和关键函数调用。

