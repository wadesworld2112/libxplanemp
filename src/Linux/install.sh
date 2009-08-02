#!/bin/bash

os=`uname`
if [ $os == "Darwin" ] ; then
	cp -v Release/Resources/plugins/X-IvAp.xpl /Applications/X-Plane\ 8.40/Resources/plugins/
else
	cp -v Release/Resources/plugins/X-IvAp.xpl ~/XPlane/Resources/plugins
fi
