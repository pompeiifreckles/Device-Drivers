# Char Drivers

## scull (Simple Character Utility for Loading Localities)
  The first step of driver writing is defining the capabilities (the mechanism) the driver will offer to user programs. Scull is a char driver that acts on memory area as though it was a device.

  scull is useful as a template for writing real drivers for real devices, we will show how to implement several device abstractions on top of the computer memory, each with deifferent personality.

  - scull0 to scull3: global and persistent devices. Global - accessible to all file descriptors. Persistent - data isn't lost while opening and closing\
  - scullpipe0 to scullpipe3: Four FIFO devices - one process reads while other write\
  - scullsingle: only one process can use the driver at a time\
  - scullpriv: private memory spaces for every process accessing it\
  - sculluid: can be accessed by multiple processes but by only one user. "Device Busy" for other users\
  - scullwuid: can be accessed by multiple processes but by only one user. Blocking open for other users
