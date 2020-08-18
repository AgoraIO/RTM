# Agora Miniapp RTM Tutorial

*Read this in other languages [English](README.md)*

## 简介

本 Demo 基于 Agora Miniapp RTM SDK 开发，帮助开发者在微信小程序中实现基础的实时消息互动功能。

本程序演示如下内容：

* 集成 Agora Miniapp RTM SDK
* 登陆账户
* 加入/离开 频道，发送/接收 频道消息，查询频道成员
* 发送/接收 点对点在线消息，点对点离线消息
* 登出账户

## 准备开发环境
1. 请确保本地已安装微信开发者工具

## 运行示例程序
1. 在 [Agora.io](http://dashboard.agora.io/signin/) 注册账号，并创建自己的测试项目，获取 App ID。如需获取 Token，请启用 App Certificate

2. 下载本页示例程序

3. 打开 *utils* 文件夹，在 *config.js* 文件中填入获取到的 App ID：
  const APPID = 'abcdefg'

4. 在 Agora 官网下载 RTM sdk 压缩包，并将文件保存在本示例程序的 *lib* 文件夹下，并重命名为 rtm.miniapp.js

5. 启动微信开发者工具并导入该示例程序

## 联系我们

- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果发现了示例代码的 bug, 欢迎提交 [issue](https://github.com/AgoraIO/RTM/issues)

## 代码许可

MIT 许可 (MIT)