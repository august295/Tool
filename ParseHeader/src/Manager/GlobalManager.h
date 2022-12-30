#ifndef __GlobalManager_h__
#define __GlobalManager_h__

#ifdef MANAGER_LIBRARY
#define MANAGER_EXPORT __declspec(dllexport)
#else
#define MANAGER_EXPORT __declspec(dllimport)
#endif

#endif
