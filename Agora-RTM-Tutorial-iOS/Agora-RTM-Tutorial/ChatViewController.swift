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

class ChatViewController: UIViewController, ShowAlertProtocol {
    @IBOutlet weak var inputTextField: UITextField!
    @IBOutlet weak var inputViewBottom: NSLayoutConstraint!
    @IBOutlet weak var tableView: UITableView!
    @IBOutlet weak var inputContainView: UIView!
    
    lazy var list = [Message]()
    
    var type: ChatType = .peer("unknow")
    var rtmChannel: AgoraRtmChannel?
    
    override func viewDidLoad() {
        addKeyboardObserver()
        updateViews()
        AgoraRtm.updateKit(delegate: self)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        switch type {
        case .peer(let name):     self.title = name
        case .group(let channel): self.title = channel; createChannel(channel)
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        leaveChannel()
    }
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
}

// MARK: Send Message
private extension ChatViewController {
    func send(message: String, type: ChatType) {
        let sent = { [unowned self] (state: Int) in
            guard state == 0 else {
                self.showAlert("send \(type.description) message error: \(state)", handler: { (_) in
                    self.view.endEditing(true)
                })
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
            AgoraRtm.kit?.send(rtmMessage, toPeer: name, completion: { (error) in
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
        let errorHandle = { [weak self] (action: UIAlertAction) in
            guard let strongSelf = self else {
                return
            }
            strongSelf.navigationController?.popViewController(animated: true)
        }
        
        guard let rtmChannel = AgoraRtm.kit?.createChannel(withId: channel, delegate: self) else {
            showAlert("join channel fail", handler: errorHandle)
            return
        }
        
        rtmChannel.join { [weak self] (error) in
            if error != .ok, let strongSelf = self {
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
                strongSelf.navigationController?.popToRootViewController(animated: true)
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
    func updateViews() {
        tableView.rowHeight = UITableView.automaticDimension
        tableView.estimatedRowHeight = 55
    }
    
    func pressedReturnToSendText(_ text: String?) -> Bool {
        guard let text = text, text.count > 0 else {
            return false
        }
        send(message: text, type: type)
        return true
    }
    
    func appendMessage(user: String, content: String) {
        let msg = Message(userId: user, text: content)
        list.append(msg)
        let end = IndexPath(row: list.count - 1, section: 0)
        DispatchQueue.main.async { [unowned self] in
            self.tableView.reloadData()
            self.tableView.scrollToRow(at: end, at: .bottom, animated: true)
        }
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
