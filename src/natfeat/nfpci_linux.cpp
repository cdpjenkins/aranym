/*
	NatFeat host PCI access, Linux PCI driver

	ARAnyM (C) 2004 Patrice Mandin

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <SDL_endian.h>

#include "sysdeps.h"
#include "cpu_emulation.h"
#include "parameters.h"
#include "nfpci.h"
#include "nfpci_linux.h"

#define DEBUG 0
#include "debug.h"

/*--- Defines ---*/

#define NFPCI_NAME	"nf:pci:linux: "

#define PCI_FILE_DEVICES	"/proc/bus/pci/devices"

/*--- Types ---*/

typedef struct {
	uint32 info[17];
	char filename[256];
} pci_device_t;

/*--- Variables ---*/

static pci_device_t *pci_devices=NULL;
static uint32 num_pci_devices=0;

/*--- Public functions ---*/

PciDriverLinux::PciDriverLinux()
{
	uint32 device[17], i;
	char buffer[32];
	FILE *f;

	D(bug(NFPCI_NAME "PciDriverLinux()"));

	f=fopen(PCI_FILE_DEVICES,"r");
	if (f==NULL) {
		return;
	}

	num_pci_devices=0;
	while (!feof(f)) {
		pci_device_t *new_device;

		for (i=0; i<17; i++) {
			fscanf(f, "%x", &device[i]);
		}

		/* Add device to list */
		++num_pci_devices;
		pci_devices=(pci_device_t *)realloc(pci_devices, num_pci_devices * sizeof(pci_device_t));
		new_device = &pci_devices[num_pci_devices-1];
		for (i=0; i<17; i++) {
			new_device->info[i]=device[i];
		}
		snprintf(new_device->filename, sizeof(new_device->filename),
			"/proc/bus/pci/%02x/%02x.%x",
			(new_device->info[0] >> 8) & 0xff,
			(new_device->info[0] >> 3) & 0x1f,
			new_device->info[0] & 0x07
		);

		/* Go to end of line */
		if (feof(f)) {
			break;
		}

		fread(buffer, sizeof(char), 1, f);
		while (buffer[0]!='\n') {
			fread(buffer, sizeof(char), 1, f);
		}
	}

	fclose(f);

	/* List devices */
#if DEBUG
	D(bug(NFPCI_NAME " %d PCI devices found:", num_pci_devices));
	for (i=0; i<num_pci_devices; i++) {
		D(bug(NFPCI_NAME "  Bus %d, Vendor 0x%04x, Device 0x%04x, %s",
			pci_devices[i].info[0], (pci_devices[i].info[1]>>16) & 0xffff,
			pci_devices[i].info[1] & 0xffff,
			pci_devices[i].filename
		));
	}
#endif	
}

PciDriverLinux::~PciDriverLinux()
{
	D(bug(NFPCI_NAME "~PciDriverLinux()"));

	if (pci_devices!=NULL) {
		free(pci_devices);
		pci_devices=NULL;
	}
}

/*--- Private functions ---*/

int32 PciDriverLinux::find_device(uint32 device_vendor_id, uint32 index)
{
	uint32 i, j;
	
	D(bug(NFPCI_NAME "find_device(0x%08x,%d)", device_vendor_id, index));

	j=0;
	for (i=0; i<num_pci_devices; i++) {
		if (device_vendor_id!=0xffffUL) {
			if (pci_devices[i].info[1]!=device_vendor_id) {
				continue;
			}
		}

		if (j==index) {
			return i;
		}
		j++;
	}

	return ATARI_PCI_DEVICE_NOT_FOUND;
}

int32 PciDriverLinux::find_classcode(uint32 class_code, uint32 index)
{
	D(bug(NFPCI_NAME "find_classcode(0x%08x,%d)", class_code, index));
	return ATARI_PCI_DEVICE_NOT_FOUND;
}

int32 PciDriverLinux::read_config_byte(uint32 device_handle, memptr data, uint32 num_register)
{
	FILE *f;

	D(bug(NFPCI_NAME "read_config_byte(0x%08x,0x%08x,%d)", device_handle, data, num_register));

	if ((device_handle>num_pci_devices) || (num_register>63)) {
		return ATARI_PCI_GENERAL_ERROR;
	}
	
	f=fopen(pci_devices[device_handle].filename, "r");
	if (f==NULL) {
		return ATARI_PCI_GENERAL_ERROR;
	}

	fseek(f, num_register, SEEK_SET);
	fread(Atari2HostAddr(data), 1, 1, f);

	fclose(f);
	return ATARI_PCI_SUCCESSFUL;
}

int32 PciDriverLinux::read_config_word(uint32 device_handle, memptr data, uint32 num_register)
{
	FILE *f;

	D(bug(NFPCI_NAME "read_config_word(0x%08x,0x%08x,%d)", device_handle, data, num_register));

	if ((device_handle>num_pci_devices) || (num_register>62)) {
		return ATARI_PCI_GENERAL_ERROR;
	}
	
	f=fopen(pci_devices[device_handle].filename, "r");
	if (f==NULL) {
		return ATARI_PCI_GENERAL_ERROR;
	}

	fseek(f, num_register, SEEK_SET);
	fread(Atari2HostAddr(data), 2, 1, f);

	fclose(f);
	return ATARI_PCI_SUCCESSFUL;
}

