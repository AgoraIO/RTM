# Agora Miniapp RTM Tutorial

*English [中文](README.zh.md)*

## Introduction

This demo is developed based on Agora miniapp RTM SDK to help developers realize basic real-time message interaction function in wechat applet.

This program demonstrates the following contents:

* Integrate Agora Miniapp RTM SDK
* Login account
* Join or leave channel, send or receive channel message, query channel member
* Send or receive peer-to-peer online messages, peer-to-peer offline messages
* Logout account

## Preparing the Developer Environment
1. Please make sure the wechat developer tool is installed locally

## Running the App
1. Create a developer account at [Agora.io](http://dashboard.agora.io/signin/), obtain an App ID, and to get Token, enable App Certificate.

2. Download this project.

3. Fill in the App ID in *config.js* in the *utils* folder of this project:
    const APPID = 'abcdefg'

4. Download RTM SDK package from Agora official website, and save the file in the * lib * folder of this sample program. Rename it rtm.miniapp.js 

5. Start the wechat developer tool and import the sample program.

## Resources

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/RTM/issues)

## License

The MIT License (MIT).
