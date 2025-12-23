

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
rsync -avz -e "ssh -p 2222 -i ~/.ssh/<priv_key>" /Users/davidperez/Documents/GitHub/alarm-system/ dubuntu@127.0.0.1:/home/dubuntu/projects/alarm_system/
```

