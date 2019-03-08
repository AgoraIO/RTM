# Agora Java RTM Tutorial

*其他语言版本： [简体中文](README.zh.md)*

The Agora Java RTM Tutorial is an open-source demo that will help you get message chat integrated directly into your Java applications using the Agora RTM SDK.

With this sample app, you can:

- Login RTM server
- Send and receive point to point message online
- Join channel
- Send and receive channel message
- Get channel member list
- Leave channel
- Logout RTM server

## Running the App
First, create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID.
Update APPID.APP_ID of RtmJavaDemo.java with your App ID .

```
public static final String APP_ID = "<#YOUR APP ID#>";

```
## Integration mode
- Step 1: Download the RTM SDK in Agora.io SDK. After decompressing, copy the *.jar and *.so under the LIBS folder to the lib/ of this project.
- Step 2: Install the dependency jar package which demo required into maven local repo, use maven command "mvn install:install-file -Dfile=lib/agora_rtm.jar -DgroupId=io.agora.rtm  -DartifactId=agora-rtm-sdk -Dversion=1.0 -Dpackaging=jar".
- Step 3: Compile and pack the project to *.jar, use "mvn package" under the directory where pom.xml locates. It may take some time to generate the package.
- Step 4: Run the demo， use "java -cp target/RTM-Client-Demo-1.0-SNAPSHOT.jar -Dsun.boot.library.path=lib/ io.agora.mainClass.RtmJavaDemo".

## Developer Environment Requirements
- Physical or virtual, more than Ubuntu Linux 14.04 LTS 64 bits
- apache-maven-3.6.0

## Connect Us
- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/RTM/issues)

## License
The MIT License (MIT).
