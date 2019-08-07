function digitalWrite(path,val)
      { 
        
        var jsonrpc={
                  "method":"digitalOut.write",
                  "mode":1,
                  "path":path,
                  "params":{"value":[val]},
                  "id": get_outgoing_id()
                }
              console.log(jsonrpc);
         window.restbed.ws.send(JSON.stringify(jsonrpc));
       
      }
    function pwmOutSetup(path,period,pulsewidth)
    {

    }
    function analogInRead(mode,path)
    {

    } 
    function analogInSetup(path)
    {

    }
    function startInterruptin(mode,path)
      { 
        console.log("send to socket message");
        var jsonrpc={
                  "method":"interruptin.start",
                  "mode":mode,
                  "path":path,
                  "params":{"value":[1]},
                  "id": get_outgoing_id()
                }
         window.restbed.ws.send(JSON.stringify(jsonrpc));
      }
      
function modbusRead(mode,path)
{

}
function modbusWrite(mode,path)
{

}
function modebusSpeed(path,speed)
{

}