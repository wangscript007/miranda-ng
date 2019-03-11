/*
	Variables Plugin for Miranda-IM (www.miranda-im.org)
	Copyright 2003-2006 P. Boon

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "stdafx.h"

static wchar_t* parseAnd(ARGUMENTSINFO *ai)
{
	if (ai->argc < 3)
		return nullptr;

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	for (unsigned i = 1; i < ai->argc; i++) {
		fi.szFormat.w = ai->argv.w[i];
		mir_free(formatString(&fi));

		if (fi.eCount > 0) {
			ai->flags |= AIF_FALSE;
			return mir_wstrdup(L"");
		}
	}

	return mir_wstrdup(L"");
}

static wchar_t* parseFalse(ARGUMENTSINFO *ai)
{
	if (ai->argc != 1)
		return nullptr;

	ai->flags |= AIF_FALSE;
	return mir_wstrdup(L"");
}

static wchar_t* parseIf(ARGUMENTSINFO *ai)
{
	if (ai->argc != 4)
		return nullptr;

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	fi.eCount = fi.pCount = 0;
	fi.szFormat.w = ai->argv.w[1];
	mir_free(formatString(&fi));

	return mir_wstrdup((fi.eCount == 0) ? ai->argv.w[2] : ai->argv.w[3]);
}

static wchar_t* parseIf2(ARGUMENTSINFO *ai)
{
	if (ai->argc != 3)
		return nullptr;

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	fi.eCount = fi.pCount = 0;
	fi.szFormat.w = ai->argv.w[1];
	wchar_t *szCondition = formatString(&fi);
	if (fi.eCount == 0)
		return szCondition;

	mir_free(szCondition);
	return mir_wstrdup(ai->argv.w[2]);
}

static wchar_t* parseIf3(ARGUMENTSINFO *ai)
{
	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	for (unsigned i = 1; i < ai->argc; i++) {
		fi.eCount = fi.pCount = 0;
		fi.szFormat.w = ai->argv.w[i];
		wchar_t *szCondition = formatString(&fi);
		if (fi.eCount == 0)
			return szCondition;

		mir_free(szCondition);
	}

	return nullptr;
}

static wchar_t* parseIfequal(ARGUMENTSINFO *ai)
{
	if (ai->argc != 5)
		return nullptr;

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	fi.szFormat.w = ai->argv.w[1];
	ptrW tszFirst(formatString(&fi));
	fi.szFormat.w = ai->argv.w[2];
	ptrW tszSecond(formatString(&fi));
	if (tszFirst == NULL || tszSecond == NULL)
		return nullptr;

	if (_wtoi(tszFirst) == _wtoi(tszSecond))
		return mir_wstrdup(ai->argv.w[3]);

	return mir_wstrdup(ai->argv.w[4]);
}

static wchar_t* parseIfgreater(ARGUMENTSINFO *ai)
{
	if (ai->argc != 5)
		return nullptr;

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	fi.szFormat.w = ai->argv.w[1];
	ptrW tszFirst(formatString(&fi));
	fi.szFormat.w = ai->argv.w[2];
	ptrW tszSecond(formatString(&fi));
	if (tszFirst == NULL || tszSecond == NULL)
		return nullptr;

	if (_wtoi(tszFirst) > _wtoi(tszSecond))
		return mir_wstrdup(ai->argv.w[3]);

	return mir_wstrdup(ai->argv.w[4]);
}

static wchar_t* parseIflonger(ARGUMENTSINFO *ai)
{
	if (ai->argc != 5)
		return nullptr;

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	fi.szFormat.w = ai->argv.w[1];
	ptrW tszFirst(formatString(&fi));
	fi.szFormat.w = ai->argv.w[2];
	ptrW tszSecond(formatString(&fi));
	if (tszFirst == NULL || tszSecond == NULL)
		return nullptr;

	if (mir_wstrlen(tszFirst) > mir_wstrlen(tszSecond))
		return mir_wstrdup(ai->argv.w[3]);

	return mir_wstrdup(ai->argv.w[4]);
}

/*

  ?for(init, cond, incr, show)

  */
static wchar_t* parseFor(ARGUMENTSINFO *ai)
{
	if (ai->argc != 5)
		return nullptr;

	wchar_t *res = mir_wstrdup(L"");

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	fi.eCount = fi.pCount = 0;
	fi.szFormat.w = ai->argv.w[1];
	mir_free(formatString(&fi));
	fi.szFormat.w = ai->argv.w[2];
	mir_free(formatString(&fi));
	while (fi.eCount == 0) {
		fi.szFormat.w = ai->argv.w[4];
		wchar_t *parsed = formatString(&fi);
		if (parsed != nullptr) {
			if (res == nullptr) {
				res = (wchar_t*)mir_alloc(mir_wstrlen(parsed) + 1 * sizeof(wchar_t));
				if (res == nullptr) {
					mir_free(parsed);
					return nullptr;
				}
			}
			else res = (wchar_t*)mir_realloc(res, (mir_wstrlen(res) + mir_wstrlen(parsed) + 1) * sizeof(wchar_t));

			mir_wstrcat(res, parsed);
			mir_free(parsed);
		}
		fi.szFormat.w = ai->argv.w[3];
		mir_free(formatString(&fi));
		fi.eCount = 0;
		fi.szFormat.w = ai->argv.w[2];
		mir_free(formatString(&fi));
	}

	return res;
}

