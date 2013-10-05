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

void newappframe(struct dtsgui *dtsgui);

class DTSApp : public wxApp {
	public:
		~DTSApp();
		void CreateFrame(dtsgui_configcb confcallback_cb, void *data, struct point wsize, struct point wpos, const char *title, const char *status);
		virtual bool OnInit();
		void ShowFrame();
	private:
		int curl;
		struct dtsgui *dtsgui;
};

extern "C" {
	int curl_progress_function(void *data, double dltotal, double dlnow, double ultotal, double ulnow);
	void curl_progress_ctrl(void *data, int pause);
	void *curl_startprogress(void *data);
}

#endif // DTSAPP_H
