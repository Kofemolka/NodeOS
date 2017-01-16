local module = {}

function module.start(port)
    if port == nil then port = 23 end
    module.stop()

    function listenFun(socket)
      local fifo = {}
      local fifo_drained = true

      local function sender(c)
          if #fifo > 0 then
              c:send(table.remove(fifo, 1))
          else
              fifo_drained = true
          end
      end

      local function s_output(str)
          table.insert(fifo, tostring(str))
          if socket ~= nil and fifo_drained then
              fifo_drained = false
              sender(socket)
          end
      end

      node.output(s_output, 0)
      socket:on("receive", function(c, l) node.input(l) end)
      socket:on("disconnection", function(c) node.output(nil) end)
      socket:on("sent", sender)

      print("Greetings, Master\n")
    end

    module.srv = net.createServer(net.TCP, 180)
    module.srv:listen(port, listenFun)
end

function module.stop()
  if module.sock ~= nil then module.sock:close() module.sock = nil end
  if module.srv ~= nil then module.srv:close() module.srv = nil end
end

return module
