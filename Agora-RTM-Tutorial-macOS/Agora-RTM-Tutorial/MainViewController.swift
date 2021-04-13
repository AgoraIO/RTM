//
//  MainViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 CavanSu. All rights reserved.
//

import Cocoa
import AgoraRtmKit

class ShowAlertProtocol: NSViewController {
    func showAlert(_ message: String, handler: ((NSApplication.ModalResponse) -> Void)?) {
        let alert = NSAlert()
        alert.messageText = message
        alert.addButton(withTitle: "OK")
        alert.alertStyle = .informational
        guard let window = NSApplication.shared.windows.first else {
            return
        }
        alert.beginSheetModal(for: window, completionHandler: handler)
    }
    
    func showAlert(_ message: String) {
        showAlert(message, handler: nil)
    }
}

class MainViewController: ShowAlertProtocol {
    @IBOutlet weak var accountTextField: NSTextField!
    @IBOutlet weak var enableOneToOneBox: NSButton!
    
    override func viewWillAppear() {
        logout()
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    override func prepare(for segue: NSStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier, identifier == "mainToTab" else {
            return
        }
        
        let peerChannelVC = segue.destinationController as! PeerChannelViewController
        peerChannelVC.delegate = self
    }
    
    @IBAction func doLoginPressed(_ sender: NSButton) {
        login()
    }
}

private extension MainViewController {
    func login() {
        let account = accountTextField.stringValue
        guard account.count > 0 else{
            return
        }
        
        AgoraRtm.updateKit(delegate: self)
        AgoraRtm.current = account
        AgoraRtm.oneToOneMessageType = enableOneToOneBox.state == .on ? .offline : .normal

        AgoraRtm.kit?.login(byToken: nil, user: account) { [unowned self] (errorCode) in
            guard errorCode == .ok else {
                self.showAlert("login error: \(errorCode.rawValue)")
                return
            }
            
            AgoraRtm.status = .online
            
            DispatchQueue.main.async {
                self.performSegue(withIdentifier: "mainToTab", sender: nil)
            }
        }
    }
    
    func logout() {
        guard AgoraRtm.status == .online else {
            return
        }
        
        AgoraRtm.kit?.logout(completion: { (error) in
            guard error == .ok else {
                return
            }
            
            AgoraRtm.status = .offline
        })
    }
}

extension MainViewController: AgoraRtmDelegate {
    // Receive one to one offline messages
    func rtmKit(_ kit: AgoraRtmKit, messageReceived message: AgoraRtmMessage, fromPeer peerId: String) {
        AgoraRtm.add(offlineMessage: message, from: peerId)
    }
}

extension MainViewController: PeerChannelVCDelegate {
    func peerChannelVCWillClose(_ vc: PeerChannelViewController) {
        vc.view.window?.contentViewController = self
    }
}
