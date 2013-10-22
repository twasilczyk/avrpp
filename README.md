avrpp
=====

**avrpp** is a small application, part of **avrxtool32**, used with AVR parallel programmer, designed by ChaN. Unfortunately, original version doesn't support my PCI LPT expansion card (with non-standard port base addresses, other than 0x378) neither my operating system (openSuSE linux). So I made some changes and (as you can see) released my own branch.

Original avrxtool32 homepage
http://elm-chan.org/works/avrx/report_e.html

This branch author's homepage
https://www.wasilczyk.pl

Differences from original version:
 *  linux port:
     *  configuration files can be hold in current dir, ~/.avrxtool32/, /usr/share/avrxtool32/, /etc/avrxtool32/;
     *  fuse descriptions (lines/frames) are provided in UTF8;
     *  install and uninstall sections in Makefile;
 *  windows version: direct port access through inpout32, instead of giveio (easier to use, no need to install);
 *  LPT port base addresses (now supported up to 99) are read from system (parport in Linux and registry in Windows);
 *  custom port address by -p0x<hex address> switch (ie. -p0x0378);
 *  improved code quality (no warnings with -Wall -Wextra -pedantic), added Makefile;
 *  double verification of read data.
