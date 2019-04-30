# Index
- [FTP Client Example for W6100-EVB](#FTP-Client-Example-for-W6100-EVB)
- [Hardware Environment](#Hardware-Environment)
- [Software Environments](#Software-Environment)
- [Run](#Run)
- [Code review](#Code-review)
  - [Test packet capture file](#Test-packet-capture-file)



------
# FTP Client Example for W6100-EVB
Common to Any MCU, Easy to Add-on. Internet Offload co-Processor, HW TCP/IP chip,
best fits for low-end Non-OS devices connecting to Ethernet for the Internet of Things. These will be updated continuously.

## Hardware Environment
* W6100EVB
  - connecting Micro usb.
  - connecting Ethernet cable. <br>
<p align="center">
  <img width="60%" src="https://wizwiki.net/wiki/lib/exe/fetch.php?w=600&tok=eabde4&media=products:w6100:w6100_evb:w6100-evb_callout.png" />
</p>

## Software Environment
In case of used to TureSTUDIO,it it the same as HTTP Server example.
 - Link : [Software Environment of W6100EVB-HTTP_Server](https://github.com/WIZnet-ioLibrary/W6100EVB-HTTP_Server#Software-Environment)



## Run
* Demo Environment & Program <br>

  - Windows 7 <br>
  - Hercules <br>
  - [TCP Server Program - FileZila](https://filezilla-project.org/download.php?type=server) <br>


* Demo Result <br>
  - Excute FTP Server Program<br>
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56953004-839e1880-6b76-11e9-84ed-8c1e2701c95e.PNG" />
  </p>

  - Add Users(Set User ID & Password)<br>
<p align="center">
<img width="100%" src="https://user-images.githubusercontent.com/24927447/56953005-839e1880-6b76-11e9-8098-4712d0c1ff89.PNG" />
</p>

  - Set shared folders<br>
<p align="center">
<img width="100%" src="https://user-images.githubusercontent.com/24927447/56953007-839e1880-6b76-11e9-90b9-8faaf749b96f.PNG" />
</p>

  - configure server listen port<br>
<p align="center">
<img width="100%" src="https://user-images.githubusercontent.com/24927447/56953008-8436af00-6b76-11e9-881a-aae9006ae6f6.PNG" />
</p>

  - Power On and push Reset button to start Program<br>
  - Program Run Serial display<br>
  - Enter User ID
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56953009-8436af00-6b76-11e9-95bf-b1507e75000a.PNG" />
  </p>


  - Enter password
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56953010-8436af00-6b76-11e9-823e-9947ed2dd4f8.PNG" />
  </p>

  - Select menu
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56953011-84cf4580-6b76-11e9-9dfa-08b9c72ae701.PNG" />
  </p>

  - When 1 is pressed, you can see the list of files on the server.
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56953012-84cf4580-6b76-11e9-9220-f844d20ea73a.PNG" />
  </p>



   ## Test packet capture file
   <p align="center">
     <img width="100%" src="https://user-images.githubusercontent.com/24927447/56953416-84837a00-6b77-11e9-91a6-40c3d669c83c.PNG" />
   </p>
   
    - [FTP_Client_Packet.zip](https://github.com/WIZnet-ioLibrary/W6100EVB-FTPC/files/3130668/FTPClientPacket.zip)
