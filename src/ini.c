// ini.c -- .ini file loader & parser
// Author: xWhitey

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ini.h"

#ifdef _WIN32
#define Q_strcasecmp _stricmp
#else
#include <strings.h>
#define Q_strcasecmp strcasecmp
#endif

/*
================
INI_Strdup

strdup isn't ISO C
================
*/
static char* INI_Strdup(const char* _Source)
{
	size_t	cbLength;
	char*	pCopy;

	if (!_Source)
		return NULL;

	cbLength = strlen(_Source) + 1;
	pCopy = (char*)malloc(cbLength);

	if (!pCopy)
		return NULL;

	memcpy(pCopy, _Source, cbLength);

	return pCopy;
}

/*
================
INI_MemFGets

fgets for an in-memory buffer.
Returns _Buffer or NULL if there's nothing left to read.
================
*/
static char* INI_MemFGets(const char* _Contents, size_t _Size, size_t* _Position, char* _Buffer, size_t _BufferSize)
{
	size_t cbIndex = 0;

	if (!_Contents || !_Position || *_Position >= _Size || _BufferSize == 0)
		return NULL;

	while (*_Position < _Size && cbIndex < _BufferSize - 1)
	{
		char cCurrent = _Contents[*_Position];

		(*_Position)++;

		if (cCurrent == '\n' || cCurrent == 0)
			break;

		_Buffer[cbIndex++] = cCurrent;
	}

	_Buffer[cbIndex] = 0;

	return _Buffer;
}

/*
================
INI_KVErrorToString

Ugly function
================
*/
const char* INI_KVErrorToString(inikvloadstat_t _Stat)
{
	switch (_Stat)
	{
	case ikvls_null_input:
		return "ikvls_null_input";
	case ikvls_missing_equals:
		return "ikvls_missing_equals";
	case ikvls_success:
		return "ikvls_success";
	case ikvls_oom:
		return "ikvls_oom";
	case ikvls_empty_key:
		return "ikvls_empty_key";
	}

	return "UNKNOWN ERROR";
}

/*
================
INI_ParseKV

Parse a .ini key value pair.

_FromWhat must be the line to parse itself,
not the file contents.

_OutKV's "key" and "value" fields are filled
by this function, and they are a copy
of the actual string, so they have to be freed
whenever the .ini is not needed anymore.
================
*/
inikvloadstat_t INI_ParseKV(const char* _FromWhat, inikv_t* _OutKV, inilogfunc_t _LogFunc)
{
	size_t	cbIndex;
	char*	lpszWhatCopy;
	char*	lpszPosition;
	char*	lpszValue;

	if (!_FromWhat || !_OutKV)
		return ikvls_null_input;

	lpszWhatCopy = INI_Strdup(_FromWhat);

	if (!lpszWhatCopy)
		return ikvls_oom;

	// remove the comments FIRST if there are any
	lpszPosition = strstr(lpszWhatCopy, ";");

	if (lpszPosition)
		lpszPosition[0] = 0;

	// find out whether there's an "equals" sign
	lpszPosition = strstr(lpszWhatCopy, "=");

	// if not = cock out
	if (!lpszPosition)
	{
		free(lpszWhatCopy);
		return ikvls_missing_equals;
	}

	// sniff the value
	lpszValue = lpszPosition;

	lpszValue++; // skip over the "equals" sign

	// cleanup preceding whitespace
	while (lpszValue[0] == ' ' || lpszValue[0] == '\t')
		lpszValue++;

	_OutKV->value = INI_Strdup(lpszValue);

	// now sniff the key
	cbIndex = (size_t)(lpszPosition - lpszWhatCopy);
	lpszWhatCopy[cbIndex] = 0; // so we don't go past the equals sign

	while (cbIndex > 0)
	{
		if (lpszWhatCopy[cbIndex - 1] == ' ' || lpszWhatCopy[cbIndex - 1] == '\t')	// clear out succeeding (leading) whitespace here as well
		{
			lpszWhatCopy[cbIndex - 1] = 0;	// Nuke shi
			cbIndex--;	// moonwalk
		}
		else
		{
			break;
		}
	}

	if (strlen(lpszWhatCopy) == 0)
	{
		free(lpszWhatCopy);
		free(_OutKV->value);
		_OutKV->value = NULL;

		return ikvls_empty_key;
	}

	_OutKV->key = INI_Strdup(lpszWhatCopy);

	free(lpszWhatCopy);

	return ikvls_success;
}

