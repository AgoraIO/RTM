# Agora Java RTM Tutorial

*Read this in other languages: [English](README.md)*

这个开源示例项目演示了如何快速集成 Agora RTM (Real Time Messaging) SDK 实现收发消息功能。

在这个示例项目中包含了以下功能：

- 登录 RTM 服务器
- 发送和接收点对点消息
- 加入频道
- 发送和接收频道消息
- 获取频道成员列表
- 离开频道
- 注销 RTM 登录

## 运行示例程序
首先在 [Agora.io 注册](https://dashboard.agora.io/cn/signup/) 注册账号，并创建自己的测试项目，获取到 AppID。

将 AppID 填写进 RtmJavaDemo.java 中的 APPID.APP_ID

```
public static final String APP_ID = "<#YOUR APP ID#>";

```

## 集成方式
- 第1步: 在 Agora.io SDK 下载 RTM SDK，解压后将其中的 libs 文件夹下的 *.jar, *.so 复制到本项目的 lib/ 下
- 第2步: 将demo依赖的jar包安装到本地maven仓库, mvn install:install-file -Dfile=lib/agora_rtm.jar -DgroupId=io.agora.rtm  -DartifactId=agora-rtm-sdk -Dversion=1.0 -Dpackaging=jar
- 第3步: 使用maven编译打包, 在pom.xml所在目录运行 “mvn package”
- 第4步: 运行Demo, “java -cp target/RTM-Client-Demo-1.0-SNAPSHOT.jar -Dsun.boot.library.path=lib/ io.agora.mainClass.RtmJavaDemo”


## 运行环境
- 物理或虚拟, Ubuntu Linux 14.04 LTS 64 位 及以上
- apache-maven-3.6.0

## 联系我们
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题, 你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题, 可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
- 如果发现了示例代码的 bug, 欢迎提交 [issue](https://github.com/AgoraIO/Rtm/issues)

## 代码许可
The MIT License (MIT).
