Arch Linux ARM {#Alarm}
==============

Thanks
------

Thank you to [Rodrigo Fernandes] for contributions to these [Arch Linux ARM] tips.

Hostname
--------

    # hostnamectl set-hostname <hostname>

Packages
--------

Full system update:

    # pacman -Syu

Perf Tools
----------

Install performance analysis tools for Linux:

    # pacman -S perf

Example:

    # perf stat -e cpu-clock,faults ls

Power Saving
------------

Disable USB bus:

    # echo 0x0 >/sys/devices/platform/bcm2708_usb/buspower

Turn off HDMI system:

    # /opt/vc/bin/tvservice -o

Users and Groups
----------------

Add group:

    # groupadd users

Add user:

    # useradd -m -g users -G wheel <username>

Delete user:

    # userdel -r alarm

[Arch Linux ARM]: http://archlinuxarm.org/
[Rodrigo Fernandes]: https://github.com/rfernandes
