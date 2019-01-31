//
//  ChatViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora.IO. All rights reserved.
//

import UIKit
import AgoraRtmKit

enum ChatType {
    case peer(String), group(String)
}

struct Message {
    var userId: String
    var text: String
}

class ChatViewController: UIViewController {
    @IBOutlet weak var inputTextField: UITextField!
    @IBOutlet weak var inputViewBottom: NSLayoutConstraint!
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var inputContainView: UIView!
    
    lazy var list = [Message]()
    
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
//        addKeyboradObserver()
        updateViews()
        AgoraRtm.updateKit(delegate: self)
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
        showAlert("\(member.userId) join")
    }
    
    func rtmKit(_ kit: AgoraRtmKit, channel: AgoraRtmChannel, memberLeft member: AgoraRtmMember) {
        showAlert("\(member.userId) left")
    }
    
    func rtmKit(_ kit: AgoraRtmKit, channel: AgoraRtmChannel, messageReceived message: AgoraRtmMessage, from member: AgoraRtmMember) {
        appendMsg(user: member.userId, content: message.text)
    }
}

private extension ChatViewController {
    func createChannel(_ channel: String) {
        AgoraRtm.kit?.createChannel(withId: channel, delegate: self)
    }
    
    func sendPeer(_ peer: String, msg: String) {
        let message = AgoraRtmMessage(text: msg)
        AgoraRtm.kit?.send(message, toPeer: peer, completion: { [unowned self] (state) in
            print("send peer state: \(state.rawValue)")
            
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
            guard let current = AgoraRtm.current else {
                return
            }
            self.appendMsg(user: current, content: msg)
        }
    }
}

private extension ChatViewController {
    func updateViews() {
        tableView.rowHeight = UITableView.automaticDimension
        tableView.estimatedRowHeight = 55
    }
    
    func appendMsg(user: String, content: String) {
        let msg = Message(userId: user, text: content)
        list.append(msg)
        let end = IndexPath(row: list.count - 1, section: 0)
        DispatchQueue.main.async { [unowned self] in
            self.tableView.reloadData()
            self.tableView.scrollToRow(at: end, at: .bottom, animated: true)
        }
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
                strongSelf.inputContainView.transform = CGAffineTransform(translationX: 0, y: -offsetY)
            } else {
                strongSelf.inputContainView.transform = .identity
            }
        }
    }
    #endif
}

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
        if let text = textField.text, text.count > 0 {
            switch type {
            case .peer(let name):     sendPeer(name, msg: text)
            case .group(let channel): sendChannel(channel, msg: text)
            }
        }

        return true
    }
}
