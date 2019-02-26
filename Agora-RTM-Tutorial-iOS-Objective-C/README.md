# Agora RTM Tutorial iOS Objective C

*其他语言版本： [简体中文](README.zh.md)*

The Agora RTM Tutorial iOS Objective C Sample App is an open-source demo that will help you get message chat integrated directly into your iOS applications using the Agora RTM SDK.

With this sample app, you can:

- Log in / log out
- Send and receive Peer-to-Peer message online
- Join / leave channel
- Send and receive channel message

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "AppId.m" with your App ID.

```
return <#YOUR APPID#>;
```

Next, download the **Agora RTM SDK** from [Agora.io SDK](https://www.agora.io/en/download/). Unzip the downloaded SDK package and copy the "libs/AgoraRtmKit.framework" file to the project folder's sub path "Agora-Rtm-Tutorial/AgoraRtmKit.framework".

Finally, Open Agora-RTM-Tutorial.xcodeproj, connect your iPhone／iPad device, setup your development signing and run.

## Developer Environment Requirements
* XCode 8.0 +
* Real devices (iPhone or iPad)
* iOS simulator is supported

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/RTM/issues)

## License

The MIT License (MIT).
