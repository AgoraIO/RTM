# Agora Miniapp RTM Tutorial

*Read this in other languages [English](README.md)*

## 简介

本 Demo 基于 Agora Miniapp RTM SDK 开发，帮助开发者在微信小程序中实现基础的实时消息互动功能。

本程序演示如下内容：

* 集成 Agora Miniapp RTM SDK
* 登陆账户
* 加入/离开 频道，发送频道消息，查询频道成员
* 发送/接收 点对点在线消息，点对点离线消息
* 登出账户

## 准备开发环境
1. 请确保本地已安装微信开发者工具

## 运行示例程序
1. 在 [Agora.io](http://dashboard.agora.io/signin/) 注册账号，并创建自己的测试项目，获取 App ID。如需获取 Token 或 Channel Key，请启用 App Certificate

2. 下载本页示例程序

3. 打开 *utils* 文件夹，在 *config.js* 文件中填入获取到的 App ID：
  const APPID = 'abcdefg'

4. 在 Agora 官网下载 RTM sdk 压缩包，并将文件保存在本示例程序的 *lib* 文件夹下

5. 启动微信开发者工具并导入该示例程序

## 关于 Token/Dynamic Key

如果启用了 App Certificate，还需要在服务端生成 Token 或 Dynamic Key 用于鉴权。将生成的 Token 或 Dynamic Key 填入如下方法中：

    	//...
    	client.join(<your key/access token here>, channel, uid, () => {
    	//...
    	
关于如何生成 Token 或 Dynamic Key 详见 [Token](https://docs.agora.io/cn/2.2/product/Video/Agora%20Basics/key_native?platform=Android) 或 [Dynamic Key](https://docs.agora.io/cn/2.2/product/Video/Agora%20Basics/key_web?platform=Web)

## 代码许可

MIT 许可 (MIT)