CC ?= gcc
build/pisuline: pisuline.c
	$(CC) $(CFLAGS) pisuline.c -o build/pisuline -lgit2
deb-package: build/pisuline
	mkdir -p build/pisuline-deb-amd64/usr/bin
	mkdir -p build/pisuline-deb-amd64/usr/share/pisuline
	cp -f build/pisuline build/pisuline-deb-amd64/usr/bin/
	cp -f pisuline.sh build/pisuline-deb-amd64/usr/share/pisuline/
	chmod 755 build/pisuline-deb-amd64/usr/bin
	chmod 755 build/pisuline-deb-amd64/usr/share/pisuline
	chmod 644 build/pisuline-deb-amd64/usr/share/pisuline/pisuline.sh
	dpkg-deb --build --root-owner-group build/pisuline-deb-amd64 build/pisuline-amd64.deb
clean:
	rm -f pisuline
	rm -f build/pisuline
	rm -fr build/pisuline-deb-amd64/usr
	rm -f build/pisuline-amd64.deb
install: pisuline
	mkdir -p /usr/share/pisuline
	cp -f pisuline /usr/bin/
	cp -f pisuline.sh /usr/share/pisuline/
	chmod 755 /usr/bin/pisuline
	chmod 755 /usr/share/pisuline/
	chmod 644 /usr/share/pisuline/pisuline.sh

	@echo ---
	@echo "don't forget to enable pisuline in your .bashrc by sourcing in end:"
	@echo ". /usr/share/pisuline/pisuline.sh"
uninstall:
	rm -f /usr/bin/pisuline
	rm -rf /usr/share/pisuline

	@echo ---
	@echo "don't forget to remove pisuline from your .bashrc