static wchar_t* parseEqual(ARGUMENTSINFO *ai)
{
	if (ai->argc != 3)
		return nullptr;

	if (_wtoi(ai->argv.w[1]) != _wtoi(ai->argv.w[2]))
		ai->flags |= AIF_FALSE;

	return mir_wstrdup(L"");
}

static wchar_t* parseGreater(ARGUMENTSINFO *ai)
{
	if (ai->argc != 3)
		return nullptr;

	if (_wtoi(ai->argv.w[1]) <= _wtoi(ai->argv.w[2]))
		ai->flags |= AIF_FALSE;

	return mir_wstrdup(L"");
}

static wchar_t* parseLonger(ARGUMENTSINFO *ai)
{
	if (ai->argc != 3)
		return nullptr;

	if (mir_wstrlen(ai->argv.w[1]) <= mir_wstrlen(ai->argv.w[2]))
		ai->flags |= AIF_FALSE;

	return mir_wstrdup(L"");
}

static wchar_t* parseNot(ARGUMENTSINFO *ai)
{
	if (ai->argc != 2) {
		return nullptr;
	}

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	fi.szFormat.w = ai->argv.w[1];
	mir_free(formatString(&fi));

	if (fi.eCount == 0)
		ai->flags |= AIF_FALSE;

	return mir_wstrdup(L"");
}

static wchar_t* parseOr(ARGUMENTSINFO *ai)
{
	if (ai->argc < 2)
		return nullptr;

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	ai->flags |= AIF_FALSE;
	for (unsigned i = 1; (i < ai->argc) && (ai->flags&AIF_FALSE); i++) {
		fi.szFormat.w = ai->argv.w[i];
		fi.eCount = 0;
		mir_free(formatString(&fi));

		if (fi.eCount == 0)
			ai->flags &= ~AIF_FALSE;
	}

	return mir_wstrdup(L"");
}

static wchar_t* parseTrue(ARGUMENTSINFO *ai)
{
	return (ai->argc != 1) ? nullptr : mir_wstrdup(L"");
}

static wchar_t* parseXor(ARGUMENTSINFO *ai)
{
	if (ai->argc != 3)
		return nullptr;

	FORMATINFO fi;
	memcpy(&fi, ai->fi, sizeof(fi));
	ai->flags = AIF_FALSE;
	fi.szFormat.w = ai->argv.w[0];
	mir_free(formatString(&fi));
	int val1 = fi.eCount == 0;

	fi.szFormat.w = ai->argv.w[1];
	mir_free(formatString(&fi));
	int val2 = fi.eCount == 0;

	ai->flags |= ((val1 & AIF_FALSE) == !(val2 & AIF_FALSE)) ? 0 : AIF_FALSE;
	return mir_wstrdup(L"");
}

void registerLogicTokens()
{
	registerIntToken(AND, parseAnd, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y, ...)\t" LPGEN("performs logical AND (x && y && ...)"));
	registerIntToken(STR_FALSE, parseFalse, TRF_FIELD, LPGEN("Logical Expressions") "\t" LPGEN("boolean FALSE"));
	registerIntToken(FOR, parseFor, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(w,x,y,z)\t" LPGEN("performs w, then shows z and performs y while x is TRUE"));
	registerIntToken(IF, parseIf, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y,z)\t" LPGEN("shows y if x is TRUE, otherwise it shows z"));
	registerIntToken(IF2, parseIf2, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y)\t" LPGEN("shows x if x is TRUE, otherwise it shows y (if(x,x,y))"));
	registerIntToken(IF3, parseIf3, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y, ...)\t" LPGEN("the first argument parsed successfully"));
	registerIntToken(IFEQUAL, parseIfequal, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(w,x,y,z)\t" LPGEN("y if w = x, else z"));
	registerIntToken(IFGREATER, parseIfgreater, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(w,x,y,z)\t" LPGEN("y if w > x, else z"));
	registerIntToken(IFLONGER, parseIflonger, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(w,x,y,z)\t" LPGEN("y if string length of w > x, else z"));
	registerIntToken(EQUAL, parseEqual, TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y)\t" LPGEN("TRUE if x = y"));
	registerIntToken(GREATER, parseGreater, TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y)\t" LPGEN("TRUE if x > y"));
	registerIntToken(LONGER, parseLonger, TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y)\t" LPGEN("TRUE if x is longer than y"));
	registerIntToken(NOT, parseNot, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x)\t" LPGEN("performs logical NOT (!x)"));
	registerIntToken(OR, parseOr, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y,...)\t" LPGEN("performs logical OR (x || y || ...)"));
	registerIntToken(STR_TRUE, parseTrue, TRF_FIELD, LPGEN("Logical Expressions") "\t" LPGEN("boolean TRUE"));
	registerIntToken(XOR, parseXor, TRF_UNPARSEDARGS | TRF_FUNCTION, LPGEN("Logical Expressions") "\t(x,y)\t" LPGEN("performs logical XOR (x ^ y)"));
}
