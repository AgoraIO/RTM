const RTMClient = require("../../utils/agora-rtm.js")
const { debounce } = require("../../utils/util.js")
const app = getApp()

Page({
  data: {
    token: undefined,
    accountName: '',
  },

  login: function () {
    return debounce(this.bindLogin(), 500)
  },

  bindLogin: function () {
    if(!this.data.accountName) {
      console.log('accountName is null')
      if(this.rtm._login) {
        console.log('already login')
        return
      }
      return
    }
    
    this.rtm.login(this.data.token, this.data.accountName).then(() => {
      console.log('login success')
      this.rtm._login = true
      wx.navigateTo({
        url: `../message/message`
      });
    }).catch((err) => {
      console.log('login failed', err)
    })
  },  

  onInputToken: function (e) {
    this.setData({
      token: e.detail.value
    })
  },

  onInputAccount: function (e) {
    this.setData({
      accountName: e.detail.value
    })
  },  

  onLoad: function () {
    this.rtm = new RTMClient()
    this.connectEventId = this.rtm._eventBus.on('ConnectionStateChanged', (newState, reason) => {
      console.log('The Connection Status', newState, reason)
      console.log('The Connection Reason', newState, reason)
    })
    //rtm 赋值给全局
    app.globalData.agoraRtm = this.rtm
  },

  onUnload: function () {
    
  }
  
})
