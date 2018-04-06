#ifndef SAFERELEASE
#define SAFERELEASE(p) { if (p) { p->Release(); p = NULL; } }
#endif
