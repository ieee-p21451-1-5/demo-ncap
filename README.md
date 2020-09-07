# Set up an NCAP that provides IEEE P21451-1-5 services

## 1. Prerequisites 

- A Raspberry Pi 3B or alternatively, a PC
- CentOS image for ARM architecture ([download link](http://mirror.centos.org/altarch/7/isos/armhfp/ ))  or alternatively, the image for your custom architecture ([list of availavle architectures and download links](http://isoredirect.centos.org/))
- `net-snmp`  source code ([download link](http://www.net-snmp.org/download.html))

In this tutorial, we use `CentOS-Userland-7-armv7hl-RaspberryPI-GNOME-2003-sda1` (which is for Raspberry Pi) and `net-snmp-5.7.3` as the example.

## 2. Prepare the Operation System

### Initialization

Burn the image onto an SD card and boot the Raspberry Pi with the card inserted.

Though a GNOME-version image was chosen, the high resource consumption of the desktop environment will deeply discourage you from using it, especially on a Raspberry Pi. The instructions in the rest of this tutorial are all accessible in command line interface (CLI) mode. If you are familiar enough with Linux CLI, you can even skip the GUI initial setup by typing `<Ctrl>+<Alt>+<F2>` to switch to virtual console `tty2`. Otherwise, continue with the GUI mode.

We assume you are an experienced Linux geek :)

In CLI mode, login as the super, who has a default password `centos`. Change it right now for security.

Let the system boot into CLI mode automatically the next time:

```shell
systemctl set-default multi-user.target
```

Follow the instructions in `~root/README`, run this command:

```shell
rootfs-expand
```

to make more storage space available. 

### Network



### Install Some Utilities 



### Not mandatory, but helpful settings

- Add an `\l` at some appropriate place inside `\etc\issue` , that prompts which `tty` you are on when trying logging in.

- If the font on your screen is too small to read, change it to another one located in `/lib/kbd/consolefonts/`. `sun12x22`, for example, is a nice option. 

  - To change the font immediately but temporarily:

    ```shel
    setfont sun12x22 
    ```
    
- To make it the default option every time the system boots, open `/etc/vconsole.conf` change `FONT="whatever"`to `FONT="sun12x22"`.
  
- Make the console quiet 

  

  



## 2. Install `net-snmp`

We continue as the super user.

It is 

If you are strict with security, some of the commands especially $PATH 

<!--- sudo how to specify PATH? -->