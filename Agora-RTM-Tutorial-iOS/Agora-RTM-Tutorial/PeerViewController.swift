//
//  PeerViewController.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

class PeerViewController: UIViewController {
    @IBOutlet weak var peerTextField: UITextField!
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        view.endEditing(true)
    }
    
    @IBAction func doChatPressed(_ sender: UIButton) {
        guard let peer = peerTextField.text, peer.count > 0 else {
            return
        }
        performSegue(withIdentifier: "peerToChat", sender: peer)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier, let peer = sender as? String else {
            return
        }
        
        switch identifier {
        case "peerToChat":
            let chatVC = segue.destination as! ChatViewController
            chatVC.type = .peer(peer)
        default:
            break
        }
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
}
