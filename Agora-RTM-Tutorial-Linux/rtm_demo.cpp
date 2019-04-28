#include <iostream>
#include <memory>
#include <string>
#include <exception>
#include <unistd.h>

#include "IAgoraService.h"
#include "IAgoraRtmService.h"

// if you want to use this demo. please modify into yourself appid.
const std::string APP_ID = "<your-appid>";


// the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

using namespace agora::rtm;

class DemoRtmEventHandler : public IRtmServiceEventHandler {
public:
  virtual void onLoginSuccess() override {
    cbPrint("on login success");
  }

  virtual void onLoginFailure(LOGIN_ERR_CODE errorCode) override {
    cbPrint("on login failure: %d\n", errorCode);
  }

  virtual void onLogout(LOGOUT_ERR_CODE errorCode) override {
    cbPrint("onLogout");
  }

  virtual void onConnectionStateChanged(CONNECTION_STATE state,
                                    CONNECTION_CHANGE_REASON reason)
                                    override{
    cbPrint("onConnectionStateChanged: %d", state);
  }

  virtual void onSendMessageResult(long long messageId, 
                                PEER_MESSAGE_ERR_CODE errorCode) override {
    cbPrint("onSendMessageResult messageID: %llderrorcode: %d",
            messageId, errorCode);
  }

  void onMessageReceivedFromPeer(const char *peerId,
                                 const IMessage *message) override {
    if (!peerId || !message) {
        return;
    }
    cbPrint("onMessageReceivedFromPeer peerId: %s message: %s",
            peerId, message->getText());
  }

