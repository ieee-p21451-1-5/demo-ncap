# Set Up an NCAP That Provides IEEE P21451-1-5 Services

## 1. Prerequisites 

- A Raspberry Pi 3B or alternatively, a PC
- CentOS image for ARM architecture ([download link](http://mirror.centos.org/altarch/7/isos/armhfp/ ))  or alternatively, the image for your custom architecture ([list of availavle architectures and download links](http://isoredirect.centos.org/))
- `net-snmp`  source code ([download link](http://www.net-snmp.org/download.html))

In this tutorial, we use `CentOS-Userland-7-armv7hl-RaspberryPI-GNOME-2003-sda1` (which is for Raspberry Pi) and `net-snmp-5.7.3` as the example.

## 2. Prepare the Operation System

### 2.1 Initialization

Burn the image onto an SD card and boot the Raspberry Pi with the card inserted.

Though a GNOME-version OS image was chosen, the high resource consumption of the desktop environment will deeply discourage you from using it, especially on a Raspberry Pi. The instructions in the rest of this tutorial are all accessible in command line interface (CLI) mode. If you are familiar enough with Linux CLI, you can even skip the GUI initial setup by typing `<Ctrl>+<Alt>+<F2>` and switching to virtual console `tty2`. Otherwise, continue with the GUI mode.

We now assume you are an experienced Linux geek :)

In CLI mode, login as the super user, who has a default password `centos`. Change it right now for security.

```shell
passwd
```

Let the system boot into CLI mode automatically the next time:

```shell
systemctl set-default multi-user.target
```

Follow the instructions in `~root/README`, run this command:

```shell
rootfs-expand
```

to make more storage space available in the SD card. 

### 2.2 Network

#### Ethernet

Once the cable is connected, the network should be working.

#### Wi-Fi

List accessible APs:

```shell
nmcli d wifi
```

Connect to one of them:

```shell
nmcli d wifi connect YOUR_SSID password YOUR_PASSWORD
```

#### Testing

To make sure the network is properly configured, test it in your preferred way. Mine is:

```shell
ping www.google.com
```

### 2.3 Text Editor 

```shell
yum update && yum -y install vim
```

### 2.4 Not mandatory, but helpful settings

- Add a `\l` at some appropriate place inside `\etc\issue` , that prompts which `tty` you are on when trying logging in.

- If the font on your screen is too small to read, change it to another one located in `/lib/kbd/consolefonts/`. `sun12x22`, for example, is a nice option. 

  - To change the font immediately but temporarily:

    ```shell
    setfont sun12x22 
    ```
    
  - To make it the default option every time the system boots, open `/etc/vconsole.conf` and change `FONT="WHATEVER"` to `FONT="sun12x22"`.
  
- Make the console quieter

  ```shell
  dmesg -D
  ```

<!---other useful softwares: zsh (with plugin) lrzsz-->

## 2. Install `net-snmp`

This tutorial continues, assuming you are the super user. It makes things easy but is **strongly recommended against** in practical deployment.

If you are strict with security and resist taking this risk (which is good), here are some tips for you:

- **A lot of** commands need privileges to be run and therefore `sudo` should precede them
- When `sudo`ing, pay attention to the environment, e.g. (and especially) `$PATH` 

<!--- sudo how to specify PATH? -->

### 2.1 Dependencies 

```shell
yum -y install gcc file perl-devel perl-Data-Dumper
```

### 2.2 Basic installation

Unpack the source and switch to its root directory:

```shell
tar -zxvf net-snmp-5.7.3.tar.gz
cd net-snmp-5.7.3
```

We denote the root directory of the source as `$NET-SNMP-SRC-ROOT`.

Start building the software: 

```shell
./configure \
--with-default-snmp-version="3" \
--with-sys-contact="Jun WU <junwuhn@sjtu.edu.cn>" \
--with-sys-location="Shanghai" \
--with-logfile="/var/log/snmpd.log" \
--with-persistent-directory="/var/net-snmp" \
#--prefix=/usr/local/net-snmp
&& make && make install
```

<!---agent选项的作用体现在哪？它也没有生成一个初始的snmpd.conf啊？-->

**Explanation:** By default, `configure` will interactively ask you some questions before finishing its job. The first 5 options, however, give immediate answer to these questions and suppress the tedious interactive procedure. They can truly save the day. For each option's meaning, run `configure` without any option and see what it prompts. Modify the value part of these options as you would like to. 

<!--- or use "--with-defaults"  -->

The option that is commented out specifies the target directory of installation. We denote this directory as `$NET-SNMP-HOME`. In this tutorial, we don't explicitly specify it, and it defaults to `/usr/local`.

Some useful directories and files of `net-snmp`:

- `${NET-SNMP-HOME}/bin`： SNMP client programs like `snmpget` etc.
- `${NET-SNMP-HOME}/sbin` `snmpd` etc.
- `${NET-SNMP-HOME}/share/snmp/mibs` MIB text files 

If success...Now you can run the 





### 2.3 Custom MIBs









