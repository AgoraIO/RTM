//
//  Message.swift
//  Agora-RTM-Tutorial
//
//  Created by SRS on 2020/4/2.
//  Copyright © 2020 Agora. All rights reserved.
//

import UIKit

enum CellType {
    case left, right
}

struct Message {
    
    var userId: String
    
    // for text message
    var text: String?
    
    // for image message
    var mediaId: String?
    
    // for thumbnail imagedata
    var thumbnail: Data?
}