/*
================
INI_LoadFile

Parse a .ini file.

You must allocate _OutFile on your own.

_OutFile's fields are filled
by this function automatically.

Use INI_FreeFile whenever you don't
need the .ini anymore.
================
*/
inifileloadstat_t INI_LoadFile(const char* _FileName, inifile_t* _OutFile, inilogfunc_t _LogFunc)
{
	FILE*			pf;
	long			lFileSize;
	size_t			cbFileSize, cbRead, cbIndex, cbLineNumber = 0, cbFilePos = 0;
	char*			pFileContents;
	char*			pszLineBuf, * pszCurrentLine;
	inisec_t*		pCurrentSection = NULL;
	inikv_t*		pCurrentKey = NULL;
	inikvloadstat_t	eKeyParseStat;
	char*			pszPosition;

	if (!_FileName || !_OutFile)
		return ifls_failed;

	_OutFile->sections_head = _OutFile->sections_tail = NULL;
	_OutFile->filecontents = NULL;
	_OutFile->filesize = 0;

	pf = fopen(_FileName, "rb");

	if (!pf)
	{
		_LogFunc("INI_LoadFile: file \"%s\" doesn't exist", _FileName);
		return ifls_failed;
	}

	fseek(pf, 0, SEEK_END);
	lFileSize = ftell(pf);
	fseek(pf, 0, SEEK_SET);

	if (lFileSize <= 0)
	{
		fclose(pf);
		return ifls_failed;
	}

	cbFileSize = (size_t)lFileSize;

	pFileContents = (char*)malloc(cbFileSize + 1);
	if (!pFileContents)
	{
		fclose(pf);
		return ifls_failed;	// TODO: add more stats?
	}

	pszLineBuf = (char*)malloc(cbFileSize + 1);	// TODO: This is ugly, rewrite this
	if (!pszLineBuf)
	{
		free(pFileContents);
		fclose(pf);
		return ifls_failed;	// TODO: add more stats?
	}

	cbRead = fread(pFileContents, sizeof(char), cbFileSize, pf);

	pFileContents[cbFileSize] = 0;	// make sure we don't read shi

	fclose(pf); // we don't need it anymore

	if (cbRead != cbFileSize)
	{
		free(pFileContents);
		free(pszLineBuf);
		return ifls_failed;	// TODO: add more stats?
	}

	// properly handle \r\n
	while ((pszCurrentLine = INI_MemFGets(pFileContents, cbFileSize, &cbFilePos, pszLineBuf, cbFileSize + 1)) != NULL)
	{
		cbLineNumber++;

		pszCurrentLine[strcspn(pszCurrentLine, "\r")] = 0; // fgets!

		// skip over whitespace
		while (pszCurrentLine[0] == ' ' || pszCurrentLine[0] == '\t')
			pszCurrentLine++;

		if (pszCurrentLine[0] == 0 || pszCurrentLine[0] == ';')
			continue;	// this is a comment or an empty line

		if (pszCurrentLine[0] == '[')
		{	// section begin
			pCurrentSection = (inisec_t*)malloc(sizeof(inisec_t));
			if (!pCurrentSection)
			{
				_LogFunc("INI_LoadFile: oom");
				INI_FreeFile(_OutFile);
				free(pFileContents);
				free(pszLineBuf);
				return ifls_failed;
			}
			memset(pCurrentSection, 0, sizeof(inisec_t));

			// find out section name
			pszPosition = strstr(pszCurrentLine, "]");
			if (!pszPosition)
			{
				_LogFunc("INI_LoadFile: Bad section syntax at line %u", (unsigned int)cbLineNumber);
				free(pCurrentSection);
				continue;
			}

			cbIndex = (size_t)(pszPosition - pszCurrentLine);
			pszCurrentLine[cbIndex] = 0; // so we don't go past the enclosing brace sign
			pszCurrentLine++;
			pCurrentSection->name = INI_Strdup(pszCurrentLine);

			if (!_OutFile->sections_head)
			{
				_OutFile->sections_head = _OutFile->sections_tail = pCurrentSection;
				pCurrentSection->m_pNext = pCurrentSection->m_pPrev = NULL;
			}
			else
			{
				_OutFile->sections_tail->m_pNext = pCurrentSection;
				pCurrentSection->m_pPrev = _OutFile->sections_tail;
				_OutFile->sections_tail = pCurrentSection;
			}

			continue;
		}

		// otherwise this is a kv pair, try to parse
		pCurrentKey = (inikv_t*)malloc(sizeof(inikv_t));
		if (!pCurrentKey)
		{
			_LogFunc("INI_LoadFile: oom");
			INI_FreeFile(_OutFile);
			free(pFileContents);
			free(pszLineBuf);
			return ifls_failed;
		}

		memset(pCurrentKey, 0, sizeof(inikv_t));

		eKeyParseStat = INI_ParseKV(pszCurrentLine, pCurrentKey, _LogFunc);
		if (eKeyParseStat != ikvls_success)
		{
			_LogFunc("INI_LoadFile: key-value pair parse failed at line %u with error %s", (unsigned int)cbLineNumber, INI_KVErrorToString(eKeyParseStat));
			free(pCurrentKey);
			continue;	// skip this line
		}

		if (!pCurrentSection)
		{
			_LogFunc("INI_LoadFile: key-value pair outside of a section at line %u", (unsigned int)cbLineNumber);
			if (pCurrentKey->key) free(pCurrentKey->key);
			if (pCurrentKey->value) free(pCurrentKey->value);
			free(pCurrentKey);
			continue;	 // key outside a section?
		}

		if (!pCurrentSection->keys_head)
		{
			pCurrentSection->keys_head = pCurrentSection->keys_tail = pCurrentKey;
			pCurrentKey->m_pNext = pCurrentKey->m_pPrev = NULL;
		}
		else
		{
			pCurrentSection->keys_tail->m_pNext = pCurrentKey;
			pCurrentKey->m_pPrev = pCurrentSection->keys_tail;
			pCurrentSection->keys_tail = pCurrentKey;
		}
	}

	free(pszLineBuf);

	_OutFile->filecontents = pFileContents;
	_OutFile->filesize = cbFileSize;

	return ifls_success;
}

