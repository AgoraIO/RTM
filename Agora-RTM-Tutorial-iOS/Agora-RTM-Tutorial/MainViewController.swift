//
//  MainViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit
import AgoraRtmKit

protocol ShowAlertProtocol: UIViewController {
    func showAlert(_ message: String, handler: ((UIAlertAction) -> Void)?)
    func showAlert(_ message: String)
}

extension ShowAlertProtocol {
    func showAlert(_ message: String, handler: ((UIAlertAction) -> Void)?) {
        view.endEditing(true)
        let alert = UIAlertController(title: nil, message: message, preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .default, handler: handler))
        present(alert, animated: true, completion: nil)
    }
    
    func showAlert(_ message: String) {
        showAlert(message, handler: nil)
    }
}

class MainViewController: UIViewController, ShowAlertProtocol {
    @IBOutlet weak var accountTextField: UITextField!
    @IBOutlet weak var enableOneToOneSwitch: UISwitch!
    
    override func viewWillAppear(_ animated: Bool) {
        logout()
    }
    
    override func viewDidLoad() {
        self.navigationController?.interactivePopGestureRecognizer?.isEnabled = false
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        view.endEditing(true)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
    
    @IBAction func doLoginPressed(_ sender: UIButton) {
        login()
    }
}

private extension MainViewController {
    func login() {
        guard let account = accountTextField.text, account.count > 0 else{
            return
        }
        
        AgoraRtm.updateKit(delegate: self)
        AgoraRtm.current = account
	
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
