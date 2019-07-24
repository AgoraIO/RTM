import './meeting/meeting.scss';
import * as $ from 'jquery';
import * as M from 'materialize-css';

import RtmClient from '../rtm-client';
import {addDialogue, removeDialogue, Toast, validator, serializeFormData, addMessage} from '../common';

$(() => {
  M.AutoInit();

  function updateChatMessage (message) {
    if (rtm._currentDialogue) {
      addMessage({
        type: message.id == rtm.accountName ? 'sender' : 'receiver',
        text: message.message,
        id: message.id
      })
    }
  }

  let rtm = new RtmClient();

  rtm.on("ConnectionStateChanged", (newState, reason) => {
    if (newState == "ABORTED") {
      if (reason == "REMOTE_LOGIN") {
        Toast.error("You have already been kicked off!");
        $("#logout").removeClass("hide");
        $("#login").addClass("hide");

        // TODO: need confirm kickoff's channel 
        rtm.clearState();
      }
    }
  })

  rtm.on("MessageFromPeer", (message, peerId) => {
    console.log("message "+ message.text + " peerId" + peerId);
    if (!rtm._peerMessages[peerId]) {
      rtm._peerMessages[peerId] = []
    }
    const peerMessage = {
      message: message.text,
      id: peerId
    }
    rtm._peerMessages[peerId].push();
    updateChatMessage(peerMessage);
  });

  rtm.on("MemberJoined", ({channelName, args}) => {
    console.log("channel ", channelName, " member: ", args, " joined");
  });

  rtm.on("ChannelMessage", ({channelName, args}) => {
    const [message, memberId] = args;
    console.log("channel message", message, memberId);
    if (!rtm._channelMessages[channelName]) {
      rtm._channelMessages[channelName] = []
    }
    const channelMessage = {
      message: message.text,
      id: memberId
    }
    rtm._channelMessages[channelName].push(channelMessage);
    updateChatMessage(channelMessage);
  });

  rtm.on("MemberLeft", ({channelName, args}) => {
    console.log("channel ", channelName, " member: ", args, " left");
  });

  $("#login").on("click", function (e) {
    e.preventDefault();

    const params = serializeFormData("form");

    if (!validator(params, ['dialogue_type', 'dialogue_name'])) {
      return;
    }

    try {
      rtm.init(params.appId);
      window.rtm = rtm;
      rtm.login(params.accountName).then(() => {
        console.log('login')
        $("#login").addClass("hide")
        $("#logout").removeClass("hide")
        rtm._logined = true
        Toast.notice("Login: " + params.accountName);
      }).catch((err) => {
        console.log(err)
      })
    } catch(err) {
      Toast.error("Login failed, please open console see more details");
      console.error(err);
    }
  })

  $("#logout").on("click", function (e) {
    e.preventDefault();
    rtm.logout().then(() => {
      console.log('logout')
      $("#login").removeClass("hide")
      $("#logout").addClass("hide")
      rtm._logined = false
      Toast.notice("Logout: " + params.accountName);
    }).catch((err) => {
      Toast.error("Logout failed, please open console see more details");
      console.log(err)
    })
  })

  $("#add_new_chat").on("click", function (e) {
    e.preventDefault();
    if (!rtm || !rtm._logined) {
      Toast.error("Please login first!");
      return;
    }
    M.Modal.init($("#dialogue")[0], {
      dismissible: false,
    }).open();
  })

  $("#cancel").on("click", function (e) {
    e.preventDefault();
    M.Modal.init($("#dialogue")[0]).close();
  })

  $("#submit").on("click", function (e) {
    e.preventDefault();
    const dialogueParams = serializeFormData("dialogueForm");
    if (!validator(dialogueParams, ['dialogue_type', 'dialogue_name'])) {
      return false;
    }

    const id = dialogueParams.dialogue_type + "-dialogue-" + dialogueParams.dialogue_name;
    if ($("#" + id)[0]) {
      Toast.error("dialogue already exists");
      return false;
    }
    addDialogue(dialogueParams.dialogue_type, dialogueParams.dialogue_name);
    if (rtm._currentDialogue == null) {
      rtm._currentDialogue = {
        type: dialogueParams.dialogue_type,
        name: dialogueParams.dialogue_name,
      }
      $("#"+id).addClass("active");
    }

    if (dialogueParams.dialogue_type == 'channel') {
      const channelName = dialogueParams.dialogue_name
      if (!rtm.channels[channelName]
        || !rtm.channels[channelName].joined) {
        rtm.joinChannel(channelName).then(() => {
          rtm.channels[channelName].joined = true;
        }).catch((err) => {
          Toast.error("Join channel " + " " + channelName + " failed: please open console see more details.");
          console.error(err);
        })
      }
    }
  
    M.Modal.init($("#dialogue")[0]).close();
  })

  $("body").on("click", ".dialogue-item", function (e) {
    e.preventDefault();
    $(".dialogue-item").removeClass("active")
    $(this).addClass('active')
    const [type, name] = $(this).attr("id").split("-dialogue-")
    rtm._currentDialogue = {
      type, name
    }
    $("#chat-message")[0].innerHTML = '';

    if (type == 'p2p' && rtm._peerMessages[name]) {
      for (let msg of rtm._peerMessages[name]) {
        updateChatMessage(msg);
      }
    }

    if (type == 'channel' && rtm._channelMessages[name]) {
      for (let msg of rtm._channelMessages[name]) {
        updateChatMessage(msg);
      }
    }
    e.stopPropagation();
    console.log("curret", rtm._currentDialogue);
  })

  $("body").on("click", ".dialogue-close", function (e) {
    e.preventDefault();
    const [type, name] = $(this).attr("id").split("-close")[0].split("-dialogue-")
    if (type == 'channel') {
     return rtm.leaveChannel(name).then(() => {
       rtm.channels[name] = null;
       rtm._channelMessages[name] = null;
      $(this).parent().remove();

      if (rtm._currentDialogue.type == 'channel'
      && rtm._currentDialogue.name == name) {
        rtm._currentDialogue = null
      }

     }).catch((err) => {
       Toast.error("Leave channel failured: please open console see more details");
       console.error(err);
     })
    }
    $(this).parent().remove();
  })

  async function sendMessageFromTextarea(message) {
    if (rtm._currentDialogue.type == 'channel') {
      const channelName = rtm._currentDialogue.name
      rtm.sendChannelMessage(message, channelName).then(() => {
        if (!rtm._channelMessages[channelName]) {
          rtm._channelMessages[channelName] = [];
        }
        const channelMessage = {
          message,
          id: rtm.accountName
        };

        rtm._channelMessages[channelName].push(channelMessage);
        updateChatMessage(channelMessage);
        // handle send channel message success
      }).catch((err) => {
        Toast.error("sendChannelMessage failured: please open console see more details.")
        console.error(err);
      })
    }

    if (rtm._currentDialogue.type == 'p2p') {
      const peerId = rtm._currentDialogue.name;
      rtm.sendPeerMessage(message, peerId).then(() => {
        if (!rtm._peerMessages[peerId]) {
          rtm._peerMessages[peerId] = [];
        }

        const peerMessage = {
          message,
          id: rtm.accountName,
        };

        rtm._peerMessages[peerId].push(peerMessage);
        // handle send peer message success
        updateChatMessage(peerMessage);
      }).catch((err) => {
        Toast.error("sendPeerMessage failured: please open console see more details.")
        console.error(err)
      })
    }
  }

  $("#message").on("keydown", function (e) {
    if (!rtm._currentDialogue) {
      if ((e.keyCode == 13 || e.keyCode == 10)) {
        Toast.error("Please Login and select dialogue first");
      }
      return;
    }
    if ((e.keyCode == 13 || e.keyCode == 10)) {
      let message = $("#message").val();
      if (e.shiftKey) {
        $("#message").val($("#message").val() + '\n');
        e.stopPropagation();
      } else {
        sendMessageFromTextarea(message);
        $("#message").val('');
      }
    }
  })

  $("#send").on("click", function (e) {
    e.preventDefault();
    if (!rtm._currentDialogue) {
      Toast.error("Please Login and select dialogue first");
      return;
    }
    const message = $("#message").val();
    sendMessageFromTextarea(message);
    $("#message").val('');
  })
})