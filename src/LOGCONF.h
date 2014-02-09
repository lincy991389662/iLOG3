#ifndef _H_LOGCONF_
#define _H_LOGCONF_

/*
 * iLOG3 - ��׼c��־������ �������ļ��ӿ�
 * author  : calvin
 * email   : 
 * history : 2014-02-09	v1.0.0		����
 */

#ifdef __cplusplus
extern "C" {
#endif

#if ( defined _WIN32 )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC		_declspec(dllexport)
#endif
#elif ( defined __unix ) || ( defined __linux__ )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOG.h"

/* ��������ֵ�� */
#define LOGCONF_RETURN_ERROR_CONFIGFILE_NOTFOUND	-51	/* �Ҳ��������ļ� */
#define LOGCONF_RETURN_ERROR_CONFIGFILE_INVALID		-52	/* �����ļ���Ч */

_WINDLL_FUNC LOG *CreateLogHandleFromConfig( char *config_filename , char *postfix );

#ifdef __cplusplus
}
#endif

#endif