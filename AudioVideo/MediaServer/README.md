# 直播服务器



## 协议介绍

- `RTMP` (Real Time Messaging Protocol)，实时消息传输协议。
- `HLS` (HTTP Live Streaming)，苹果公司提出的基于HTTP的流媒体网络传输协议。
- `HTTP-FLV`，将RTMP等负载信息携带在HTTP协议之上的码流传输协议。

|              | RTMP           | HTTP-FLV         | HLS      |
| ------------ | -------------- | ---------------- | -------- |
| 传输协议     | TCP            | HTTP             | HTTP     |
| 视频封装格式 | flv            | flv              | ts       |
| 延时         | 1-3秒          | 1-3秒            | 5-20秒   |
| Web支持      | H5需要使用插件 | H5中需要使用插件 | 支持H5   |
| 数据         | 连续流         | 连续流           | 切片文件 |



## HLS

`HLS` (HTTP Live Streaming)是一个由苹果公司提出的基于 `HTTP` 的流媒体网络传输协议。

它的工作原理是把整个流分成一个个小的基于 `HTTP` 的 `TS` 文件来下载，每次只下载一些。当媒体流正在播放时，客户端可以选择从许多不同的备用源中以不同的速率下载同样的资源，允许流媒体会话适应不同的数据速率。

`HLS` 请求基本的 `HTTP` 报文，`HLS` 可以穿过任何允许 `HTTP` 数据通过的防火墙或者代理服务器。它也很容易使用内容分发网络来传输媒体流。

- 视频的封装格式是 `TS`。
- 音视频采用 `H264` 编码和 `AAC` 编码。
- 除了 `TS` 视频文件本身，还定义了用来控制播放的 `m3u8` 索引文件。

```cpp
// 1. 客户端请求 m3u8 文件
// 2. 根据 m3u8 文件内容依次请求 ts 文件
```



## RTMP

`RTMP`（Real Time Messaging Protocol，实时消息传输协议）。

该协议基于 `TCP`，是一个协议族，包括 `RTMP` 基本协议及`RTMPT/RTMPS/RTMPE` 等多种变种。

- 应用层协议，依靠 `TCP` 保证可靠传输。
- 默认端口：`1953`，可能被防火墙屏蔽。
- 在流媒体/交互服务器之间进行音视频和数据通信。



## HTTP-FLV

`HTTP-FLV` 和 `RTMP` 类似，都是针对于 `FLV` 视频格式做的直播分发流。

- 直接发起长连接，下载对应的 `FLV` 文件
- 头部信息简单

```cpp
// HTTP 发送 FLV 数据即可
```
