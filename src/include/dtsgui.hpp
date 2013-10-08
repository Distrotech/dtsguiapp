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

#if !defined(DTSGUI_HPP_INCLUDED) && defined(__cplusplus)
#define DTSGUI_HPP_INCLUDED

#define DTS_OJBREF_CLASS(classtype)	void *operator new(size_t sz) {\
			return objalloc(sz, &classtype::unref);\
		}\
		void operator delete(void *obj) {\
		}\
		static void unref(void *data) {\
			delete (classtype*)data;\
		}\
		~classtype()


wxDECLARE_EVENT(DTS_APP_EVENT, wxCommandEvent);

#endif // DTSGUI_HPP_INCLUDED
