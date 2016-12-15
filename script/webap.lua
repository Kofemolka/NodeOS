m = mqtt.Client("ESP", 120, "czaauaxu", "Fg9Kmwf-1V31")

print("Ready to start soft ap AND station")
     local str=wifi.ap.getmac();
     local ssidTemp=string.format("%s%s%s",string.sub(str,10,11),string.sub(str,13,14),string.sub(str,16,17));
     wifi.setmode(wifi.STATIONAP)
     
     local cfg={}
     cfg.ssid="ESP8266_"..ssidTemp;
     cfg.pwd="12345678"
     wifi.ap.config(cfg)
     cfg={}
     cfg.ip="192.168.2.1";
     cfg.netmask="255.255.255.0";
     cfg.gateway="192.168.2.1";
     wifi.ap.setip(cfg);
     
     wifi.sta.config("SS-MOBILE","")
     wifi.sta.connect()
     
     local cnt = 0
     gpio.mode(0,gpio.OUTPUT);
     tmr.alarm(0, 1000, 1, function() 
         if (wifi.sta.getip() == nil) and (cnt < 20) then 
             print("Trying Connect to Router, Waiting...")
             cnt = cnt + 1 
                  if cnt%2==1 then gpio.write(0,gpio.LOW);
                  else gpio.write(0,gpio.HIGH); end
         else 
             tmr.stop(0);
             print("Soft AP started")
             print("Heep:(bytes)"..node.heap());
             print("MAC:"..wifi.ap.getmac().."\r\nIP:"..wifi.ap.getip());
             if (cnt < 20) then print("Conected to Router\r\nMAC:"..wifi.sta.getmac().."\r\nIP:"..wifi.sta.getip())
                 else print("Conected to Router Timeout")
             end
     gpio.write(0,gpio.LOW);
             cnt = nil;cfg=nil;str=nil;ssidTemp=nil;
            collectgarbage()

			setupMqtt()

			setupWebServer()
         end 
     end)

	 function setupMqtt()
		-- setup Last Will and Testament (optional)
		-- Broker will publish a message with qos = 0, retain = 0, data = "offline" 
		-- to topic "/lwt" if client don't send keepalive packet
		m:lwt("/lwt", "offline", 0, 0)

		m:on("connect", function(client) print ("mqtt:connected") end)
		m:on("offline", function(client) print ("mqtt:offline") end)

		m:on("message", function(client, topic, data) 
		  print("MQTT Msg:" .. topic .. ":" ) 
		  if data ~= nil then
			print(data)
		  end
		end)

		m:connect("m20.cloudmqtt.com", 17690, 0, function(client) print("connected") end, 
											 function(client, reason) print("failed reason: "..reason) end)

	end

	function setupWebServer()
		 local srv=net.createServer(net.TCP)
		 srv:listen(80,function(conn)
			conn:on("receive", function(conn,payload)
				print("[Request]")
				print(payload)
				m:publish("/esp/out", payload,0,0, function(client) print("sent") end)
			end)
		 end)
	end


	

