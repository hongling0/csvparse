#ifndef CSV_PARSE_H
#define CSV_PARSE_H

#ifdef __plusplus
#endif

struct csvparse
{
  char* parse_ptr;
  int need_nextline;
  char* errmsg;
  int errmsg_len;
};

void csvparse_init(struct csvparse *ctx,char *str,char *err,int len);
int csvparser_next_column(struct csvparse *ctx,char **out);
int csvparser_next_line(struct csvparse *ctx);

struct csvdata
{
  int col_cnt;
  int line_cnt;
  int line_cap;
  char errmsg[128];
  char** data;
};

struct csvdata* csvdata_new();
void csvdata_delete(struct csvdata *csv);
int csvdata_parse(struct csvdata *csv,char* str);
const char* csvdata_value_string(struct csvdata*csv,int line,int row,const char*def);
long csvdata_value_long(struct csvdata *csv,int line, int row, long def);
double csvdata_value_double(struct csvdata *csv,int line, int row, double def);

struct csvfile
{
  struct csvdata data;
  char* buf;
};

struct csvfile* csvfile_new();
void csvfile_delete(struct csvfile* csv);
int csvfile_load(struct csvfile* csv,const char* path);


#ifdef __plusplus
#endif

#endif //CSV_PARSE_H

