/******************************************************************************
 *                                                                            *
 * lptaddr - module for reading port base addresses in Linux/Windows systems. *
 * Currently only LPT is fully supported.                                     *
 *                                                                            *
 * Author: Tomasz Wasilczyk (http://www.wasilczyk.pl)                         *
 * License: GNU LGPL v3 (http://www.gnu.org/licenses/lgpl.html)               *
 *                                                                            *
 ******************************************************************************/

/**
 * Function reads LPT port base address from Windows registry or parport
 * (depending on system type).
 *
 * @param portNo port number (1-based)
 * @return port base address or zero, if not found
 */
unsigned short getLPTPortBaseAddress(unsigned char portNo);
