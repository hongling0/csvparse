#ifndef TRANSFER_H
#define TRANSFER_H

#ifdef __cplusplus
extern "C" {
#endif

double trans_double(const char* str, double def);
long trans_long(const char* str, long def);
inline const char* trans_string(const char* str,const char*def) {
  return str?str:def;
}

#ifdef __cplusplus
}
#endif

#endif

