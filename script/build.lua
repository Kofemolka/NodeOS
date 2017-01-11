local files = {
	"config.lua",
	"boot.lua",
	"app.lua",
	"update.lua"}

function fileExist(fileName)
	if file.open(fileName,"r") then
		file.close(filename)
		return true
	end
	return false
end

for i, f in ipairs(files) do
	if fileExist(f) then
		local status, err = pcall(function() node.compile(f) end)
		if status then
			print("Compiled:" .. f)
			file.remove(f)
		else
			print("Error compiling " .. f ..":" .. err)
		end
	end
end

for k,v in pairs(file.list()) do
	l = string.format("%-15s",k)
	print(l.."   "..v.." bytes")
end
