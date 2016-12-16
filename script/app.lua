local module = {}

function module.init(env)
	print("Application started")

	local ledOn = false
	gpio.mode(4,gpio.OUTPUT)

	tmr.alarm(0, 500, 1, function()
		ledOn = not ledOn
		if ledOn then
			gpio.write(4,gpio.HIGH)
		else
			gpio.write(4,gpio.LOW)
		end
	end)
end

function module.shutdown()

end

return module
