
var ClientWebInterface = function(baseUrl, initCallback)
{
    var socket = new WebSocket(baseUrl);
                  socket.onclose = function()
                  {
                      console.error("web channel closed");
                  };
                  socket.onerror = function(error)
                  {
                      console.error("web channel error: " + error);
                  };

                  socket.onopen = function()
                  {
                       new QWebChannel(socket, function(channel) {
                          clientJS = channel.objects.clientJS;
                          clientJS.call();
                      });
                  }

}
