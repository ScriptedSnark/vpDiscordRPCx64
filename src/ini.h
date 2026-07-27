/* ini.h */
#ifndef _INI_H_
#define _INI_H_
#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

typedef struct inikv_s
{
	char*	key;		// must be freed
	char*	value;		// must be freed
	struct inikv_s* m_pPrev;
	struct inikv_s* m_pNext;
} inikv_t;

typedef struct inisec_s
{
	char*		name;	// must be freed
	inikv_t*	keys_head;
	inikv_t*	keys_tail;
	struct inisec_s* m_pPrev;
	struct inisec_s* m_pNext;
} inisec_t;

typedef struct
{
	char*		filecontents;	// TODO: Do we need this? Anyways it must be freed since it's malloced.
	size_t		filesize;
	inisec_t*	sections_head;
	inisec_t*	sections_tail;
} inifile_t;

typedef enum inikvloadstat_e
{
	ikvls_null_input = 0,
	ikvls_missing_equals,
	ikvls_success,
	ikvls_oom,
	ikvls_empty_key,
} inikvloadstat_t;

typedef enum inifileloadstat_e
{
	ifls_failed = 0,
	ifls_success,
	ifls_completed_with_errors
} inifileloadstat_t;

typedef void(*inilogfunc_t)(const char* _Format, ...);

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	const char*			INI_KVErrorToString( inikvloadstat_t _Stat );
	inikvloadstat_t		INI_ParseKV( const char* _FromWhat, inikv_t* _OutKV, inilogfunc_t _LogFunc );
	inifileloadstat_t	INI_LoadFile( const char* _FileName, inifile_t* _OutFile, inilogfunc_t _LogFunc );
	void				INI_FreeFile( inifile_t* _OutFile );
	inisec_t*			INI_FindSection( inifile_t* _File, const char* _SectionName );
	inikv_t*			INI_FindKey( inisec_t* _Section, const char* _KeyName );
	inisec_t*			INI_FindISection( inifile_t* _File, const char* _SectionName );
	inikv_t*			INI_FindIKey( inisec_t* _Section, const char* _KeyName );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !_INI_H_