/******************************************************************************
 *                                                                            *
 * lptaddr - module for reading port base addresses in Linux/Windows systems. *
 * Currently only LPT is fully supported.                                     *
 *                                                                            *
 * Author: Tomasz Wasilczyk (http://www.wasilczyk.pl)                         *
 * License: GNU LGPL v3 (http://www.gnu.org/licenses/lgpl.html)               *
 *                                                                            *
 ******************************************************************************/

#include "lptaddr.h"

#include <stdio.h>

#ifdef _WIN32

#include <windows.h>


#pragma pack(push, 4)
typedef struct _regResourceParameter
{
	unsigned char type; /* unknown types: 0, 129; other: described below */
	unsigned char sharing;
	unsigned short flags;

	union
	{
		struct
		{
			LARGE_INTEGER base;
			unsigned long length;
		} port; /* type: 1 */
		struct
		{
			unsigned long level;
			unsigned long number;
			unsigned long similarity;
		} IRQ; /* type: 2 */
		struct
		{
			LARGE_INTEGER base;
			unsigned long length;
		} memory; /* type: 3 */
		struct
		{
			unsigned long channel;
			unsigned long port;
			unsigned long reserved;
		} DMA; /* type: 4 */
		struct
		{
			unsigned long start;
			unsigned long length;
			unsigned long reserved;
		} busNumber; /* type: 6 (not 5!) */
	} data;
} regResourceParameter;
#pragma pack(pop)

typedef struct _regResource
{
	DWORD interfaceType; /* 1: ISA; 5: PCI; 15: unknown */
	unsigned long busNumber;
	unsigned short version; /* always 1? */
	unsigned short revision; /* always 1? */
	unsigned long parametersCount;
	regResourceParameter parameters[16];
} regResource;

typedef struct _regResourceList
{
	unsigned long count; /* always 1? */
	regResource elements[2];
} regResourceList;

#define PORT_TYPE_COM 0
#define PORT_TYPE_LPT 1

/**
 * Function reads port base address from Windows registry.
 * Unfortunately, linux version is not yet available.
 *
 * @param portType type of port (PORT_TYPE_COM or PORT_TYPE_LPT)
 * @param portNo port number (1-based)
 * @return port base address or zero, if not found
 */
