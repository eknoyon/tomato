/*

  Tomato Firmware
  Copyright (C) 2006-2008 Jonathan Zarate
*/

#include "rc.h"

void start_arpbind(void) {

	char *nvp, *nv, *b;
	const char *ipaddr, *macaddr;
	const char *name, *bind;

	int anyways = nvram_get_int("arpbind_listed");

	nvp = nv = strdup(nvram_safe_get("dhcpd_static"));

	if (!nv) return;

// clear arp table first
	stop_arpbind();

	while ((b = strsep(&nvp, ">")) != NULL) {
		/*
			macaddr<ip.ad.dr.ess<hostname<arpbind>anotherhwaddr<other.ip.addr.ess<othername<arpbind
		*/

		if ((vstrsep(b, "<", &macaddr, &ipaddr, &name, &bind)) != 4)
			continue;
		if (strchr(macaddr,',') != NULL)
			continue;
		if ((strcmp(bind,"1") == 0) || (anyways == 1))
			eval ("arp", "-s", (char *)ipaddr, (char *)macaddr);
	}
	free(nv);
}

void stop_arpbind(void) {

	FILE *f;
	char buf[512];
	char ipaddr[48] = "";

	if ((f = fopen("/proc/net/arp", "r")) != NULL) {
		fgets(buf, sizeof(buf), f);	// header - 1st line should be indeed ignored
		while (fgets(buf, sizeof(buf), f)) {
			if (sscanf(buf, "%s %*s %*s %*s %*s %*s", ipaddr) != 1) continue;
			eval ("arp", "-d", (char *)ipaddr);
		}
		fclose(f);
	}

}
