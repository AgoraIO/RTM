import './meeting.css';
import {APP_ID as appId} from '../../config';
import Toastr from 'toastr';

class View {

  static load(rtm) {
    this.rtm = rtm;
  }

  static addRoom ({name, userName, type, className}) {
    $(".rooms").append(`
        <div class="room ${className}" data-name=${name} data-type=${type}>
          <h8>${userName}</h8>
          <span>${type}</span>
        </div>
    `)
    this.textarea = true
  }

  static addMessage ({type, text, className, channelName}) {
    const {rtm} = this;
    if (type === 'p2p') {
      return rtm.sendMsg({
        type: 'p2p',
        text,
        className,
        peerId: channelName
      })
    }
    return rtm.sendMsg({
      type: 'channel',
      text,
      className,
      channelName,
    })
  }

  static addMessageView ({type, userName, content, className, channelName, result, append}) {
    const {rtm} = this;
    if (append) {
      $(".messages").append(`
      <div class="message ${className}">
        <div>
          <div class="avatar">${userName}</div>
        </div>
        <div style="position: relative">
          <div class="content ${className}">
          ${content}
          </div>
          ${className === "sender" && type == 'p2p' ?
          `
            ${result && result.hasPeerReceived === true ?
              `<i class="received"></i>`
              : `<i class="not_received"></i>`}
          ` : ''
          }
        </div>
      </div>
    `)
    }
    const msg = {
      userName,
      content,
      className
    }
    if (type === 'p2p') {
      rtm.p2pChannelMsgs[channelName] = rtm.p2pChannelMsgs[channelName] || [];
      rtm.p2pChannelMsgs[channelName].push(msg);
    } else {
      rtm.channelMsgs[channelName] = rtm.channelMsgs[channelName] || [];
      rtm.channelMsgs[channelName].push(msg);
    }
    append && $(".messages").scrollTop($(".messages")[0].scrollHeight);
  }

  static buildMessage ({userName, content, className}) {
    return `
      <div class="message ${className}">
        <div>
          <div class="avatar">${userName}</div>
        </div>
        <div>
          <div class="content ${className}">
          ${content}
          </div>
        </div>
      </div>
    `;
  }
}

class Modal {

  static load(rtm) {
    this.rtm = rtm;
  }

  static mount () {
    const modal = $(".modal");
    const closeButtons = $(".close-modal");
    const modalInner = $(".modal-inner");
    const modalContent = $(".modal-content");

    const handleSubmit = (target) => {
      if ($(target).data().button === 'submit') {
        const channelName = $("#addAccount").val()
        if (!channelName) {
          $("#accountNameRequired")
            .text("Please input a valid name.")
            .css({
              "display": "block",
              "color": "red"
            })
          return;
        }
        const type = $(':radio[name="type"]')
            .filter(':checked')
            .val();
        if (type === 'p2p') {
          let hasCurrent = $(".current").length
          if (this.rtm.dialogues[channelName]) {
            $("#accountNameRequired")
            .text("Name Exists.")
            .css({
              "display": "block",
              "color": "red"
            })
            return ;
          }
          this.rtm.dialogues[channelName] = {
            peerId: channelName
          };
          View.addRoom({
            name: this.rtm.dialogues[channelName].peerId,
            userName: this.rtm.dialogues[channelName].peerId,
            type,
            className: hasCurrent ? '' : 'current'
          });
          modal.toggleClass('modal-open');
          $("#session").text($(".current").data().name)
          return ;
        }
        this.rtm.joinChannel(channelName).then(result => {
          let hasCurrent = $(".current").length
          if (result === false) {
            $("#accountNameRequired")
            .text("Name Exists.")
            .css({
              "display": "block",
              "color": "red"
            })
            return;
          }
          RTM.channelName = channelName;
          View.addRoom({
            name: RTM.channelName,
            userName: RTM.channelName,
            type,
            className: hasCurrent ? '' : 'current'
          })
          modal.toggleClass('modal-open');
          $("#session").text($(".current").data().name)
        }).catch(err => {
          console.log("[RTM-DEMO] error", err);
          return err;
        })
      }
    }
    closeButtons.on('click', (event) => {
      $("#accountNameRequired")
      .text("")
      .css({
        "display": "block",
        "color": "red"
      })
      handleSubmit(event.currentTarget);
      if ($(event.currentTarget).data().button !== 'submit') {
        modal.toggleClass('modal-open');
      }
      $("#addAccount").val('');
    });

    modalInner.on('click', () => {
      modal.toggleClass('modal-open');
    });

    modalContent.on('click', (event) => {
      event.stopPropagation();
    })

    const buttonOpenModal = $(".but.open-modal");
    buttonOpenModal.on('click', () => {
      modal.toggleClass('modal-open');
    })
  }

}

