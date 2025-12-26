## Host Machine Setup (setting up intellisence on VS Code):

See the following page for install esp idf and the required build tools from:
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html 
TODO: install idf.py then run
```
idf.py reconfigure
```
after this is built find <b>build/compile_commands.json</b>
2. Add the following to <b>.vscode/c_cpp_properties.json</b>. This will allow intellisense to use the the esp-idf toolchain instead of our default compiler.
```
{
    "configurations": [
        {
            "name": "Mac",
            "compileCommands": "${workspaceFolder}/build/compile_commands.json"
        }
    ],
    "version": 4
}
```



## Host Machine Clange Format Setup (VS Code)
1. Install clang-format with brew
```
brew install clang-format
```
2. Add the following your "User Settings (JSON)"
```
"editor.formatOnSave": true,
"clang-format.executable": "/usr/local/bin/clang-format",
"[c]": {
    "editor.defaultFormatter": "ms-vscode.cpptools"
}
```

# VM Setup

## Headless init of VirtualBox VM
```
VBoxManage startvm "ubuntu 24.04.3" --type headless
```

## Port forwarding from mac to vm for ssh connection
1. there's a config inside virtual box 

## Setting up serial port link:
1. Note you'll have to reboot after this step
```
sudo usermod -aG dialout $USER
```


## Copying files from mac to vm
```
rsync -avz \
  --exclude build/ \
  --exclude sdkconfig \
  --exclude sdkconfig.old \
  -e "ssh -p 2222 -i ~/.ssh/<priv_key>" \
  /Users/davidperez/Documents/GitHub/alarm-system/ \
  dubuntu@127.0.0.1:/home/dubuntu/projects/alarm_system/
```

