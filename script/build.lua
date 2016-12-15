local files = {
	"config.lua",
	"boot.lua",
	"app.lua"}

function fileExist(fileName)
	if file.open(fileName,"r") then
		file.close(filename)
		return true
	end
	return false
end

for i, f in ipairs(files) do
	if fileExist(f) then
		if pcall(function() node.compile(f) end) then
			print("Compiled:" .. f)
			file.remove(f)
		end
	end
end

for k,v in pairs(file.list()) do 
	l = string.format("%-15s",k)
	print(l.."   "..v.." bytes")
end

