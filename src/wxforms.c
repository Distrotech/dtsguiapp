/*
    Distrotech Solutions wxWidgets Gui Interface
    Copyright (C) 2013 Gregory Hinton Nietsky <gregory@distrotech.co.za>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#ifdef __WIN32
#define UNICODE 1
#include <winsock2.h>
#include <dirent.h>
#include <shlobj.h>
#endif

#include <dtsapp.h>

#include "dtsgui.h"

#ifndef DATA_DIR
#define DATA_DIR	"/usr/share/dtsguiapp"
#endif

struct app_data {
	struct dtsgui *dtsgui;
	struct xml_doc *xmldoc;
	const char *datadir;
	const char *openconf;
	dtsgui_menuitem editconf;
};

struct listitem {
	const char *name;
	const char *value;
};

void test_posturl(struct dtsgui *dtsgui, const char *user, const char *passwd, const char *url) {
	struct curlbuf *cbuf;
	struct basic_auth *auth;
	struct curl_post *post;

	if (user && passwd) {
		auth = dtsgui_pwdialog(user, passwd, dtsgui);
	} else {
		auth = NULL;
	}

	post = curl_newpost();
	curl_postitem(post, "firstname", "gregory hinton");
	curl_postitem(post, "lastname", "nietsky");
	curl_postitem(post, "test", "&<>@%");

	if (!(cbuf = curl_posturl(url, auth, post, dtsgui_pwdialog, dtsgui))) {
		objunref(auth);
		return;
	}
/*
	curl_ungzip(cbuf);

	if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
		xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize, 1);
	}*/

	if (cbuf && cbuf->body) {
		dtsgui_alert(dtsgui, (char*)cbuf->body);
	}

	objunref(cbuf);
	objunref(auth);
}

void xml_config(struct xml_doc *xmldoc) {
	void *xmlbuf;

	xmlbuf = xml_doctobuffer(xmldoc);
	printf("%s\n", xml_getbuffer(xmlbuf));
	objunref(xmlbuf);
}

void set_temp_xml(struct xml_doc *xmldoc) {
	struct xml_search *xs;
	struct xml_node *xn, *xn2;
	const char *hdir, *sdir, *extif, *conn;
	int domc;

	xml_createpath(xmldoc, "/config/tmp/dcon");
	xml_createpath(xmldoc, "/config/tmp/extif");

	xs = xml_xpath(xmldoc, "/config/FileServer", NULL);
	xn = xml_getfirstnode(xs, NULL);
	objunref(xs);

	hdir = xml_getattr(xn, "homedir");
	sdir = xml_getattr(xn, "sharedir");

	if (hdir && sdir && strcmp(hdir, "") && strcmp(sdir, "")) {
		domc = 1;
	} else {
		domc = 0;
	}

	objunref(xn);

	xs = xml_xpath(xmldoc, "/config/tmp/dcon", NULL);
	xn = xml_getfirstnode(xs, NULL);
	objunref(xs);

	if (domc) {
		xml_modify(xmldoc, xn, "1");
	} else {
		xml_modify(xmldoc, xn, "0");
	}

	objunref(xn);

	xs = xml_xpath(xmldoc, "/config/tmp/extif", NULL);
	xn2 = xml_getfirstnode(xs, NULL);
	objunref(xs);

	xs = xml_xpath(xmldoc, "/config/IP/SysConf/Option[@option = 'External']", NULL);
	xn = xml_getfirstnode(xs, NULL);
	objunref(xs);
	extif = strdup(xn->value);
	xml_setattr(xmldoc, xn2, "External", extif);
	objunref(xn);

	xs = xml_xpath(xmldoc, "/config/IP/Dialup/Option[@option = 'Connection']", NULL);
	xn = xml_getfirstnode(xs, NULL);
	objunref(xs);
	conn = strdup(xn->value);
	xml_setattr(xmldoc, xn2, "Connection", conn);
	if (!strcmp(xn->value, "ADSL")) {
		xml_modify(xmldoc, xn2, extif);
		xml_setattr(xmldoc, xn2, "pppoe", "ADSL");
	} else {
		if (!strcmp(extif, "Dialup")) {
			xml_modify(xmldoc, xn2, conn);
		} else {
			xml_modify(xmldoc, xn2, extif);
		}
		xml_setattr(xmldoc, xn2, "pppoe", "Dialup");
	}
	objunref(xn);
	objunref(xn2);

	if (extif) {
		free((void*)extif);
	}
	if (conn) {
		free((void*)conn);
	}
}

void rem_temp_xml(struct xml_doc *xmldoc) {
	struct xml_search *xs;
	struct xml_node *xn, *xn2, *xn3;

	xs = xml_xpath(xmldoc, "/config/tmp/dcon", NULL);
	xn = xml_getfirstnode(xs, NULL);
	objunref(xs);

	xs = xml_xpath(xmldoc, "/config/FileServer", NULL);
	xn2 = xml_getfirstnode(xs, NULL);
	objunref(xs);

	if (xn && xn->value && !strcmp("1", xn->value)) {
		xml_setattr(xmldoc, xn2, "homedir", "U");
		xml_setattr(xmldoc, xn2, "sharedir", "S");
	} else {
		xml_setattr(xmldoc, xn2, "homedir", "");
		xml_setattr(xmldoc, xn2, "sharedir", "");
	}

	objunref(xn);
	objunref(xn2);

	xs = xml_xpath(xmldoc, "/config/tmp/extif", NULL);
	xn = xml_getfirstnode(xs, NULL);
	objunref(xs);

	xs = xml_xpath(xmldoc, "/config/IP/SysConf/Option[@option = 'External']", NULL);
	xn2 = xml_getfirstnode(xs, NULL);
	objunref(xs);

	xs = xml_xpath(xmldoc, "/config/IP/Dialup/Option[@option = 'Connection']", NULL);
	xn3 = xml_getfirstnode(xs, NULL);
	objunref(xs);

	if (!strcmp(xn->value, "3G") || !strcmp(xn->value, "3GIPW") || !strcmp(xn->value, "Dialup") || !strcmp(xn->value, "Leased")) {
		xml_modify(xmldoc, xn2, "Dialup");
		xml_modify(xmldoc, xn3, xn->value);
	} else {
		xml_modify(xmldoc, xn2, xn->value);
		xml_modify(xmldoc, xn3, xml_getattr(xn, "pppoe"));
	}
	objunref(xn);
	objunref(xn2);
	objunref(xn3);

	xs = xml_xpath(xmldoc, "/config/tmp", NULL);
	xn = xml_getfirstnode(xs, NULL);
	objunref(xs);
	xml_delete(xn);
	objunref(xn);
}

