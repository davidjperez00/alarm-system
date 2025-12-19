# alarm-system



## Host Machine Setup


### WSL Install
1. Open powershell as administrator and run (Project Version):
```wsl --intall```

### WSL Connect Serial Ports From Windows
1. When we setup esp idf project build system we will have to connect our Windows usb serial ports to our WSL operation system.
```winget install usbipd ```
2. RESTART your pc, <b>This is only a one time process</b>, once you set
this up it will connect the port service on all boots?
3. After the reboot, In a Powershell window, connect the windows usb port to a WSL port. <b>You will need to do this every reboot</b>. 
</r>TODO: consider connecting the port and through windows only? 

In Powershell admin terminal, look for device Connected to Windows machine: 
```
usbipd list
```

5.  Bind port to WSL OS, Example: 7-1
```
usbipd bind --busid <BUSID>
usbipd attach --wsl --busid <BUSID>
```
6. Back in wsl windows you should see new device connected:
```
# New device should appear in list
ls /dev/tty*
```
7. Then, again in WSL terminal Give current user permission to access port: (TODO: might only need to do once.)
```
sudo usermod -a -G dialout $USER
```

### Github SSH Key Setup for WSL
1. In WSL create keys:
```ssh-keygen -t ed25519 -C "your_email@example.com"```
2. Create directory and manage permissions (Optional), replace <ssh_private_key> with private key name from last step.
```
# Create ssh directory
install -d ~/.ssh
chmod 700 ~/.ssh
# Move private key to ssh directory and set permissions
mv <ssh_private_key>
chmod 600 <ssh_private_key>
```
3. Start ssh agent to add ssh key to working memory (avoids having to specify the key with "ssh -i")
```
# launch ssh-agent
eval "$(ssh-agent -s)"
# Add the key to the agent
ssh-add ~/.ssh/<ssh_private_key>
```


### Required Software Setup 
1. Run the following to install all required software for building this project:
```
./host_dependencies_setup.sh
```

### Building and Running Project
1. Setup esp-idf environemnt variables by running (Only required ONCE when new terminal is opened):
```
source $IDF_PATH/export.sh
```
1. 1 (Optional) Add it to bash profile to load everytime bash terminal window is launched
```
echo "source $IDF_PATH/export.sh" >> ~/.bashrc
```
2. Setting our own ports environment variables?
```
TODO: implemement and test this:
export IDF_TARGET=esp32
export ESPPORT=
export ESPBAUD=
```

3. Building from <b>Src/blink</b>
```
idf.py build
```


4. 

5. Flashing to Device
```
idf.py flash 
```



