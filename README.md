# Index
- [FTP SClient Example for W6100-EVB](#FTP-Client-Example-for-W6100-EVB)
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
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56015759-16485600-5d35-11e9-855f-8983e91a5765.PNG" />
  </p>

  - Add Users<br>
<p align="center">
<img width="100%" src="https://user-images.githubusercontent.com/24927447/56015760-16485600-5d35-11e9-8ea2-4887f5e5d606.PNG" />
</p>

  - Set shared folders<br>
<p align="center">
<img width="100%" src="https://user-images.githubusercontent.com/24927447/56015761-16485600-5d35-11e9-8ca6-f4ed96367d0d.PNG" />
</p>

  - configure server listen port<br>
<p align="center">
<img width="100%" src="https://user-images.githubusercontent.com/24927447/56015763-16485600-5d35-11e9-9913-b38869443825.PNG" />
</p>

  - Power On and push Reset button to start Program<br>
  - Program Run Serial display<br>
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56018661-abe7e380-5d3d-11e9-88db-f0913d837aa7.PNG" />
  </p>

  - Enter User ID
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56019278-2d8c4100-5d3f-11e9-845a-251c65218c99.PNG" />
  </p>


  - Enter password
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56019279-2d8c4100-5d3f-11e9-978c-a5404eeb75b5.PNG" />
  </p>

  - Select menu
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56019280-2d8c4100-5d3f-11e9-9d5f-51b62dcd6a8b.PNG" />
  </p>

  - When 1 is pressed, you can see the list of files on the server.
  <p align="center">
    <img width="100%" src="https://user-images.githubusercontent.com/24927447/56019281-2e24d780-5d3f-11e9-8206-e027a8abf4e6.PNG" />
  </p>



   ## Test packet capture file
   <p align="center">
     <img width="100%" src="https://user-images.githubusercontent.com/24927447/56022905-a8f1f080-5d47-11e9-9f5a-2e0dbab43902.PNG" />
   </p>
   
    - [FTP_Client_Packet.zip](https://github.com/WIZnet-ioLibrary/W6100EVB-FTPC/files/3072310/FTPClient_packet.zip)
