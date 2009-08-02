#!/bin/sh

QTDIR=`qmake -v|grep "Using Qt version"|sed -e "s/.* \//\//"`
FWDIR=p2ptester.app/Contents/Frameworks

rm -rf p2ptester.app build
make clean
qmake -config release
make

mkdir $FWDIR
cp -R $QTDIR/QtGui.framework $FWDIR
cp -R $QTDIR/QtCore.framework $FWDIR
cp -R $QTDIR/QtNetwork.framework $FWDIR

rm -rf $FWDIR/*/Versions/4/*_debug
rm -rf $FWDIR/*/*_debug*


install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $FWDIR/QtCore.framework/Versions/4/QtCore 
install_name_tool -id @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork $FWDIR/QtNetwork.framework/Versions/4/QtNetwork 
install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $FWDIR/QtGui.framework/Versions/4/QtGui 

install_name_tool -change $QTDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore p2ptester.app/Contents/MacOS/p2ptester 
install_name_tool -change $QTDIR/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui p2ptester.app/Contents/MacOS/p2ptester 
install_name_tool -change $QTDIR/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork p2ptester.app/Contents/MacOS/p2ptester 

install_name_tool -change $QTDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $FWDIR/QtGui.framework/Versions/4/QtGui 
install_name_tool -change $QTDIR/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $FWDIR/QtNetwork.framework/Versions/4/QtNetwork

