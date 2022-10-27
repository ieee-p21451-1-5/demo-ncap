# Tutorial: How to set up an NCAP that provides IEEE P21451-1-5 services

> **NOTICE**
>
> This repository was mainly used at [INTEROP 2020](http://www.ieeeiciea.org/iecon2020/download/InterOp_IES_IECON2020_r7.pdf) for other working groups to set up their devices and networks that are interoperable with ours.
>
> *Some information is outdated as of 2022*. For example, that specific server mentioned in this article and used for our test purposes has been shut down.
>
> That being said, we are considering revising this repository, and making the code and docs generic as well as informational. Hopefully the revised contents can remain fresh and applicable over time.

## 1. Prerequisites

- A Raspberry Pi 3B or alternatively, a PC.
- CentOS image for ARM architecture ([download link](http://mirror.centos.org/altarch/7/isos/armhfp/ )) or alternatively, the image for your custom architecture ([list of available architectures and download links](http://isoredirect.centos.org/)).
- `net-snmp` source code ([download link](http://www.net-snmp.org/download.html)).

In this tutorial, we use `CentOS-Userland-7-armv7hl-RaspberryPI-GNOME-2003-sda1` (which is for Raspberry Pi) and `net-snmp-5.7.3` as the example.

## 2. Preparing the system

### 2.1 Initialization

Burn the image onto an SD card and boot the Raspberry Pi with the card inserted.

Though a GNOME-version OS image was chosen, the high resource consumption of the desktop environment will deeply discourage you from using it, especially on a Raspberry Pi. The instructions in the rest of this tutorial are all accessible in command line interface (CLI) mode. If you are familiar enough with shell, you can even skip the GUI initial setup by typing `<Ctrl>+<Alt>+<F2>` and switching to virtual console `tty2`. Otherwise, continue with the GUI mode.

We now assume you are an experienced Linux geek :)

In CLI mode, login as the super user, who has a default password `centos`. Change it right now for security.

```shell
passwd
```

Let the system boot into CLI mode automatically the next time:

```shell
systemctl set-default multi-user.target
```

As instructed in `/root/README`, run this command:

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

### 2.3 Text editor

```shell
yum -y update && yum -y install vim
```

### 2.4 Not mandatory, but helpful settings

- Add a `\l` at some appropriate place inside `/etc/issue` , that prompts which `tty` you are on when trying logging in.
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

## 2. Installing `net-snmp`

This tutorial continues, assuming you are the super user. It makes things easy but is **strongly recommended against** in practical deployment.

For those who are strict with security and resist taking this risk (which is good), here are some tips:

- **A lot of** commands need privileges to be run and therefore `sudo` should precede them.
- When `sudo`ing, pay attention to the environment, e.g. (and especially) `$PATH` .

### 2.1 Dependencies

```shell
yum -y install gcc file perl-devel perl-Data-Dumper python-devel
```

### 2.2 Basic installation

By *basic*, we mean that the installed SNMP server can only provide traditional network management services. For example, you can ask the server for its ARP cache, or change the behavior of a certain NIC on it.

In other words, at this moment, the server cannot provide IEEE P1451 services.

#### Compilation

Extract the source and switch to its root directory:

```shell
tar -zxvf net-snmp-5.7.3.tar.gz
cd net-snmp-5.7.3
```

We denote the root directory of the source as `${NET_SNMP_SRC_ROOT}`.

Start building the software:

```shell
./configure                                           \
    --with-default-snmp-version="3"                   \
    --with-sys-contact="Jun Wu <junwuhn@sjtu.edu.cn>" \
    --with-sys-location="Shanghai, China"             \
    --with-logfile="/var/log/snmpd.log"               \
    --with-persistent-directory="/var/net-snmp"       \
    --prefix="/usr/local/"
make && make install
```

> **Explanation:**
>
> By default, `./configure` will interactively ask you some questions before finishing its job. The first 5 options, however, give immediate answer to these questions and suppress the tedious interactive procedure. They can truly save the day.
>
> For each option's meaning, run `./configure` without any option and see what it prompts. Modify the value part in these options if you would like to.
>
> The last option specifies the target directory of installation, which defaults to `/usr/local`. We denote this directory as `${NET_SNMP_PREFIX}`. In the command above, we explicitly set `${NET_SNMP_PREFIX}` to its default value.  If a none-default value is specified, you may have to type in the full pathname of the program when you issue certain commands later.
>

#### Important directories and files of `net-snmp`

- `${NET_SNMP_SRC_ROOT}/agent/mibgroup/`: Source code that handles read/write requests for each management object in MIBs. Examples of native implementations (i.e. for traditional SNMP applications) are given below:

  | File Pathname            | Management Object(s)              |
  | ------------------------ | --------------------------------- |
  | `mibII/system_mib.{c,h}` | 1.3.6.1.2.1.1 (mib-2.system)      |
  | `mibII/interfaces.{c,h}` | 1.3.6.1.2.1.2 (mib-2.interfaces)  |
  | `mibII/ip.{c,h}`         | 1.3.6.1.2.1.4 (mib-2.ip)          |

  This is also where you will craft your own code for your custom MIBs.

- `${NET_SNMP_PREFIX}/bin/`: SNMP client programs (like `snmpget`), utilities (like `mib2c`)
- `${NET_SNMP_PREFIX}/sbin/snmpd`: SNMP daemon (server) program
- `${NET_SNMP_PREFIX}/share/snmp/mibs/*.txt`: MIB files in textual format
- `${NET_SNMP_PREFIX}/share/snmp/snmpd.conf`: `snmpd` configuration file (There are other possible locations for storing configuration files. See [`snmp_config(5)`](http://www.net-snmp.org/docs/man/snmp_config.html) for more information.)

#### Client

Once the installation finishes, you should be able to run:

```shell
snmpget -v 2c -c public 106.14.14.168 1.3.6.1.2.1.1.1.0
```

and you should see results similar to:

```
SNMPv2-MIB::sysDescr.0 = STRING: Greetings from IEEE P21451-1-5 Working Group, Shanghai Jiao Tong University, Shanghai, China
```

> **Explanation:**
>
> `snmpget` is the program in `net-snmp` that sends [GetResquest PDU](https://en.wikipedia.org/wiki/Simple_Network_Management_Protocol#Protocol_details) and receives [GetResponse PDU](https://en.wikipedia.org/wiki/Simple_Network_Management_Protocol#Protocol_details).
>
> - `-v 2c` specifies the version of SNMP to use. Alternatives include `1`, `2c` and `3`.
> - `-c public` sets the value of [community string](https://en.wikipedia.org/wiki/Simple_Network_Management_Protocol#Protocol_details) to "public", which is recognized by the SNMP server. Community strings introduce bare-bones authentication in SNMPv1 and SNMPv2.
> - `106.14.14.168` specifies the destination of SNMP requests. An SNMP server maintained by IEEE P21451-1-5 working group listens to this address.
> - `1.3.6.1.2.1.1.1.0` is the [OID](https://en.wikipedia.org/wiki/Object_identifier) of the content that is being asked for. SNMP uses OID as its naming scheme. OIDs are organized in a hierarchical way. Each layer is represented by a number, and different layers are separated by a period. Such a numeric way of displaying OID can be converted to a textual, human-readable equivalent. For example, `1.3.6.1.2.1.1.1.0` can be translated into `iso.identified-organization.dod.internet.mgmt.mib-2.system.sysDescr.0`. It implies that we were asking the server for its system description string. Besides `sysDescr`, several other variables (management information) related to common system management can be found under the node `system`. A nice tool for browsing the structure of commonly recognized OIDs can be found on [this site](http://www.oid-info.com/).

In order to get a group of variables all at once:

```shell
snmpwalk -v 2c -c public 106.14.14.168 1.3.6.1.2.1.1
```

and you should see results similar to:

```
SNMPv2-MIB::sysDescr.0 = STRING: Greetings from IEEE P21451-1-5 Working Group, Shanghai Jiao Tong University, Shanghai, China
SNMPv2-MIB::sysObjectID.0 = OID: NET-SNMP-MIB::netSnmpAgentOIDs.10
DISMAN-EVENT-MIB::sysUpTimeInstance = Timeticks: (4710) 0:00:47.10
SNMPv2-MIB::sysContact.0 = STRING: Jun Wu <junwuhn@sjtu.edu.cn>
SNMPv2-MIB::sysName.0 = STRING: public-snmp-server
SNMPv2-MIB::sysLocation.0 = STRING: Shanghai, China
...
```

Besides *getting* variables' value, you can also *set* some of them, by issuing `snmpset`.

All client programs in the above examples send SNMP requests and receive SNMP responses. `net-snmp` also provides useful tool programs like `snmptranslate`, which don't actually send or receive any packets. See [`snmptranslate(1)`](http://www.net-snmp.org/docs/man/snmptranslate.html) for more information.

#### Server

First, tell the firewall to allow SNMP packets in.

```shell
firewall-cmd --zone=public --add-service=snmp             # once
firewall-cmd --zone=public --permanent --add-service=snmp # and for all
```

Verify that `firewalld` is working and that the port for SNMP is open:

```shell
systemctl status firewalld
firewall-cmd --zone=public --list-services
```

Next we should craft a bare-bones configuration file.

```shell
echo -e "rwcommunity\tpublic" > /usr/local/share/snmp/snmpd.conf
```

The above command creates a configuration file `snmpd.conf` with a single line:

```
rwcommunity	public
```

It basically tells the server to recognize "public" as the [community string](https://en.wikipedia.org/wiki/Simple_Network_Management_Protocol#Protocol_details) and to allow whoever can provide this string to perform read/write operations using SNMP. We won't dig into the details of SNMP daemon configuration. Those interested may refer to [`snmpd.conf(5)`](http://www.net-snmp.org/docs/man/snmpd.conf.html).

With the configuration file created, we can finally launch the SNMP daemon:

```shell
snmpd -Lo -af
```

> **Explanation:**
>
> `snmpd` is the SNMP daemon program provided by `net-snmp` that accepts incoming SNMP requests and sends back response messages. By default, it listens on UDP port 161 on all IPv4 interfaces.
>
> - `-Lo` specifies that the daemon program will output its log to `stdout`.
> - `-a` makes the program display the source address of incoming packets.
> - `-f` prevents the program from `fork`ing from the shell.

Open another terminal and try:

```shell
snmpget -v 2c -c public localhost 1.3.6.1.2.1.1.1.0
```

You should see results that are a subset of `uname -a` output. For example:

```
SNMPv2-MIB::sysDescr.0 = STRING: Linux ncap 5.4.56-v7.1.el7 #1 SMP Sat Aug 8 20:58:22 UTC 2020 armv7l
```

At this moment, you are able to manually launch an SNMP daemon, which, however, vanishes once you log out from the terminal. To let `snmpd` start automatically in the background each time the machine boots, make it a `systemd` service. We will name it as `snmp-daemon.service`.

Before doing that, we have to look after yet another thing: system time synchronization.

Since Raspberry Pi doesn't have any [RTC](https://en.wikipedia.org/wiki/Real-time_clock), in its early stage of booting, the system time is "somewhen" on January 1, 1970 ([Unix epoch](https://en.wikipedia.org/wiki/Unix_time)). To avoid such archaic (and incorrect, of course) timestamps in the naming of log files, we'd better start our services after the system clock has been synchronized through Internet. Therefore, we are going to craft a special service named `check-sync.service`, which will complete initializing itself only when the system clock has been synchronized.

Besides, we will create a logging service named `snmpd-traffic.service` using `tcpdump`.

First, install some dependencies:

```shell
yum -y install git ntp tcpdump
```

Then, run the following commands:

```shell
cd /root/
git clone https://github.com/ieee-p21451-1-5/demo-ncap.git
cp ./demo-ncap/sync/check-sync.service /etc/systemd/system/
cp ./demo-ncap/snmpd/systemd/* /etc/systemd/system/
systemctl daemon-reload
systemctl enable check-sync.service snmp-daemon.service snmpd-traffic.service
```

Reboot the system.

> **How to uninstall a service?**
>
> - Delete `*.service` files themselves. For our example, they were put under `/etc/systemd/system/`.
> - Remove the symbolic links under certain `*.wants/` directories that point to the unwanted service. For the three services we just created, they have their symbolic links under `/etc/systemd/system/multi-user.target.wants/`.
> - Clear log files.
>
>   ```shell
>   rm /root/demo-ncap/snmpd/logs/*
>   ```

To view the log files (replace `<date_and_time>` with appropriate value):

```shell
tail -f /root/demo-ncap/snmpd/logs/snmpd-log-<date_and_time>         # snmpd's log
tail -f /root/demo-ncap/snmpd/logs/snmpd-traffic-log-<date_and_time> # tcpdump's log
```

or view the latest log files:

```shell
tail -f $(ls /root/demo-ncap/snmpd/logs/snmpd-log-* | sort | tail -n 1)         # snmpd's log
tail -f $(ls /root/demo-ncap/snmpd/logs/snmpd-traffic-log-* | sort | tail -n 1) # tcpdump's log
```

### 2.3 Custom MIBs

We assume that:

- You've installed `net-snmp` to `/usr/local/`, which will be referred to as `${NET_SNMP_PREFIX}`.
- The source is extracted and put under `/root/net-snmp-5.7.3/`, which will be referred to as `${NET_SNMP_SRC_ROOT}`.

Run the following commands:

```shell
export NET_SNMP_PREFIX="/usr/local"
export NET_SNMP_SRC_ROOT="/root/net-snmp-5.7.3"
```

It is recommended to add these two lines into the initialization scripts for your shell, e.g. `~/.bashrc`.

First, create your custom MIB file conforming to [SMI](https://en.wikipedia.org/wiki/Structure_of_Management_Information) specifications, that defines what variables (management information) you wish to read and write remotely with SNMP. Native MIB files distributed within `net-snmp` source tree are located under `${NET_SNMP_PREFIX}/share/snmp/mibs/`. They are good references and could be your starting point of writing your custom MIBs.

For convenience, we provide you with [a simple example](snmpd/mibs/IEEE-P1451-SIMPLE-DEMO-MIB.txt). This MIB defines two sensors (read-only) and two actuators (read-write), and we are going to access them using SNMP.

Put this example (or your own file) to the right place:

```shell
cp /root/demo-ncap/snmpd/mibs/IEEE-P1451-SIMPLE-DEMO-MIB.txt ${NET_SNMP_PREFIX}/share/snmp/mibs/
```

Make sure that binaries of `net-snmp` can correctly find and load the file, by looking up one of your custom management information entries. In our example:

```shell
snmptranslate -On IEEE-P1451-SIMPLE-DEMO-MIB::seTemperature.0
```

Your should get its OID in numeric form:

```
.1.3.6.1.4.1.7934.1451.1.1.0
```

Or you can print the graph (tree) of the whole file:

```shell
snmptranslate -Tp IEEE-P1451-SIMPLE-DEMO-MIB::sjtu
```

The result would look like:

```
+--sjtu(7934)
   |
   +--ieeeP1451Project(1451)
      |
      +--ieeeP1451Sensor(1)
      |  |
      |  +-- -R-- String    seTemperature(1)
      |  |        Textual Convention: DisplayString
      |  |        Size: 0..255
      |  +-- -R-- String    sePressure(2)
      |           Textual Convention: DisplayString
      |           Size: 0..255
      |
      +--ieeeP1451Actuator(2)
         |
         +-- -RW- Integer32 acRelay(1)
         +-- -RW- Integer32 acLed(2)
         +-- -RW- String    acLcd(3)
                  Textual Convention: DisplayString
                  Size: 0..255
```
