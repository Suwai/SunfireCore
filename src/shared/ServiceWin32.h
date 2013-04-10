/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

#ifdef WIN32
#ifndef _WIN32_SERVICE_
#define _WIN32_SERVICE_

bool WinServiceInstall();
bool WinServiceUninstall();
bool WinServiceRun();

#endif                                                      // _WIN32_SERVICE_
#endif                                                      // WIN32

