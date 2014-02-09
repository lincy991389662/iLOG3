/*
 * iLOG3 - ��׼c��־������
 * author  : calvin
 * email   : 
 * history : 2014-01-19	v1.0.0		����
 */

#if ( defined _WIN32 )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC		_declspec(dllexport)
#endif
#elif ( defined __unix ) || ( defined __linux__ )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOGS.h"

/* ��־������Ͻṹ */
struct tagLOGS
{
	char	*g_id[ LOGS_MAXCNT_LOG + 1 ] ;
	LOG	*g[ LOGS_MAXCNT_LOG + 1 ] ;
} ;

/* �̱߳��ش洢ȫ�ֶ��� */
#if ( defined _WIN32 )
__declspec( thread ) LOGS	*tls_gs = NULL ;
#elif ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
__thread LOGS			*tls_gs = NULL ;
#endif

/* ������־��� */
void DestroyLogsHandle( LOGS *gs )
{
	if( gs )
	{
		long	g_no ;
		
		for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
		{
			if( gs->g_id[g_no] && gs->g[g_no] )
			{
				free( gs->g_id[g_no] );
				
				DestroyLogHandle( gs->g[g_no] );
			}
		}
		
		free( gs );
	}
	
	return;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
void DestroyLogsHandleG()
{
	DestroyLogsHandle( tls_gs );
}
#endif

/* ������־��� */
LOGS *CreateLogsHandle()
{
	LOGS	*gs = NULL ;
	
	gs = (LOGS *)malloc( sizeof(LOGS) ) ;
	if( gs == NULL )
		return NULL;
	memset( gs , 0x00 , sizeof(LOGS) );
	
	return gs;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
LOGS *CreateLogsHandleG()
{
	tls_gs = CreateLogsHandle() ;
	return tls_gs;
}
#endif

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 ) /* Ϊ�����溯����strdup�ܱ��벻������ */
char *strdup(const char *s);
#endif

int AddLogToLogs( LOGS *gs , char *g_id , LOG *g )
{
	long		g_no ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] == NULL && gs->g[g_no] == NULL )
		{
			gs->g_id[g_no] = strdup( g_id ) ;
			if( gs->g_id[g_no] == NULL )
				return LOG_RETURN_ERROR_ALLOC;
			gs->g[g_no] = g ;
			return 0;
		}
	}
	
	return LOGS_RETURN_ERROR_TOOMANY_LOG;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int AddLogToLogsG( char *g_id , LOG *g )
{
	return AddLogToLogs( tls_gs , g_id , g );
}
#endif

LOG *RemoveOutLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	LOG		*g = NULL ;
	
	if( g_id == NULL )
		return NULL;
	
	for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] && gs->g[g_no] )
		{
			if( STRCMP( gs->g_id[g_no] , == , g_id ) )
			{
				free( gs->g_id[g_no] ); gs->g_id[g_no] = NULL ;
				g = gs->g[g_no] ;
				gs->g[g_no] = NULL ;
				return g;
			}
		}
	}
	
	return NULL;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
LOG *RemoveOutLogFromLogsG( char *g_id )
{
	return RemoveOutLogFromLogs( tls_gs , g_id );
}
#endif

LOG *GetLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	
	if( g_id == NULL )
		return NULL;
	
	for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] && gs->g[g_no] )
		{
			if( STRCMP( gs->g_id[g_no] , == , g_id ) )
			{
				return gs->g[g_no];
			}
		}
	}
	
	return NULL;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
LOG *GetLogFromLogsG( char *g_id )
{
	return GetLogFromLogs( tls_gs , g_id );
}
#endif

int TravelLogFromLogs( LOGS *gs , long *p_index , char **pp_g_id , LOG **pp_g )
{
	for( ++(*p_index) ; (*p_index) >= LOGS_MAXCNT_LOG ; (*p_index)++ )
	{
		if( gs->g_id[(*p_index)] && gs->g[(*p_index)] )
		{
			if( pp_g_id )
				(*pp_g_id) = gs->g_id[(*p_index)] ;
			if( pp_g )
				(*pp_g) = gs->g[(*p_index)] ;
			return 0;
		}
	}
	
	return LOGS_RETURN_INFO_NOTFOUND;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int TravelLogFromLogsG( long *p_index , char **pp_id , LOG **pp_g )
{
	return TravelLogFromLogs( tls_gs , p_index , pp_id , pp_g );
}
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
/* �õ������̱߳��ش洢��ȱʡ��־������ϵĺ����汾 */
LOGS *GetGlobalLOGS()
{
	return tls_gs;
}
void SetGlobalLOGS( LOGS *gs )
{
	tls_gs = gs ;
	return;
}
#endif

extern int WriteLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , va_list valist );

