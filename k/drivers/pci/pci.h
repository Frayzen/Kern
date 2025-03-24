#ifndef PCI_H
#define PCI_H


/*
Offset |Bits 31-24 | Bits 23-16  | Bits 15-8    | Bits 7-0 
=============================================================
0x0    |      Device ID          |         Vendor ID
0x4 	 |      Status             |          Command
0x8 	 | Class code | Subclass 	 | Prog IF 	    | Revision ID
0xC 	 |    BIST 	  | Header Type| Latency Timer| Cache Line Size

/!\ The lower addresses contain the least significant portions of the field
*/
#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

void checkAllBuses(void);


#endif /* !PCI_H */
