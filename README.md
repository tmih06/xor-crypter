# Basic static XOR crypter
-This can be used to encrypt executable files.

-please dont use this as a fud crypter, it wont work :)

-btw this only work on window 10+

# Preparing
-compile both xor.cpp and stub.cpp to executable files first

-xor.cpp --> xor.exe

-stub.cpp --> stub.exe

-these files have to be in the same folder to work

-I'm using gcc compiler (haven't test on clang or cmake or anything else)

**Compile `xor.cpp`:**
```shell
g++ .\xor.cpp -o xor
```

**Compile `stub.cpp`:**
```shell
g++ .\stub.cpp -o stub -static
```
**The stub must be compiled with the `-static` flag so it can work on every device**

# How to use
open cmd in the same folder

**Use:**
```shell
xor.exe <input_path> -o <output_path> -k <key>
```
The encryption key can be selected specifically or randomly if left blank

**Example:**
```shell
xor.exe my_file.exe -o my_encrypted_file -k ambatukam
```

**Or just:**
```shell
xor.exe my_file.exe
```
This way the default output would be Encrypted_my_file.exe and the encryption key would be random

# How does it work ?
**The stub:** read data from it self's resource then decrypt the data and run in memory

**The xor encryptor:** red data from selected file then encrypt the data and write it and the key in to stub.exe's resource
![how it work](https://github.com/tmih06/xor-crypter/blob/main/how%20it%20work.png)
