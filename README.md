# lcransom

"Ransomware" (not really harmfull the way it is now)

## Build

- Could easily support Windows (pr are welcome)

- On Linux do `make`
> You will need the openssl lib

>> can change the IP:PORT in random.lua:33

## Usage

```
usage to encrypt:
  ./ransom [path]
usage to decrypt:
  ./ransom [path] [key] [iv]
```
> decrypt if the file extensions is .Pwnd
> encrypt if the file extensions isnt't .Pwnd

>> can change the extension in ransomlib.h:14

## Why

Just to do some C and learn how to glue Lua and C together
