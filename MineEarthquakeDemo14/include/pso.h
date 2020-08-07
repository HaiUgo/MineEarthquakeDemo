//
// MATLAB Compiler: 7.0 (R2018b)
// Date: Thu Aug  6 23:06:03 2020
// Arguments:
// "-B""macro_default""-W""cpplib:pso,all""-T""link:lib""-d""C:\Users\13696\Desk
// top\pso\for_testing""-v""C:\Users\13696\Desktop\bianyi.m""C:\Users\13696\Desk
// top\mubiao3.m""C:\Users\13696\Desktop\mysort3.m""C:\Users\13696\Desktop\PSO.m
// "
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
bool MW_CALL_CONV mlxBianyi(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

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

extern LIB_pso_CPP_API void MW_CALL_CONV bianyi(int nargout, mwArray& result, const mwArray& fv, const mwArray& sample, const mwArray& step, const mwArray& len, const mwArray& v);

extern LIB_pso_CPP_API void MW_CALL_CONV mubiao3(int nargout, mwArray& f, const mwArray& fv, const mwArray& sample, const mwArray& v);

extern LIB_pso_CPP_API void MW_CALL_CONV mysort3(int nargout, mwArray& ff, mwArray& one, const mwArray& fv, const mwArray& sample, const mwArray& v, const mwArray& diedai, const mwArray& lead);

extern LIB_pso_CPP_API void MW_CALL_CONV PSO(int nargout, mwArray& x, mwArray& y, mwArray& z, mwArray& t, const mwArray& sample, const mwArray& v);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
