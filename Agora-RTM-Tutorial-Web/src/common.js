import md5 from 'blueimp-md5';
import Identicon from 'identicon.js';

let avatars = {};

export function getRandomAvatar () {
  if (!avatars.default) {
  const str = new Identicon(md5(Date.now()), {
    background: [255, 255, 255, 255],
    margin: 0.2,
    size: 88,
    format: 'svg'
  }).toString()
    avatars.default = `data:image/svg+xml;base64,${str}`;
  }
  return avatars.default;
}

export function addDialogue(type, name) {
  const $dialogue = $("#dialogue-list");
  const id = type + "-dialogue-" + name;

  if (type == 'p2p' || type == 'channel') {
    $("<div/>", {
      id,
      class: 'dialogue-item',
      text: name,
    }).appendTo($dialogue);
  }

  $("<i/>", {
    id: id + "-close",
    class: 'iconfont icon-close dialogue-close'
  }).appendTo($("#"+id));
}

export function removeDialogue(name) {
  const dialogueId = "#"+name;
  if ($(dialogueId)[0]) {
    $(dialogueId).remove();
  }
}

function Toastify (options) {
  M.toast({html: options.text, classes: options.classes});
}

export const Toast = {
  info: (msg) => {
    Toastify({
      text: msg,
      classes: "info-toast"
    })
  },
  notice: (msg) => {
    Toastify({
      text: msg,
      classes: "notice-toast"
    })
  },
  error: (msg) => {
    Toastify({
      text: msg,
      classes: "error-toast"
    })
  }
};

export function validator(formData, fields) {
  const keys = Object.keys(formData);
  for (let key of keys) {
    if (fields.indexOf(key) != -1) {
      if (!formData[key]) {
        Toast.error("Please Enter " + key);
        return false;
      }
    }
  }
  return true;
}

export function serializeFormData(domId) {
  const formData = $("#"+domId).serializeArray();
  const obj = {}
  for (var item of formData) {
    var key = item.name;
    var val = item.value;
    obj[key] = val;
  }
  console.log("form data", obj);
  return obj;
}

function genAvatar (name) {
  const str = new Identicon(md5(name), {
    background: [255, 255, 255, 255],
    margin: 0.2,
    size: 88,
    format: 'svg'
  }).toString()
  return `data:image/svg+xml;base64,${str}`;
}

function getAvatarByName(name) {
  const avatars = {};
  if (!avatars[name]) {
    avatars[name] = genAvatar(name);
  }
  return avatars[name];
}

export function addMessage (msg) {
  const chatContainer = $("<div/>", {
    class: msg.type,
  });
  $("#chat-message").append(chatContainer);

  $("<img/>", {
    class: 'id',
    src: msg.src ? msg.src : getAvatarByName(msg.type + msg.id)
  }).appendTo(chatContainer);

  const panel = $("<div/>", {
    class: 'panel bubble',
  }).appendTo(chatContainer);

  $("<pre/>", {
    class: 'text',
    text: msg.text
  }).appendTo(panel);

  requestAnimationFrame(() => {
    $("#chat-message").scrollTop($("#chat-message")[0].scrollHeight);
  });
}