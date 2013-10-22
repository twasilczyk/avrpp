CFLAGS=-Wall -Wextra -pedantic

avrpp:
	gcc $(CFLAGS) avrpp.c hwctrl.c lptaddr.c -o $@

install: avrpp avrpp.ini fuse.txt
	cp avrpp /bin/avrpp
	chmod +x /bin/avrpp
	mkdir -p /etc/avrxtool32
	cp avrpp.ini /etc/avrxtool32
	cp fuse.txt /etc/avrxtool32
	@echo
	@echo "*** avrpp has been installed, you can now add that line to /etc/sudoers:"
	@echo "***  ALL ALL= NOPASSWD: /bin/avrpp"
	@echo

uninstall:
	rm -f /bin/avrpp
	rm -f /etc/avrxtool32/avrpp.ini
	rm -f /etc/avrxtool32/fuse.txt
	rmdir --ignore-fail-on-non-empty /etc/avrxtool32

clean:
	rm -f avrpp
