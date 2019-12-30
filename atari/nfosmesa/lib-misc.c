/*
	OSMesa LDG linker, misc functions

	Copyright (C) 2004	Patrice Mandin

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

/*--- Includes ---*/

#include <stdlib.h>
#include <string.h>

#include <mint/osbind.h>

#include "lib-osmesa.h"
#include "lib-oldmesa.h"
#include "nfosmesa_nfapi.h"

/*--- Defines ---*/

#define GL_VENDOR     0x1F00
#define GL_RENDERER   0x1F01
#define GL_VERSION    0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C

/*--- Variables ---*/

static const GLubyte empty_str[1];

/*--- Functions ---*/

const GLubyte* APIENTRY internal_glGetString(gl_private *priv, GLenum name)
{
	int i;
	size_t len;
	unsigned int c = CTX_TO_IDX(priv->cur_context);
	TRACE(("glGetString(priv=0x%x, name=0x%x)\n", (unsigned int)priv, name));
	if (c == 0)
		return empty_str;
	
	switch(name) {
		case GL_VERSION:
			i=1;
			break;
		case GL_RENDERER:
			i=2;
			break;
		case GL_VENDOR:
			i=3;
			break;
		case GL_EXTENSIONS:
			i=4;
			break;
		default:
			return empty_str;
	}

	if (priv->contexts[c].gl_strings[i]==NULL) {
		unsigned long params[3];
		
		params[0] = (unsigned long) priv->cur_context;
		params[1] = (unsigned long) name;

		len = (*HostCall_p)(NFOSMESA_LENGLGETSTRING, priv->cur_context, params);
		priv->contexts[c].gl_strings[i] = (GLubyte *)priv->pub.m_alloc(len+1);
		if (priv->contexts[c].gl_strings[i]) {
			params[0] = (unsigned long) priv->cur_context;
			params[1] = (unsigned long) name;
			params[2] = (unsigned long) priv->contexts[c].gl_strings[i];
			(*HostCall_p)(NFOSMESA_PUTGLGETSTRING, priv->cur_context, params);
		} else {
			return empty_str;
		}
	}

	return priv->contexts[c].gl_strings[i];
}

const GLubyte* APIENTRY internal_glGetStringi(gl_private *priv, GLenum name, GLuint index )
{
	int i;
	size_t len;
	unsigned int c = CTX_TO_IDX(priv->cur_context);
	TRACE(("glGetStringi(priv=0x%x, name=0x%x, index=%u)\n", (unsigned int)priv, name, index));
	if (c == 0)
		return empty_str;
	
	switch(name) {
		case GL_EXTENSIONS:
			i=5;
			break;
		case GL_SHADING_LANGUAGE_VERSION:
			i=6;
			break;
		default:
			return empty_str;
	}

	if (priv->contexts[c].gl_strings[i]!=NULL) {
		if (priv->pub.m_free)
			priv->pub.m_free(priv->contexts[c].gl_strings[i]);
		priv->contexts[c].gl_strings[i] = NULL;
	}
	{
		unsigned long params[4];
		
		params[0] = (unsigned long) priv->cur_context;
		params[1] = (unsigned long) name;
		params[2] = (unsigned long) index;

		len = (*HostCall_p)(NFOSMESA_LENGLGETSTRINGI, priv->cur_context, params);
		if (len < 0)
			return NULL;
		priv->contexts[c].gl_strings[i] = (GLubyte *)priv->pub.m_alloc(len+1);
		if (priv->contexts[c].gl_strings[i]) {
			params[0] = (unsigned long) priv->cur_context;
			params[1] = (unsigned long) name;
			params[2] = (unsigned long) index;
			params[3] = (unsigned long) priv->contexts[c].gl_strings[i];
			(*HostCall_p)(NFOSMESA_PUTGLGETSTRINGI, priv->cur_context, params);
		} else {
			return empty_str;
		}
	}

	return priv->contexts[c].gl_strings[i];
}

void freeglGetString(gl_private *priv, OSMesaContext ctx)
{
	int i;
	unsigned int c = CTX_TO_IDX(ctx);
	
	for (i=1;i<7;i++) {
		if (priv->contexts[c].gl_strings[i]) {
			if (priv->pub.m_free)
				priv->pub.m_free(priv->contexts[c].gl_strings[i]);
			priv->contexts[c].gl_strings[i]=NULL;
		}
	}
}

void APIENTRY internal_tinyglswapbuffer(gl_private *priv, void *buf)
{
	TRACE(("swapbuffer(priv=0x%x, 0x%x)\n", (unsigned int)priv, (unsigned int)buf));
	(*HostCall_p)(NFOSMESA_TINYGLSWAPBUFFER, priv->cur_context, &buf);
}


void APIENTRY internal_tinyglexception_error(gl_private *priv, void CALLBACK (*exception)(GLenum param))
{
	TRACE(("exception_error(priv=0x%x, exception=0x%x)\n", (unsigned int)priv, (unsigned int)exception));
	priv->gl_exception = exception;
}


int gl_exception_error(gl_private *priv, GLenum exception)
{
	if (priv->gl_exception)
	{
		(*priv->gl_exception)(exception);
		return 1;
	}
	return 0;
}


void gl_fatal_error(gl_private *priv, GLenum error, long except, const char *format)
{
	if (!gl_exception_error(priv, except))
	{
		static char const err[] = "TinyGL: fatal error: ";
		(void) Fwrite(2, sizeof(err) - 1, err);
		(void) Fwrite(2, strlen(format), format);
		(void) Fwrite(2, 2, "\r\n");
	}
}
