let url = "https://api.heclouds.com/devices/1064***674/datapoints";
let apikey = "8Hq4KKZr5c40yMUHQkY26pRNK9E="

Page({
  data: {
   temp:0,
   humi:0,
   light:0,
   Led:false
  },

 /* onLedchange(event){
    const that = this
    console.log(event.detail.value);
    const sw = event.detail.value
    that.setData(Led : event.detail.value)

    
  },    挽救小白*/

  // 事件处理函数
  getinfo(){
    var that = this   //this变量赋值给that
    wx.request({
    url: "https://api.heclouds.com/devices/1158646451/datapoints",   
    //将请求行中的数字换成自己的设备ID
    header: {
      "api-key": "8Hq4KKZr5c40yMUHQkY26pRNK9E=" //自己的api-key
    },
    method: "GET",
    success: function (e) {
      console.log("获取成功",e)
      that.setData({
        temp:e.data.data.datastreams[1].datapoints[0].value,
        humi:e.data.data.datastreams[2].datapoints[0].value,
        light:e.data.data.datastreams[0].datapoints[0].value
      })
      console.log("temp==",that.data.temp)
    }
   });
  },




  kai:function(){
    //按钮发送命令控制硬件
     wx.request({
       url:"https://api.heclouds.com/cmds?device_id=1158646451",
       header: {
         'content-type': 'application/json',
         'api-key': apikey 
       },
       method: 'POST',
       data:{"LED2":1},
       success(res){
         console.log("成功",res.data)
       },
       fail(res){
         console.log("失败",res)
       }
     })
  },
 
  guan:function(){
  //按钮发送命令控制硬件
   wx.request({
     url:"https://api.heclouds.com/cmds?device_id=1158646451",
     header: {
       'content-type': 'application/json',
       'api-key':apikey
     },
     method: 'POST',
     data:{"LED2":0},
     success(res){
       console.log("成功",res.data)
     },
     fail(res){
       console.log("失败",res)
     }
   })
 },
 


  onLoad() {
    var that = this
    setInterval(function(){
      that.getinfo()
    },3000)

  }
})