unsigned short getPortBaseAddress(unsigned char portType, unsigned char portNo)
{
	HKEY baseKey;
	int baseKeyEnum = 0;
	DWORD dummyBufferLength; /* RegEnumKeyEx and RegQueryValueEx requires that */
	long tmpResult;
	char portFindName[32];
	unsigned short foundPortAddress = 0;
	
	if (portType == PORT_TYPE_COM)
		sprintf(portFindName, "COM%d", portNo);
	else if (portType == PORT_TYPE_LPT)
		sprintf(portFindName, "LPT%d", portNo);
	else
		return 0;
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Enum", 0,
		KEY_ENUMERATE_SUB_KEYS, &baseKey) != ERROR_SUCCESS)
		return 0;
	
	/* Find all device groups */
	while (1)
	{
		char groupKeyName[256];
		HKEY groupKey;
		int groupKeyEnum = 0;
		
		groupKeyName[255] = '\0';
		
		dummyBufferLength = 255;
		tmpResult = RegEnumKeyEx(baseKey, baseKeyEnum++, groupKeyName,
			&dummyBufferLength, NULL, NULL, NULL, NULL);
		if (tmpResult == ERROR_MORE_DATA)
			continue;
		if (tmpResult != ERROR_SUCCESS)
			break;
		
		if (RegOpenKeyEx(baseKey, groupKeyName, 0,
			KEY_ENUMERATE_SUB_KEYS, &groupKey) != ERROR_SUCCESS)
			continue;
		
		/* Find all devices in group */
		while (1)
		{
			char deviceKeyName[256];
			HKEY deviceKey;
			int deviceKeyEnum = 0;
			
			deviceKeyName[255] = '\0';
			
			dummyBufferLength = 255;
			tmpResult = RegEnumKeyEx(groupKey, groupKeyEnum++, deviceKeyName,
				&dummyBufferLength, NULL, NULL, NULL, NULL);
			if (tmpResult == ERROR_MORE_DATA)
				continue;
			if (tmpResult != ERROR_SUCCESS)
				break;
			
			if (RegOpenKeyEx(groupKey, deviceKeyName, 0,
				KEY_ENUMERATE_SUB_KEYS, &deviceKey) != ERROR_SUCCESS)
				continue;
			
			/* Find all device children in device. In example, one port adapter
			 * can provide two or three ports
			 */
			while (1)
			{
				char childKeyName[256];
				HKEY childKey, parametersKey, controlKey;
				char portName[256];
				regResourceList res;
				unsigned int i, j;
				
				childKeyName[255] = portName[0] = portName[255] = '\0';
				
				dummyBufferLength = 255;
				tmpResult = RegEnumKeyEx(deviceKey, deviceKeyEnum++,
					childKeyName, &dummyBufferLength, NULL, NULL, NULL, NULL);
				if (tmpResult == ERROR_MORE_DATA)
					continue;
				if (tmpResult != ERROR_SUCCESS)
					break;
				
				/* WARNING: remember about closing childKey before loop ends
				 * (breaks or continues).
				 */
				if (RegOpenKeyEx(deviceKey, childKeyName, 0,
					STANDARD_RIGHTS_READ, &childKey) != ERROR_SUCCESS)
					continue;
				
				/* read port name */
				if (RegOpenKeyEx(childKey, "Device Parameters", 0,
					KEY_QUERY_VALUE, &parametersKey) == ERROR_SUCCESS)
				{
					dummyBufferLength = 255;
					if (RegQueryValueEx(parametersKey, "PortName", NULL, NULL,
						(LPBYTE)portName,
						&dummyBufferLength) != ERROR_SUCCESS)
						portName[0] = '\0';
					RegCloseKey(parametersKey);
				}
				
				/* read port parameters (in example, base address) */
				if (RegOpenKeyEx(childKey, "Control", 0, KEY_QUERY_VALUE,
					&controlKey) == ERROR_SUCCESS)
				{
					dummyBufferLength = sizeof(regResourceList);
					memset(&res, 0, sizeof(regResourceList));
					if (RegQueryValueEx(controlKey, "AllocConfig", NULL, NULL,
						(LPBYTE)&res,
						&dummyBufferLength) != ERROR_SUCCESS)
						res.count = 0;
					RegCloseKey(controlKey);
				}
				
				/* NOTE: see warning about closing that key */
				RegCloseKey(childKey);

				/* we need only certain port */
				if (strcmp(portName, portFindName) != 0)
					continue;
				
				for (i = 0; i < res.count; i++)
				{
					for (j = 0; j < res.elements[i].parametersCount; j++)
					{
						LARGE_INTEGER portBaseAddr;
						
						/* 1 stands for port base address
						 * (see regResourceParameter.data)
						 */
						if (res.elements[i].parameters[j].type != 1)
							continue;

						portBaseAddr =
							res.elements[i].parameters[j].data.port.base;
						
						/* we need only valid port addresses */
						if (portBaseAddr.HighPart > 0)
							continue;
						if (portBaseAddr.LowPart <= 0)
							continue;
						
						foundPortAddress = portBaseAddr.LowPart;
						break;
					}
					
					if (foundPortAddress)
						break;
				}
				
				if (foundPortAddress)
					break;
			}
			
			RegCloseKey(deviceKey);
			if (foundPortAddress)
				break;
		}
		
		RegCloseKey(groupKey);
		if (foundPortAddress)
			break;
	}
	
	RegCloseKey(baseKey);
	return foundPortAddress;
}

#endif /* _WIN32 */

/**
 * Function reads LPT port base address from Windows registry or parport
 * (depending on system type).
 *
 * @param portNo port number (1-based)
 * @return port base address or zero, if not found
 */
unsigned short getLPTPortBaseAddress(unsigned char portNo)
{
	unsigned short addr = 0;
#ifdef _WIN32
	addr = getPortBaseAddress(PORT_TYPE_LPT, portNo);
#else
	FILE *parportBAF;
	char parportPath[64];

	if (portNo == 0 || portNo > 99)
		return 0;

	snprintf(parportPath, 64, "/proc/sys/dev/parport/parport%u/base-addr", portNo - 1);
	parportBAF = fopen(parportPath, "r");
	if (parportBAF != NULL)
	{
		if (!fscanf(parportBAF, "%hu", &addr))
			addr = 0;
		
		fclose(parportBAF);
	}
#endif /* _WIN32 */

	if (addr == 0)
		switch (portNo)
		{
			case 1:
				return 0x0378;
			case 2:
				return 0x0278;
			case 3:
				return 0x03BC;
		}

	return addr;
}
