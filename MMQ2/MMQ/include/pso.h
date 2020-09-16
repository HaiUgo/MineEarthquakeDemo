//
// MATLAB Compiler: 6.6 (R2018a)
// Date: Wed Sep 16 11:22:47 2020
// Arguments:
// "-B""macro_default""-W""cpplib:pso,all""-T""link:lib""-d""E:\¿óÉ½ÏîÄ¿\zhangga
// ng1-microquake-system_-liaoning-university1-master\microquake-system_-liaonin
// g-university1\ÀíÂÛËã·¨\Á£×ÓÈºËã·¨\PSO\for_testing""-v""E:\¿óÉ½ÏîÄ¿\zhanggang1
// -microquake-system_-liaoning-university1-master\microquake-system_-liaoning-u
// niversity1\ÀíÂÛËã·¨\Á£×ÓÈºËã·¨\¶¨Î»¾«¶ÈÑÐ¾¿-8.4\mubiao3.m""E:\¿óÉ½ÏîÄ¿\zhangg
// ang1-microquake-system_-liaoning-university1-master\microquake-system_-liaoni
// ng-university1\ÀíÂÛËã·¨\Á£×ÓÈºËã·¨\¶¨Î»¾«¶ÈÑÐ¾¿-8.4\mysort3.m""E:\¿óÉ½ÏîÄ¿\zh
// anggang1-microquake-system_-liaoning-university1-master\microquake-system_-li
// aoning-university1\ÀíÂÛËã·¨\Á£×ÓÈºËã·¨\¶¨Î»¾«¶ÈÑÐ¾¿-8.4\PSO.m""-a""E:\¿óÉ½ÏîÄ
// ¿\zhanggang1-microquake-system_-liaoning-university1-master\microquake-system
// _-liaoning-university1\ÀíÂÛËã·¨\Á£×ÓÈºËã·¨\¶¨Î»¾«¶ÈÑÐ¾¿-8.4\mubiao3.m""-a""E:
// \¿óÉ½ÏîÄ¿\zhanggang1-microquake-system_-liaoning-university1-master\microquak
// e-system_-liaoning-university1\ÀíÂÛËã·¨\Á£×ÓÈºËã·¨\¶¨Î»¾«¶ÈÑÐ¾¿-8.4\mysort3.m
// ""-a""E:\¿óÉ½ÏîÄ¿\zhanggang1-microquake-system_-liaoning-university1-master\m
// icroquake-system_-liaoning-university1\ÀíÂÛËã·¨\Á£×ÓÈºËã·¨\¶¨Î»¾«¶ÈÑÐ¾¿-8.4\P
// SO.m"
//

#ifndef __pso_h
#define __pso_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_pso_C_API 
#define LIB_pso_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_pso_C_API 
bool MW_CALL_CONV psoInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_pso_C_API 
bool MW_CALL_CONV psoInitialize(void);

extern LIB_pso_C_API 
void MW_CALL_CONV psoTerminate(void);

extern LIB_pso_C_API 
void MW_CALL_CONV psoPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_pso_C_API 
bool MW_CALL_CONV mlxMubiao3(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_pso_C_API 
bool MW_CALL_CONV mlxMysort3(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_pso_C_API 
bool MW_CALL_CONV mlxPSO(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_pso
#define PUBLIC_pso_CPP_API __declspec(dllexport)
#else
#define PUBLIC_pso_CPP_API __declspec(dllimport)
#endif

#define LIB_pso_CPP_API PUBLIC_pso_CPP_API

#else

#if !defined(LIB_pso_CPP_API)
#if defined(LIB_pso_C_API)
#define LIB_pso_CPP_API LIB_pso_C_API
#else
#define LIB_pso_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_pso_CPP_API void MW_CALL_CONV mubiao3(int nargout, mwArray& f, const mwArray& fv, const mwArray& sample, const mwArray& v);

extern LIB_pso_CPP_API void MW_CALL_CONV mysort3(int nargout, mwArray& ff, mwArray& one, const mwArray& fv, const mwArray& sample, const mwArray& v, const mwArray& diedai, const mwArray& lead);

extern LIB_pso_CPP_API void MW_CALL_CONV PSO(int nargout, mwArray& x, mwArray& y, mwArray& z, mwArray& t, const mwArray& sample, const mwArray& v);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
