//
//  PeerChannelViewController.swift
//  Agora-Rtm-Tutorial-Mac
//
//  Created by CavanSu on 2019/1/31.
//  Copyright © 2019 Agora. All rights reserved.
//

import Cocoa
import AgoraRtmKit

protocol PeerChannelVCDelegate: NSObjectProtocol {
    func peerChannelVCWillClose(_ vc: PeerChannelViewController)
}

class PeerChannelViewController: NSViewController, ShowAlertProtocol {
    @IBOutlet weak var peerTextField: NSTextField!
    @IBOutlet weak var channelTextField: NSTextField!
    
    var delegate: PeerChannelVCDelegate?
    
    override func viewWillAppear() {
        AgoraRtm.updateKit(delegate: self)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    override func prepare(for segue: NSStoryboardSegue, sender: Any?) {
        guard let _ = segue.identifier, let type = sender as? ChatType else {
            return
        }
        let chatVC = segue.destinationController as! ChatViewController
        chatVC.type = type
        chatVC.delegate = self
    }
    
    @IBAction func doChatPressed(_ sender: NSButton) {
        let peer = peerTextField.stringValue
        guard peer.count > 0 else {
            return
        }
        let type = ChatType.peer(peer)
        performSegue(withIdentifier: "peerChannelToChat", sender: type)
    }
    
    @IBAction func doJoinPressed(_ sender: NSButton) {
        let channel = channelTextField.stringValue
        guard  channel.count > 0 else {
            return
        }
        let type = ChatType.group(channel)
        performSegue(withIdentifier: "peerChannelToChat", sender: type)
    }
    
    @IBAction func doBackPressed(_ sender: NSButton) {
        self.delegate?.peerChannelVCWillClose(self)
    }
}

extension PeerChannelViewController: AgoraRtmDelegate {
    func rtmKit(_ kit: AgoraRtmKit, connectionStateChanged state: AgoraRtmConnectionState, reason: AgoraRtmConnectionChangeReason) {
        showAlert("connection state changed: \(state.rawValue)") { [weak self] (_) in
            if reason == .remoteLogin, let strongSelf = self {
                strongSelf.delegate?.peerChannelVCWillClose(strongSelf)
            }
        }
    }
    
    // Receive one to one offline messages
    func rtmKit(_ kit: AgoraRtmKit, messageReceived message: AgoraRtmMessage, fromPeer peerId: String) {
        AgoraRtm.add(offlineMessage: message, from: peerId)
    }
    
    func rtmKit(_ kit: AgoraRtmKit, imageMessageReceived message: AgoraRtmImageMessage, fromPeer peerId: String) {
        AgoraRtm.add(offlineMessage: message, from: peerId)
    }
}

extension PeerChannelViewController: ChatVCDelegate {
    func chatVCWillClose(_ vc: ChatViewController) {
        vc.view.window?.contentViewController = self
    }
    
    func chatVCBackToRootVC(_ vc: ChatViewController) {
        self.delegate?.peerChannelVCWillClose(self)
    }
}
