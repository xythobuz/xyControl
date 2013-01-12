INJAR = *.class
#CFLAGS = -Xlint:unchecked
# Path to jni.h
ifdef SystemRoot
RM = del
INJAR += Serial.dll
PLATFORM = Win
else
ifdef SYSTEMROOT
# Looks like cygwin or mingw shell
RM = rm -rf
INJAR += Serial.dll
PLATFORM = Win
else
RM = rm -rf
UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
INJAR += libSerial.jnilib
PLATFORM = Mac
else
INJAR += libSerial.so
PLATFORM = Linux
endif
endif
endif

# All java files to be compiled
# List so it works as target
JAVAFILES = Remote.java HelperUtility.java SerialCommunicator.java

# --------------------------------------

all: FlightRemote.jar

run: FlightRemote.jar
	java -jar FlightRemote$(PLATFORM).jar

doc: doc/index.html

FlightRemote.jar: HelperUtility.class manifest.txt libSerial
	jar -cmf manifest.txt "FlightRemote$(PLATFORM).jar" $(INJAR)

HelperUtility.class: $(JAVAFILES)
	javac $(CFLAGS) -g $(JAVAFILES)

doc/index.html: $(JAVAFILES)
	javadoc -d doc $(JAVAFILES)

ifdef SystemRoot
libSerial: Serial.dll
else
ifdef SYSTEMROOT
libSerial: Serial.dll
else
ifeq ($(UNAME),Darwin)
libSerial: libSerial.jnilib
else
libSerial: libSerial.so
endif
endif
endif

Serial.dll: libSerial/serialHelper.c libSerial/winSerial.c
	make -C libSerial
	mv libSerial/Serial.dll Serial.dll

libSerial.jnilib: libSerial/serialHelper.c libSerial/unixSerial.c
	make -C libSerial
	mv libSerial/libSerial.jnilib libSerial.jnilib

libSerial.so: libSerial/serialHelper.c libSerial/unixSerial.c
	make -C libSerial
	mv libSerial/libSerial.so libSerial.so

# Delete intermediate files
clean:
	make -C libSerial clean
	$(RM) *.class
	$(RM) *.o
ifdef SystemRoot
	$(RM) *.dll
	$(RM) FlightRemoteWin.jar
else
ifdef SYSTEMROOT
	$(RM) *.dll
	$(RM) FlightRemoteWin.jar
else
ifeq ($(UNAME),Darwin)
	$(RM) *.jnilib
	$(RM) FlightRemoteMac.jar
else
	$(RM) *.so
	$(RM) FlightRemoteLinux.jar
endif
endif
endif