int system_wizard(struct dtsgui *dtsgui, void *data, const char *filename, struct xml_doc *xmldoc) {
	struct listitem cos[] = {{"Internal Extensions", "0"},
							 {"Local PSTN", "1"},
							 {"Long Distance PSTN", "2"},
							 {"Cellular", "3"},
							 {"Premium", "4"},
							 {"International", "5"}};
	dtsgui_pane dp[12], pg;
	struct dtsgui_wizard *twiz;
	struct form_item *ilist;
	const char *newfile;
	int res, cnt;

	set_temp_xml(xmldoc);

	twiz = dtsgui_newwizard(dtsgui, "System Configuration Wizard");

	dp[0] = dtsgui_wizard_addpage(twiz, "Customer Information", NULL, xmldoc);
	dp[1] = dtsgui_wizard_addpage(twiz, "Network/IP Information", NULL, xmldoc);
	dp[2] = dtsgui_wizard_addpage(twiz, "Dynamic DNS", NULL, xmldoc);
	dp[3] = dtsgui_wizard_addpage(twiz, "DNS", NULL, xmldoc);
	dp[4] = dtsgui_wizard_addpage(twiz, "X.509", NULL, xmldoc);
	dp[5] = dtsgui_wizard_addpage(twiz, "Fileserver", NULL, xmldoc);
	dp[6] = dtsgui_wizard_addpage(twiz, "Firewall", NULL, xmldoc);
	dp[7] = dtsgui_wizard_addpage(twiz, "Extensions", NULL, xmldoc);
	dp[8] = dtsgui_wizard_addpage(twiz, "PBX Setup", NULL, xmldoc);
	dp[9] = dtsgui_wizard_addpage(twiz, "Attendant", NULL, xmldoc);
	dp[10] = dtsgui_wizard_addpage(twiz, "Trunk Setup", NULL, xmldoc);
	dp[11] = dtsgui_wizard_addpage(twiz, "Least Cost Routing", NULL, xmldoc);

	pg=dp[0];
	dtsgui_xmltextbox(pg, "Customer Name", NULL, "/config/Customer/Option[@option = 'Name']", NULL);
	dtsgui_xmltextbox(pg, "Contact Person", NULL, "/config/Customer/Option[@option = 'Contact']", NULL);
	dtsgui_xmltextbox(pg, "Email Address", NULL, "/config/Customer/Option[@option = 'Email']", NULL);
	dtsgui_xmltextbox_multi(pg, "Customer Address (Postal)", NULL, "/config/Customer/Option[@option = 'Postal Addr']", NULL);
	dtsgui_xmltextbox_multi(pg, "Customer Address (Physical)", NULL, "/config/Customer/Option[@option = 'Phys Addr']", NULL);
	dtsgui_xmltextbox(pg, "Customer Tel", NULL, "/config/Customer/Option[@option = 'Tel']", NULL);
	dtsgui_xmltextbox(pg, "Customer Company ID", NULL, "/config/Customer/Option[@option = 'ID']", NULL);
	dtsgui_xmltextbox(pg, "VAT No.", NULL, "/config/Customer/Option[@option = 'VAT']", NULL);

	pg=dp[1];
	dtsgui_xmltextbox(pg, "IPv4 Address", NULL, "/config/IP/Interfaces/Interface[/config/IP/SysConf/Option[@option = 'Internal'] = .]", "ipaddr");
	dtsgui_xmltextbox(pg, "IPv4 Subnet Length", NULL, "/config/IP/Interfaces/Interface[/config/IP/SysConf/Option[@option = 'Internal'] = .]", "subnet");
	dtsgui_xmltextbox(pg, "Domain Name", NULL, "/config/DNS/Config/Option[@option = 'Domain']", NULL);
	dtsgui_xmltextbox(pg, "Default Gateway", NULL, "/config/IP/SysConf/Option[@option = 'Nexthop']", NULL);
	dtsgui_xmltextbox(pg, "SMTP Gateway", NULL, "/config/Email/Config/Option[@option = 'Smarthost']", NULL);
	dtsgui_xmltextbox(pg, "NTP Server", NULL, "/config/IP/SysConf/Option[@option = 'NTPServer']", NULL);

	pg=dp[2];
	dtsgui_xmltextbox(pg, "Dynamic DNS Server", NULL, "/config/DNS/Config/Option[@option = 'DynServ']", NULL);
	dtsgui_xmltextbox(pg, "Dynamic DNS Zone", NULL, "/config/DNS/Config/Option[@option = 'DynZone']", NULL);
	dtsgui_xmltextbox(pg, "Dynamic DNS KEY", NULL, "/config/DNS/Config/Option[@option = 'DynKey']", NULL);

	pg=dp[3];
	dtsgui_xmltextbox(pg, "Primary DNS", NULL, "/config/IP/SysConf/Option[@option = 'PrimaryDns']", NULL);
	dtsgui_xmltextbox(pg, "Secondary DNS", NULL, "/config/IP/SysConf/Option[@option = 'SecondaryDns']", NULL);
	dtsgui_xmltextbox(pg, "Primary WINS", NULL, "/config/IP/SysConf/Option[@option = 'PrimaryWins']", NULL);
	dtsgui_xmltextbox(pg, "Secondary WINS", NULL, "/config/IP/SysConf/Option[@option = 'SecondaryWins']", NULL);
	dtsgui_xmltextbox(pg, "Primary MX", NULL, "/config/Email/Config/Option[@option = 'MailExchange1']", NULL);
	dtsgui_xmltextbox(pg, "Secondary MX", NULL, "/config/Email/Config/Option[@option = 'MailExchange2']", NULL);

	pg=dp[4];
	dtsgui_xmltextbox(pg, "Country Code", NULL, "/config/X509/Option[@option = 'Country']", NULL);
	dtsgui_xmltextbox(pg, "Province/State", NULL, "/config/X509/Option[@option = 'State']", NULL);
	dtsgui_xmltextbox(pg, "City", NULL, "/config/X509/Option[@option = 'City']", NULL);
	dtsgui_xmltextbox(pg, "Company", NULL, "/config/X509/Option[@option = 'Company']", NULL);
	dtsgui_xmltextbox(pg, "Division", NULL, "/config/X509/Option[@option = 'Division']", NULL);
	dtsgui_xmltextbox(pg, "Name", NULL, "/config/X509/Option[@option = 'Name']", NULL);
	dtsgui_xmltextbox(pg, "Email", NULL, "/config/X509/Option[@option = 'Email']", NULL);

	pg=dp[5];
	dtsgui_xmltextbox(pg, "Workgroup/Domain", NULL, "/config/FileServer/Setup/Option[@option = 'Domain']", NULL);
	dtsgui_xmltextbox(pg, "Aliases", NULL, "/config/FileServer/Config/Option[@option = 'netbios name']", NULL);
	dtsgui_xmltextbox(pg, "Domain Controllers", NULL, "/config/FileServer/Setup/Option[@option = 'ADSServer']", NULL);
	dtsgui_xmltextbox(pg, "Realm [If Joining ADS]", NULL, "/config/FileServer/Setup/Option[@option = 'ADSRealm']", NULL);
	dtsgui_xmlcheckbox(pg, "Domain Controller", NULL, "1", "0", "/config/tmp/dcon", NULL);

	pg=dp[6];
	ilist = dtsgui_xmlcombobox(pg, "External Interface", NULL, "/config/tmp/extif", NULL);
	dtsgui_listbox_add(ilist, "br0", "br0");
	dtsgui_listbox_add(ilist, "ethB", "ethB");
	dtsgui_listbox_add(ilist, "br0.100", "br0.100");
	dtsgui_listbox_add(ilist, "3G", "3G");
	objunref(ilist);
	dtsgui_xmlcheckbox(pg, "External Device Is PPPoE", NULL, "ADSL", "Dialup", "/config/tmp/extif", "pppoe");
	dtsgui_xmltextbox(pg, "Number/Service/APN", NULL, "/config/IP/Dialup/Option[@option = 'Number']", NULL);
	dtsgui_xmltextbox(pg, "Username", NULL, "/config/IP/Dialup/Option[@option = 'Username']", NULL);
	dtsgui_xmltextbox(pg, "Password", NULL, "/config/IP/Dialup/Option[@option = 'Password']", NULL);
	dtsgui_xmltextbox(pg, "MTU", NULL, "/config/IP/Dialup/Option[@option = 'MTU']", NULL);


	pg=dp[7];
	ilist = dtsgui_xmllistbox(pg, "Default Extension Permision", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'Context']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(ilist, cos[cnt].name, cos[cnt].value);
	}
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "Default Auth Extension Permision", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'AuthContext']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(ilist, cos[cnt].name, cos[cnt].value);
	}
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "Default After Hours Extension Permision", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'DEFALOCK']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(ilist, cos[cnt].name, cos[cnt].value);
	}
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "Snom Network Port Config", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'SnomNet']", NULL);
	dtsgui_listbox_add(ilist, "100Mb/s Full Duplex", "100full");
	dtsgui_listbox_add(ilist, "100Mb/s Half Duplex", "100half");
	dtsgui_listbox_add(ilist, "10Mb/s Full Duplex", "10full");
	dtsgui_listbox_add(ilist, "10Mb/s Half Duplex", "10half");
	dtsgui_listbox_add(ilist, "Auto", "auto");
	objunref(ilist);
	dtsgui_xmltextbox(pg, "Voip Vlan Interface", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'AutoVLAN']", NULL);
	dtsgui_xmltextbox(pg, "Default Extension Prefix", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'DefaultPrefix']", NULL);
	dtsgui_xmltextbox(pg, "Auto Config Start", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'AutoStart']", NULL);
	dtsgui_xmltextbox(pg, "Auto Config End", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'AutoEnd']", NULL);
	dtsgui_xmltextbox(pg, "Default Ring Timeout", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'Timeout']", NULL);
	dtsgui_xmlcheckbox(pg, "Require Authorization", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoAuth']", NULL);
	dtsgui_xmlcheckbox(pg, "Lock Snom Phone Settings", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoLock']", NULL);
	dtsgui_xmlcheckbox(pg, "Enable Voice Mail", NULL, "0", "1", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFNOVMAIL']", NULL);
	dtsgui_xmlcheckbox(pg, "Enable Call Logging", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFRECORD']", NULL);
	dtsgui_xmlcheckbox(pg, "Require Extension Number With PIN", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'ADVPIN']", NULL);

	pg=dp[8];
	dtsgui_xmltextbox(pg, "Local Area Code", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'AreaCode']", NULL);
	dtsgui_xmltextbox(pg, "Local Extension Prefix", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'ExCode']", NULL);
	ilist = dtsgui_xmllistbox(pg, "ISDN PRI Framing [T1-E1]", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'PRIframing']", NULL);
	dtsgui_listbox_add(ilist, "ccs - esf", "ccs");
	dtsgui_listbox_add(ilist, "cas - d4/Superframe", "cas");
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "ISDN PRI Coding [T1-E1]", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'PRIcoding']", NULL);
	dtsgui_listbox_add(ilist, "hdb3 - b8zs", "hdb3");
	dtsgui_listbox_add(ilist, "ami", "ami");
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "Line Build Out", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'PRIlbo']", NULL);
	dtsgui_listbox_add(ilist, "0 db (CSU) / 0-133 feet (DSX-1)", "0");
	dtsgui_listbox_add(ilist, "133-266 feet (DSX-1)", "1");
	dtsgui_listbox_add(ilist, "266-399 feet (DSX-1)", "2");
	dtsgui_listbox_add(ilist, "399-533 feet (DSX-1)", "3");
	dtsgui_listbox_add(ilist, "533-655 feet (DSX-1)", "4");
	dtsgui_listbox_add(ilist, "-7.5db (CSU)", "5");
	dtsgui_listbox_add(ilist, "-15db (CSU)", "6");
	dtsgui_listbox_add(ilist, "-22.5db (CSU)", "7");
	objunref(ilist);
	dtsgui_xmlcheckbox(pg, "Calls To Internal Extensions Follow Forward Rules", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'LocalFwd']", NULL);
	dtsgui_xmlcheckbox(pg, "Hangup Calls To Unknown Numbers/DDI", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'UNKDEF']", NULL);
	dtsgui_xmlcheckbox(pg, "ISDN BRI Immeadiate Routeing (No MSN/DDI)", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNimm']", NULL);
	dtsgui_xmlcheckbox(pg, "ISDN BRI Use Round Robin Routing", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNrr']", NULL);
	dtsgui_xmlcheckbox(pg, "ISDN Allow Automatic Setting Of CLI (DDI Required)", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoCLI']", NULL);
	dtsgui_xmlcheckbox(pg, "ISDN PRI CRC4 Checking (E1 Only)", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'PRIcrc4']", NULL);

	pg=dp[9];
	dtsgui_xmltextbox(pg, "Queue Timeout Checked Every 18s", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'AATimeout']", NULL);
	dtsgui_xmltextbox(pg, "Auto Attendant Mailbox/Forward On No Agent/Timeout", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'AANext']", NULL);
	dtsgui_xmltextbox(pg, "IVR Delay Between Digits", NULL, "/config/IP/VOIP/ASTDB/Option[@option = 'AADelay']", NULL);
	dtsgui_xmlcheckbox(pg, "Disable Default Auto Attendant Prompts", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AANOPROMPT']", NULL);
	dtsgui_xmlcheckbox(pg, "Music On Hold When Calling Reception", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AAMOH']", NULL);
	dtsgui_xmlcheckbox(pg, "Record Inbound Calls", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AAREC']", NULL);

	pg=dp[10];
	ilist = dtsgui_xmllistbox(pg, "Primary Trunk", NULL, NULL, NULL);
	dtsgui_listbox_add(ilist, "None", NULL);
	dtsgui_listbox_add(ilist, "ISDN PRI", NULL);
	dtsgui_listbox_add(ilist, "ISDN BRI", NULL);
	dtsgui_listbox_add(ilist, "Analogue", NULL);
	objunref(ilist);
	dtsgui_xmltextbox(pg, "No. Of Trunk Ports", NULL, NULL, NULL);
	ilist = dtsgui_xmllistbox(pg, "Gateway Trunk", NULL, NULL, NULL);
	dtsgui_listbox_add(ilist, "None", NULL);
	dtsgui_listbox_add(ilist, "ISDN PRI", NULL);
	dtsgui_listbox_add(ilist, "ISDN BRI", NULL);
	dtsgui_listbox_add(ilist, "Analogue", NULL);
	objunref(ilist);
	dtsgui_xmltextbox(pg, "No. Of Gateway Ports", NULL, NULL, NULL);
	dtsgui_xmlcheckbox(pg, "Extensions Are On Gateway By Default", NULL, "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'REMDEF']", NULL);

	pg=dp[11];
	ilist = dtsgui_xmllistbox(pg, "Protocol", NULL, "/config/IP/VOIP", "protocol");
	dtsgui_listbox_add(ilist, "SIP", "SIP");
	dtsgui_listbox_add(ilist, "IAX2", "IAX2");
	objunref(ilist);
	dtsgui_xmlcheckbox(pg, "Use DTMF INFO (SIP)", NULL, "info", "rfc2833", "/config/IP/VOIP", "dtmf");
	dtsgui_xmlcheckbox(pg, "Use SRTP (SIP)", NULL, "true", "false", "/config/IP/VOIP", "srtp");
	dtsgui_xmlcheckbox(pg, "Use From User (SIP [Disables Sending CLI])", NULL, "true", "false", "/config/IP/VOIP", "fromuser");
	dtsgui_xmlcheckbox(pg, "Disable Video", NULL, "true", "false", "/config/IP/VOIP", "novideo");
	dtsgui_xmlcheckbox(pg, "Register To Server", NULL, "true", "false", "/config/IP/VOIP", "register");

	res = dtsgui_runwizard(twiz);

	if (res) {
		cnt = sizeof(dp)/sizeof(dp[0])-1;
		for(; cnt >= 0;cnt--) {
			dtsgui_xmlpanel_update(dp[cnt]);
		}

		rem_temp_xml(xmldoc);

		if (!filename) {
			do {
				newfile = dtsgui_filesave(dtsgui, "Save New Customer Config To File", NULL, "newcustomer.xml", "XML Configuration|*.xml");
			} while (!newfile && !dtsgui_confirm(dtsgui, "No file selected !!!\nDo you want to continue (And loose settings)"));

			if (newfile) {
				xml_savefile(xmldoc, newfile, 1, 9);
				objunref((void*)newfile);
			}
		} else if (filename) {
			xml_savefile(xmldoc, filename, 1, 9);
		}
	}

	objunref(twiz);
	if (xmldoc) {
		objunref(xmldoc);
	}
	return res;
}

int newsys_wizard(struct dtsgui *dtsgui, void *data) {
	char defconf[PATH_MAX];
	struct xml_doc *xmldoc;
	struct app_data *appdata;

	appdata = dtsgui_userdata(dtsgui);
	snprintf(defconf, PATH_MAX-1, "%s/default.xml", appdata->datadir);
	if (!is_file(defconf)) {
		dtsgui_alert(dtsgui, "Default configuration not found.\nCheck Installation.");
		return 0;
	}

	if (!(xmldoc = xml_loaddoc(defconf, 1))) {
		dtsgui_alert(dtsgui, "Default configuration failed to load.\nCheck Installation.");
		return 0;
	}

	return system_wizard(dtsgui, data, NULL, xmldoc);
}

int editsys_wizard(struct dtsgui *dtsgui, void *data) {
	struct xml_doc *xmldoc;
	const char *filename;

	if (!(filename = dtsgui_fileopen(dtsgui, "Select Customer Configuration To Edit", NULL, "", "XML Configuration|*.xml"))) {
		return 0;
	}

	if (!(xmldoc = xml_loaddoc(filename, 1	))) {
		dtsgui_alert(dtsgui, "Configuration failed to load.\n");
		return 0;
	}

	return system_wizard(dtsgui, data, filename, xmldoc);
}

void handle_test(dtsgui_pane p, int type, int event, void *data) {
	switch(event) {
		case wx_PANEL_BUTTON_YES:
			printf("Ok\n");
			break;

		case wx_PANEL_BUTTON_NO:
			printf("Cancel\n");
			break;

		default:
			printf("Other %i\n", event);
			break;
	}
}

void testpanel(struct dtsgui *dtsgui, dtsgui_menu menu) {
	dtsgui_pane p;
	struct form_item *l;

	p = dtsgui_panel(dtsgui, "Test Panel", wx_PANEL_BUTTON_ALL, 1, NULL);
	dtsgui_setevcallback(p, handle_test, NULL);

	dtsgui_textbox(p, "TextBox", "text", "Text", NULL);
	dtsgui_textbox_multi(p, "Multi Line", "multi", "L\nL\n", NULL);
	dtsgui_checkbox(p, "Checkbox", "check", 1, NULL);

	l = dtsgui_combobox(p, "ComboBox", "combo", NULL);
	dtsgui_listbox_add(l, "Test1", NULL);
	dtsgui_listbox_add(l, "Test2", NULL);
	dtsgui_listbox_add(l, "Test3", NULL);
	dtsgui_listbox_add(l, "Alt Test3", NULL);
	objunref(l);

	l = dtsgui_listbox(p, "ListBox", "list", NULL);
	dtsgui_listbox_add(l, "Test1", NULL);
	dtsgui_listbox_add(l, "Test2", NULL);
	dtsgui_listbox_add(l, "Test3", NULL);
	dtsgui_listbox_add(l, "Alt Test3", NULL);
	objunref(l);

	dtsgui_newmenuitem(menu, dtsgui, "&Test", p);
}

void pbx_settings(struct dtsgui *dtsgui, dtsgui_tabview tabv) {
	struct listitem trunks[] = {{"Linux Modular ISDN Group 1", "mISDN/g:out/"},
								{"Linux Modular ISDN Group 2", "mISDN/g:out2/"},
								{"Linux Modular ISDN Group 3", "mISDN/g:out3/"},
								{"Linux Modular ISDN Group 4", "mISDN/g:out4/"},
								{"Digium Trunk Group 1", "DAHDI/r1/"},
								{"Digium Trunk Group 2", "DAHDI/r2/"},
								{"Digium Trunk Group 3", "DAHDI/r3/"},
								{"Digium Trunk Group 4", "DAHDI/r4/"},
								{"Woomera Trunk Group 1", "WOOMERA/g1/"},
								{"Woomera Trunk Group 2", "WOOMERA/g2/"},
								{"Woomera Trunk Group 3", "WOOMERA/g3/"},
								{"Woomera Trunk Group 4", "WOOMERA/g4/"}};

	struct listitem cos[] = {{"Internal Extensions", "0"},
							 {"Local PSTN", "1"},
							 {"Long Distance PSTN", "2"},
							 {"Cellular", "3"},
							 {"Premium", "4"},
							 {"International", "5"}};

	char defconf[PATH_MAX];
	struct xml_doc *xmldoc;
	struct app_data *appdata;
	dtsgui_pane dp[11], pg;
	struct form_item *lb;
	int cnt, i;

	appdata = dtsgui_userdata(dtsgui);
	snprintf(defconf, PATH_MAX-1, "%s/default.xml", appdata->datadir);
	if (!is_file(defconf)) {
		dtsgui_alert(dtsgui, "Default configuration not found.\nCheck Installation.");
		return;
	}

	if (!(xmldoc = xml_loaddoc(defconf, 1))) {
		dtsgui_alert(dtsgui, "Default configuration failed to load.\nCheck Installation.");
		return;
	}

	dp[0] = dtsgui_addpage(tabv, "Routing", 0, NULL, xmldoc);
	dp[1] = dtsgui_addpage(tabv, "mISDN", 0, NULL, xmldoc);
	dp[2] = dtsgui_addpage(tabv, "E1", 0, NULL, xmldoc);
	dp[3] = dtsgui_addpage(tabv, "MFC/R2", 0, NULL, xmldoc);
	dp[4] = dtsgui_addpage(tabv, "Defaults", 0, NULL, xmldoc);
	dp[5] = dtsgui_addpage(tabv, "IVR Password", 0, NULL, xmldoc);
	dp[6] = dtsgui_addpage(tabv, "Location", 0, NULL, xmldoc);
	dp[7] = dtsgui_addpage(tabv, "Inbound", 0, NULL, xmldoc);
	dp[8] = dtsgui_addpage(tabv, "Num Plan", 0, NULL, xmldoc);
	dp[9] = dtsgui_addpage(tabv, "Auto Add", 0, NULL, xmldoc);
	dp[10] = dtsgui_addpage(tabv, "Save", wx_PANEL_BUTTON_YES, NULL, xmldoc);

	pg = dp[0];
	lb = dtsgui_xmllistbox(pg, "PSTN Trunk", "Trunk", "/config/IP/VOIP/ASTDB/Option[@option = 'Trunk']", NULL);
	dtsgui_listbox_add(lb, "Do Not Use 1 Trunk", "NONE");
	cnt = sizeof(trunks)/sizeof(trunks[0]);
	for(i=0;i < cnt;i++) {
		dtsgui_listbox_add(lb, trunks[i].name, trunks[i].value);
	}
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "PSTN Second Trunk", "Trunk2", "/config/IP/VOIP/ASTDB/Option[@option = 'Trunk2']", NULL);
	dtsgui_listbox_add(lb, "Do Not Use 2 Trunk", "NONE");
	for(i=0;i < cnt;i++) {
		dtsgui_listbox_add(lb, trunks[i].name, trunks[i].value);
	}
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "PSTN Third Trunk", "Trunk3", "/config/IP/VOIP/ASTDB/Option[@option = 'Trunk3']", NULL);
	dtsgui_listbox_add(lb, "Do Not Use 3 Trunk", "NONE");
	for(i=0;i < cnt;i++) {
		dtsgui_listbox_add(lb, trunks[i].name, trunks[i].value);
	}
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "PSTN Fourth Trunk", "Trunk4", "/config/IP/VOIP/ASTDB/Option[@option = 'Trunk4']", NULL);
	dtsgui_listbox_add(lb, "Do Not Use 4 Trunk", "NONE");
	for(i=0;i < cnt;i++) {
		dtsgui_listbox_add(lb, trunks[i].name, trunks[i].value);
	}
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Forward Trunk", "FTrunk", "/config/IP/VOIP/ASTDB/Option[@option = 'FTrunk']", NULL);
	dtsgui_listbox_add(lb, "Do Not Use Forward Trunk", "NONE");
	dtsgui_listbox_add(lb, "Linux Modular ISDN Forward Group", "mISDN/g:fwd/");
	for(i=0;i < cnt;i++) {
		dtsgui_listbox_add(lb, trunks[i].name, trunks[i].value);
	}
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "GSM Trunk", "CellGateway", "/config/IP/VOIP/ASTDB/Option[@option = 'CellGateway']", NULL);
	dtsgui_listbox_add(lb, "Do Not Use GSM Trunk", "");
	dtsgui_listbox_add(lb, "Linux Modular ISDN Forward Group", "mISDN/g:fwd/");
	for(i=0;i < cnt;i++) {
		dtsgui_listbox_add(lb, trunks[i].name, trunks[i].value);
	}
	objunref(lb);

	lb = dtsgui_xmllistbox(pg, "Level To Route Calls", "IPContext", "/config/IP/VOIP/ASTDB/Option[@option = 'IPContext']", NULL);
	dtsgui_listbox_add(lb, "Local PSTN Calls", "1");
	dtsgui_listbox_add(lb, "Long Distance PSTN Calls", "2");
	dtsgui_listbox_add(lb, "Cellular Calls", "3");
	dtsgui_listbox_add(lb, "Premium Calls", "4");
	dtsgui_listbox_add(lb, "International Calls", "5");
	dtsgui_listbox_add(lb, "No IP Routing", "6");
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Number Plan For Routing", "IntLocal", "/config/IP/VOIP/ASTDB/Option[@option = 'IntLocal']", NULL);
	dtsgui_listbox_add(lb, "International", "1");
	dtsgui_listbox_add(lb, "International With Access Code", "2");
	dtsgui_listbox_add(lb, "International With +", "3");
	objunref(lb);
	dtsgui_xmltextbox(pg, "Maximum Concurency On Voip Trunk", "VLIMIT", "/config/IP/VOIP/ASTDB/Option[@option = 'VLIMIT']", NULL);
	dtsgui_xmltextbox(pg, "Prefix Trunk Calls With", "TrunkPre", "/config/IP/VOIP/ASTDB/Option[@option = 'TrunkPre']", NULL);
	dtsgui_xmltextbox(pg, "Number Of Digits To Strip On Trunk", "TrunkStrip", "/config/IP/VOIP/ASTDB/Option[@option = 'TrunkStrip']", NULL);
	dtsgui_xmltextbox(pg, "Maximum Call Length On Analogue Trunks (mins)", "MaxAna", "/config/IP/VOIP/ASTDB/Option[@option = 'MaxAna']", NULL);
	dtsgui_xmlcheckbox(pg, "Apply Call Limt To All Trunks", "MaxAll", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'MaxAll']", NULL);
	dtsgui_xmlcheckbox(pg, "Allow VOIP Fallover When Trunk Is Unavailable", "VoipFallover", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'VoipFallover']", NULL);
	dtsgui_xmlcheckbox(pg, "Allow International Trunk Failover When Voip Fails", "IntFallover", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'IntFallover']", NULL);
	dtsgui_xmlcheckbox(pg, "Use ENUM Lookups On Outgoing Calls", "NoEnum", "0", "1", "/config/IP/VOIP/ASTDB/Option[@option = 'NoEnum']", NULL);
	dtsgui_xmlcheckbox(pg, "Use Configured GSM Routers", "GSMRoute", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'GSMRoute']", NULL);
	dtsgui_xmlcheckbox(pg, "Allow Trunk Failover When Using Configured GSM Routers", "GSMTrunk", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'GSMTrunk']", NULL);
	dtsgui_xmlcheckbox(pg, "Calls To Internal Extensions Follow Forward Rules", "LocalFwd", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'LocalFwd']", NULL);
	dtsgui_xmlcheckbox(pg, "Inbound Calls Forwarded To Reception If No Voicemail", "Default_9", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'Default_9']", NULL);
	dtsgui_xmlcheckbox(pg, "Disable Billing Engine", "PPDIS", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'PPDIS']", NULL);
	dtsgui_xmlcheckbox(pg, "Allow DISA Passthrough On Trunks", "DISADDI", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'DISADDI']", NULL);
	dtsgui_xmlcheckbox(pg, "Disable Native Bridging On Outbound", "NoBridge", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'NoBridge']", NULL);
	dtsgui_xmlcheckbox(pg, "Disable access to invalid accounts", "ValidAcc", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'ValidAcc']", NULL);

	pg = dp[1];
	dtsgui_xmltextbox(pg, "Isdn Ports To Use (Group 1)", "mISDNports", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNports']", NULL);
	dtsgui_xmltextbox(pg, "Isdn Ports To Use (Group 2)", "mISDNports2", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNports2']", NULL);
	dtsgui_xmltextbox(pg, "Isdn Ports To Use (Group 3)", "mISDNports3", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNports3']", NULL);
	dtsgui_xmltextbox(pg, "Isdn Ports To Use (Group 4)", "mISDNports4", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNports4']", NULL);
	dtsgui_xmltextbox(pg, "Isdn Ports To Use (In Only)", "mISDNinports", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNinports']", NULL);
	dtsgui_xmltextbox(pg, "Isdn Ports To Use (Forwarding)", "mISDNfwdports", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNfwdports']", NULL);
	dtsgui_xmltextbox(pg, "RX Gain", "mISDNgainrx","/config/IP/VOIP/ASTDB/Option[@option = 'mISDNgainrx']", NULL);
	dtsgui_xmltextbox(pg, "TX Gain", "mISDNgaintx", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNgaintx']", NULL);
	dtsgui_xmlcheckbox(pg, "Immeadiate Routing (No MSN/DDI)", "mISDNimm", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNimm']", NULL);
	dtsgui_xmlcheckbox(pg, "Use Round Robin Routing", "mISDNrr", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNrr']", NULL);

	pg = dp[2];
	lb = dtsgui_xmllistbox(pg, "Line Build Out", "PRIlbo","/config/IP/VOIP/ASTDB/Option[@option = 'PRIlbo']", NULL);
	dtsgui_listbox_add(lb, "0 db (CSU) / 0-133 feet (DSX-1)", "0");
	dtsgui_listbox_add(lb, "133-266 feet (DSX-1)", "1");
	dtsgui_listbox_add(lb, "266-399 feet (DSX-1)", "2");
	dtsgui_listbox_add(lb, "399-533 feet (DSX-1)", "3");
	dtsgui_listbox_add(lb, "533-655 feet (DSX-1)", "4");
	dtsgui_listbox_add(lb, "-7.5db (CSU)", "5");
	dtsgui_listbox_add(lb, "-15db (CSU)", "6");
	dtsgui_listbox_add(lb, "-22.5db (CSU)", "7");
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "PRI Framing (E1 - T1)", "PRIframing", "/config/IP/VOIP/ASTDB/Option[@option = 'PRIframing']", NULL);
	dtsgui_listbox_add(lb, "cas - d4/sf/superframe", "cas");
	dtsgui_listbox_add(lb, "ccs - esf", "ccs");
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "PRI Coding (E1 - T1)", "PRIcoding", "/config/IP/VOIP/ASTDB/Option[@option = 'PRIcoding']", NULL);
	dtsgui_listbox_add(lb, "ami", "ami");
	dtsgui_listbox_add(lb, "hdb3 - b8zs", "hdb3");
	objunref(lb);
	dtsgui_xmlcheckbox(pg, "CRC4 Checking (E1 Only)", "PRIcrc4", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'PRIcrc4']", NULL);

	pg = dp[3];
	lb = dtsgui_xmllistbox(pg, "Variant", "E1mfcr2_variant", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_variant']", NULL);
	dtsgui_listbox_add(lb, "ITU Standard", "itu");
	dtsgui_listbox_add(lb, "Argentina", "ar");
	dtsgui_listbox_add(lb, "Brazil", "br");
	dtsgui_listbox_add(lb, "China", "cn");
	dtsgui_listbox_add(lb, "Czech Republic", "cz");
	dtsgui_listbox_add(lb, "Columbia", "co");
	dtsgui_listbox_add(lb, "Ecuador", "ec");
	dtsgui_listbox_add(lb, "Mexico", "mx");
	dtsgui_listbox_add(lb, "Philippines", "ph");
	dtsgui_listbox_add(lb, "Venezuela", "ve");
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Caller Category", "E1mfcr2_category", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_category']", NULL);
	dtsgui_listbox_add(lb, "National Subscriber", "national_subscriber");
	dtsgui_listbox_add(lb, "National Priority Subscriber", "national_priority_subscriber");
	dtsgui_listbox_add(lb, "International Subscriber", "international_subscriber");
	dtsgui_listbox_add(lb, "International Priority Subscriber", "international_priority_subscriber");
	dtsgui_listbox_add(lb, "Collect Call", "collect_call");
	objunref(lb);
	dtsgui_xmltextbox(pg, "Max ANI Digits", "E1mfcr2_max_ani", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_max_ani']", NULL);
	dtsgui_xmltextbox(pg, "Max DNIS Digits", "E1mfcr2_max_dnis", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_max_dnis']", NULL);
	dtsgui_xmlcheckbox(pg, "ANI Before DNIS", "E1mfcr2_get_ani_first", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_get_ani_first']", NULL);
	dtsgui_xmlcheckbox(pg, "Allow Collect Calls (BR:llamadas por cobrar)", "E1mfcr2_allow_collect_calls", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_allow_collect_calls']", NULL);
	dtsgui_xmlcheckbox(pg, "Block Collect Calls With Double Answer", "E1mfcr2_double_answer", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_double_answer']", NULL);
	dtsgui_xmlcheckbox(pg, "Immeadiate Answer", "E1mfcr2_immediate_accept", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_immediate_accept']", NULL);
	dtsgui_xmlcheckbox(pg, "Forced Release (BR)", "E1mfcr2_forced_release", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_forced_release']", NULL);
	dtsgui_xmlcheckbox(pg, "Accept Call With Charge", "E1mfcr2_charge_calls", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'E1mfcr2_charge_calls']", NULL);

	pg = dp[4];
	lb = dtsgui_xmllistbox(pg, "Default Extension Permision", "Context", "/config/IP/VOIP/ASTDB/Option[@option = 'Context']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(lb, cos[cnt].name, cos[cnt].value);
	}
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Default Auth Extension Permision", "AuthContext", "/config/IP/VOIP/ASTDB/Option[@option = 'AuthContext']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(lb, cos[cnt].name, cos[cnt].value);
	}
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Default After Hours Extension Permision", "DEFALOCK", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFALOCK']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(lb, cos[cnt].name, cos[cnt].value);
	}
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Valid Line Authentication", "LINEAUTH", "/config/IP/VOIP/ASTDB/Option[@option = 'LINEAUTH']", NULL);
	dtsgui_listbox_add(lb, "Line PW. That Is Same As Extension And All Bellow", "1");
	dtsgui_listbox_add(lb, "Allow VM PW. Or Line PW. Not The Same As Exten", "2");
	dtsgui_listbox_add(lb, "Allow Only VM PW. Not The Same As Line PW. Or Exten.", "3");
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Snom Network Port Speed/Duplex", "SnomNet", "/config/IP/VOIP/ASTDB/Option[@option = 'SnomNet']", NULL);
	dtsgui_listbox_add(lb, "Auto Negotiation", "auto");
	dtsgui_listbox_add(lb, "10 Mbit Half Duplex", "10half");
	dtsgui_listbox_add(lb, "10 Mbit Full Duplex", "10full");
	dtsgui_listbox_add(lb, "100 Mbit Half Duplex", "100half");
	dtsgui_listbox_add(lb, "100 Mbit Full Duplex", "100full");
	objunref(lb);
	dtsgui_xmltextbox(pg, "Default FAX Handler", "FAXBOX", "/config/IP/VOIP/ASTDB/Option[@option = 'FAXBOX']", NULL);
	dtsgui_xmltextbox(pg, "Default Ring Timeout", "Timeout", "/config/IP/VOIP/ASTDB/Option[@option = 'Timeout']", NULL);
	dtsgui_xmltextbox(pg, "Default Extension Prefix (2 Digit Dialing)", "DefaultPrefix", "/config/IP/VOIP/ASTDB/Option[@option = 'DefaultPrefix']", NULL);
	dtsgui_xmltextbox(pg, "Default CLI (Number Displayed To Called Party)", "DefCLI", "/config/IP/VOIP/ASTDB/Option[@option = 'DefCLI']", NULL);
	dtsgui_xmltextbox(pg, "Default ACD Queue Timeout", "QTimeout", "/config/IP/VOIP/ASTDB/Option[@option = 'QTimeout']", NULL);
	dtsgui_xmltextbox(pg, "Default ACD Queue Agent Timeout", "QATimeout", "/config/IP/VOIP/ASTDB/Option[@option = 'QATimeout']", NULL);
	dtsgui_xmltextbox(pg, "Default ACD Queue Agent Penalty Factor", "QAPenalty", "/config/IP/VOIP/ASTDB/Option[@option = 'QAPenalty']", NULL);
	dtsgui_xmltextbox(pg, "Recording Options", "RecOpt", "/config/IP/VOIP/ASTDB/Option[@option = 'RecOpt']", NULL);
	dtsgui_xmltextbox(pg, "Default SIP IP Subnet", "DEFIPNET", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFIPNET']", NULL);
	dtsgui_xmlcheckbox(pg, "Record Calls By Default", "DEFRECORD", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFRECORD']", NULL);
	dtsgui_xmlcheckbox(pg, "Enable Voice Mail By Default", "DEFNOVMAIL", "0", "1", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFNOVMAIL']", NULL);
	dtsgui_xmlcheckbox(pg, "Hangup Calls To Unknown Numbers/DDI", "UNKDEF", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'UNKDEF']", NULL);
	dtsgui_xmlcheckbox(pg, "Extensions Are Remote By Default", "REMDEF", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'REMDEF']", NULL);
	dtsgui_xmlcheckbox(pg, "Disable Routing Of Voice Mail To Reception", "NoOper", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'NoOper']", NULL);
	dtsgui_xmlcheckbox(pg, "Require Extension Number With PIN", "ADVPIN", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'ADVPIN']", NULL);
	dtsgui_xmlcheckbox(pg, "Add Billing Group To CLI (Inbound)", "AddGroup", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AddGroup']", NULL);
	dtsgui_xmlcheckbox(pg, "Follow DDI If Exten (Inbound)", "FollowDDI", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'FollowDDI']", NULL);
	dtsgui_xmlcheckbox(pg, "Authorise Only When Registered By Default (SIP)", "DEFAUTHREG", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFAUTHREG']", NULL);

	pg=dp[5];
	dtsgui_xmlpasswdbox(pg, "Admin Password", "AdminPass", "/config/IP/VOIP/ASTDB/Option[@option = 'AdminPass']", NULL);

	pg=dp[6];
	dtsgui_xmltextbox(pg, "Local Country Code", "CountryCode", "/config/IP/VOIP/ASTDB/Option[@option = 'CountryCode']", NULL);
	dtsgui_xmltextbox(pg, "Local Area Code", "AreaCode", "/config/IP/VOIP/ASTDB/Option[@option = 'AreaCode']", NULL);
	dtsgui_xmltextbox(pg, "Local Exchange Prefix", "ExCode", "/config/IP/VOIP/ASTDB/Option[@option = 'ExCode']", NULL);
	dtsgui_xmltextbox(pg, "Local Call Distance (Or Blank To Base Calls On Area Code)", "LDDist", "/config/IP/VOIP/ASTDB/Option[@option = 'LDDist']", NULL);
	dtsgui_xmltextbox(pg, "Local Number Length (0 To Disable)", "LocalLength", "/config/IP/VOIP/ASTDB/Option[@option = 'LocalLength']", NULL);
	dtsgui_xmltextbox(pg, "National Number Length (0 To Disable)", "NatLength", "/config/IP/VOIP/ASTDB/Option[@option = 'NatLength']", NULL);
	dtsgui_xmltextbox(pg, "National Access Code", "LocalAccess", "/config/IP/VOIP/ASTDB/Option[@option = 'LocalAccess']", NULL);
	dtsgui_xmltextbox(pg, "International Access Code", "IntAccess", "/config/IP/VOIP/ASTDB/Option[@option = 'IntAccess']", NULL);

	pg=dp[7];
	lb = dtsgui_xmllistbox(pg, "Default Attendant", "Attendant", "/config/IP/VOIP/ASTDB/Option[@option = 'Attendant']", NULL);
	dtsgui_listbox_add(lb, "Auto Attendant", "0"); /*XXX Add Extensions List*/
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Auto Attendant Queue", "AttendantQ", "/config/IP/VOIP/ASTDB/Option[@option = 'AttendantQ']", NULL);
	dtsgui_listbox_add(lb, "Default Auto Attendant (Simple Ring All)", "799"); /*XXX Addd ACD List*/
	dtsgui_listbox_add(lb, "No Default Attendant", "-1");
	objunref(lb);
	lb = dtsgui_xmllistbox(pg, "Default Fax Terminal", "FAXT", "/config/IP/VOIP/ASTDB/Option[@option = 'FAXT']", NULL);
	dtsgui_listbox_add(lb, "Auto Fax Detect & Receive", ""); /*XXX ADD Fax Capable Devices*/
	objunref(lb);
	dtsgui_xmlcheckbox(pg, "Enable Inbound FAX Detect", "IFAXD", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'IFAXD']", NULL);

	pg = dp[8];
	dtsgui_xmltextbox(pg, "4 Digit Telco Number Pattern", "InternalPat", "/config/IP/VOIP/ASTDB/Option[@option = 'InternalPat']", NULL);
	dtsgui_xmltextbox(pg, "Telco Number Pattern (Premium)", "TPremiumPat", "/config/IP/VOIP/ASTDB/Option[@option = 'TPremiumPat']", NULL);
	dtsgui_xmltextbox(pg, "Genral Premium Numbers Pattern", "PremiumPat", "/config/IP/VOIP/ASTDB/Option[@option = 'PremiumPat']", NULL);
	dtsgui_xmltextbox(pg, "Cellular Numbers Pattern", "GSMPat", "/config/IP/VOIP/ASTDB/Option[@option = 'GSMPat']", NULL);
	dtsgui_xmltextbox(pg, "Explicit Long Distance  Numbers Pattern", "NationalPat", "/config/IP/VOIP/ASTDB/Option[@option = 'NationalPat']", NULL);
	dtsgui_xmltextbox(pg, "Explicit Local Numbers Pattern", "LocalPat", "/config/IP/VOIP/ASTDB/Option[@option = 'LocalPat']", NULL);
	dtsgui_xmltextbox(pg, "Toll Free Numbers Pattern", "FreePat", "/config/IP/VOIP/ASTDB/Option[@option = 'FreePat']", NULL);
	dtsgui_xmltextbox(pg, "Voip Numbers Pattern", "VoipPat", "/config/IP/VOIP/ASTDB/Option[@option = 'VoipPat']", NULL);
	dtsgui_xmltextbox(pg, "GSM Router Trunk Failover Allow Pattern", "GSMFOPat", "/config/IP/VOIP/ASTDB/Option[@option = 'GSMFOPat']", NULL);
	dtsgui_xmltextbox(pg, "Inbound Local Call Pattern (Trunk Forward)", "TRUNKDDIPat", "/config/IP/VOIP/ASTDB/Option[@option = 'TRUNKDDIPat']", NULL);
	dtsgui_xmltextbox(pg, "DDI Limit Pattern (Accepted DDI More Than 4 Digits)", "DDIPAT", "/config/IP/VOIP/ASTDB/Option[@option = 'DDIPAT']", NULL);

	pg = dp[9];
	dtsgui_xmltextbox(pg, "Start Exten.", "AutoStart", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoStart']", NULL);
	dtsgui_xmltextbox(pg, "End Exten.", "AutoEnd", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoEnd']", NULL);
	dtsgui_xmltextbox(pg, "VLAN", "AutoVLAN", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoVLAN']", NULL);
	dtsgui_xmltextbox(pg, "STUN Server (Linksys)", "AutoSTUN", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoSTUN']", NULL);
	dtsgui_xmlcheckbox(pg, "Lock Settings (Snom)", "AutoLock", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoLock']", NULL);
	dtsgui_xmlcheckbox(pg, "Require Authorisation", "AutoAuth", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoAuth']", NULL);

	dtsgui_setevcallback(dp[10], handle_test, &dp);

	cnt = sizeof(dp)/sizeof(dp[0]);

	for(i = 0; i < cnt;i++) {
		dtsgui_item_xmlcreate(dp[i], "/config/IP/VOIP/ASTDB", "Option", "option");
	}

	for(i = 0; i < cnt;i++) {
		dtsgui_showpanel(dp[i], !i);
	}
	objunref(xmldoc);
}

int post_test(struct dtsgui *dtsgui, void *data) {
	test_posturl(dtsgui, NULL, NULL, "https://sip1.speakezi.co.za:666/auth/test.php");

	return 1;
}

void file_menu(struct dtsgui *dtsgui) {
	dtsgui_menu file;
	dtsgui_treeview tree;
	dtsgui_tabview tabv;
	struct app_data *appdata;

	appdata = dtsgui_userdata(dtsgui);

	file = dtsgui_newmenu(dtsgui, "&File");

	dtsgui_newmenucb(file, dtsgui, "&New System (Wizard)", "New System Configuration Wizard", newsys_wizard, NULL);
	appdata->editconf = dtsgui_newmenucb(file, dtsgui, "&Edit Saved System (Wizard)", "Reconfigure Saved System File With Wizard ", editsys_wizard, NULL);

	dtsgui_menusep(file);
	tabv = dtsgui_tabwindow(dtsgui, "PBX Setup");

	dtsgui_newmenuitem(file, dtsgui, "P&BX Configuration", tabv);

	pbx_settings(dtsgui, tabv);

	dtsgui_menusep(file);
	testpanel(dtsgui, file);
	tree = dtsgui_treewindow(dtsgui, "Tree Window");
	dtsgui_newmenuitem(file, dtsgui, "T&ree", tree);
	dtsgui_newmenucb(file, dtsgui, "Test &Post", "Send POST request to callshop server (requires callshop user)", post_test, NULL);

	dtsgui_menusep(file);
	dtsgui_close(file, dtsgui);
	dtsgui_exit(file, dtsgui);
}

void help_menu(struct dtsgui *dtsgui) {
	dtsgui_menu help;
	dtsgui_pane p;

	help = dtsgui_newmenu(dtsgui, "&Help");

	p = dtsgui_textpane(dtsgui, "TEST", "Hello This is a text box");
	dtsgui_newmenuitem(help, dtsgui, "&Hello...\tCtrl-H", p);
	dtsgui_menusep(help);
	dtsgui_about(help, dtsgui, "This is a test application!!!!");
}

int guiconfig_cb(struct dtsgui *dtsgui, void *data) {
	struct app_data *appdata = data;

	if (!data || !objref(appdata)) {
		return 0;
	}

	/* menus*/
	file_menu(dtsgui);
	help_menu(dtsgui);

	objunref(appdata);
	return 1;

	/*load xml config via http*/
	if (!(appdata->xmldoc = dtsgui_loadxmlurl(dtsgui, "admin", "", "https://callshop.distrotech.co.za:666/cshop"))) {
		objunref(appdata);
		dtsgui_confirm(dtsgui, "Config DL Failed (as expected after 3 tries)\nHello Dave\n\nWould You Like To Play .... Thermo Nuclear War ?");
		return 1;
	}

	objunref(appdata);
	return 1;
}

void free_appdata(void *data) {
	struct app_data *appdata = data;

	if (appdata->datadir) {
		free((void*)appdata->datadir);
	}

	if (appdata->dtsgui) {
		objunref(appdata->dtsgui);
	}

	if (appdata->xmldoc) {
		objunref(appdata->xmldoc);
	}
}

int main(int argc, char **argv) {
	struct point wsize = {800, 600};
	struct point wpos = {50, 50};
	struct app_data *appdata;
	int res;
	char apppath[PATH_MAX];

	if (!(appdata = objalloc(sizeof(*appdata), free_appdata))) {
		return -1;
	}

#ifdef __WIN32
	getwin32folder(CSIDL_COMMON_APPDATA, apppath);
	appdata->datadir = malloc(strlen(apppath)+12);
	snprintf((char*)appdata->datadir, strlen(apppath)+12, "%s\\Distrotech", apppath);
	chdir(appdata->datadir);
	_putenv("XML_DEBUG_CATALOG=\"\"");
	_putenv("XML_CATALOG_FILES=catalog.xml");
#else
	appdata->datadir = strdup(DATA_DIR);
	snprintf(apppath, strlen(appdata->datadir)+13, "%s/catalog.xml", appdata->datadir);
	if (is_file(apppath)) {
		setenv("XML_CATALOG_FILES", apppath, 1);
	}
	setenv("XML_DEBUG_CATALOG", "", 0);
#endif

	startthreads();
	xml_init();
	xslt_init();

	appdata->dtsgui = dtsgui_config(guiconfig_cb, appdata, wsize, wpos, "Distrotech System App", "Welcome to Distrotech App!");
	res = dtsgui_run(argc, argv);
	objunref(appdata);

	xslt_close();
	xml_close();
	stopthreads();

	return res;
}
