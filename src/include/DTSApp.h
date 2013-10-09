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

#ifndef DTSAPP_H
#define DTSAPP_H

class dtsgui {
	public:
		DTS_OJBREF_CLASS(dtsgui);
		dtsgui(const char *title, const char *stat, struct point w_size, struct point w_pos, dtsgui_configcb confcallback_cb , void *userdata);
		int SetupAPPFrame(void);
		void *GetUserData(void);
		void SetStatusText(void);
		void AppendTitle(const char *text);
		class DTSFrame *GetFrame(void);
		void ShowToolbar(bool show = true);
	private:
		void *userdata;
		class DTSFrame *appframe;
		const char *title;
		const char *status;
		dtsgui_configcb cb;
		struct point wsize;
		struct point wpos;
};

class dtsgui_curl {
	public:
		DTS_OJBREF_CLASS(dtsgui_curl);
		dtsgui_curl(class dtsgui *dtsgui);
		void Close();
		DTSFrame *GetFrame();
	private:
		static struct basic_auth *CurlPasswd(const char *user, const char *passwd, void *data);
		static void *curl_startprogress(void *data);
		static void curl_progress_ctrl(void *data, int pause);
		static int curl_progress_function(void *data, double dltotal, double dlnow, double ultotal, double ulnow);
		class DTSFrame *frame;
		int curl;
};

class curl_progress {
	public:
		DTS_OJBREF_CLASS(curl_progress);
		curl_progress(class dtsgui_curl *dc);
		DTSFrame *GetFrame();
		int pd;
		int pause;
	private:
		class dtsgui_curl *owner;
};

class DTSApp : public wxApp {
	public:
		DTSApp(dtsgui_configcb confcallback_cb, void *data, struct point wsize, struct point wpos, const char *title, const char *status);
		~DTSApp();
	private:
		virtual bool OnInit();
		class dtsgui *dtsgui;
		class dtsgui_curl *curl;
};

#endif // DTSAPP_H
