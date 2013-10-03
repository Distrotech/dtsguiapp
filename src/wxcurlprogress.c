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

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <dtsapp.h>

#include "dtsgui.h"

struct curlprog {
	int pd;
	int pause;
	void *data;
};

static void free_curlprog(void *data) {
	struct curlprog *curl_pd = data;

	if (curl_pd->data) {
		objunref(curl_pd->data);
	}
}

void *curl_startprogress(void *data) {
	struct curlprog *curl_pd;

	if (!(curl_pd = objalloc(sizeof(*curl_pd), free_curlprog))){
		return NULL;
	}
	curl_pd->pd = 0;
	if (data && objref(data)) {
		curl_pd->data = data;
	} else {
		curl_pd->data = NULL;
	}
	curl_pd->pause = 0;
	return curl_pd;
}

int curl_progress_function(void *data, double dltotal, double dlnow, double ultotal, double ulnow) {
	struct curlprog *curl_pd = data;
	struct dtsgui *dtsgui = curl_pd->data;
	int val, tot, cur;

	if (!curl_pd || curl_pd->pause) {
		return 0;
	}

	tot = dltotal+ultotal;
	cur = ulnow+dlnow;

	val = (tot && cur) ? (1000/tot) * cur : 0;
	val = ceil(val);

	if (!curl_pd->pd) {
		curl_pd->pd = dtsgui_progress_start(dtsgui, "Web Transfer Progress", 1000, 1);
	};

	if (curl_pd->pd && val && !dtsgui_progress_update(dtsgui, val, NULL)) {
		return 1;
	}

	return 0;
}

void curl_progress_ctrl(void *data, int pause) {
	struct curlprog *curl_pd = data;

	switch(pause) {
		case 0:
			curl_pd->pause = pause;
			break;
		case 1:
			dtsgui_progress_end(curl_pd->data);
			curl_pd->pause = pause;
			curl_pd->pd = 0;
			break;
		case -1:
			dtsgui_progress_end(curl_pd->data);
			objunref(curl_pd);
			break;
	}
}