int32 PciDriverLinux::read_config_long(uint32 device_handle, memptr data, uint32 num_register)
{
	FILE *f;

	D(bug(NFPCI_NAME "read_config_long(0x%08x,0x%08x,%d)", device_handle, data, num_register));

	if ((device_handle>num_pci_devices) || (num_register>60)) {
		return ATARI_PCI_GENERAL_ERROR;
	}
	
	f=fopen(pci_devices[device_handle].filename, "r");
	if (f==NULL) {
		return ATARI_PCI_GENERAL_ERROR;
	}

	fseek(f, num_register, SEEK_SET);
	fread(Atari2HostAddr(data), 4, 1, f);

	fclose(f);
	return ATARI_PCI_SUCCESSFUL;
}

int32 PciDriverLinux::read_config_byte_fast(uint32 device_handle, uint32 num_register)
{
	FILE *f;
	unsigned char reg_value;

	D(bug(NFPCI_NAME "read_config_byte_fast(0x%08x,%d)", device_handle, num_register));

	if ((device_handle>num_pci_devices) || (num_register>63)) {
		return 0;
	}
	
	f=fopen(pci_devices[device_handle].filename, "r");
	if (f==NULL) {
		return 0;
	}

	fseek(f, num_register, SEEK_SET);
	fread(&reg_value, 1, 1, f);

	fclose(f);
	return (int32) reg_value;
}

int32 PciDriverLinux::read_config_word_fast(uint32 device_handle, uint32 num_register)
{
	FILE *f;
	unsigned short reg_value;

	D(bug(NFPCI_NAME "read_config_word_fast(0x%08x,%d)", device_handle, num_register));

	if ((device_handle>num_pci_devices) || (num_register>62)) {
		return 0;
	}
	
	f=fopen(pci_devices[device_handle].filename, "r");
	if (f==NULL) {
		return 0;
	}

	fseek(f, num_register, SEEK_SET);
	fread(&reg_value, 2, 1, f);

	fclose(f);
	return (int32) reg_value;
}

int32 PciDriverLinux::read_config_long_fast(uint32 device_handle, uint32 num_register)
{
	FILE *f;
	unsigned long reg_value;

	D(bug(NFPCI_NAME "read_config_long_fast(0x%08x,%d)", device_handle, num_register));

	if ((device_handle>num_pci_devices) || (num_register>60)) {
		return 0;
	}
	
	f=fopen(pci_devices[device_handle].filename, "r");
	if (f==NULL) {
		return 0;
	}

	fseek(f, num_register, SEEK_SET);
	fread(&reg_value, 4, 1, f);

	fclose(f);
	return (int32) reg_value;
}

int32 PciDriverLinux::write_config_byte(uint32 device_handle, uint32 num_register, uint32 value)
{
	D(bug(NFPCI_NAME "write_config_byte(0x%08x,%d,0x%08x)", device_handle, num_register, value));
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::write_config_word(uint32 device_handle, uint32 num_register, uint32 value)
{
	D(bug(NFPCI_NAME "write_config_word(0x%08x,%d,0x%08x)", device_handle, num_register, value));
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::write_config_long(uint32 device_handle, uint32 num_register, uint32 value)
{
	D(bug(NFPCI_NAME "write_config_long(0x%08x,%d,0x%08x)", device_handle, num_register, value));
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::hook_interrupt(uint32 device_handle, memptr data, uint32 parameter)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::unhook_interrupt(uint32 device_handle)
{
	return ATARI_PCI_GENERAL_ERROR;
} 

int32 PciDriverLinux::special_cycle(uint32 num_bus, uint32 data)
{
	return ATARI_PCI_GENERAL_ERROR;
} 

/* get_routing */

/* set_interrupt */	

int32 PciDriverLinux::get_resource(uint32 device_handle)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::get_card_used(uint32 device_handle, memptr callback)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::set_card_used(uint32 device_handle, memptr callback)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_mem_byte(uint32 device_handle, uint32 pci_address, uint32 num_register)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_mem_word(uint32 device_handle, uint32 pci_address, uint32 num_register)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_mem_long(uint32 device_handle, uint32 pci_address, uint32 num_register)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_mem_byte_fast(uint32 device_handle, uint32 pci_address)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_mem_word_fast(uint32 device_handle, uint32 pci_address)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_mem_long_fast(uint32 device_handle, uint32 pci_address)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::write_mem_byte(uint32 device_handle, uint32 pci_address, uint32 value)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::write_mem_word(uint32 device_handle, uint32 pci_address, uint32 value)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::write_mem_long(uint32 device_handle, uint32 pci_address, uint32 value)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_io_byte(uint32 device_handle, uint32 pci_address, memptr data)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_io_word(uint32 device_handle, uint32 pci_address, memptr data)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_io_long(uint32 device_handle, uint32 pci_address, memptr data)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_io_byte_fast(uint32 device_handle, uint32 pci_address)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_io_word_fast(uint32 device_handle, uint32 pci_address)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::read_io_long_fast(uint32 device_handle, uint32 pci_address)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::write_io_byte(uint32 device_handle, uint32 pci_address, uint32 value)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::write_io_word(uint32 device_handle, uint32 pci_address, uint32 value)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::write_io_long(uint32 device_handle, uint32 pci_address, uint32 value)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::get_machine_id(void)
{
	return ARANYM_PCI_ID;
}

int32 PciDriverLinux::get_pagesize(void)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::virt_to_bus(uint32 device_handle, memptr virt_cpu_address, memptr data)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::bus_to_virt(uint32 device_handle, uint32 pci_address, memptr data)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::virt_to_phys(memptr virt_cpu_address, memptr data)
{
	return ATARI_PCI_GENERAL_ERROR;
}

int32 PciDriverLinux::phys_to_virt(memptr phys_cpu_address, memptr data)
{
	return ATARI_PCI_GENERAL_ERROR;
}