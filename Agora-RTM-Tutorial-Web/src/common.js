import md5 from 'blueimp-md5'
import Identicon from 'identicon.js'
import * as M from 'materialize-css'

function Toastify (options) {
  M.toast({ html: options.text, classes: options.classes })
}

export const Toast = {
  info: (msg) => {
    Toastify({
      text: msg,
      classes: 'info-toast'
    })
  },
  notice: (msg) => {
    Toastify({
      text: msg,
      classes: 'notice-toast'
    })
  },
  error: (msg) => {
    Toastify({
      text: msg,
      classes: 'error-toast'
    })
  }
}

export function validator (formData, fields) {
  const keys = Object.keys(formData)
  for (const key of keys) {
    if (fields.indexOf(key) !== -1) {
      if (!formData[key]) {
        Toast.error('Please Enter ' + key)
        return false
      }
    }
  }
  return true
}

export function serializeFormData (domId) {
  const formData = $('#' + domId).serializeArray()
  const obj = {}
  for (var item of formData) {
    var key = item.name
    var val = item.value
    obj[key] = val
  }
  console.log('form data', obj)
  return obj
}

// export function makeBlobByFetch (src) {
//   return fetch(src).then(res => {
//     const blob = res.blob()
//     return blob
//   })
// }
  
export function canvasToDataURL (canvas, format, quality) {
  return canvas.toDataURL(format||'image/jpeg', quality||1.0)
}

export function dataURLToBlob(dataurl){
  var arr = dataurl.split(',')
  var mime = arr[0].match(/:(.*?);/)[1]
  var bstr = atob(arr[1])
  var n = bstr.length
  var u8arr = new Uint8Array(n)
  while(n--){
    u8arr[n] = bstr.charCodeAt(n)
  }
  return new Blob([u8arr], {type:mime})
}

export function imageToCanvas(src, cb){
  var canvas = document.createElement('CANVAS')
  var ctx = canvas.getContext('2d')
  var img = new Image()
  img.src = src
  img.onload = function (){
    canvas.width = img.width
    canvas.height = img.height
    ctx.drawImage(img, 0, 0)
    cb(canvas)
  }
}

export function fileOrBlobToDataURL(obj, cb){
  var a = new FileReader()
  a.readAsDataURL(obj)
  a.onload = function (e){
    cb(e.target.result)
  }
}

//Blob 转 image
export function blobToImage (blob, cb) {
  fileOrBlobToDataURL(blob, function (dataurl){
    var img = new Image()
    img.src = dataurl
    cb(img)
  })
}

//image 转 Blob
export function imageToBlob(src, cb){
  imageToCanvas(src, function (canvas){
    cb(dataURLToBlob(canvasToDataURL(canvas)))
  })
}
