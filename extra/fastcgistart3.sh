#!/bin/bash
# Starting script for FastCgi Daemon

ETCDIR='/etc'
THIS=`basename $0 | sed 's/^[SK][0123456789]*b/b/'`
MAILHOST="yandex-team.ru"

SUBJECT="$THIS on `hostname`"
FASTCGI_CONFIG=/etc/fastcgi3/available/$1.conf
FASTCGI_NICE=/etc/fastcgi3/available/$1.nice
FASTCGI_ENV=/etc/fastcgi3/available/$1.env
FASTCGI_START=/etc/fastcgi3/available/$1.start
LOCKER=/var/run/fastcgi3/fastcgistart2.$1.pid
RDELAY=1

if [ -e $FASTCGI_CONFIG  ]; then
	if [ ! -r $FASTCGI_START ]; then
	    COREDIR=/var/tmp/core-files
            ulimit -c unlimited
            ulimit -s 1024

            if [ -d "$COREDIR" ]; then
                mkdir -p $COREDIR 2>/dev/null
            fi
	fi

	if [ -e $FASTCGI_NICE ]; then
		FASTCGI_NICE=`cat $FASTCGI_NICE`
	else
		FASTCGI_NICE=0
	fi

	if [ -e $FASTCGI_ENV ]; then
		export `cat $FASTCGI_ENV`
	fi

        if [ -z $LOGDIR ]; then
		LOGDIR=/var/log/fastcgi3
        fi

    [ -d "$LOGDIR" ] || mkdir -p $LOGDIR 2>/dev/null

    touch $LOCKER
    trap "rm $LOCKER 2>/dev/null" 0
    echo $$ > $LOCKER

    while [ -f $LOCKER ]; do
	    echo " /usr/sbin/fastcgi-container3 --config=$FASTCGI_CONFIG"
        nice -n $FASTCGI_NICE /usr/sbin/fastcgi-container3 --config=$FASTCGI_CONFIG >> $LOGDIR/err.$1.log 2>&1
        echo "on `date` ::$THIS:: $THIS on `hostname` restarted"  >>$LOGDIR/err.$1.log
        sleep $RDELAY
    done

else
    echo "$FASTCGI_CONFIG don't found"
    exit 1
fi
