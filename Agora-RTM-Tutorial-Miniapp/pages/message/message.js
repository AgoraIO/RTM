
Page({
  data: {
    channelName: '',
    channelMessage: '',
    peerId: '',
    peerMessage: '',
    accountName: '',    
    channelMessageArray: [],
    peerMessageArray: [],
    joinStatus: false,
    bottom: '',
    currentTab: 0,
    channelMembers: [],
    queryMembers: false,
    showMsg: false,
    msgDetails: '',
  },

  onChannelName: function (e) {
    this.setData({
      channelName: e.detail.value
    })
  },

  onChannelMessage: function (e) {
    this.setData({
      channelMessage: e.detail.value
    }) 
  },

  onPeerId: function (e) {
    this.setData({
      peerId: e.detail.value
    })
  },

  onPeerMessage: function (e) {
    this.setData({
      peerMessage: e.detail.value
    })
  },

  bindJoin: function () {
    if(!this.data.channelName) {
      wx.showModal({
        title: 'message',
        content: 'Please enter the channel name',
      })
      return
    }
    this.setData({
      joinStatus: true,
    })
    this.rtm.joinChannel(this.data.channelName).then(() => {
      console.log('join channel success')
    }).catch((err) => {
      console.log('join channel failed', err)
    })
  },

  bindLeave: function () {
    this.setData({
      joinStatus: false
    })
    this.rtm.leaveChannel().then(() => {
      console.log('leave success')
    }).catch((err) => {
      console.log('leave failed', err)
    })
  },

  bindQuery: function() {
    if(!this.data.joinStatus) {
      console.log('Please join the channel')
      return
    }
    this.setData({
      queryMembers: true
    })
    this.rtm.getMembers().then((members) => {
      console.log('channel members', members)
      this.setData({
        channelMembers: members
      })
    }).catch((err) => {
      console.log('get members failed', err)
    })
  },

  bindChannelSend: function () {
    if(!this.data.channelMessage) {
      return
    }
    if(!this.data.joinStatus) {
      wx.showModal({
        title: 'message',
        content: 'Please join the channel',
      })
      return
    }
    this.rtm.sendChannel(this.data.channelMessage).then(() => {
      console.log('send channel meaasge success')
    }).catch((err) => {
      console.log('send channel meaasge failed', err)
    })
    let object = {
      uid: this.rtm._accountName,
      message: this.data.channelMessage
    }
    this.data.channelMessageArray.push(object)
    this.setData({
      channelMessageArray: this.data.channelMessageArray
    })
    // 控制滚动条在最下方 即显示最新消息, 清空输入框的值
    this.setData({
      bottom: 'scrollBottom',
      channelMessage: ''
    })
  },

  // 默认不将该消息设为离线消息，开启后设置离线消息
  openOffMsg: function() {
    console.log('open offline message')
    this.rtm.isOff = true
  },

  bindPeerSend: function () {
    if(!this.data.peerMessage) {
      return
    }
    if(!this.data.peerId) {
      wx.showModal({
        title: 'message',
        content: 'Please enter the peer id',
      })
      return
    }
    
    this.rtm.sendPeer(this.data.peerMessage, this.data.peerId).then((e) => {
      console.log('send peer message success', e)
      if(e.hasPeerReceived) {
        console.log('peer received success')
        this.msgBounced('peer received success')
      } else {
        console.log('peer received failed')
        this.msgBounced('peer received failed')
      }
    }).catch((err) => {
      console.log('send peer message failed', err)
    })
    let object = {
      peerId: this.data.peerId,
      message: this.data.peerMessage, 
      isLocal: true
    }
    this.data.peerMessageArray.push(object)
    this.setData({
      peerMessageArray: this.data.peerMessageArray
    })
    // 控制滚动条 清空输入框
    this.setData({
      bottom: 'scrollBottom',
      peerMessage: ''
    })
  },

  //tab切换
  swichNav: function(e) {
    if( this.data.currentTab === e.target.dataset.current ) {
      return false
    } else {
      this.setData({
        currentTab: e.target.dataset.current
      })
    }
  },

  bindChange: function(e) {
    this.setData({ 
      currentTab: e.detail.current 
    })
  },

  // 弹窗消息
  msgBounced: function(details) {
    this.setData({
      showMsg: true,
      msgDetails: details,
    })
    setTimeout(() => {
      this.setData({
        showMsg: false
      })
    }, 2000)
  },

  // 点对点离线消息
  // 从 rtm messageCache 数组中获取缓存的离线消息
  peerOffMsg: function() {
    this.rtm.messageCache.forEach((item) => {
      item.isLocal = false
      this.data.peerMessageArray.push(item)
    })
    this.setData({
      peerMessageArray: this.data.peerMessageArray 
    })
  },

  // 监听点对点消息
  onPeerMsgEvent: function() {
    this.rtm.on('MessageFromPeer', (message, peerId, isOfflineMessage) => {
      let object = {
        message: message.text,
        peerId: peerId,
        isLocal: false,
        isOfflineMessage: isOfflineMessage
      }
      this.data.peerMessageArray.push(object)
      this.setData({
        peerMessageArray: this.data.peerMessageArray 
      })
    })
  },

  onChannelEvent: function() {
    // 频道消息
    this.rtm.on('ChannelMessage', (message, memberId) => {
      let object = {
        uid: memberId,
        message: message.text
      }    
      this.data.channelMessageArray.push(object)
      this.setData({
        channelMessageArray: this.data.channelMessageArray
      })
    })
    //频道成员进出通知
    this.rtm.on('MemberJoined', (memberId) => {
      console.log('memberId: ', memberId)
      this.msgBounced(`${memberId} join channel`)
    })
    this.rtm.on('MemberLeft', (memberId) => {
      console.log('memberId: ', memberId)
      this.msgBounced(`${memberId} already left`)
    })
  },

   // 页面创建时执行
  onLoad: function() {
    // 获取全局赋值的 rtm
    this.rtm = getApp().globalData.agoraRtm
    // 拿到登陆的账户名
    this.setData({
      accountName: this.rtm._accountName
    })
    this.peerOffMsg()
    this.onPeerMsgEvent()
    this.onChannelEvent()
    // sdk连接状态
    this.rtm.on('ConnectionStateChanged', (newState, reason) => {
      console.log('The connection status', newState)
      console.log('The reason for the state change', reason)
    })
  },
  onShow: function() {
    // 页面出现在前台时执行
  },
  onReady: function() {
    // 页面首次渲染完毕时执行
  },
  onHide: function() {
    // 页面从前台变为后台时执行
  },
  onUnload: function() {
    // 页面销毁时执行
    if(this.rtm.isLogin) {
      this.rtm.logout().then(() => {
        this.rtm.isLogin = false
        console.log('logout success')
      }).catch((err) => {
        console.log('logout failed', err)
      })
    } 
  },
  onPullDownRefresh: function() {
    // 触发下拉刷新时执行
  },
  onReachBottom: function() {
    // 页面触底时执行
  },
  onShareAppMessage: function () {
    // 页面被用户分享时执行
  },
  onPageScroll: function() {
    // 页面滚动时执行
  },
  onResize: function() {
    // 页面尺寸变化时执行
  }
})