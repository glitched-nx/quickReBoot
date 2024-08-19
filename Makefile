export GITHASH 		:= $(shell git rev-parse --short HEAD)
export VERSION 		:= 1.5.0

all: applet overlay dist

applet:
    $(make) -f Makefile.applet

overlay:
    $(make) -f Makefile.overlay

dist: applet overlay
    mkdir -p dist/switch/.overlays
    mkdir -p dist/switch/quickReLoader
    cp overlay/quickReLoader.ovl dist/switch/.overlays/
    cp applet/quickReLoader.nro dist/switch/quickReLoader/
    cd dist; zip -r quickReLoader-$(VERSION)-$(GITHASH).zip ./**/; cd ../;

clean:
    $(MAKE) -f Makefile.applet clean
    $(MAKE) -f Makefile.overlay clean

.PHONY: all applet overlay dist clean
