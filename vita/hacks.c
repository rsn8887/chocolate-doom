//
// Copyright(C) 2018 fgsfds
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//  Vita-specific hacks
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "config.h"

// increased heap size just in case

unsigned int _newlib_heap_size_user = 128 * 1024 * 1024;

// SDL2_net looks for these and uses them as rand() and srand()
// for some reason, and we don't have them in libc

long int random(void)
{
  return rand();
}

void srandom(unsigned int seed)
{
  srand(seed);
}

// don't have these in our libc

char *inet_ntoa(struct in_addr in)
{
  static char buf[32];
  int ip;
  ip = ntohl(in.s_addr);
  snprintf(buf, sizeof(buf), "%d.%d.%d.%d",
           (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
  return buf;
}

in_addr_t inet_addr(const char *cp)
{
  int b1, b2, b3, b4;
  int res;
  res = sscanf(cp, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
  if (res != 4) return (in_addr_t)(-1); // is actually expected behavior
  return htonl((b1 << 24) | (b2 << 16) | (b3 << 8) | b4);
}

struct hostent *gethostbyaddr(const void *addr,
                              socklen_t len, int type)
{
  static struct hostent ent;
  static char sname[256];
  static struct SceNetInAddr saddr;
  static char *addrlist[2];
  int rid, e;

  addrlist[0] = (char *)&saddr;
  addrlist[1] = NULL;

  if (type != AF_INET || len != sizeof(uint32_t)) return NULL;

  rid = sceNetResolverCreate("d_resolv", NULL, 0);
  if (rid < 0) return NULL;

  memcpy(&saddr.s_addr, addr, sizeof(uint32_t));

  e = sceNetResolverStartAton(rid, &saddr, sname, sizeof(sname), 0, 0, 0);
  sceNetResolverDestroy(rid);
  if (e < 0) return NULL;

  ent.h_name = sname;
  ent.h_aliases = 0;
  ent.h_addrtype = AF_INET;
  ent.h_length = sizeof(struct SceNetInAddr);
  ent.h_addr_list = addrlist;
  ent.h_addr = addrlist[0];

  return &ent;
}
