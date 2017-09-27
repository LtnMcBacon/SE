#ifndef SE_DLL_EXPROT_H_
#define SE_DLL_EXPROT_H_
#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#endif // SE_DLL_EXPROT_H_