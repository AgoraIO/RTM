//
//  MessageCell.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
//

import UIKit

class MessageCell: UITableViewCell {
    @IBOutlet weak var rightUserLabel: UILabel!
    @IBOutlet weak var rightContentLabel: UILabel!
    @IBOutlet weak var leftUserLabel: UILabel!
    @IBOutlet weak var leftContentLabel: UILabel!
    
    @IBOutlet weak var rightUserBgView: UIView!
    @IBOutlet weak var rightContentBgView: UIView!
    @IBOutlet weak var leftUserBgView: UIView!
    @IBOutlet weak var leftContentBgView: UIView!
    
    override func awakeFromNib() {
        rightUserBgView.layer.cornerRadius = 20
        rightContentBgView.layer.cornerRadius = 5
        
        leftUserBgView.layer.cornerRadius = 20
        leftContentBgView.layer.cornerRadius = 5
    }
    
    private var type: CellType = .right {
        didSet {
            let rightHidden = type == .left ? true : false
            
            rightUserLabel.isHidden = rightHidden
            rightContentLabel.isHidden = rightHidden
        
            leftUserLabel.isHidden = !rightHidden
            leftContentLabel.isHidden = !rightHidden
            
            rightUserBgView.isHidden = rightHidden
            rightContentBgView.isHidden = rightHidden
            
            leftUserBgView.isHidden = !rightHidden
            leftContentBgView.isHidden = !rightHidden
        }
    }
    
    private var user: String? {
        didSet {
            switch type {
            case .left:  leftUserLabel.text = user
            case .right: rightUserLabel.text = user
            }
        }
    }
    
    private var content: String? {
        didSet {
            switch type {
            case .left:  leftContentLabel.text = content
            case .right: rightContentLabel.text = content
            }
        }
    }
    
    func update(type: CellType, message: Message) {
        self.type = type
        self.user = message.userId
        self.content = message.text
    }
}
