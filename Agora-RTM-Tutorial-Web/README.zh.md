# Agora RTM Tutorial Web

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成 Agora RTM Web SDK，实现消息通讯。

在这个示例项目中包含了以下功能：

- 登录和登出；
- 发送和接收在线点对点消息；
- 加入频道和离开频道；
- 发送和接收频道消息；

## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。将 AppID 填写进 ["config.js"](src/config.js)

```javascript
export const APP_ID = <#Your Agora APPID#>;
```

然后在 [Agora.io SDK](https://www.agora.io/cn/download/) 下载 **RTM SDK**，解压后将其中的 "AgoraRTM-production.js" 文件复制到本项目文件夹下的子路径 "src/assets/rtm.min.js"。

最后。安装该示例的npm依赖并且执行`npm run start`。

## 运行环境
* nodejs 10.0.0+

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题, 你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题, 可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug, 欢迎提交 [issue](https://github.com/AgoraIO/RTM/issues)

## 代码许可

The MIT License (MIT).
