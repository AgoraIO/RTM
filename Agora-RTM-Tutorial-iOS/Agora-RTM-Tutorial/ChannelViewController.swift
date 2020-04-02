//
//  ChannelViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit
import AgoraRtmKit

class ChannelViewController: UIViewController, ShowAlertProtocol {
    @IBOutlet weak var channelTextField: UITextField!
    
    override func viewWillAppear(_ animated: Bool) {
        AgoraRtm.updateKit(delegate: self)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        view.endEditing(true)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier, let channel = sender as? String else {
            return
        }
        
        switch identifier {
        case "channelToChat":
            let chatVC = segue.destination as! ChatViewController
            chatVC.type = .group(channel)
        default:
            break
        }
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
    
    @IBAction func doJoinPressed(_ sender: UIButton) {
        guard let channel = channelTextField.text, channel.count > 0 else {
            return
        }
        performSegue(withIdentifier: "channelToChat", sender: channel)
    }
}

extension ChannelViewController: AgoraRtmDelegate {
    func rtmKit(_ kit: AgoraRtmKit, connectionStateChanged state: AgoraRtmConnectionState, reason: AgoraRtmConnectionChangeReason) {
        showAlert("connection state changed: \(state.rawValue)") { [weak self] (_) in
            if reason == .remoteLogin, let strongSelf = self {
                strongSelf.navigationController?.popToRootViewController(animated: true)
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
