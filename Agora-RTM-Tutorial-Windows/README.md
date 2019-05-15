# Agora RTMing Tutorial for Windows
*其他语言版本： [简体中文](README.zh.md)*

The Agora Windows Sample App is an open-source demo that will help you get message chat integrated directly into your Windows applications using the Agora RTM SDK.

With this sample app, you can:

- Login into RTM Serve
- Send point to point message and receive point to point message off line
- Display point to point history message
- Join channel
- Send channel message, receive channel message
- Leave channel
- Logout RTM server

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID.
Then select the editor in the test project, click App Certificate, and get the App Certificate according to the operation.
Update "Agora-RTM-Tutorial-Windows/AgoraRTM.ini" with your App ID and App Certificate.

```
[BaseInfo]
AppID=
AppCertificatedId=
```
The user can generate a tokenID based on the AppCertificateID, and the tokenID can also be '_no_need_token'

## Developer Environment Requirements
- win7 above
- Visual Studio 2013

## Operating Instructions
- 1.Create SDK directory at the same level of Sln directory
- 2.You need to copy the agora_sig_sdk.dll file in the dLL directory of the SDK to the compiled executable directory (debug / release)

## Connect Us
- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/RTM/issues)

## License
The MIT License (MIT).
