const eventBus = require("./event-bus.js")
const AgoraRTM = require("../lib/rtm.miniapp.js")
const { APPID } = require("./config.js")

class RTMClient {
  constructor () {
    this._client = AgoraRTM.createInstance(APPID)
    this._accountName = ''
    this._channel = null
    this.isLogin = false 
    this.isOff = false
    this.messageCache = []
    this._eventBus = eventBus
    this.subscribeLoginEvents()
  }
  
  login(token, accountName) {
    this._accountName = accountName
    const object = {
      token: token,
      uid: accountName
    }
    return this._client.login(object)
  }

  subscribeLoginEvents() {
    this._client.on('ConnectionStateChanged', (newState, reason) => {
      this._eventBus.emit('ConnectionStateChanged', newState, reason)
    })
    this._client.on('MessageFromPeer', (message, peerId, { isOfflineMessage }) => {
      console.log('MessageFromPeer')
      // 判断是否是离线消息 如果是 进行消息缓存 
      if(isOfflineMessage) {
        let object = {
          message: message.text,
          peerId: peerId,
          isOfflineMessage: isOfflineMessage
        }
        this.messageCache.push(object)
      }
      this._eventBus.emit('MessageFromPeer', message, peerId, isOfflineMessage)
    })
  }

  logout() {
    return this._client.logout().then(() => {
      this._eventBus.clear()
      this.messageCache = []
    })
  }

  sendPeer(msg, peerId) {
    return this._client.sendMessageToPeer({ text: msg }, peerId, {
      enableHistoricalMessaging: false,
      enableOfflineMessaging: this.isOff
    })
  }

  joinChannel(id) {
    this._channel = this._client.createChannel(id)
    this.subscribeChannelEvents(id)
    return this._channel.join()
  }

  subscribeChannelEvents() {
    this._channel.on('MemberJoined', (e) => {
      this._eventBus.emit('MemberJoined', e)
    })
    this._channel.on('MemberLeft', (e) => {
      this._eventBus.emit('MemberLeft', e)
    })
    this._channel.on('MemberCountUpdated', (e) => {
      this._eventBus.emit('MemberCountUpdated', e)
    })
    
    this._channel.on('ChannelMessage', (message, memberId) => {
      this._eventBus.emit('ChannelMessage', message, memberId)
    })
  }

  sendChannel(msg) {
    return this._channel.sendMessage({text: msg})
  }

  getMembers() {
    return this._channel.getMembers()
  }

  leaveChannel() {
    return this._channel.leave()
  }

  on(event, callback) {
    this._eventBus.on(event, callback)
  }

}

module.exports = RTMClient