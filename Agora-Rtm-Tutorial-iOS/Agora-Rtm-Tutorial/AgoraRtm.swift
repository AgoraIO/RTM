//
//  AgoraRtm.swift
//  Agora-Rtm-Tutorial
//
//  Created by CavanSu on 2019/1/17.
//  Copyright © 2019 Agora. All rights reserved.
//

import Foundation
import AgoraRtmKit

enum LoginStatus {
    case online, offline
}

class AgoraRtm: NSObject {
    static let kit = AgoraRtmKit(appId: AppId.id, delegate: nil)
    static var current: String?
    static var status: LoginStatus = .offline
    
    static func updateKit(delegate: AgoraRtmDelegate) {
        guard let kit = kit else {
            return
        }
        kit.agoraRtmDelegate = delegate
    }
}

