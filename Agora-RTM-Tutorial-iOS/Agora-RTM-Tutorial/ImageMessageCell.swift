//
//  ImageMessageCell.swift
//  Agora-RTM-Tutorial
//
//  Created by SRS on 2020/4/2.
//  Copyright © 2020 Agora. All rights reserved.
//

import UIKit

class ImageMessageCell: UITableViewCell {
    
    @IBOutlet weak var leftUserLabel: UILabel!
    @IBOutlet weak var leftUserBgView: UIView!
    @IBOutlet weak var leftContentImage: UIImageView!
    
    @IBOutlet weak var rightUserLabel: UILabel!
    @IBOutlet weak var rightUserBgView: UIView!
    @IBOutlet weak var rightContentImage: UIImageView!
    
    fileprivate var requestId: Int64 = 0
    fileprivate var mediaId: String?
    fileprivate var imageData: Data?
    
    override func awakeFromNib() {
        rightUserBgView.layer.cornerRadius = 20
        leftUserBgView.layer.cornerRadius = 20
    }
    
    private var type: CellType = .right {
        didSet {
            let rightHidden = type == .left ? true : false
            
            rightUserLabel.isHidden = rightHidden;
            rightUserBgView.isHidden = rightHidden;
            rightContentImage.isHidden = rightHidden;
            
            leftUserLabel.isHidden = !rightHidden;
            leftUserBgView.isHidden = !rightHidden;
            leftContentImage.isHidden = !rightHidden;
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
                    self.updateImageData(imageData: data)
                }
            })
        }
    }
    
    fileprivate func updateImageData(imageData: Data) {
        self.imageData = imageData
        if let image = UIImage(data: imageData) {
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
