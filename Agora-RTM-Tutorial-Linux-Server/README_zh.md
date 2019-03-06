# Agora Linux-Server RTM Tutorial

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成 Agora RTM SDK 实现发消息功能。

在这个示例项目中包含了以下功能：

- 登录RTM服务器
- 创建RTM多实例，切换多实例
- 发送和接收点对点消息
- 加入频道
- 发送频道消息，接收频道消息
- 获取频道成员列表
- 离开频道
- 注销RTM登录

## 集成方式&运行示例程序
- 首先：在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID.
- 其次：在 Agora.io SDK 下载RTM SDK，解压后将libs中libagora_rtm_sdk.so文件复制到本项目的lib下，也将include中AgoraBase.h，IAgoraRtmService.h，IAgoraService.h 头文件也复制到本项目的include文件夹下
- 最后：打开src文件夹，运行./build.sh，根据提示进行编译，编译完后再运行./rtmServerDemo，根据提示填入相应信息即可,如: ./rtmServerDemo appid

## 运行环境
- 物理或虚拟, Ubuntu Linux 14.04 LTS 64 位 及以上

## 联系我们
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题, 你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题, 可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug, 欢迎提交 [issue](https://github.com/AgoraIO/Signaling/issues)

## 代码许可
The MIT License (MIT).
