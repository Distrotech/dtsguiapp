#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#ifdef __WIN32
#define UNICODE 1
#include <winsock2.h>
#include <dirent.h>
#include <shlobj.h>
#endif

#include <dtsapp.h>

#include "dtsgui.h"
#include "private.h"

#ifndef DATA_DIR
#define DATA_DIR	"/usr/share/dtsguiapp"
#endif

dtsgui_pane pbx_settings(struct dtsgui *dtsgui, const char *title, void *data) {
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

	dtsgui_tabview tabv;
	struct app_data *appdata;
	dtsgui_pane dp[11], pg;
	struct form_item *lb;
	int cnt, i;

	appdata = dtsgui_userdata(dtsgui);
	tabv = dtsgui_tabwindow(dtsgui, title);

	dp[0] = dtsgui_addpage(tabv, "Routing", 0, NULL, appdata->xmldoc);
	dp[1] = dtsgui_addpage(tabv, "mISDN", 0, NULL, appdata->xmldoc);
	dp[2] = dtsgui_addpage(tabv, "E1", 0, NULL, appdata->xmldoc);
	dp[3] = dtsgui_addpage(tabv, "MFC/R2", 0, NULL, appdata->xmldoc);
	dp[4] = dtsgui_addpage(tabv, "Defaults", 0, NULL, appdata->xmldoc);
	dp[5] = dtsgui_addpage(tabv, "IVR Password", 0, NULL, appdata->xmldoc);
	dp[6] = dtsgui_addpage(tabv, "Location", 0, NULL, appdata->xmldoc);
	dp[7] = dtsgui_addpage(tabv, "Inbound", 0, NULL, appdata->xmldoc);
	dp[8] = dtsgui_addpage(tabv, "Num Plan", 0, NULL, appdata->xmldoc);
	dp[9] = dtsgui_addpage(tabv, "Auto Add", 0, NULL, appdata->xmldoc);
	dp[10] = dtsgui_addpage(tabv, "Save", wx_PANEL_BUTTON_YES, NULL, appdata->xmldoc);

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

	dtsgui_setevcallback(dp[10], NULL, &dp);

	cnt = sizeof(dp)/sizeof(dp[0]);

	for(i = 0; i < cnt;i++) {
		dtsgui_item_xmlcreate(dp[i], "/config/IP/VOIP/ASTDB", "Option", "option");
	}

	for(i = 0; i < cnt;i++) {
		dtsgui_showpanel(dp[i], !i);
	}

	return tabv;
}
