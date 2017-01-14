local module = {}

local _FS="/dev/fs/"
local _LS=_FS .. "ls"
local _CAT=_FS .. "cat"
local _PUT=_FS .. "put"
local _RM=_FS .. "rm"
local _STATUS=_FS .. "status"

function module.init(env)
  module.pub = env.pub
  env.sub(_LS, module.ls)
  env.sub(_CAT, module.cat)
  env.sub(_PUT, module.put)
  env.sub(_RM, module.rm)
end

function module.ls(d)
  local s = ""
  for k,v in pairs(file.list()) do
     s = s .. string.format("%-15s %i\n",k,v)
  end
  module.pub(_STATUS, s)
end

function module.cat(d)
  if d==nill or file.open(d,"r")==nil then
    module.pub(_STATUS, "File not found")
    return
  end

  f = ""
  while true
  do
    l=file.readline()
    if l==nil then
      break
    end
    f=f .. l
  end
  file.close()

  module.pub(_STATUS, f)
end

function module.put(d)

end

function module.rm(d)
  if d==nill or not file.exists(d) then
    module.pub(_STATUS, "File not found")
    return
  end

  file.remove(d)
  module.ls(d)
end

return module
