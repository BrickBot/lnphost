/* =========================================================================== 

    lnpdump.c

    Multiprotocol packet monitoring program for the Mindstorms IR tower.

    This is free software, released under the terms of the GNU General
    Public License.

    Stephan Höhrmann, November 2004

=========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "lnphost.h"


/* ---- IR callback routines -------------------------------------------- */

int lnpmask=0xf0;

void lnp_raw_hexdump(unsigned char *data, unsigned length, int isvalid)
{
  lnp_hexdump("[RAW ]",data,length);
}

void lnp_raw_hexdump_invalid(unsigned char *data, unsigned length, int isvalid)
{
  if (!isvalid) lnp_hexdump("[RAW ]",data,length);
}

void lnp_lego_hexdump(unsigned char *data, unsigned length)
{
  lnp_hexdump("[LEGO]",data,length);
}

void lnp_integrity_hexdump(unsigned char *data, unsigned char length)
{
  printf("Integrity layer message (%i bytes)\n",length);
  lnp_hexdump("[LNPI]",data,length);
}

void lnp_addressing_hexdump(unsigned char *data, unsigned char length,
			    unsigned char source, unsigned char dest)
{
  if ((lnpmask==0x00) || (lnpmask==0xff))
  printf("Addressing layer message (%i bytes from %02X to %02X)\n",
	 length,source,dest); else
  printf("Addressing layer message (%i bytes from %02X:%02X to %02X:%02X)\n",
	 length,
	 lnp_addressing_host(lnpmask,source),
	 lnp_addressing_port(lnpmask,source),
	 lnp_addressing_host(lnpmask,dest),
	 lnp_addressing_port(lnpmask,dest));
  lnp_hexdump("[LNPA]",data,length);
}


/* ---- Main program ---------------------------------------------------- */

char usage[]= {
  "Usage:\n"
  "  lnpdump [options]\n"
  "\n"
  "Dump all packets received from the Lego Mindstorms tower\n"
  "\n"
  "Options:\n"
  "  -a, --all                  show packets even if they are invalid\n"
  "  -d, --device device        set tower device name\n"
  "  -f, --fast                 use 4800 instead of 2400 baud\n"
  "  -h, --help                 show this summary\n"
  "  -m, --mask n               LNP addressing layer mask, default 0xf0\n"
  "  -p, --noparity             legOS compatibility mode\n"
  "  -r, --raw                  don't interpret packages (implies --all)\n"
  "\n"
  "If no tower device name is specified, it is taken from the environment\n"
  "variable "LNP_RCXTTY_ENV" or the default setting "LNP_DEFAULTDEVICE".\n"
  "\n"
  "Copyright 2004 Stephan Höhrmann, published under the GPL.\n"
};

static struct option long_options[] = {
  { "all",        no_argument,       NULL, 'a' },
  { "device",     required_argument, NULL, 'd' },
  { "fast",       no_argument,       NULL, 'f' },
  { "help",       no_argument,       NULL, 'h' },
  { "mask",       required_argument, NULL, 'm' },
  { "noparity",   no_argument,       NULL, 'p' },
  { "raw",        no_argument,       NULL, 'r' },
  { 0,0,0,0 }
};

int main(int argc, char *argv[]) {
  struct lnptower tower;
  char *device = NULL;
  unsigned flags=0;
  int rawmode=0;
  int allmode=0;
  int option_index;
  char c;

  /* ---- Parse command line arguments ---------------------------------- */
  while (1) {
    c=getopt_long(argc,argv,"ad:fhm:pr",long_options,&option_index);
    if (c==-1) break;
    switch (c) {
      case 'a':
	allmode=1;
        break;
      case 'd':
        device = optarg;
        break;
      case 'f':
	flags|=LNP_FAST;
        break;
      case 'h':
        printf("%s\n",usage);
        return(EXIT_SUCCESS);
      case 'm':
	if (sscanf(optarg,"%i",&lnpmask)!=1) {
	  fprintf(stderr,"Invalid mask value '%s'\n",optarg);
	  exit(EXIT_FAILURE);
	}
	if ((lnpmask<0) || (lnpmask>0xff)) {
          fprintf(stderr,"The mask value must be between 0x00 and 0xff\n");
	  exit(EXIT_FAILURE);
	}
        break;
      case 'p':
	flags|=LNP_NOPARITY;
        break;
      case 'r':
	rawmode=1;
        break;
      default:
        return(EXIT_FAILURE);
    }
  }

  /* ---- Set the tower up, start monitoring packets -------------------- */
  if (lnp_open(&tower,device,flags)) {
    fprintf(stderr,"Failed to connect to tower\n");
    exit(EXIT_FAILURE);
  }
  if (rawmode) {
    lnp_raw_set_handler(&tower,lnp_raw_hexdump);
  } else {
    if (allmode) lnp_raw_set_handler(&tower,lnp_raw_hexdump_invalid);
    lnp_lego_set_handler(&tower,lnp_lego_hexdump);
    lnp_integrity_set_handler(&tower,lnp_integrity_hexdump);
    lnp_addressing_set_multi(&tower,0,0,lnp_addressing_hexdump);
  }

  /* ---- Program never terminates -------------------------------------- */
  while (1) ;
  lnp_close(&tower);
  return(EXIT_SUCCESS);
}
