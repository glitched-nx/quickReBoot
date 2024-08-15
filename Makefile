export GITHASH 		:= $(shell git rev-parse --short HEAD)
export VERSION 		:= 1.0.0

all: applet overlay

applet:
	$(MAKE) -f Makefile.applet

overlay:
	$(MAKE) -f Makefile.overlay

clean:
	$(MAKE) -f Makefile.applet clean
	$(MAKE) -f Makefile.overlay clean

dist: all
	mkdir -p dist/switch/.overlays
	cp overlay/quickReLoader.ovl dist/switch/.overlays/
	mkdir -p dist/switch/quickReLoader
	cp applet/quickReLoader.nro dist/switch/quickReLoader/
	cd dist; zip -r quickReLoader-$(VERSION)-$(GITHASH).zip ./**/; cd ../;

.PHONY: all applet overlay
