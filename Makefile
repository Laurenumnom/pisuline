CC ?= gcc
pisuline: pisuline.c
	$(CC) $(CFLAGS) pisuline.c -o pisuline
clean:
	rm pisuline
install: pisuline
	mkdir -p /usr/share/pisuline
	cp pisuline /usr/bin/
	cp pisuline.sh /usr/share/pisuline/
	chmod 755 /usr/bin/pisuline
	chmod 755 /usr/share/pisuline/
	chmod 644 /usr/share/pisuline/pisuline.sh

	@echo ---
	@echo "don't forget to enable pisuline in your .bashrc by sourcing in end:"
	@echo ". /usr/share/pisuline/"
uninstall:
	rm /usr/bin/pisuline
	rm -rf /usr/share/pisuline

	@echo ---
	@echo "don't forget to remove pisuline from your .bashrc
