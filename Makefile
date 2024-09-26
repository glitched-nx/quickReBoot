export GITHASH 		:= $(shell git rev-parse --short HEAD)
export VERSION 		:= 1.5.0

all: applet overlay dist

applet:
	$(MAKE) -f Makefile.applet

overlay:
	$(MAKE) -f Makefile.overlay

clean:
	$(MAKE) -f Makefile.applet clean
	$(MAKE) -f Makefile.overlay clean

dist:
	mkdir -p dist/switch/.overlays
	mkdir -p dist/switch/quickReBoot
	cp overlay/quickReBoot.ovl dist/switch/.overlays/
	cp applet/quickReBoot.nro dist/switch/quickReBoot/
	cd dist; zip -r quickReBoot-$(VERSION)-$(GITHASH).zip ./**/; cd ../;

.PHONY: all applet overlay
