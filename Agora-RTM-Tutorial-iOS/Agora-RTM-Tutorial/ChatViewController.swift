//
//  ChatViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
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
    var channel: AgoraRtmChannel?
    
    var type: ChatType = .peer("unknow") {
        didSet {
            switch type {
            case .peer(let name):
                self.title = name
            case .group(let channel):
                self.title = channel;
                createChannel(channel)
            }
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        addKeyboardObserver()
        updateViews()
        AgoraRtm.updateKit(delegate: self)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        leaveChannel()
    }
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    
    @discardableResult func pressedReturnToSendText(_ text: String?) -> Bool {
        guard let text = text, text.count > 0 else {
                return false
        }
        
        switch type {
        case .peer(let name):     sendPeer(name, msg: text)
        case .group(_): sendChannelMessage(text)
        }
        return true
    }
}

// MARK: Peer
extension ChatViewController: AgoraRtmDelegate {
    func rtmKit(_ kit: AgoraRtmKit, connectionStateChanged state: AgoraRtmConnectionState) {
        print("connection state changed: \(state.rawValue)")
    }
    
    func rtmKit(_ kit: AgoraRtmKit, messageReceived message: AgoraRtmMessage, fromPeer peerId: String) {
        appendMsg(user: peerId, content: message.text)
    }
}

// MARK: Channel
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
        self.channel = rtmChannel
    }
    
    func leaveChannel() {
        guard let channel = channel else {
            return
        }
        channel.leave { (error) in
            print("leave channel error: \(error.rawValue)")
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
    
    func sendChannelMessage(_ msg: String) {
        guard let rtmChannel = self.channel else {
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
        let alert = UIAlertController(title: nil, message: message, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
        present(alert, animated: true, completion: nil)
    }
    
    func addKeyboardObserver() {
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
            strongSelf.view.layoutIfNeeded()
        }
    }
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
        if pressedReturnToSendText(textField.text) {
            textField.text = nil
        } else {
            view.endEditing(true)
        }
        
        return true
    }
}