 private:
  void cbPrint(const char* fmt, ...) {
    printf("\x1b[32m<< RTM async callback: ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(" >>\x1b[0m\n");
  }
};

class DemoChannelEventHandler : public IChannelEventHandler
{
 public:
  DemoChannelEventHandler() {}

  ~DemoChannelEventHandler() {}

  void SetChannel(const std::string& channel) {
    channel_ = channel;
  }

 public:
  virtual void onJoinSuccess() override {
    cbPrint("join channel succeed");
  }

  virtual void onJoinFailure(JOIN_CHANNEL_ERR errorCode)
                             override {
    cbPrint("join channel failed errorcode: ", errorCode);
  }


  virtual void onLeave(LEAVE_CHANNEL_ERR errorCode) override {
    cbPrint("leave channel reason code: ", errorCode);
  }

  virtual void onMessageReceived(const char* userId,
                                 const IMessage *message) override {
    cbPrint("recived message from channel: %s srcuser:%s message: %s",
                                                            channel_.c_str(),
                                                            userId,
                                                            message->getText());
  }

  virtual void onSendMessageResult(long long messageId,
                                  CHANNEL_MESSAGE_ERR_CODE state)
                                  override {
    cbPrint("channel message send result messageid:%d state:%d ", messageId,
             state);
  }

  virtual void onMemberJoined(IChannelMember *member) override {
    cbPrint("channel member joined channel:%s member:%s ",
             member->getChannelId(), member->getUserId());
  }

  virtual void onMemberLeft(IChannelMember *member) override {
    cbPrint("channel member left channel:%s member:%s ",
             member->getChannelId(), member->getUserId());
  }

  virtual void onGetMembers(IChannelMember **members,
                            int userCount, 
                            GET_MEMBERS_ERR errorCode) override {
    cbPrint("show channel:%s members num:%d", channel_.c_str(), userCount);
    for (int index = 0; index < userCount; index++) {
      cbPrint(" member :%s", members[index]->getUserId());
    }
  }

 private:
  void cbPrint(const char* fmt, ...) {
    printf("\x1b[32m<< RTM async callback: ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf(" >>\x1b[0m\n");
  }

 private:
  std::string channel_;
};

class DemoMessaging {
public:
  DemoMessaging()
      : agoraService_(createAgoraService()),
        eventHandler_(new DemoRtmEventHandler()),
        channelEvent_(new DemoChannelEventHandler()),
        rtmService_(nullptr) { Init(); }

  void Init() {
    if (!agoraService_) {
      std::cout << RED << "error creating agora service!" << std::endl;
      exit(0);
  }
  int ret = agoraService_->initialize(context_);
  if (ret) {
    std::cout << RED << "error initializing agora service: "
              << ret << std::endl;
    exit(0);
  }
  rtmService_ = agoraService_->createRtmService();
  if (!rtmService_) {
    std::cout << RED <<"error creating rtm service!" << std::endl;
    exit(0);
  }
  ret = rtmService_->initialize(APP_ID.c_str(), eventHandler_.get());
  if (ret) {
    std::cout << RED <<  "error initializing rtm service: " << ret << std::endl;
    exit(0);
  }
}

  void login() {
    while (true) {
      std::cout << YELLOW << "Please enter userID (literal \"null\" or starting"
          << "with space is not allowed, no more than 64 charaters!):"
          << std::endl;
      std::string userID;
      std::getline(std::cin, userID);
      int ret = rtmService_->login(APP_ID.c_str(), userID.c_str());
      std::cout << BOLDBLUE << "login ret:" << ret << std::endl;
      if (ret) {
        login();
      }
      mainMeun();
      std::cout << YELLOW << "quit ? yes/no" << std::endl;
      std::string input;
      std::getline(std::cin, input);
      if (input.compare("yes") == 0) {
        exit(0);
      }
      }
  }

  void logout() {
    int ret = rtmService_->logout();
    std::cout << BOLDBLUE <<"logout ret: " << ret << std::endl;
  }

  void sendMessageToPeer(std::string peerID, std::string message) {
    IMessage* rtmMessage = rtmService_->createMessage();
    rtmMessage->setText(message.c_str());
    int ret = rtmService_->sendMessageToPeer(peerID.c_str(), rtmMessage);
    std::cout << BOLDBLUE << "sendMessageToPeer ret:" << ret << std::endl;
  }

  void sendMessageToChannel(IChannel * channelHandler,
                            std::string& msg) {
    IMessage* rtmMessage = rtmService_->createMessage();
    rtmMessage->setText(msg.c_str());
    channelHandler->sendMessage(rtmMessage);
  }

  void mainMeun() {
    bool quit  = false;
    while (!quit) {
      std::cout << WHITE << "1: p2p\n"
                << "2: group\n"
                << "3: logout" <<std::endl;
      std::cout << YELLOW <<"please input your choice" << std::endl;
      std::string input;
      std::getline(std::cin, input);
      int32_t choice = 0;
      try {
        choice = std::stoi(input);
      } catch(...) {
        std::cout <<RED << "invalid input" << std::endl;
        continue;
      }
      switch (choice)
      {
        case 1: {
          std::cout << YELLOW << "please input dst user id" << std::endl;
          std::string dst;
          std::getline(std::cin, dst);
          p2pChat(dst);
        }
          break;
        case 2: {
          std::cout << YELLOW << "please input channel id" << std::endl;
          std::string channel;
          std::getline(std::cin, channel);
          groupChat(channel);
        }
          break;
        case 3: {
          logout();
          return;
        }
        break;
        default:
          break;
      }
    }
  }

  void p2pChat(const std::string& dst) {
    std::string message;
    while (true) {
      std::cout << YELLOW << "please input message "
                << "or input \"quit\" to leave p2pchat"
                << std::endl;
      std::getline(std::cin, message);
      if (message.compare("quit") == 0) {
        return;
      } else {
        sendMessageToPeer(dst, message);
      }
    }
  }

  void groupChat(const std::string& channel) {
    std::string message;
    channelEvent_->SetChannel(channel);
    IChannel * channelHandler =
        rtmService_->createChannel(channel.c_str(), channelEvent_.get());
    if (channelHandler == nullptr) {
      std::cout << RED << "err create channel failed." << std::endl;
    }
    channelHandler->join();
    while (true) {
      std::cout << YELLOW << "please input message "
                << "or input \"quit\" to leave groupchat,"
                << "or input members select members "
                << std::endl;
      std::getline(std::cin, message);
      if (message.compare("quit") == 0) {
        channelHandler->leave();
        return;
      } else if (message.compare("members") == 0) {
        channelHandler->getMembers();
      } else {
        sendMessageToChannel(channelHandler, message);
      }
    }
  }

 private:
    std::unique_ptr<agora::base::IAgoraService> agoraService_;
    agora::base::AgoraServiceContext context_;
    std::unique_ptr<IRtmServiceEventHandler> eventHandler_;
    std::unique_ptr<DemoChannelEventHandler> channelEvent_;
    IRtmService* rtmService_;
};

int main(int argc, const char * argv[]) {
  DemoMessaging messaging;
  messaging.login();
  return 0;
}
