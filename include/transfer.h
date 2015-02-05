#ifndef TRANSFER_H
#define TRANSFER_H

#ifdef __cplusplus
extern "C" {
#endif

double trans_double(const char* str, double def);
long trans_long(const char* str, long def);
#define trans_string(str,def) ((str)?(str):(def))

#ifdef __cplusplus
}
#endif

#endif

