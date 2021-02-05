local io = require"io"
local string = require"string"

function io.readfile(filename)
  local file = io.open(filename, "rb")
  if file then
    local content = file:read("*a")
    file:close()
    return content
  end
end

function io.writefile(filename, content)
  local file = io.open(filename, "w+b")
  if file then
    file:write(content)
    file:close()
    return true
  end
end

local boot_bin = io.readfile("build/boot/boot.bin")
local kernel = io.readfile("build/kernel/kernel.elf")

local nos_img = io.open("nos.img", "w+b")

nos_img:write(boot_bin)
nos_img:write(string.rep("\x00", 510 - #boot_bin))
nos_img:write("\x55\xAA")

nos_img:write(kernel)
local nleft = (10*1024*1024) - #kernel - 512
nos_img:write(string.rep("\x00", nleft))

nos_img:close()