class RTM {
  constructor (accountName) {
    this.accountName = accountName;
    this.client = AgoraRTM.createInstance(appId);

    this.client.on('MessageFromPeer', ({text}, peerId) => {
      const msg = {
        userName: peerId,
        content: text
      };
      let currentData = $(".current").data();
      if (!this.dialogues[peerId]) {
        !currentData && $(".current").length > 0 && $(".current").toggleClass(".current")
          const roomHTMLTmpl = `
            <div class="room ${$(".current").length > 0 ? '' : 'current'}" data-name="${peerId}" data-type="p2p">
              <h8>${peerId}</h8>
              <span>p2p</span>
            </div>
          `;
          View.textarea = true;
          this.dialogues[peerId] = {
            peerId,
          }
          $(".rooms").append(roomHTMLTmpl);
          currentData = {
            type: 'p2p',
            name: peerId
          }
      }
      View.addMessageView({
        channelName: msg.userName,
        type: 'p2p',
        userName: msg.userName,
        content: msg.content,
        className: this.accountName === msg.userName ? 'sender' : '',
        append: currentData.type === 'p2p' && currentData.name == peerId
      })
      console.log(`[RTM-DEMO] MessageFromPeer [${peerId}] channel message text: ${text}`);
    })
    this.channels = {};
    this.dialogues = {};
    this.channelUsers = {};
    this.channelMsgs = {};
    this.p2pChannelMsgs = {};
  }

  async joinChannel(name) {
    const {accountName, channels, client, channelUsers, channelMsgs} = this;

    if (channels[name]) {
      $("#channel_dup").css({"display": "block", "color": "red"});
      return false;
    }
    try {
      const channel = client.createChannel(name);
      channels[name] = channel;
      channelUsers[name] = [];
      channelMsgs[name] = [];

      channel.on('ChannelMessage', ({ text: message }, senderId) => {
        if (senderId === accountName) {
          return;
        }
        const msg = {
          userName: senderId,
          content: message
        };
        const currentData = $(".current").data();
        View.addMessageView({
          channelName: name,
          type: 'channel',
          userName: msg.userName,
          content: msg.content,
          className: this.accountName === senderId ? 'sender' : '',
          append: currentData.type === 'channel' && currentData.name == name
        })
        console.log(`[RTM-DEMO] [createChannel] [${senderId}] channel message text: ${message}, name: ${name} , curName: ${currentData.name}`);
      });
      channel.on('MemberJoined', memberId => {
        console.log("[RTM-DEMO] MemberJoin", memberId);
      });
      channel.on('MemberLeft', memberId => {
        console.log('[RTM-DEMO] MemberLeft', memberId)
      });
      return await channel.join(name);
    } catch (err) {
      console.error('[[RTM-DEMO] [ERR] joinChannel', err);
    }
  }

  async leaveChannel(name) {
    const {channels, channelUsers, channelMsgs, p2pChannelMsgs} = this;
    const channel = channels[name];
    if (!channel) return;
    return new Promise((resolve) => {
      channel.leave().then(_ => {
        delete channels[name];
        delete channelUsers[name];
        delete channelMsgs[name];
        delete p2pChannelMsgs[name];
        resolve(name);
      })
    });
  }

