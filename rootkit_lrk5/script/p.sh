#!/bin/sh

find / -name "*.php" 2>>/dev/null|xargs grep -i passw 2>>/dev/null |awk -F\: '{print $1}' |sort -u >> /tmp/..../phpfile
find / -name "*.ini" 2>>/dev/null|xargs grep -i passw  2>>/dev/null|awk -F\: '{print $1}' |sort -u >> /tmp/..../inifile
find / -name "*.sh" 2>>/dev/null|xargs grep -i passw 2>>/dev/null |awk -F\: '{print $1}' |sort -u >> /tmp/..../shfile
find / -name "*.pl" 2>>/dev/null|xargs grep -i passw  2>>/dev/null|awk -F\: '{print $1}' |sort -u >> /tmp/..../plfile
find / -name "*.py" 2>>/dev/null|xargs grep -i passw  2>>/dev/null|awk -F\: '{print $1}' |sort -u >> /tmp/..../pyfile
find / -name "*.cfg" 2>>/dev/null|xargs grep -i passw  2>>/dev/null|awk -F\: '{print $1}' |sort -u >> /tmp/..../cfgfile
