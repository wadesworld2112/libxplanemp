all: XPWidgetsEx_lib

clean: clean_XPWidgetsEx

##############################
#######  TARGET: XPWidgetsEx
##############################

XPWidgetsEx_lib:
	g++ -c -m32 -DLIN=1 -I../../SDK/CHeaders/XPLM/ -I../../SDK/CHeaders/Widgets/ ../../SampleCode/XPListBox.cpp -o ../../SampleCode/XPListBox.o

	g++ -c -m32 -DLIN=1 -I../../SDK/CHeaders/XPLM/ -I../../SDK/CHeaders/Widgets/ ../../SampleCode/XPPopups.cpp -o ../../SampleCode/XPPopups.o

	ar -r ../../Binaries/LinuxGCC/XPWidgetsEx.lib ../../SampleCode/XPListBox.o ../../SampleCode/XPPopups.o

clean_XPWidgetsEx:
	rm -f ${OBJS_XPWidgetsEx}

##############################

# end Makefile
