# Contents
   * [Configuration Notes on "CentOS on Raspberry Pi"](#configuration-notes-on-centos-on-raspberry-pi)
      * [Common Configurations](#common-configurations)
      * [net-snmp Configurations](#net-snmp-configurations)
         * [1. install](#1-install)
         * [2. configure the agent snmpd](#2-configure-the-agent-snmpd)
         * [3. run the agent](#3-run-the-agent)
         * [4. client-side tools](#4-client-side-tools)
            * [common options for displaying OID](#common-options-for-displaying-oid)
            * [snmptable](#snmptable)
            * [snmptranslate](#snmptranslate)
         * [some convenience](#some-convenience)
# Configuration Notes on "CentOS on Raspberry Pi"

> The used OS mirror is `CentOS-Userland-7-armv7hl-RaspberryPI-GNOME-2003-sda`.

## Common Configurations
1. Set password.

2. Modify hostname.

3. (optional) Install some good programs. Say, `lrzsz`, `wget` and so forth.

4. (optional) Install `zsh` and Oh My Zsh. 

	Use the theme `agnoster`. Modify the theme itself a little bit.

5. Modify initialization files for shells.

> **Note for `zsh` on CentOS:**
>
> Initialization files for `zsh` are: `/etc/zlogin`, `/etc/zshrc` and so forth, which are different from `/etc/zsh/z*` on Debian-based systems like Ubuntu and Raspbian.
> 
> Moreover, `/etc/zprofile` will source `/etc/profile`, which is the login file for `bash`. Therefore, this part in `/etc/zprofile` had better be commented out.

## `net-snmp` Configurations

> The `net-snmp` package used here is `net-snmp-5.7.3.tar.gz`.

Though strongly recommended against in production environment, run as super user for convenience.

### 1. install

``` shell
yum -y install perl-devel file perl-Data-Dumper
```
where `perl-Data-Dumper` is for `snmpconf -g basic_setup` to run later.
```shell
cd ~
tar -zxvf net-snmp-5.7.3.tar.gz
cd net-snmp-5.7.3
```

```shell
./configure \
--with-default-snmp-version="3" \
--with-sys-contact="junwuhn@sjtu.edu.cn" \
--with-sys-location="Shanghai" \
--with-logfile="/var/log/snmpd.log" \
--with-persistent-directory="/var/net-snmp" \
&& make && make install
```
### 2. configure the agent `snmpd`

There are alternative ways:
1. run the following command to configure `snmpd` in an interactive way:
	```shell
	snmpconf -g basic_setup
	```
	the resulting `snmpd.conf` will be located in `${PWD}`
	<!--- minimum security level? -->
2. make a copy of `~/net-snmp-5.7.3/EXAMPLE.conf` and modify it as needed.

`snmpd.conf` consists of a series of *token*s, which is simply `<variable> <value>` declarations, also a.k.a *configuration directive*s. Put the final configuration file in `/usr/local/etc/snmp/` (the direcotry may not exist at first).

> See `man 5 snmp_config` for alternative paths to put `net-snmp` configuration files.
>
> Also refer to `snmpd -H` to see acceptable tokens for `snmpd`.

### 3. run the agent
```shell
snmpd -Lo -fadV
```
- `-L[o|e|f|s]` specifies where to ouput information. `o` is good, which stands for `stdout`.

- `-f` means running in the foreground. Otherwise the agent runs as a daemon.

- `-a`: show the source address of incoming messages.

- `-d`: act on SNMP messages like `tcpdump` does.

- `-V`: adopt a more concise output format.

### 4. client-side tools

#### common options for displaying OID

- `-Of`: full.

- `-On`: numeric OID.

- `-Os`: string name.

- `-OS`: string name plus MIB name.

- `-Ot`: timeticks in its raw value.

- `-OX`: underscore table indices.

#### `snmptable`
 
```shell
snmptable -v1 -c public -Cc 10 -Cw 100 -Ci localhost IF-MIB:ifTable
```
- `-Cc`: specify column width.

- `-Cw`: specify line width.

- `-Ci`: show index before each row.

> It seems nice to pipe the output to other programs like `awk`. For example:
> 
>	```shell
>	snmptable -v1 -c public  localhost IF-MIB:ifTable | awk 'NR > 2 {printf "%-8s%-8s%-17s%-6s\n", $1, $2, $3, $4}'
>	```

<!--- 20200717 -->

http://www.net-snmp.org/docs/man/

#### `snmptranslate`

### some convenience
Add this to rc files:
<!---
```shell
alias ks="kill -9 \`ps aux | grep snmpd | awk '\$11 != \"grep\" {print \$2}'\`"
```
the following one may be better:
-->
```shell
alias ks="kill -9 \`ps aux | awk '\$11 == \"snmpd\" {print \$2}'\`"
```
with `ks` standing for "kill snmpd".
---

Applications - System Tools - Settings - 
Disable black screen. (not sure if it is working?) (seems good)
Shortcut for GNOME terminal (not really necessary because of the bad GUI experience)

vim /etc/vconsole.conf
sun12x22

adaptive theme of oh my zsh


TODO: sudo without passwd? pi is already a memeber of "wheel"
	disable dmesg on TTY1
## EPEL for ARM
vim /etc/yum.repos.d/epel.repo
```
[epel]
name=Epel rebuild for armhfp
baseurl=https://armv7.dev.centos.org/repodir/epel-pass-1/
enabled=1
gpgcheck=0
```
```shell
yum update
yum repolist
```
```
yum -y install terminus-fonts-console
```
ter-128n


yum -y install usbutils pciutils

> It seems that Raspberry Pi doesn't have PCI bus.

Boot Mode
```shell
systemctl get-default
systemctl set-default multi-user.target
systemctl set-default graphicl.target
```
```shell
yum -y install hwinfo
hwinfo --help 2>/dev/stdout | less
hwinfo --network | less
```

dmesg -D
dmesg -E
/etc/rc.local

static IP through GUI (NetworkManager)
==> how does NetworkManager.service and networkd.service work? If the GUI setting is done, no need to create the config file?
[Unit]
Description=Disable dmesg ouputs to tty

[Service]
ExecStart=/bin/dmesg -D

[Install]
WantedBy=multi-user.target

systemctl enable quietworld.service
/etc/systemd/system/...


dmesg 's color option does not support arguments ("always")
a workaround dmesg -H (--human) pager view and colored fonts


## latest
sudo systemctl status firewalld

tcpdump -i any 'udp port 161 or tcp port 10086'


snmpd -Lo -f tcp:10086

snmpget -v2c -c public tcp:happy508.zicp.io:56913 1.3.6.1.2.1.1.1.0
