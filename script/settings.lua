local module  = {}

local storageName = "kvstorage"
local storage = {}

local function loadStorage()
  if file.open(storageName, "r") == nil then
    return
  end

  while true
  do
    line = file.readline()
    if line == nil then
      break
    end

    key, value = line:match("([^,]+),([^,]+)")
    if key ~= nil then
      storage[key] = value
    end
  end

  file.close()
end

local function saveStorage()
  file.open(storageName, "w")

  for k, v in pairs(storage) do
    file.writeline(k .. "," .. v)
  end

  file.close()
end

function module.set(key, value)
  local update = storage[key] ~= value
  storage[key] = value

  if update then
    saveStorage()
  end
end

function module.get(key, defvalue)
  if storage[key] == nil then
    module.set(key, defvalue)
  end

  return storage[key]  
end

loadStorage()

return module
