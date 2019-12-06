#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <sstream>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <map>
#include <algorithm>

#include "IAgoraRtmService.h"

using namespace std;

string APP_ID = "";

class RtmEventHandler: public agora::rtm::IRtmServiceEventHandler {
  public:
    RtmEventHandler() {}
    ~RtmEventHandler() {}

    virtual void onLoginSuccess() override {
        cout << "on login success" << endl;
    }

    virtual void onLoginFailure(agora::rtm::LOGIN_ERR_CODE errorCode) override {
        cout << "on login failure: errorCode = " << errorCode << endl;
    }

    virtual void onLogout(agora::rtm::LOGOUT_ERR_CODE errorCode) override {
        cout << "on login out" << endl;
    }

    virtual void onConnectionStateChanged(agora::rtm::CONNECTION_STATE state,
                        agora::rtm::CONNECTION_CHANGE_REASON reason) override {
        cout << "on connection state changed: state = " << state << endl;
    }

    virtual void onSendMessageResult(long long messageId,
                        agora::rtm::PEER_MESSAGE_ERR_CODE state) override {
        cout << "on send message messageId: " << messageId << " state: "
             << state << endl;
    }

    virtual void onMessageReceivedFromPeer(const char *peerId,
                        const agora::rtm::IMessage *message) override {
        cout << "on message received from peer: peerId = " << peerId
             << " message = " << message->getText() << endl;
    }
};

class ChannelEventHandler: public agora::rtm::IChannelEventHandler {
  public:
    ChannelEventHandler(string channel) {
        channel_ = channel;    
    }
    ~ChannelEventHandler() {}

    virtual void onJoinSuccess() override {
        cout << "on join channel success" << endl;
    }

    virtual void onJoinFailure(agora::rtm::JOIN_CHANNEL_ERR errorCode) override{
        cout << "on join channel failure: errorCode = " << errorCode << endl;
    }

    virtual void onLeave(agora::rtm::LEAVE_CHANNEL_ERR errorCode) override {
        cout << "on leave channel: errorCode = " << errorCode << endl;
    }

    virtual void onMessageReceived(const char* userId,
                        const agora::rtm::IMessage *msg) override {
        cout << "receive message from channel: " << channel_.c_str()
             << " user: " << userId << " message: " << msg->getText()
             << endl;
    }

    virtual void onMemberJoined(agora::rtm::IChannelMember *member) override {
        cout << "member: " << member->getUserId() << " joined channel: "
             << member->getChannelId() << endl;
    }

    virtual void onMemberLeft(agora::rtm::IChannelMember *member) override {
        cout << "member: " << member->getUserId() << " lefted channel: "
             << member->getChannelId() << endl;
    }

    virtual void onGetMembers(agora::rtm::IChannelMember **members,
                    int userCount,
                    agora::rtm::GET_MEMBERS_ERR errorCode) override {
        cout << "list all members for channel: " << channel_.c_str()
             << " total members num: " << userCount << endl;
        for (int i = 0; i < userCount; i++) {
            cout << "index[" << i << "]: " << members[i]->getUserId();
        }
    }

    virtual void onSendMessageResult(long long messageId,
                    agora::rtm::CHANNEL_MESSAGE_ERR_CODE state) override {
        cout << "send messageId: " << messageId << " state: " << state << endl;
    }
    
    private:
        string channel_;
};

class Demo {
  public:
    Demo() {
        eventHandler_.reset(new RtmEventHandler());
        agora::rtm::IRtmService* p_rs = agora::rtm::createRtmService();
        rtmService_.reset(p_rs, [](agora::rtm::IRtmService* p) {
            p->release();                                                           
        });                                                                         

        if (!rtmService_) {
            cout << "rtm service created failure!" << endl;
            exit(0);
        }

        if (rtmService_->initialize(APP_ID.c_str(), eventHandler_.get())) {
            cout << "rtm service initialize failure! appid invalid?" << endl;
            exit(0);
        }
    }
    ~Demo() {
        rtmService_->release();
    }

  public:
    bool login() {
        cout << "Please enter userID (literal \"null\" or starting " 
             << "with space is not allowed, no more than 64 charaters!):"
             << endl;
        string userID;
        getline(std::cin, userID);
        if (rtmService_->login(APP_ID.c_str(), userID.c_str())) {
            cout << "login failed!" << endl;
            return false;
        }
        cout << "here" << endl;
        return true;
    }

    void logout() {
        rtmService_->logout();
        cout << "log out!" << endl;
    }

