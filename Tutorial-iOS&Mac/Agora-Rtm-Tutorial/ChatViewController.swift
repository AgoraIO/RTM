//
//  ChatViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora.IO. All rights reserved.
//

#if os(iOS)
import UIKit
#else
import Cocoa
#endif
import AgoraRtmKit

enum ChatType {
    case peer(String), group(String)
}

struct Message {
    var userId: String
    var text: String
}

#if os(macOS)
protocol ChatVCDelegate: NSObjectProtocol {
    func chatVCWillClose(_ vc: ChatViewController)
}
#endif

class ChatViewController: AGViewController {
    @IBOutlet weak var inputTextField: AGTextField!
    @IBOutlet weak var inputViewBottom: NSLayoutConstraint!
    @IBOutlet weak var tableView: AGTableView!
    @IBOutlet weak var inputContainView: AGView!
    
    lazy var list = [Message]()
    
    #if os(macOS)
    var delegate: ChatVCDelegate?
    #endif
    
    var type: ChatType = .peer("unknow") {
        didSet {
            switch type {
            case .peer(let name): self.title = name
            case .group(let channel): self.title = channel; createChannel(channel)
            }
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        #if os(iOS)
        addKeyboradObserver()
        #endif
        updateViews()
        AgoraRtm.updateKit(delegate: self)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        leaveChannel()
    }
    
    func leaveChannel() {
        var lChannel: String
        switch type {
        case .group(let channel): lChannel = channel
        default: return
        }

        guard let rtmChannels = AgoraRtm.kit?.channels, let rtmChannel = rtmChannels[lChannel] as? AgoraRtmChannel else {
            return
        }
        rtmChannel.leave { (error) in
            print("leave channel error: \(error.rawValue)")
        }
    }
    
    #if os(macOS)
    @IBAction func doSendMsgPressed(_ sender: NSTextField) {
        if pressedReturnToSendText(sender.text) {
            sender.text = ""
        }
    }
    @IBAction func doBackPressed(_ sender: NSButton) {
        self.delegate?.chatVCWillClose(self)
    }
    #endif
    
    @discardableResult func pressedReturnToSendText(_ text: String?) -> Bool {
        guard let text = text, text.count > 0 else {
                return false
        }
        
        switch type {
        case .peer(let name):     sendPeer(name, msg: text)
        case .group(let channel): sendChannel(channel, msg: text)
        }
        return true
    }
}

extension ChatViewController: AgoraRtmDelegate {
    func rtmKit(_ kit: AgoraRtmKit, connectionStateChanged state: AgoraRtmConnectionState) {
        print("connection state changed: \(state.rawValue)")
    }
    
    func rtmKit(_ kit: AgoraRtmKit, messageReceived message: AgoraRtmMessage, fromPeer peerId: String) {
        appendMsg(user: peerId, content: message.text)
    }
}

extension ChatViewController: AgoraRtmChannelDelegate {
    func rtmKit(_ kit: AgoraRtmKit, channel: AgoraRtmChannel, memberJoined member: AgoraRtmMember) {
        DispatchQueue.main.async { [unowned self] in
            self.showAlert("\(member.userId) join")
        }
    }
    
    func rtmKit(_ kit: AgoraRtmKit, channel: AgoraRtmChannel, memberLeft member: AgoraRtmMember) {
        DispatchQueue.main.async { [unowned self] in 
            self.showAlert("\(member.userId) left")
        }
    }
    
    func rtmKit(_ kit: AgoraRtmKit, channel: AgoraRtmChannel, messageReceived message: AgoraRtmMessage, from member: AgoraRtmMember) {
        appendMsg(user: member.userId, content: message.text)
    }
}

private extension ChatViewController {
    func createChannel(_ channel: String) {
        guard let rtmChannel = AgoraRtm.kit?.createChannel(withId: channel, delegate: self) else {
            return
        }
        rtmChannel.join { (error) in
            print("join channel error: \(error.rawValue)")
        }
    }
    
    func sendPeer(_ peer: String, msg: String) {
        let message = AgoraRtmMessage(text: msg)
        AgoraRtm.kit?.send(message, toPeer: peer, completion: { [unowned self] (state) in
            print("send peer msg state: \(state.rawValue)")
            
            guard let current = AgoraRtm.current else {
                return
            }
            self.appendMsg(user: current, content: msg)
        })
    }
    
