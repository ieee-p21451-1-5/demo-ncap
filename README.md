# Set Up an NCAP That Provides IEEE P21451-1-5 Services

## 1. Prerequisites 

- A Raspberry Pi 3B or alternatively, a PC.
- CentOS image for ARM architecture ([download link](http://mirror.centos.org/altarch/7/isos/armhfp/ ))  or alternatively, the image for your custom architecture ([list of available architectures and download links](http://isoredirect.centos.org/)).
- `net-snmp`  source code ([download link](http://www.net-snmp.org/download.html)).

In this tutorial, we use `CentOS-Userland-7-armv7hl-RaspberryPI-GNOME-2003-sda1` (which is for Raspberry Pi) and `net-snmp-5.7.3` as the example.

## 2. Prepare the Operating System

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

### 2.4 Not Mandatory, but Helpful Settings

- Add a `\l` at some appropriate place inside `\etc\issue` , that prompts which `tty` you are on when trying logging in.

- If the font on your screen is too small to read, change it to another one located in `/lib/kbd/consolefonts/`. `sun12x22`, for example, is a nice option. 

  - To change the font immediately but temporarily:

    ```shell
    setfont sun12x22 
    ```
    
  - To make it the default option every time the system boots, open `/etc/vconsole.conf` and change `FONT="WHATEVER"` to `FONT="sun12x22"`.
  
- Make the console quieter:

  ```shell
  dmesg -D
  ```

<!---other useful softwares: zsh (with plugin) lrzsz-->

## 2. Install `net-snmp`

This tutorial continues, assuming you are the super user. It makes things easy but is **strongly recommended against** in practical deployment.

For those who are strict with security and resist taking this risk (which is good), here are some tips:

- **A lot of** commands need privileges to be run and therefore `sudo` should precede them.
- When `sudo`ing, pay attention to the environment, e.g. (and especially) `$PATH` .

<!--- sudo how to specify PATH? -->

### 2.1 Dependencies 

```shell
yum -y install gcc file perl-devel perl-Data-Dumper
```

### 2.2 Basic Installation

By *basic*, we mean that the installed SNMP server can only provide traditional network management services. For example, you can ask the server for its ARP cache, or change the behavior of a certain NIC on it.

In other words, as this moment, the server cannot provide IEEE P1451 services.  

#### Installation Process

Unpack the source and switch to its root directory:

```shell
tar -zxvf net-snmp-5.7.3.tar.gz
cd net-snmp-5.7.3
```

We denote the root directory of the source as `${NET-SNMP-SRC-ROOT}`.

Start building the software: 

```shell
./configure \
--with-default-snmp-version="3" \
--with-sys-contact="Jun WU <junwuhn@sjtu.edu.cn>" \
--with-sys-location="Shanghai, China" \
--with-logfile="/var/log/snmpd.log" \
--with-persistent-directory="/var/net-snmp" \
--prefix="/usr/local/" \
&& make && make install
```

<!---agent选项的作用体现在哪？它也没有生成一个初始的snmpd.conf啊？-->

Explanation:

By default, `configure` will interactively ask you some questions before finishing its job. The first 5 options, however, give immediate answer to these questions and suppress the tedious interactive procedure. They can truly save the day. 

For each option's meaning, run `configure` without any option and see what it prompts. Modify the value part in these options if you would like to. 

<!--- or use "--with-defaults"  -->

The last option specifies the target directory of installation, which defaults to `/usr/local`. We denote this directory as `${NET-SNMP-HOME}`. In the command above, we explicitly set `${NET-SNMP-HOME}` to its default value.  If a none-default value is specified, you may have to type in the full pathname of the program when you execute commands later.

#### Important Directories and Files of `net-snmp`

- `${NET-SNMP-SRC-ROOT}/agent/mibgroup/`: Source code that handles read/write requests for each management object in MIBs. Examples of native implementations (i.e. for traditional SNMP applications) are given below:

  | File Pathname           | Management Object(s)              |
  | ----------------------- | --------------------------------- |
  | `mibII/system_mib.[ch]` | 1.3.6.1.2.1.1 (mib-2.system)      |
  | `mibII/interfaces.[ch]` | 1.3.6.1.2.1.2  (mib-2.interfaces) |
  | `mibII/interfaces.[ch]` | 1.3.6.1.2.1.4 (mib-2.ip)          |

  <!---TODO 这些源码要好好研究一下 -->

  The code for your custom MIBs will also be put here.

- `${NET-SNMP-HOME}/bin`: SNMP client programs (like `snmpget`), utilities (like `mib2c`)

- `${NET-SNMP-HOME}/sbin/snmpd`: SNMP daemon (server) program

- `${NET-SNMP-HOME}/share/snmp/mibs/*.txt`: MIB files in textual format 

- `${NET-SNMP-HOME}/share/snmp/snmpd.conf`: `snmpd` configuration file (There are other possible locations. See `man snmp_config` for more information.)

#### Client

Once the installation finishes, you should be able to run: 

```shell
snmpget -v2c -c public 47.88.61.169 1.3.6.1.2.1.1.1.0
```

and see results similar to:





#### Server





Make it a `systemd` service

```shell
cd ~root/
git clone https://github.com/ieee-p21451-1-5/demo-ncap.git
cp ./demo-ncap/sync/check-sync.service /etc/systemd/system/
cp ./demo-ncap/snmpd/systemd/* /etc/systemd/system/
systemctl daemon-reload
systemctl enable check-sync.service snmp-daemon.service snmpd-traffic.service 
```



remove



/etc/systemd/system/

/etc/systemd/system/default.target.wants/



create a sync test service

yum -y install ntp





git clone 





### 2.3 Custom MIBs







## SNMP临时

https://stackoverflow.com/questions/21564/is-there-a-unix-utility-to-prepend-timestamps-to-stdin

创建AWK脚本`snmpd_output_process.awk` ：

```awk
{
	if (NR == 1) {
		print $0 # 针对第一行
	}
	else {
		if ($2 == "TERM")
			color = 33
		else
			color = 34
		print strftime("%Y/%m/%d %H:%M:%S"), "\033[01;"color"m"$0"\033[00m"; fflush(); 
	}
}
```

创建脚本`snmp-daemon.sh` 

```shell
#! /bin/bash
cd /root/
/usr/local/sbin/snmpd -Lo -af | awk -f ./snmpd_output_process.awk > \
./snmpd-log-$(date +'%Y%m%d%H%M%S')
```









