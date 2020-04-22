//
//  ImageMessageCell.swift
//  Agora-RTM-Tutorial
//
//  Created by SRS on 2020/4/2.
//  Copyright © 2020 Agora. All rights reserved.
//

import Cocoa

class ImageMessageCell: NSTableCellView {
    
    @IBOutlet weak var leftUserLabel: NSTextField!
    @IBOutlet weak var leftContentImage: NSImageView!
    
    @IBOutlet weak var rightUserLabel: NSTextField!
    @IBOutlet weak var rightContentImage: NSImageView!
    
    fileprivate var requestId: Int64 = 0
    fileprivate var mediaId: String?
    fileprivate var imageData: Data?
    
    private var type: CellType = .right {
        didSet {
            let rightHidden = type == .left ? true : false
            
            rightUserLabel.isHidden = rightHidden;
            rightContentImage.isHidden = rightHidden;
            
            leftUserLabel.isHidden = !rightHidden;
            leftContentImage.isHidden = !rightHidden;
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
    
    func update(type: CellType, message: Message) {
        self.type = type
        self.user = message.userId

        // complete down
        if(self.mediaId != nil && self.mediaId == message.mediaId && self.imageData != nil){
            self.updateImageData(imageData: self.imageData!)
            return
        }
        
        // downing
        if(self.mediaId != nil && self.mediaId == message.mediaId && self.requestId > 0){
            return
        }
        
        self.mediaId = message.mediaId;
        
        // stop last download
        if(self.requestId > 0){
            AgoraRtm.kit?.cancelMediaDownload(self.requestId, completion: nil)
        }
        
        self.requestId = 0
        if let imageData = message.thumbnail {
            self.updateImageData(imageData: imageData)
        }
        
        if let currentMediaId = message.mediaId {
            AgoraRtm.kit?.downloadMedia(toMemory: currentMediaId, withRequest: &self.requestId, completion: {[weak self] (requestId, data, errorCode) in
                
                guard let `self` = self else {
                    return
                }
                self.requestId = 0
                
                guard let `data` = data else {
                    return
                }
                if(errorCode == .ok) {
                    self.imageData = data
                    self.updateImageData(imageData: data)
                }
            })
        }
    }
    
    fileprivate func updateImageData(imageData: Data) {

        if let image = NSImage(data: imageData) {
            switch (self.type) {
            case .left:
                self.leftContentImage.isHidden = false
                self.leftContentImage.image = image
                break
            case .right:
                self.rightContentImage.isHidden = false
                self.rightContentImage.image = image
                break
            }
        }
    }
}
