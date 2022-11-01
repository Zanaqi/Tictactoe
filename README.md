To install gtk4 (windows)

In MSYS2 shell, run:

pacman -S mingw-w64-x86_64-gtk4

proceed with installation

after installation complete, run:

pacman -S mingw-w64-x86_64-toolchain base-devel

install everything

make sure you've set your system variable (path)
if not, look for where your mingw64 bin file is (should be standard filepath)

control panel -> search for system -> edit the system environment variables -> environment variables 
-> under system variables scroll down to Path -> double click -> new -> paste file path

![image](https://user-images.githubusercontent.com/99416793/199211118-92062400-01c1-4587-8762-bb14d882c8bd.png)

 
