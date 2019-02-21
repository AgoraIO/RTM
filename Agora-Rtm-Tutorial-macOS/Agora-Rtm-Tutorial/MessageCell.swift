//
//  MessageCell.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
//

import Cocoa

enum CellType {
    case left, right
}

class MessageCell: NSTableCellView {
    @IBOutlet weak var rightUserLabel: NSTextField!
    @IBOutlet weak var rightContentLabel: NSTextField!
   
    @IBOutlet weak var leftUserLabel: NSTextField!
    @IBOutlet weak var leftContentLabel: NSTextField!
    
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
            case .left:  leftUserLabel.stringValue = user!
            case .right: rightUserLabel.stringValue = user!
            }
        }
    }
    
    private var content: String? {
        didSet {
            switch type {
            case .left:  leftContentLabel.stringValue = content!
            case .right: rightContentLabel.stringValue = content!
            }
        }
    }
    
    func update(type: CellType, message: Message) {
        self.type = type
        self.user = message.userId
        self.content = message.text
    }
}