    func sendChannel(_ channel: String, msg: String) {
        guard let rtmChannels = AgoraRtm.kit?.channels, let rtmChannel = rtmChannels[channel] as? AgoraRtmChannel else {
            return
        }
        let message = AgoraRtmMessage(text: msg)
        rtmChannel.send(message) { [unowned self] (state) in
            print("send channel msg state: \(state.rawValue)")
            
            guard let current = AgoraRtm.current else {
                return
            }
            self.appendMsg(user: current, content: msg)
        }
    }
}

private extension ChatViewController {
    func updateViews() {
        #if os(iOS)
        tableView.rowHeight = UITableView.automaticDimension
        tableView.estimatedRowHeight = 55
        #endif
    }
    
    func appendMsg(user: String, content: String) {
        let msg = Message(userId: user, text: content)
        list.append(msg)
        #if os(iOS)
        let end = IndexPath(row: list.count - 1, section: 0)
        DispatchQueue.main.async { [unowned self] in
            self.tableView.reloadData()
            self.tableView.scrollToRow(at: end, at: .bottom, animated: true)
        }
        #else
        let end = list.count - 1
        DispatchQueue.main.async { [unowned self] in
            self.tableView.insertRows(at: IndexSet(integer: end), withAnimation: NSTableView.AnimationOptions())
            self.tableView.scrollRowToVisible(end)
        }
        #endif
    }
    
    func showAlert(_ message: String) {
        #if os(iOS)
        let alert = UIAlertController(title: nil, message: message, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
        present(alert, animated: true, completion: nil)
        #else
        let alert = NSAlert()
        alert.messageText = message
        alert.addButton(withTitle: "OK")
        alert.alertStyle = .informational
        
        guard let window = NSApplication.shared.windows.first else {
            return
        }
        
        alert.beginSheetModal(for: window, completionHandler: nil)
        #endif
    }
    
    #if os(iOS)
    func addKeyboradObserver() {
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(keyboardFrameWillChange(notification:)),
                                               name: UIResponder.keyboardWillChangeFrameNotification, object: nil)
    }
    
    @objc func keyboardFrameWillChange(notification: NSNotification) {
        guard let userInfo = notification.userInfo,
            let endKeyboardFrameValue = userInfo[UIResponder.keyboardFrameEndUserInfoKey] as? NSValue,
            let durationValue = userInfo[UIResponder.keyboardAnimationDurationUserInfoKey] as? NSNumber else {
                return
        }
        
        let endKeyboardFrame = endKeyboardFrameValue.cgRectValue
        let duration = durationValue.doubleValue
        
        let isShowing: Bool = endKeyboardFrame.maxY > UIScreen.main.bounds.height ? false : true
        
        UIView.animate(withDuration: duration) { [weak self] in
            guard let strongSelf = self else {
                return
            }
            
            if isShowing {
                let offsetY = strongSelf.inputContainView.frame.maxY - endKeyboardFrame.minY
                guard offsetY > 0 else {
                    return
                }
                strongSelf.inputViewBottom.constant = -offsetY
            } else {
                strongSelf.inputViewBottom.constant = 0
            }
        }
    }
    #endif
}

#if os(iOS)
extension ChatViewController: UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return list.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let msg = list[indexPath.row]
        let type: CellType = msg.userId == AgoraRtm.current ? .right : .left
        let cell = tableView.dequeueReusableCell(withIdentifier: "MessageCell", for: indexPath) as! MessageCell
        cell.update(type: type, message: msg)
        return cell
    }
}

extension ChatViewController: UITextFieldDelegate {
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        if pressedReturnToSendText(textField.text) {
            textField.text = nil
        } else {
            view.endEditing(true)
        }
        
        return true
    }
}

#else
extension ChatViewController: NSTableViewDelegate, NSTableViewDataSource {
    func numberOfRows(in tableView: NSTableView) -> Int {
        return list.count
    }
    
    func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
        let msg = list[row]
        let type: CellType = msg.userId == AgoraRtm.current ? .right : .left
        let cell = tableView.makeView(withIdentifier: NSUserInterfaceItemIdentifier(rawValue: "MessageCell"), owner: self) as! MessageCell
        cell.update(type: type, message: msg)
        return cell
    }
    
    func tableView(_ tableView: NSTableView, heightOfRow row: Int) -> CGFloat {
        let defaultHeight: CGFloat = 39
        let viewWidth = view.bounds.size.width - (32 + 16 + 16 + 50)
        let msg = list[row]
        let string: NSString = msg.text as NSString

        let textRect = string.boundingRect(with: NSMakeSize(viewWidth, 0), options: [NSString.DrawingOptions.usesLineFragmentOrigin], attributes: [NSAttributedString.Key.font: NSFont.systemFont(ofSize: 13)])

        var textHeight = textRect.height + 6 + 6
        
        if textHeight <= defaultHeight {
            textHeight = defaultHeight;
        }
        return textHeight;
    }
}
#endif