  async sendMsg({type, peerId, text, channelName}) {
    let {client} = this;
    try {
      if (type === 'channel') {
        const channel = this.channels[channelName];
        let result = await channel.sendMessage({text});
        console.log('[RTM-DEMO] [send to channel] received', result);
        return result;
      } else {
        console.log("[RTM-DEMO] p2p ", {text}, peerId);
        let result = await client.sendMessageToPeer({text}, `${peerId}`);
        console.log('[RTM-DEMO] [send to peer] received', result);
        return result;
      }
    } catch (err) {
      console.log('[ERR] sendMsg', err);
      return err;
    }
  }
}

class MeetingPage {

  static load(rtm) {
    this.rtm = rtm;
    this.accountName = this.rtm.accountName;
  }

  static mountRooms() {
    $(".rooms").on("click", ".room", (event) => {
      const {accountName, rtm} = this;
      this.currentRoom = $(".current");
      if (this.currentRoom.length) {
        this.currentRoom.toggleClass('current');
        const $dom = $(event.currentTarget);
        $dom.toggleClass('current');
        this.currentRoom = $dom;
      }
      const currentData = this.currentRoom.data();
      currentData.name = ''+currentData.name;
      $("#session").text(currentData.name);
      let msgHTML = '';
      if (currentData.type === 'channel') {
        if (!rtm.channelMsgs[currentData.name] || rtm.channelMsgs[currentData.name].length === 0) {
          $(".messages").html(msgHTML);
          return;
        }
        for (let msg of rtm.channelMsgs[currentData.name]) {
          msgHTML += View.buildMessage({
            userName: msg.userName,
            content: msg.content,
            className: msg.userName === accountName ? 'sender' : '',
          })
        }
      } else {
        if (!rtm.p2pChannelMsgs[currentData.name] || rtm.p2pChannelMsgs[currentData.name].length === 0) {
          $(".messages").html(msgHTML);
          return;
        }
        for (let msg of rtm.p2pChannelMsgs[currentData.name]) {
          msgHTML += View.buildMessage({
            userName: msg.userName,
            content: msg.content,
            className: msg.className
          })
        }
      }
      $(".messages").html(msgHTML);
    })
  }
}

$(() => {
  View.textarea = false;
  const accountName = location.href.split("?")[1].split("=")[1];

  let rtm = new RTM(accountName);
  rtm.client.on("ConnectionStateChange", (newState, reason) => {
    let type = 'info';
    if (newState === 'ABORTED') {
      type = 'error';
    } else if (newState === 'CONNECTED') {
      type = 'success';
    } else if (newState === 'RECONNECTING') {
      type = 'warning';
    } else if (newState === 'DISCONNECTED') {
      type = 'warning';
    }
    newState == 'ABORTED' && setTimeout(() => {
      location.replace('/');
    }, 1000) && Toastr[type](reason);
  })
  rtm.client.login({uid: accountName}).then(_ => _);

  RTM.channelName = '';
  $('#message').keypress(function (e) {
    if (View.textarea && e.which == 13) {
      const currentData = $(".current").data();
      const text = $("#message").val();
      View.addMessage({
        type: currentData.type,
        text,
        className: 'sender',
        channelName: currentData.name
      }).then(result => {
        View.addMessageView({
          type: currentData.type,
          userName: rtm.accountName,
          content: text,
          className: 'sender',
          result,
          channelName: currentData.name,
          append: true
        })
        $("#message").val('');
      })
      return false;
    }
  });
  $("#user").text(`${accountName}`)

  $("#quit").on('click', () => {
    const promises = []
    if (rtm.channels) {
    for (let name of Object.keys(rtm.channels)) {
      promises.push(rtm.leaveChannel(name));
    }
    Promise.all(promises).then(e => {
      console.log('leave all channels success');
      $(location).attr('href', `/`);
    })
    } else {
      $(location).attr('href', `/`);
    }
  })
  Modal.load(rtm);
  MeetingPage.load(rtm);
  View.load(rtm);

  Modal.mount();
  MeetingPage.mountRooms();
})