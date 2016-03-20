Raspberry PI CI {#Pici}
===============

Rapsberry PI is a great Continual Integration (CI) platform for ARM builds.

Note that this guide assumes Arch Linux.

Gmail SMTP
----------

Install `msmtp`:

    # pacman -S msmtp

Create `msmtp` config file:

    $ cat <<EOD >~/.msmtprc
    account swirly
    host smtp.gmail.com
    port 587
    tls on
    tls_certcheck off
    auth on
    from admin@swirlycloud.com
    user mark.aylett@swirlycloud.com
    password <password>
    account default : swirly
    EOD

Ensure config is readable only by user:

    $ chmod 600 ~/.msmtprc

Send test email:

    $ echo 'Subject: test' | msmtp -v  mark.aylett@swirlycloud.com

Cron
----

Install `cronie`:

    # pacman -S cronie

Enable and start cron daemon:

    # systemctl enable cronie
    # systemctl start cronie

Add CI script to crontab:

    */5 * * * * $HOME/bin/pici.sh
