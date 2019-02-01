//
//  MainViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 CavanSu. All rights reserved.
//

#if os(iOS)
import UIKit
#else
import Cocoa
#endif

class MainViewController: AGViewController {
    @IBOutlet weak var accountTextField: AGTextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    #if os(iOS)
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        logout()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        view.endEditing(true)
    }
    
    #else
    override func viewWillAppear() {
        logout()
    }
    
    override func prepare(for segue: NSStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier, identifier == "mainToTab" else {
            return
        }
        
        let peerChannelVC = segue.destinationController as! PeerChannelViewController
        peerChannelVC.delegate = self
    }
    #endif
    
    @IBAction func doLoginPressed(_ sender: AGButton) {
        login()
    }
}

private extension MainViewController {
    func login() {
        guard let account = accountTextField.text, account.count > 0 else{
            return
        }
        
        AgoraRtm.current = account

        AgoraRtm.kit?.login(byToken: nil, user: account) { [unowned self] (errorCode) in
            guard errorCode == .ok else {
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

#if os(macOS)
extension MainViewController: PeerChannelVCDelegate {
    func peerChannelVCWillClose(_ vc: PeerChannelViewController) {
        vc.view.window?.contentViewController = self
    }
}
#endif
