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

protocol ChatVCDelegate: NSObjectProtocol {
    func chatVCWillClose(_ vc: ChatViewController)
    func chatVCBackToRootVC(_ vc: ChatViewController)
}

class ChatViewController: NSViewController, ShowAlertProtocol {
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
    
    @IBAction func sendImage(_ sender: Any) {
        guard let imagePath = Bundle.main.path(forResource: "image", ofType: "png") else {
            return
        }
        
        var requestId: Int64 = 0
        AgoraRtm.kit?.createImageMessage(byUploading: imagePath, withRequest: &requestId, completion: {[weak self] (requestId, message, errorCode) in
            
            guard let `self` = self, let `message` = message else {
                return
            }
            
            // thumbnailImage 8KB
            if let thumbnailImage = self.generateThumbnail(imagePath: imagePath, toByte: 8 * 1024) {
                
                if let imageData = self.getJPGImageData(image: thumbnailImage) {
                    message.thumbnail = imageData
                    message.thumbnailWidth = Int32(thumbnailImage.size.width)
                    message.thumbnailHeight = Int32(thumbnailImage.size.height)
                }  
            }
            
            if(errorCode != .ok) {
                self.showAlert("send image message error: \(errorCode)")
                return
            }
            self.send(rtmMessage: message, type: self.type)
        })
    }
    
    func getJPGImageData(image: NSImage) -> Data? {
        
        guard let data = image.tiffRepresentation else {
            return nil
        }
        
        let imageRep = NSBitmapImageRep(data: data)
        let imageProps: [NSBitmapImageRep.PropertyKey: Any] = [.compressionFactor: 1]
        return imageRep?.representation(using: .jpeg, properties: imageProps)
    }
    
    func generateThumbnail(imagePath: String, toByte maxLength: UInt) -> NSImage? {
        
        guard let image = NSImage(contentsOfFile: imagePath), var imageData = self.getJPGImageData(image: image) else {
            return nil
        }
        
        // If the original image is already small, no thumbnail is needed
        if(imageData.count <= maxLength) {
            return nil
        }

        var resultImage = image
        var lastDataLength = 0
        
        while (imageData.count > maxLength && imageData.count != lastDataLength) {
            lastDataLength = imageData.count
            let ratio: CGFloat = CGFloat(maxLength) / CGFloat(imageData.count)
            let size: CGSize = CGSize(width: Int(resultImage.size.width * sqrt(ratio)), height: Int(resultImage.size.height * sqrt(ratio)))// Use Int to prevent white blank
            
            let newImage = NSImage(size: size)
            newImage.lockFocus()
            let fromRect = NSRect(x: 0, y: 0, width: resultImage.size.width, height: resultImage.size.height)
            let toRect = NSRect(x: 0, y: 0, width: size.width, height: size.height)
            resultImage.draw(in: toRect, from: fromRect, operation: .copy, fraction: 1.0)
            newImage.unlockFocus()
            
            if let _imageData = self.getJPGImageData(image: newImage) {
                imageData = _imageData
            } else {
                return nil
            }
            
            resultImage = newImage;
        }

        return resultImage
    }
}

// MARK: Send Message
private extension ChatViewController {
    func send(rtmMessage: AgoraRtmMessage, type: ChatType) {
        
        let sent = { [weak self] (state: Int) in
            
            guard let `self` = self else {
                return
            }
            
            guard state == 0 else {
                self.showAlert("send \(type.description) message error: \(state)")
                return
            }
            
            guard let current = AgoraRtm.current else {
                return
            }
            
            if(rtmMessage.type == .text){
                self.appendMessage(user: current, content: rtmMessage.text, mediaId: nil, thumbnail: nil)
            } else if(rtmMessage.type == .image){
                if let imageMessage = rtmMessage as? AgoraRtmImageMessage {
                    self.appendMessage(user: current, content: nil, mediaId: imageMessage.mediaId, thumbnail:imageMessage.thumbnail)
                }
            }
        }
        
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
    
    func send(message: String, type: ChatType) {
        let rtmMessage = AgoraRtmMessage(text: message)
        send(rtmMessage: rtmMessage, type: type)
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
        if(state.rawValue == 4){
            
        }
        showAlert("connection state changed: \(state.rawValue)") { [weak self] (_) in
            if reason == .remoteLogin, let strongSelf = self {
                strongSelf.delegate?.chatVCBackToRootVC(strongSelf)
            }
        }
    }
    
    func rtmKit(_ kit: AgoraRtmKit, messageReceived message: AgoraRtmMessage, fromPeer peerId: String) {
        appendMessage(user: peerId, content: message.text, mediaId: nil, thumbnail:nil)
    }
    
    func rtmKit(_ kit: AgoraRtmKit, imageMessageReceived message: AgoraRtmImageMessage, fromPeer peerId: String) {
        appendMessage(user: peerId, content: nil, mediaId: message.mediaId, thumbnail: message.thumbnail)
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
        appendMessage(user: member.userId, content: message.text, mediaId: nil, thumbnail:nil)
    }
    
    func channel(_ channel: AgoraRtmChannel, imageMessageReceived message: AgoraRtmImageMessage, from member: AgoraRtmMember) {
        appendMessage(user: member.userId, content: nil, mediaId: message.mediaId, thumbnail:message.thumbnail)
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
                if(item.type == .text){
                    appendMessage(user: name, content: item.text, mediaId: nil, thumbnail: nil)
                } else if(item.type == .image){
                    if let imageMessage = item as? AgoraRtmImageMessage {
                        appendMessage(user: name, content: nil, mediaId: imageMessage.mediaId, thumbnail:imageMessage.thumbnail)
                    }
                }
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
    
    func appendMessage(user: String, content: String?, mediaId: String?, thumbnail: Data?) {
        DispatchQueue.main.async { [unowned self] in
            
            let msg = Message(userId: user, text: content, mediaId: mediaId, thumbnail: thumbnail)
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

        if let _ = msg.mediaId {
            let cell = tableView.makeView(withIdentifier: NSUserInterfaceItemIdentifier(rawValue: "ImageMessageCell"), owner: nil) as! ImageMessageCell
            cell.update(type: type, message: msg)
            return cell
            
        } else {
            let cell = tableView.makeView(withIdentifier: NSUserInterfaceItemIdentifier(rawValue: "MessageCell"), owner: nil) as! MessageCell
            cell.update(type: type, message: msg)
            return cell
        }
    }
    
    func tableView(_ tableView: NSTableView, heightOfRow row: Int) -> CGFloat {
        let msg = list[row]
        if let _ = msg.mediaId {
            return 212
        }
       
        return 36
    }
    
    func tableViewSelectionDidChange(_ notification: Notification) {
        let row = self.tableView.selectedRow
        self.tableView.deselectRow(row)
    }
}
