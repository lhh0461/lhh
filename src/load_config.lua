function string.split(str, delimiter)
    if str == nil or str == '' or delimiter == nil then
        return nil
    end

    local result = {}
    for match in (str..delimiter):gmatch("(.-)"..delimiter) do
        print(result, match)
        table.insert(result, match)
        end
    return result
end

function Split(szFullString, szSeparator)  
    local nFindStartIndex = 1  
    local nSplitIndex = 1  
    local nSplitArray = {}  
    while true do  
        local nFindLastIndex = string.find(szFullString, szSeparator, nFindStartIndex)  
        if not nFindLastIndex then  
            nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, string.len(szFullString))  
            break  
        end  
        nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, nFindLastIndex - 1)  
        nFindStartIndex = nFindLastIndex + string.len(szSeparator)  
        nSplitIndex = nSplitIndex + 1  
    end  
    return nSplitArray  
end  


--local str = "MASA  =    123"
--local subStr = Split(str, "=")
--print(subStr[1])
--print(subStr[2])
 
--local i, j =string.find(str, "%s=%s")
--subStr =string.sub(str, i, j)
--print(subStr)


--local config_name = ...
--local file = io.open(config_name, "r")
--local code = assert(file)
--config = {}
--for line in file:lines() do
--    local elem = Split(line, "=")
--    config[elem[1]] = elem[2]
--end
--
--for k,v in pairs(config) do
--    print(k,v)
--end
--file:close()

local config_name = ...
local file = assert(io.open(config_name))
local code = assert(file:read '*a')
local function getenv(name) 
    return assert(os.getenv(name), 'os.getenv() failed: ' .. name) 
end
code = string.gsub(code, '%$([%w_%d]+)', getenv)
file:close()
local result = {}
print(code)
assert(load(code,'=(load)','t',result))()
--for k,v in pairs(result) do
--    print(k,v)
--end
return result