    void p2pChat(const std::string& dst) {
        string msg;
        while(true) {
            cout << "please input message you want to send, or input \"quit\" "
                 << "to leave p2pChat" << endl;
            getline(std::cin, msg);
            if (msg.compare("quit") == 0) {
                return;
            } else {
                sendMessageToPeer(dst, msg);
            }
        }
    }

    void groupChat(const std::string& channel) {
        string msg;
        channelEvent_.reset(new ChannelEventHandler(channel));
        agora::rtm::IChannel * channelHandler =
            rtmService_->createChannel(channel.c_str(), channelEvent_.get());
        if (!channelHandler) {
            cout << "create channel failed!" << endl;
        }
        channelHandler->join();
        while(true) {
            cout << "please input message you want to send, or input \"quit\" "
                 << " to leave groupChat, or input \"members\" to list members"
                 << endl;
            getline(std::cin, msg);
            if (msg.compare("quit") == 0) {
                channelHandler->leave();
                return;
            } else if (msg.compare("members") == 0) {
                channelHandler->getMembers();
            } else {
                sendMessageToChannel(channelHandler, msg);
            }
        }
    }

    void sendMessageToPeer(std::string peerID, std::string msg) {
        agora::rtm::IMessage* rtmMessage = rtmService_->createMessage();
        rtmMessage->setText(msg.c_str());
        int ret = rtmService_->sendMessageToPeer(peerID.c_str(),
                                        rtmMessage);
        rtmMessage->release();
        if (ret) {
            cout << "send message to peer failed! return code: " << ret
                 << endl;
        }
    }

    void sendMessageToChannel(agora::rtm::IChannel * channelHandler,
                            string &msg) {
        agora::rtm::IMessage* rtmMessage = rtmService_->createMessage();
        rtmMessage->setText(msg.c_str());
        channelHandler->sendMessage(rtmMessage);
        rtmMessage->release();
    }

    private:
        std::unique_ptr<agora::rtm::IRtmServiceEventHandler> eventHandler_;
        std::unique_ptr<ChannelEventHandler> channelEvent_;
        std::shared_ptr<agora::rtm::IRtmService> rtmService_;
};

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        cout << "Usage: ./rtmServerDemo <AppID>" << endl;
        exit(-1);
    }
    APP_ID = argv[1];
    int count;
    while(true) {
        cout << "Please input the number which indicates how many RTM instance "
             << "will be created... Limit 3 instances for this demo" << endl;
        string input;
        getline(std::cin, input);
        try {
            count = std::stoi(input);
        } catch (...) {
            cout << "invalid input" << endl;
            continue;
        }
        if (count <= 0 || count > 3) {
            cout << "valid range: 1~3" << endl;
            continue;
        }
        break;
    }

    std::vector<std::unique_ptr<Demo>> DemoList;
    std::vector<bool> loginStatus;
    for (int i = 0; i < count; i++) {
        std::unique_ptr<Demo> tmp;
        tmp.reset(new Demo());
        DemoList.push_back(std::move(tmp));
        loginStatus.push_back(false);
    }
    int index;
    while(true) {
        cout << "Please input which RTM instance you want to use range[1,3]"
             << endl;
        string input_idx;
        getline(std::cin, input_idx);
        try {
            index = std::stoi(input_idx);
        } catch (...) {
            cout << "invalid input" << endl;
            continue;
        }
        if (index < 1 || index > count) {
            cout << "invalid index range" << endl;
            continue;
        }

        while(true) {
            if (!loginStatus[index-1]) {
                if (!DemoList[index-1]->login())
                    continue;
                loginStatus[index-1] = true;
            }
            cout << "1: peer to peer chat\n"
                 << "2: group chat\n"
                 << "3: logout"
                 << endl;
            cout << "please input your choice: " << endl;
            string input_choice;
            getline(std::cin, input_choice);
            int choice = 0;
            try {
                choice = std::stoi(input_choice);
            } catch (...) {
                cout << "invalid input" << endl;
                continue;
            }
            if (choice == 1) {
                cout << "please input your destination user id" << endl;
                string dst;
                getline(std::cin, dst);
                DemoList[index-1]->p2pChat(dst);
                continue;
            } else if (choice == 2) {
                cout << "please input your channel id" << endl;
                string channel;
                getline(std::cin, channel);
                DemoList[index-1]->groupChat(channel);
                continue;
            } else if (choice == 3) {
                DemoList[index-1]->logout();
                loginStatus[index-1] = false;
                break;
            } else {
                continue;
            }
        }

        cout << "Quit the demo? yes/no" << endl;
        string input_quit;
        getline(std::cin, input_quit);
        if (input_quit.compare("yes") == 0) {
            break;
        }         
    }
    
    exit(0);
}



























