//
//  PeerChannelViewController.swift
//  Agora-Rtm-Tutorial-Mac
//
//  Created by CavanSu on 2019/1/31.
//  Copyright © 2019 Agora. All rights reserved.
//

import Cocoa

protocol PeerChannelVCDelegate: NSObjectProtocol {
    func peerChannelVCWillClose(_ vc: PeerChannelViewController)
}

class PeerChannelViewController: NSViewController {
    @IBOutlet weak var peerTextField: NSTextField!
    @IBOutlet weak var channelTextField: NSTextField!
    
    var delegate: PeerChannelVCDelegate?
    
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

extension PeerChannelViewController: ChatVCDelegate {
    func chatVCWillClose(_ vc: ChatViewController) {
        vc.view.window?.contentViewController = self
    }
}
