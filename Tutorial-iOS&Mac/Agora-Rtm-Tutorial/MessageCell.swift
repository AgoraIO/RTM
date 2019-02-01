//
//  MessageCell.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora.IO. All rights reserved.
//

#if os(iOS)
import UIKit
#else
import Cocoa
#endif

enum CellType {
    case left, right
}

class MessageCell: AGTableViewCell {
    @IBOutlet weak var rightUserLabel: AGLabel!
    @IBOutlet weak var rightContentLabel: AGLabel!
   
    @IBOutlet weak var leftUserLabel: AGLabel!
    @IBOutlet weak var leftContentLabel: AGLabel!
    
    #if os(iOS)
    @IBOutlet weak var rightUserBgView: AGView!
    @IBOutlet weak var rightContentBgView: AGView!
    @IBOutlet weak var leftUserBgView: AGView!
    @IBOutlet weak var leftContentBgView: AGView!
    #endif
    
    override func awakeFromNib() {
        super.awakeFromNib()
        #if os(iOS)
        rightUserBgView.layer.cornerRadius = 20
        rightContentBgView.layer.cornerRadius = 5
        
        leftUserBgView.layer.cornerRadius = 20
        leftContentBgView.layer.cornerRadius = 5
        #else
        rightUserLabel?.usesSingleLineMode = false
        rightUserLabel?.cell?.wraps = true
        rightUserLabel?.cell?.isScrollable = false
        
        leftContentLabel?.usesSingleLineMode = false
        leftContentLabel?.cell?.wraps = true
        leftContentLabel?.cell?.isScrollable = false
        #endif
    }
    
    private var type: CellType = .right {
        didSet {
            let rightHidden = type == .left ? true : false
            
            rightUserLabel.isHidden = rightHidden
            rightContentLabel.isHidden = rightHidden
        
            leftUserLabel.isHidden = !rightHidden
            leftContentLabel.isHidden = !rightHidden
            
            #if os(iOS)
            rightUserBgView.isHidden = rightHidden
            rightContentBgView.isHidden = rightHidden
            
            leftUserBgView.isHidden = !rightHidden
            leftContentBgView.isHidden = !rightHidden
            #endif
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
