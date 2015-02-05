#ifndef CSV_PARSE_H
#define CSV_PARSE_H

#ifdef __plusplus
#endif

struct csvparse;
struct csvparse* csvparse_new(char *str,char *err,int len);
void csvparse_delete(struct csvparse* ctx);
int csvparser_next_column(struct csvparse *ctx,char **out);
int csvparser_next_line(struct csvparse *ctx);

struct csvdata;
struct csvdata* csvdata_new();
void csvdata_delete(struct csvdata *csv);
int csvdata_parse(struct csvdata *csv,char* str);
const char*csvdata_value_string(struct csvdata*csv,int line,int row,const char*def);
long csvdata_value_long(struct csvdata *csv,int line, int row, long def);
double csvdata_value_double(struct csvdata *csv,int line, int row, double def);
const char* csvdata_error(struct csvdata *csv);

struct csvfile;
struct csvfile* csvfile_new();
void csvfile_delete(struct csvfile* csv);
int csvfile_load(struct csvfile* csv,const char* path);
struct csvdata* csvfile_data(struct csvfile* csv);


#ifdef __plusplus
#endif

#endif //CSV_PARSE_H

