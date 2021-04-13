//
//  ChatViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
//

import Cocoa
import AgoraRtmKit

enum ChatType {
    case peer(String), group(String)
    
    var description: String {
        switch self {
        case .peer:  return "peer"
        case .group: return "channel"
        }
    }
}

struct Message {
    var userId: String
    var text: String
}

protocol ChatVCDelegate: NSObjectProtocol {
    func chatVCWillClose(_ vc: ChatViewController)
    func chatVCBackToRootVC(_ vc: ChatViewController)
}

class ChatViewController: ShowAlertProtocol {
    @IBOutlet weak var inputTextField: NSTextField!
    @IBOutlet weak var tableView: NSTableView!
    
    lazy var list = [Message]()
    
    var delegate: ChatVCDelegate?
    var rtmChannel: AgoraRtmChannel?
    var type: ChatType = .peer("unknow")
    
    override func viewDidLoad() {
        ifLoadOfflineMessages()
        AgoraRtm.updateKit(delegate: self)
    }
    
    override func viewDidAppear() {
        switch type {
        case .peer(let name):     self.title = name
        case .group(let channel): self.title = channel; createChannel(channel)
        }
    }
    
    override func viewWillDisappear() {
        leaveChannel()
    }
    
    @IBAction func doSendMsgPressed(_ sender: NSTextField) {
        if pressedReturnToSendText(sender.stringValue) {
            sender.stringValue = ""
        }
    }
    
    @IBAction func doBackPressed(_ sender: NSButton) {
        self.delegate?.chatVCWillClose(self)
    }
}

// MARK: Send Message
private extension ChatViewController {
    func send(message: String, type: ChatType) {
        let sent = { [unowned self] (state: Int) in
            guard state == 0 else {
                self.showAlert("send \(type.description) message error: \(state)")
                return
            }
            
            guard let current = AgoraRtm.current else {
                return
            }
            self.appendMessage(user: current, content: message)
        }
        
        let rtmMessage = AgoraRtmMessage(text: message)
        
        switch type {
        case .peer(let name):
            let option = AgoraRtmSendMessageOptions()
            option.enableOfflineMessaging = (AgoraRtm.oneToOneMessageType == .offline ? true : false)
            
            AgoraRtm.kit?.send(rtmMessage, toPeer: name, sendMessageOptions: option, completion: { (error) in
                sent(error.rawValue)
            })
        case .group(_):
            rtmChannel?.send(rtmMessage) { (error) in
                sent(error.rawValue)
            }
        }
    }
}

// MARK: Chanel
private extension ChatViewController {
    func createChannel(_ channel: String) {
        let errorHandle = { [weak self] (response: NSApplication.ModalResponse) in
            guard let strongSelf = self else {
                return
            }
            strongSelf.delegate?.chatVCWillClose(strongSelf)
        }
        
        guard let rtmChannel = AgoraRtm.kit?.createChannel(withId: channel, delegate: self) else {
            showAlert("join channel fail", handler: errorHandle)
            return
        }
        
        rtmChannel.join { [weak self] (error) in
            if error != .channelErrorOk, let strongSelf = self {
                strongSelf.showAlert("join channel error: \(error.rawValue)", handler: errorHandle)
            }
        }
        
        self.rtmChannel = rtmChannel
    }
    
    func leaveChannel() {
        rtmChannel?.leave { (error) in
            print("leave channel error: \(error.rawValue)")
        }
    }
}

// MARK: AgoraRtmDelegate
extension ChatViewController: AgoraRtmDelegate {
    func rtmKit(_ kit: AgoraRtmKit, connectionStateChanged state: AgoraRtmConnectionState, reason: AgoraRtmConnectionChangeReason) {
        showAlert("connection state changed: \(state.rawValue)") { [weak self] (_) in
            if reason == .remoteLogin, let strongSelf = self {
                strongSelf.delegate?.chatVCBackToRootVC(strongSelf)
            }
        }
    }
    
    func rtmKit(_ kit: AgoraRtmKit, messageReceived message: AgoraRtmMessage, fromPeer peerId: String) {
        appendMessage(user: peerId, content: message.text)
    }
}

// MARK: AgoraRtmChannelDelegate
extension ChatViewController: AgoraRtmChannelDelegate {
    func channel(_ channel: AgoraRtmChannel, memberJoined member: AgoraRtmMember) {
        DispatchQueue.main.async { [unowned self] in
            self.showAlert("\(member.userId) join")
        }
    }
    
    func channel(_ channel: AgoraRtmChannel, memberLeft member: AgoraRtmMember) {
        DispatchQueue.main.async { [unowned self] in
            self.showAlert("\(member.userId) left")
        }
    }
    
    func channel(_ channel: AgoraRtmChannel, messageReceived message: AgoraRtmMessage, from member: AgoraRtmMember) {
        appendMessage(user: member.userId, content: message.text)
    }
}

private extension ChatViewController {
    func ifLoadOfflineMessages() {
        switch type {
        case .peer(let name):
            guard let messages = AgoraRtm.getOfflineMessages(from: name) else {
                return
            }
            
            for item in messages {
                appendMessage(user: name, content: item.text)
            }
            
            AgoraRtm.removeOfflineMessages(from: name)
        default:
            break
        }
    }
    
    func pressedReturnToSendText(_ text: String?) -> Bool {
        guard let text = text, text.count > 0 else {
            return false
        }
        send(message: text, type: type)
        return true
    }
    
    func appendMessage(user: String, content: String) {
        DispatchQueue.main.async { [unowned self] in
            let msg = Message(userId: user, text: content)
            self.list.append(msg)
            if self.list.count > 100 {
                self.list.removeFirst()
            }
            let end = self.list.count - 1
            
            self.tableView.reloadData()
            self.tableView.scrollRowToVisible(end)
        }
    }
}

extension ChatViewController: NSTableViewDelegate, NSTableViewDataSource {
    func numberOfRows(in tableView: NSTableView) -> Int {
        return list.count
    }
    
    func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
        let msg = list[row]
        let type: CellType = msg.userId == AgoraRtm.current ? .right : .left
        let cell = tableView.makeView(withIdentifier: NSUserInterfaceItemIdentifier(rawValue: "MessageCell"), owner: nil) as! MessageCell
        cell.update(type: type, message: msg)
        return cell
    }
    
    func tableView(_ tableView: NSTableView, heightOfRow row: Int) -> CGFloat {
        return 36
    }
}
