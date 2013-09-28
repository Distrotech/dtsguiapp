/*  Distrotech Solutions wxWidgets Gui Interface
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
#include <string.h>

#include <dtsapp.h>
#include "dtsgui.h"

struct form_result {
	const char *name;
	const char *keyval;
	const enum widget_type type;
	struct xml_node *node;
	void *widget;
};

struct xmlform {
	struct xml_search *xsearch;
	const char *attrkey;
	const char *xpath;
	struct xml_doc *xmldoc;
};

static int results_hash(const void *data, int key) {
	int ret = 0;
	const struct form_result *fr = data;
	const char *hashkey = (key) ? (const char *)data : fr->name;

	if (hashkey) {
		ret = jenhash(hashkey, strlen(hashkey), 0);
	}

	return(ret);
}

static void free_result(void *data) {
	struct form_result *fr = data;

	if (fr->name) {
		free((char *)fr->name);
	}

	if (fr->keyval) {
		free((char *)fr->keyval);
	}

	if (fr->node) {
		objunref(fr->node);
	}
}

extern struct form_result *createresult(struct xml_search *xsearch, const char *node, const char *attr, const char **value) {
	struct form_result *result;

	result = objalloc(sizeof(*result), free_result);

	if (node) {
		if (!attr) {
			ALLOC_CONST(result->keyval, node);
		};

		result->node = xml_getnode(xsearch, node);
	} else {
		result->node = xml_getfirstnode(xsearch, NULL);
	}

	if (result->node && attr) {
		*value = xml_getattr(result->node, attr);
	} else if (result->node) {
		*value = result->node->value;
	} else {
		*value = NULL;
	}

	ALLOC_CONST(result->name, attr);
//        addtobucket(fbox->results, result);
	return result;
}

extern struct bucket_list *form_results(void) {
	return create_bucketlist(0, results_hash);
}

void delete_node(struct bucket_list *results, const char *akey) {
	struct form_result *root, *tnode;

	if (results && akey) {
		root = bucket_list_find_key(results, akey);
		tnode = bucket_list_find_key(results, NULL);

		if (tnode && root && tnode->node) {
			xml_delete(tnode->node);
		}

		objunref(root);
		objunref(tnode);
	}
}

