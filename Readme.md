CakeHax
=======

Are you tired of seeing that semi-reversed LauncherTemplate.dat in every single project that launches a payload in ARM9 via spider (Nintendo 3DS Web Browser)?  
Well, I sure am.  
This was originally made for [CakesFW](https://github.com/mid-kid/CakesForeveryWan), but I've separated it for the ease of use for other projects.  
In most cases it can be a drop-in replacement for the hideous LauncherTemplate.dat.  

Entry points
------------

* MSET (System Settings DS Profile exploit) 4.x (only this version) on firmwares 4.x and 9.0-9.2.
* Spider (Web Browser exploit) on firmwares 4.0-9.2.

How to use this in your project
-------------------------------

You copy this into your project directory, and add a few lines into your Makefile:  
```
make dir_out=out name=Launcher.dat -C CakeHax launcher
dd if=mypayload.bin of=out/Launcher.dat bs=512 seek=256

clean:
    make dir_out=out name=Launcher.dat -C CakeHax clean
```

Pretty simple, right?  
Well, you can always configure it to fit exactly your needs.  

### Bigger payloads

CakeHax by default has a size limit for payloads. This is because entry points have size restrictions.  
To get around this problem, CakeHax comes with a small-ish (a bit more than 7KB) payload, that loads a bigger payload from the SD card.  
To use this feature, replace `launcher` with `bigpayload` when calling the makefile, and use dd with a seek of 272.  
Please note that some changes will be made to your environment (see [this](https://github.com/mid-kid/CakeHax/blob/master/source/payload/start.s#L8) and [this](https://github.com/mid-kid/CakeHax/blob/master/source/payload/main.c#L35)).  
This environment is mostly identical to the LauncherTemplate.dat's environment.

### Makefile options

The makefile of CakeHax has some options for your convenience:  

Name    |Default  |Description
--------|---------|-----------
dir\_out|.        |Where the to-be-injected .dat file will be placed (and removed when doing clean!)
name    |Cakes.dat|The name of the .dat file. (NOTE: This can't be renamed after it is built, or it won't work.)
path    |/        |Path in the SD card where the .dat file will be located and loaded from the ROP

Thanks
------

* Normmatt for the sdmmc code
* ChaN for FatFs
* Roxas75 for firmlaunchax on MSET
* YifanLu and dukesrg for Spider3DSTools
* naehrwert for p3ds
* The KARL and OSKA guys for providing example code and firmware offsets (bootstrap)
* 3dbrew for info
* Yellows8 for firmlaunchax and memchunkhax
* smea and Yellows8 for gspwn
* Apache Thunder for being a fearless test-kanninchen.
* Gateway for some offsets

License
-------
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 2.0

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.