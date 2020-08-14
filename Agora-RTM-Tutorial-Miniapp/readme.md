# Agora Miniapp RTM Tutorial

*Read this in other languages [English](README.md)*

## Introduction

This demo is developed based on Agora miniapp RTM SDK to help developers realize basic real-time message interaction function in wechat applet.

This program demonstrates the following contents:

* integrate Agora Miniapp RTM SDK
* Login account
* Join / leave channel, send channel message, query channel member
* Send / receive peer-to-peer online messages, peer-to-peer offline messages
* Log out of account

## Preparing the Developer Environment
1. Please make sure the wechat developer tool is installed locally

## Running the App
1. Create a developer account at [Agora.io](http://dashboard.agora.io/signin/), obtain an App ID, and enable the App Certificate.

2. Download this project.

3. Fill in the App ID in *config.js* in the *utils* folder of this project:
    const APPID = 'abcdefg'

4. Download RTM SDK package from Agora official website, and save the file in the * lib * folder of this sample program.

5. Start the wechat developer tool and import the sample program.

## About Token/Dynamic Key

If you have enabled the App Cerficate, you will need to generate the Token/Dynamic Key at the server for authentication purposes. Use it in the following method:

    //...
    client.join(<your key/access token here>, channel, uid, () => {
    //...
    	
See [Token](https://docs.agora.io/en/2.2/product/Video/Agora%20Basics/key_native?platform=Android) or [Dynamic Key](https://docs.agora.io/en/2.2/product/Video/Agora%20Basics/key_web?platform=Web) for generating the Token or Key at the server.

## License

The MIT License (MIT).
