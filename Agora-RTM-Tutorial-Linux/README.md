# Agora Linux RTM Tutorial

*其他语言版本： [简体中文](README_zh.md)*

The Agora Linux-RTM-Tutorial Sample is an open-source demo that will help you get message chat integrated directly into your Linux applications using the Agora RTM SDK.

With this sample app, you can:

- Login RTM server
- Enable single-instance for RTM
- Send point to point message and receive point to point message
- Join channel
- Send channel message, receive channel message
- Get member list of the channel
- Leave channel
- Logout RTM server

## Running the App
- step 1:create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID.
- step 2:download the RTM sdk, extract the packet and copy 'libagora_rtm_sdk.so' to our project's 'lib' folder,and also copy 'AgoraBase.h','IAgoraRtmService.h','IAgoraService.h' to our project's 'include' folder.
> For RTM 1.4.0 and 1.4.1, you need to rename 'libagora_rtm_sdk.so' to 'libagora-rtm-sdk-jni.so' to make it work.
- step 3:open rtm_demo.cpp write yourself AppId,
- step 4:open our project run clean_build.sh, and according to the prompt to compile,then cd build floder run ./rtmDemo according to the prompt please.

## Developer Environment Requirements
- Physical or virtual, more than Ubuntu Linux 14.04 LTS 64 bits


## Connect Us
- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Signaling/issues)

## License
The MIT License (MIT).
