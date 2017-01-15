local module = {}

function module.start()
    module.stop()
    inUse = false
    function listenFun(sock)
        module.sock = sock
        if inUse then
            sock:send("Already in use.\n")
            sock:close()
            return
        end
        inUse = true

        function s_output(str)
            if(sock ~=nil) then
                sock:send(str)
            end
        end

        node.output(s_output, 0)

        sock:on("receive",function(sock, input)
                node.input(input)
            end)

        sock:on("disconnection",function(sock)
                node.output(nil)
                inUse = false
            end)

        sock:send("Greetings, Master!\n> ")
    end

    module.srv = net.createServer(net.TCP, 180)
    module.srv:listen(23, listenFun)
end

function module.stop()
  if module.sock ~= nil then module.sock:close() module.sock = nil end
  if module.srv ~= nil then module.srv:close() module.srv = nil end
end

return module
