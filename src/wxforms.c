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
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <dtsapp.h>

#include "dtsgui.h"

#ifndef DATA_DIR
#define DATA_DIR	"/usr/share/dtsguiapp"
#endif // DATA_DIR

struct app_data {
	struct dtsgui *dtsgui;
	struct xml_doc *xmldoc;
};

void xml_config(struct xml_doc *xmldoc) {
	void *xmlbuf;

	xmlbuf = xml_doctobuffer(xmldoc);
	printf("%s\n", xml_getbuffer(xmlbuf));
	objunref(xmlbuf);
}

struct xml_doc *loadxmlconf(struct dtsgui *dtsgui) {
	struct basic_auth *auth;
	struct curlbuf *cbuf;
	struct xml_doc *xmldoc = NULL;

	auth = dtsgui_pwdialog("admin", "", dtsgui);
	if (!(cbuf = curl_geturl("https://callshop.distrotech.co.za:666/cshop", auth, dtsgui_pwdialog, dtsgui))) {
		objunref(auth);
		return NULL;
	}

	curl_ungzip(cbuf);

	if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
		xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize, 1);
	}

	objunref(cbuf);
	objunref(auth);
	return xmldoc;
}

int system_wizard(struct dtsgui *dtsgui, void *data, const char *filename, struct xml_doc *xmldoc) {
	const char *cos[] = {"Internal Extensions", "Local PSTN", "Long Distance PSTN", "Cellular", "Premium", "International"};
	const char *cosv[] = {"0", "1", "2", "3", "4", "5"};
	dtsgui_pane dp[12], pg;
	struct dtsgui_wizard *twiz;
	struct form_item *ilist;
	const char *newfile;
	int res, cnt;

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
	dtsgui_xmltextbox(pg, "Customer Name", "/config/Customer/Option[@option = 'Name']", NULL);
	dtsgui_xmltextbox(pg, "Contact Person", "/config/Customer/Option[@option = 'Contact']", NULL);
	dtsgui_xmltextbox(pg, "Email Address", "/config/Customer/Option[@option = 'Email']", NULL);
	dtsgui_xmltextbox_multi(pg, "Customer Address (Postal)", "/config/Customer/Option[@option = 'Postal Addr']", NULL);
	dtsgui_xmltextbox_multi(pg, "Customer Address (Physical)", "/config/Customer/Option[@option = 'Phys Addr']", NULL);
	dtsgui_xmltextbox(pg, "Customer Tel", "/config/Customer/Option[@option = 'Tel']", NULL);
	dtsgui_xmltextbox(pg, "Customer Company ID", "/config/Customer/Option[@option = 'ID']", NULL);
	dtsgui_xmltextbox(pg, "VAT No.", "/config/Customer/Option[@option = 'VAT']", NULL);

	pg=dp[1];
	dtsgui_xmltextbox(pg, "IPv4 Address", "/config/IP/Interfaces/Interface[/config/IP/SysConf/Option[@option = 'Internal'] = .]", "ipaddr");
	dtsgui_xmltextbox(pg, "IPv4 Subnet Length", "/config/IP/Interfaces/Interface[/config/IP/SysConf/Option[@option = 'Internal'] = .]", "subnet");
	dtsgui_xmltextbox(pg, "Domain Name", "/config/DNS/Config/Option[@option = 'Domain']", NULL);
	dtsgui_xmltextbox(pg, "Default Gateway", "/config/IP/SysConf/Option[@option = 'Nexthop']", NULL);
	dtsgui_xmltextbox(pg, "SMTP Gateway", "/config/Email/Config/Option[@option = 'Smarthost']", NULL);
	dtsgui_xmltextbox(pg, "NTP Server", "/config/IP/SysConf/Option[@option = 'NTPServer']", NULL);

	pg=dp[2];
	dtsgui_xmltextbox(pg, "Dynamic DNS Server", "/config/DNS/Config/Option[@option = 'DynServ']", NULL);
	dtsgui_xmltextbox(pg, "Dynamic DNS Zone", "/config/DNS/Config/Option[@option = 'DynZone']", NULL);
	dtsgui_xmltextbox(pg, "Dynamic DNS KEY", "/config/DNS/Config/Option[@option = 'DynKey']", NULL);

	pg=dp[3];
	dtsgui_xmltextbox(pg, "Primary DNS", "/config/IP/SysConf/Option[@option = 'PrimaryDns']", NULL);
	dtsgui_xmltextbox(pg, "Secondary DNS", "/config/IP/SysConf/Option[@option = 'SecondaryDns']", NULL);
	dtsgui_xmltextbox(pg, "Primary WINS", "/config/IP/SysConf/Option[@option = 'PrimaryWins']", NULL);
	dtsgui_xmltextbox(pg, "Secondary WINS", "/config/IP/SysConf/Option[@option = 'SecondaryWins']", NULL);
	dtsgui_xmltextbox(pg, "Primary MX", "/config/Email/Config/Option[@option = 'MailExchange1']", NULL);
	dtsgui_xmltextbox(pg, "Secondary MX", "/config/Email/Config/Option[@option = 'MailExchange2']", NULL);

	pg=dp[4];
	dtsgui_xmltextbox(pg, "Country Code", "/config/X509/Option[@option = 'Country']", NULL);
	dtsgui_xmltextbox(pg, "Province/State", "/config/X509/Option[@option = 'State']", NULL);
	dtsgui_xmltextbox(pg, "City", "/config/X509/Option[@option = 'City']", NULL);
	dtsgui_xmltextbox(pg, "Company", "/config/X509/Option[@option = 'Company']", NULL);
	dtsgui_xmltextbox(pg, "Division", "/config/X509/Option[@option = 'Division']", NULL);
	dtsgui_xmltextbox(pg, "Name", "/config/X509/Option[@option = 'Name']", NULL);
	dtsgui_xmltextbox(pg, "Email", "/config/X509/Option[@option = 'Email']", NULL);

	pg=dp[5];
	dtsgui_xmltextbox(pg, "Workgroup/Domain", "/config/FileServer/Setup/Option[@option = 'Domain']", NULL);
	dtsgui_xmltextbox(pg, "Aliases", "/config/FileServer/Config/Option[@option = 'netbios name']", NULL);
	dtsgui_xmltextbox(pg, "Domain Controllers", "/config/FileServer/Setup/Option[@option = 'ADSServer']", NULL);
	dtsgui_xmltextbox(pg, "Realm [If Joining ADS]", "/config/FileServer/Setup/Option[@option = 'ADSRealm']", NULL);
	dtsgui_xmlcheckbox(pg, "Domain Controller", NULL, NULL, NULL, NULL);

	pg=dp[6];
	/*XXX USE TMP VAL and set later unckeced could be 3G... ext int/pppoe*/
	ilist = dtsgui_xmlcombobox(pg, "External Interface", NULL, NULL);
	dtsgui_listbox_add(ilist, "br0", NULL);
	dtsgui_listbox_add(ilist, "ethB", NULL);
	dtsgui_listbox_add(ilist, "br0.100", NULL);
	dtsgui_listbox_add(ilist, "3G", NULL);
	objunref(ilist);
	dtsgui_xmlcheckbox(pg, "External Device Is PPPoE", "ADSL", "Dialup", "/config/IP/Dialup/Option[@option = 'Connection']", NULL);
	dtsgui_xmltextbox(pg, "Number/Service/APN", "/config/IP/Dialup/Option[@option = 'Number']", NULL);
	dtsgui_xmltextbox(pg, "Username", "/config/IP/Dialup/Option[@option = 'Username']", NULL);
	dtsgui_xmltextbox(pg, "Password", "/config/IP/Dialup/Option[@option = 'Password']", NULL);
	dtsgui_xmltextbox(pg, "MTU", "/config/IP/Dialup/Option[@option = 'MTU']", NULL);


	pg=dp[7];
	ilist = dtsgui_xmllistbox(pg, "Default Extension Permision", "/config/IP/VOIP/ASTDB/Option[@option = 'Context']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(ilist, cos[cnt], cosv[cnt]);
	}
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "Default Auth Extension Permision", "/config/IP/VOIP/ASTDB/Option[@option = 'AuthContext']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(ilist, cos[cnt], cosv[cnt]);
	}
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "Default After Hours Extension Permision", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFALOCK']", NULL);
	for(cnt=0;cnt <= 5; cnt++) {
		dtsgui_listbox_add(ilist, cos[cnt], cosv[cnt]);
	}
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "Snom Network Port Config", "/config/IP/VOIP/ASTDB/Option[@option = 'SnomNet']", NULL);
	dtsgui_listbox_add(ilist, "100Mb/s Full Duplex", "100full");
	dtsgui_listbox_add(ilist, "100Mb/s Half Duplex", "100half");
	dtsgui_listbox_add(ilist, "10Mb/s Full Duplex", "10full");
	dtsgui_listbox_add(ilist, "10Mb/s Half Duplex", "10half");
	dtsgui_listbox_add(ilist, "Auto", "auto");
	objunref(ilist);
	dtsgui_xmltextbox(pg, "Voip Vlan Interface", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoVLAN']", NULL);
	dtsgui_xmltextbox(pg, "Default Extension Prefix", "/config/IP/VOIP/ASTDB/Option[@option = 'DefaultPrefix']", NULL);
	dtsgui_xmltextbox(pg, "Auto Config Start", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoStart']", NULL);
	dtsgui_xmltextbox(pg, "Auto Config End", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoEnd']", NULL);
	dtsgui_xmltextbox(pg, "Default Ring Timeout", "/config/IP/VOIP/ASTDB/Option[@option = 'Timeout']", NULL);
	dtsgui_xmlcheckbox(pg, "Require Authorization", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoAuth']", NULL);
	dtsgui_xmlcheckbox(pg, "Lock Snom Phone Settings", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoLock']", NULL);
	dtsgui_xmlcheckbox(pg, "Enable Voice Mail", "0", "1", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFNOVMAIL']", NULL);
	dtsgui_xmlcheckbox(pg, "Enable Call Logging", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'DEFRECORD']", NULL);
	dtsgui_xmlcheckbox(pg, "Require Extension Number With PIN", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'ADVPIN']", NULL);

	pg=dp[8];
	dtsgui_xmltextbox(pg, "Local Area Code", "/config/IP/VOIP/ASTDB/Option[@option = 'AreaCode']", NULL);
	dtsgui_xmltextbox(pg, "Local Extension Prefix", "/config/IP/VOIP/ASTDB/Option[@option = 'ExCode']", NULL);
	ilist = dtsgui_xmllistbox(pg, "ISDN PRI Framing [T1-E1]", "/config/IP/VOIP/ASTDB/Option[@option = 'PRIframing']", NULL);
	dtsgui_listbox_add(ilist, "ccs - esf", "ccs");
	dtsgui_listbox_add(ilist, "cas - d4/Superframe", "cas");
	objunref(ilist);
	ilist = dtsgui_xmllistbox(pg, "ISDN PRI Coding [T1-E1]", "/config/IP/VOIP/ASTDB/Option[@option = 'PRIcoding']", NULL);
	dtsgui_listbox_add(ilist, "hdb3 - b8zs", "hdb3");
	dtsgui_listbox_add(ilist, "ami", "ami");
	objunref(ilist);
	dtsgui_xmlcheckbox(pg, "Calls To Internal Extensions Follow Forward Rules", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'LocalFwd']", NULL);
	dtsgui_xmlcheckbox(pg, "Hangup Calls To Unknown Numbers/DDI", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'UNKDEF']", NULL);
	dtsgui_xmlcheckbox(pg, "ISDN BRI Immeadiate Routeing (No MSN/DDI)", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNimm']", NULL);
	dtsgui_xmlcheckbox(pg, "ISDN BRI Use Round Robin Routing", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'mISDNrr']", NULL);
	dtsgui_xmlcheckbox(pg, "ISDN Allow Automatic Setting Of CLI (DDI Required)", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AutoCLI']", NULL);
	dtsgui_xmlcheckbox(pg, "ISDN PRI CRC4 Checking (E1 Only)", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'PRIcrc4']", NULL);

	/*XXXX PRI CONFIG LBO*/
	pg=dp[9];
	dtsgui_xmltextbox(pg, "Queue Timeout Checked Every 18s", "/config/IP/VOIP/ASTDB/Option[@option = 'AATimeout']", NULL);
	dtsgui_xmltextbox(pg, "Auto Attendant Mailbox/Forward On No Agent/Timeout", "/config/IP/VOIP/ASTDB/Option[@option = 'AANext']", NULL);
	dtsgui_xmltextbox(pg, "IVR Delay Between Digits", "/config/IP/VOIP/ASTDB/Option[@option = 'AADelay']", NULL);
	dtsgui_xmlcheckbox(pg, "Disable Default Auto Attendant Prompts", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AANOPROMPT']", NULL);
	dtsgui_xmlcheckbox(pg, "Music On Hold When Calling Reception", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AAMOH']", NULL);
	dtsgui_xmlcheckbox(pg, "Record Inbound Calls", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'AAREC']", NULL);

	pg=dp[10];
	ilist = dtsgui_xmllistbox(pg, "Primary Trunk", NULL, NULL);
	dtsgui_listbox_add(ilist, "None", NULL);
	dtsgui_listbox_add(ilist, "ISDN PRI", NULL);
	dtsgui_listbox_add(ilist, "ISDN BRI", NULL);
	dtsgui_listbox_add(ilist, "Analogue", NULL);
	objunref(ilist);
	dtsgui_xmltextbox(pg, "No. Of Trunk Ports", NULL, NULL);
	ilist = dtsgui_xmllistbox(pg, "Gateway Trunk", NULL, NULL);
	dtsgui_listbox_add(ilist, "None", NULL);
	dtsgui_listbox_add(ilist, "ISDN PRI", NULL);
	dtsgui_listbox_add(ilist, "ISDN BRI", NULL);
	dtsgui_listbox_add(ilist, "Analogue", NULL);
	objunref(ilist);
	dtsgui_xmltextbox(pg, "No. Of Gateway Ports", NULL, NULL);
	dtsgui_xmlcheckbox(pg, "Extensions Are On Gateway By Default", "1", "0", "/config/IP/VOIP/ASTDB/Option[@option = 'REMDEF']", NULL);

	pg=dp[11];
	ilist = dtsgui_xmllistbox(pg, "Protocol", "/config/IP/VOIP", "protocol");
	dtsgui_listbox_add(ilist, "SIP", "SIP");
	dtsgui_listbox_add(ilist, "IAX2", "IAX2");
	objunref(ilist);
	dtsgui_xmlcheckbox(pg, "Use DTMF INFO (SIP)", "info", "rfc2833", "/config/IP/VOIP", "dtmf");
	dtsgui_xmlcheckbox(pg, "Use SRTP (SIP)", "true", "false", "/config/IP/VOIP", "srtp");
	dtsgui_xmlcheckbox(pg, "Use From User (SIP [Disables Sending CLI])", "true", "false", "/config/IP/VOIP", "fromuser");
	dtsgui_xmlcheckbox(pg, "Disable Video", "true", "false", "/config/IP/VOIP", "novideo");
	dtsgui_xmlcheckbox(pg, "Register To Server", "true", "false", "/config/IP/VOIP", "register");

	res = dtsgui_runwizard(twiz);

	if (res) {
		cnt = sizeof(dp)/sizeof(dp[0])-1;
		for(; cnt >= 0;cnt--) {
			dtsgui_xmlpanel_update(dp[cnt]);
		}
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
	const char defconf[PATH_MAX];
	struct xml_doc *xmldoc;

	snprintf((char*)defconf, PATH_MAX-1, "%s/default.xml", DATA_DIR);
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

	dtsgui_textbox(p, "TextBox", "Text", NULL);
	dtsgui_textbox_multi(p, "Multi Line", "L\nL\n", NULL);
	dtsgui_checkbox(p, "Checkbox", 1, NULL);

	l = dtsgui_combobox(p, "ComboBox", NULL);
	dtsgui_listbox_add(l, "Test1", NULL);
	dtsgui_listbox_add(l, "Test2", NULL);
	dtsgui_listbox_add(l, "Test3", NULL);
	dtsgui_listbox_add(l, "Alt Test3", NULL);
	objunref(l);

	l = dtsgui_listbox(p, "ListBox", NULL);
	dtsgui_listbox_add(l, "Test1", NULL);
	dtsgui_listbox_add(l, "Test2", NULL);
	dtsgui_listbox_add(l, "Test3", NULL);
	dtsgui_listbox_add(l, "Alt Test3", NULL);
	objunref(l);

	dtsgui_newmenuitem(menu, dtsgui, "&Test", p);
}

void file_menu(struct dtsgui *dtsgui) {
	dtsgui_menu file;
	dtsgui_treeview tree;

	file = dtsgui_newmenu(dtsgui, "&File");

	dtsgui_newmenucb(file, dtsgui, "&New System", "New System Configuration Wizard", newsys_wizard, NULL);
	dtsgui_newmenucb(file, dtsgui, "&Edit Saved System", "Reconfigure Saved System File", editsys_wizard, NULL);
	dtsgui_menusep(file);

	testpanel(dtsgui, file);
	tree = dtsgui_treewindow(dtsgui, "Tree Window");
	dtsgui_newmenuitem(file, dtsgui, "T&ree", tree);

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

	return 1;

	/*load xml config via http*/
	if (!(appdata->xmldoc = loadxmlconf(dtsgui))) {
		objunref(appdata);
		dtsgui_confirm(dtsgui, "Config DL Failed (as expected after 3 tries)\nHello Dave\n\nWould You Like To Play .... Thermo Nuclear War ?");
		return 1;
	}

	return 1;
}

void free_appdata(void *data) {
	struct app_data *appdata = data;

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
	const char xmlcat[PATH_MAX];

	if (!(appdata = objalloc(sizeof(*appdata), free_appdata))) {
		return -1;
	}

	snprintf((char*)xmlcat, PATH_MAX-1, "%s/catalog.xml", DATA_DIR);
	if (is_file(xmlcat)) {
		setenv("XML_CATALOG_FILES", xmlcat, 1);
	}

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