/* ����� */
#define WRITELOGSBASE(_gs_,_log_level_) \
	long		g_no ; \
	LOG		**pp_g = NULL ; \
	va_list		valist; \
	int		nret ; \
	if( (_gs_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	for( g_no = 0 , pp_g = & ((_gs_)->g[0]) ; g_no < LOGS_MAXCNT_LOG ; g_no++ , pp_g++ ) \
	{ \
		if( (*pp_g) == NULL ) \
			continue ; \
		if( (*pp_g)->output == LOG_OUTPUT_FILE && (*pp_g)->log_pathfilename[0] == '\0' ) \
			continue ; \
		if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (*pp_g) ) ) \
			continue ; \
		va_start( valist , format ); \
		nret = WriteLogBase( (*pp_g) , c_filename , c_fileline , _log_level_ , format , valist ) ; \
		va_end( valist ); \
		if( nret < 0 ) \
			return nret; \
	}

/* ����־�ȼ���д��־ */
int WriteLogs( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGSBASE( gs , log_level )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int WriteLogsG( char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGSBASE( tls_gs , log_level )
	return 0;
}

/* д������־ */
int DebugLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_DEBUG )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int DebugLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_DEBUG )
	return 0;
}
#endif

/* д��ͨ��Ϣ��־ */
int InfoLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_INFO )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int InfoLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_INFO )
	return 0;
}
#endif

/* д������־ */
int WarnLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_WARN )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int WarnLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_WARN )
	return 0;
}
#endif

/* д������־ */
int ErrorLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_ERROR )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int ErrorLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_ERROR )
	return 0;
}
#endif

/* д����������־ */
int FatalLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_FATAL )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int FatalLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_FATAL )
	return 0;
}
#endif

extern int WriteHexLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist );

/* ����� */
#define WRITEHEXLOGSBASE(_gs_,_log_level_) \
	long		g_no ; \
	LOG		**pp_g = NULL ; \
	va_list		valist; \
	int		nret ; \
	if( (_gs_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	for( g_no = 0 , pp_g = & ((_gs_)->g[0]) ; g_no < LOGS_MAXCNT_LOG ; g_no++ , pp_g++ ) \
	{ \
		if( (*pp_g) == NULL ) \
			continue ; \
		if( (*pp_g)->output == LOG_OUTPUT_FILE && (*pp_g)->log_pathfilename[0] == '\0' ) \
			continue ; \
		if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (*pp_g) ) ) \
			continue ; \
		va_start( valist , format ); \
		nret = WriteHexLogBase( (*pp_g) , c_filename , c_fileline , _log_level_ , buffer , buflen , format , valist ) ; \
		va_end( valist ); \
		if( nret ) \
			return nret; \
	}

/* ����־�ȼ���дʮ�����ƿ���־ */
int WriteHexLogs( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , log_level )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int WriteHexLogsG( char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , log_level )
	return 0;
}
#endif

/* дʮ�����ƿ������־ */
int DebugHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_DEBUG )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int DebugHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_DEBUG )
	return 0;
}
#endif

/* дʮ�����ƿ���ͨ��Ϣ��־ */
int InfoHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_INFO )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int InfoHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_INFO )
	return 0;
}
#endif

/* дʮ�����ƿ龯����־ */
int WarnHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_WARN )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int WarnHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_WARN )
	return 0;
}
#endif

/* дʮ�����ƿ������־ */
int ErrorHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_ERROR )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int ErrorHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_ERROR )
	return 0;
}
#endif

/* дʮ�����ƿ�����������־ */
int FatalHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_FATAL )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int FatalHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_FATAL )
	return 0;
}
#endif

#endif
