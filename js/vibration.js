 function startVibration(path)
      {  var id=get_outgoing_id();
        var jsonrpc={
        "method":"readFFT",
        "mode":2,
        "path":path,
        "params":{"value":[0]},
        "id": id
       }
          console.log(jsonrpc);
            window.restbed.ws.send(JSON.stringify(jsonrpc));
            
      }
      function stopVibration()
      {

      }
      function readVibration()
      {

      }
      function readRawvibration()
      {
          
      }