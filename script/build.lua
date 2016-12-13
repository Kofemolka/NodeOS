node.compile("config.lua")
node.compile("app.lua")
node.compile("boot.lua")

for k,v in pairs(file.list()) do l = string.format("%-15s",k) print(l.."   "..v.." bytes") end