/*
================
INI_FreeFile

Free the memory occupied by a .ini file.
Make sure to nullify it after calling this :)
================
*/
void INI_FreeFile(inifile_t* _OutFile)
{
	inisec_t* pCurrentSection, * pNextSection;
	inikv_t* pCurrentKey, * pNextKey;

	if (!_OutFile)
		return;

	if (_OutFile->filecontents)
	{
		free(_OutFile->filecontents);
		_OutFile->filecontents = NULL;
	}

	pCurrentSection = _OutFile->sections_head;
	while (pCurrentSection)
	{
		pNextSection = pCurrentSection->m_pNext;

		pCurrentKey = pCurrentSection->keys_head;
		while (pCurrentKey)
		{
			pNextKey = pCurrentKey->m_pNext;

			if (pCurrentKey->key)
				free(pCurrentKey->key);
			if (pCurrentKey->value)
				free(pCurrentKey->value);

			free(pCurrentKey);

			pCurrentKey = pNextKey;
		}

		if (pCurrentSection->name)
			free(pCurrentSection->name);

		free(pCurrentSection);

		pCurrentSection = pNextSection;
	}

	_OutFile->sections_head = _OutFile->sections_tail = NULL;
}

/*
================
INI_FindSection

CASE SENSITIVE!!!
Find a section by name inside a file
================
*/
inisec_t* INI_FindSection(inifile_t* _File, const char* _SectionName)
{
	inisec_t* pCurrent;

	if (!_File || !_SectionName)
		return NULL;

	pCurrent = _File->sections_head;
	while (pCurrent)
	{
		if (!strcmp(pCurrent->name, _SectionName))
			return pCurrent;

		pCurrent = pCurrent->m_pNext;
	}

	return NULL;
}

/*
================
INI_FindKey

CASE SENSITIVE!!!
Find a key by name inside a section
================
*/
inikv_t* INI_FindKey(inisec_t* _Section, const char* _KeyName)
{
	inikv_t* pCurrent;

	if (!_Section || !_KeyName)
		return NULL;

	pCurrent = _Section->keys_head;
	while (pCurrent)
	{
		if (!strcmp(pCurrent->key, _KeyName))
			return pCurrent;

		pCurrent = pCurrent->m_pNext;
	}

	return NULL;
}

/*
================
INI_FindISection

CASE INSENSITIVE!!!
Find a section by name inside a file
================
*/
inisec_t* INI_FindISection(inifile_t* _File, const char* _SectionName)
{
	inisec_t* pCurrent;

	if (!_File || !_SectionName)
		return NULL;

	pCurrent = _File->sections_head;
	while (pCurrent)
	{
		if (!Q_strcasecmp(pCurrent->name, _SectionName))
			return pCurrent;

		pCurrent = pCurrent->m_pNext;
	}

	return NULL;
}

/*
================
INI_FindIKey

CASE INSENSITIVE!!!
Find a key by name inside a section
================
*/
inikv_t* INI_FindIKey(inisec_t* _Section, const char* _KeyName)
{
	inikv_t* pCurrent;

	if (!_Section || !_KeyName)
		return NULL;

	pCurrent = _Section->keys_head;
	while (pCurrent)
	{
		if (!Q_strcasecmp(pCurrent->key, _KeyName))
			return pCurrent;

		pCurrent = pCurrent->m_pNext;
	}

	return NULL;
}
