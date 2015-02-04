#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transfer.h"
#include "csvparse.h"


#define SET_CHAR(ptr,n,c) do{ *(ptr-n)=c; } while(0)
#define MV_CHAR(ptr,n) do{ if(n>0)*(ptr-n)=*ptr; } while(0)

static void parse_error(struct csvparse *ctx,const char *fmt,...)
{
  va_list va;
  if(ctx->errmsg && ctx->errmsg_len>0) {
    va_start(va,fmt);
    vsnprintf(ctx->errmsg,ctx->errmsg_len,fmt,va);
    va_end(va);
  }
}

static inline void csvdata_init(struct csvdata* csv)
{
  csv->col_cnt=0;
  csv->line_cnt=0;
  csv->line_cap=0;
  csv->data=NULL;
  csv->errmsg[0]='\0';
}

void csvparse_init(struct csvparse *ctx,char *str,char *err,int len)
{
  ctx->parse_ptr=str;
  ctx->need_nextline=0;
  ctx->errmsg=err;
  ctx->errmsg_len=len;
}

int csvparse_next_column(struct csvparse *ctx,char **out)
{
  int with_quote, bytes;
  char* ptr;

  *out = NULL;
  if (!ctx->parse_ptr) return 0;
  if (ctx->need_nextline) return 0;
  with_quote = *ctx->parse_ptr == '"' ? 1 : 0;
  ptr = with_quote > 0 ? (ctx->parse_ptr + 1) : ctx->parse_ptr;
  *out = ptr;
  bytes = 0;
loop:
  switch (*ptr) {
  case '\0':
      {
        ctx->parse_ptr = NULL;
        return 0;
      }
    break;
  case '\"':
      {
        switch (*(ptr + 1)) {
        case  ',':
          if (with_quote == 0)
            goto parse_error;
          else {
            SET_CHAR(ptr, bytes, '\0');
            ctx->parse_ptr = ptr + 2;
            return 0;
          }
          break;
        case '\r':
          if (*(ptr + 2) == '\n') {
            if (with_quote == 0)
              goto parse_error;
            else {
              SET_CHAR(ptr, bytes, '\0');
              ctx->parse_ptr = ptr + 3;
              ctx->need_nextline = 1;
              return 0;
            }
          }
          else
            goto parse_error;
          break;
        case  '\"':
            {
              MV_CHAR(ptr, bytes);
              bytes++;
              ptr++;
            }
          break;
        default:
          goto parse_error;
          break;
        }
      }
    break;
  case '\r':
      {
        if (with_quote == 0 && *(ptr + 1) == '\n') {
          SET_CHAR(ptr, bytes, '\0');
          ctx->parse_ptr = ptr + 2;
          ctx->need_nextline = 1;
          return 0;
        }
        else
          MV_CHAR(ptr, bytes);
      }
    break;
  case ',':
      {
        if (with_quote == 0) {
          SET_CHAR(ptr, bytes, '\0');
          ctx->parse_ptr = ptr + 1;
          return 0;
        }
        else
          MV_CHAR(ptr, bytes);
      }
    break;
  default:
    MV_CHAR(ptr, bytes);
    break;
  }
  ++ptr;
  goto loop;
parse_error:
  parse_error(ctx,"unexpected char %c on %s\n", *ptr, ptr);
  *out = NULL;
  return -1;
}

int csvparse_next_line(struct csvparse *ctx)
{
  ctx->need_nextline = 0;
  return ctx->parse_ptr != NULL && (*ctx->parse_ptr) != '\0';
}



struct csvdata* csvdata_new()
{
  struct csvdata* csv=(struct csvdata*)malloc(sizeof(*csv));
  csvdata_init(csv);
  return csv;
}

void csvdata_delete(struct csvdata *csv)
{
  free(csv->data);
  free(csv);
}

struct column_node
{
  struct column_node * next;
  char * data;
};

struct line_node
{
  struct line_node * next;
  struct column_node head;
  struct column_node* tail;
};

int csvdata_parse(struct csvdata*csv,char* str)
{
  struct csvparse parse,*p;

  int line_num,col_num,line,row;
  struct line_node line_head,*tail,*lnode,*lcur,*lnext;

  struct column_node *cnode,*ccur,*cnext;

  char *ptr;

  p=&parse;
  csvparse_init(p,str,p->errmsg,sizeof(p->errmsg));

  line_num=0;
  line_head.next = NULL;
  tail = &line_head;

  for(;csvparse_next_line(p);) {
    lnode=(struct line_node*)malloc(sizeof(*lnode));
    lnode->head.next=NULL;
    lnode->tail=&lnode->head;
    tail->next = lnode;
    tail = lnode;
    col_num = 0;
    for(;;){
      ptr = NULL;
      if(!csvparse_next_column(p,&ptr)){
        // todo memory leak
        return -1;
      }
      if(!ptr)
        break;
      cnode=(struct column_node*)malloc(sizeof(*cnode));
      cnode->next=NULL;
      cnode->data=ptr;
      tail->tail->next=cnode;
      tail->tail = cnode;
      col_num++;
    }
    if(col_num>csv->col_cnt)
       csv->col_cnt = col_num;
    line_num++;
  }
  csv->line_cnt=line_num;

  csv->data = (char**)malloc(sizeof(char*)*csv->line_cnt*csv->col_cnt);
  line = 0;
  for (lcur = line_head.next; lcur; ++line) {
    lnext = lcur->next;
    row = 0;
    for (ccur = lcur->head.next; ccur; ++row) {
      cnext = ccur->next;
      csv->data[line*csv->col_cnt + row] = ccur->data;
      free(ccur);
      ccur = cnext;
    }
    for (; row < csv->col_cnt; ++row)
      csv->data[line*csv->col_cnt + row] = "";
    free(lcur);
    lcur = lnext;
  }
  return 0;
}

const char* csvdata_value_string(struct csvdata *csv,int line, int row, const char* def)
{
  if (line >= csv->line_cnt || row >= csv->col_cnt) return def;
  return csv->data[line*csv->col_cnt + row];
}

long csvdata_value_long(struct csvdata *csv,int line, int row, long def)
{
  const char* val = csvdata_value_string(csv,line, row, (const char*)NULL);
  if (!val) return def;
  return trans_long(val, def);
}

double csvdata_value_double(struct csvdata *csv,int line, int row, double def)
{
  const char* val = csvdata_value_string(csv,line, row, (const char*)NULL);
  if (!val) return def;
  return trans_double(val, def);
}

struct csvfile* csvfile_new()
{
  struct csvfile * csv=(struct csvfile *)malloc(sizeof(*csv));
  csvdata_init(&csv->data);
  csv->buf=NULL;
  return csv;
}

void csvfile_delete(struct csvfile* csv)
{
  free(csv->data.data);
  free(csv->buf);
  free(csv);
}

int csvfile_load(struct csvfile* csv,const char* path)
{
  size_t len, r;
  char *b;
  FILE* f = fopen(path, "rb");
  if (!f)
    goto do_errno;

  fseek(f, 0, SEEK_END);
  len = ftell(f);
  b = (char*)malloc(len + 1);
  b[len] = '\0';
  fseek(f, 0, SEEK_SET);
  r = fread(b, 1, len, f);
  if (r != len)
    goto file_errno;
  fclose(f);
  if (csvdata_parse(&csv->data,b)) {
    free(b);
    return -1;
  }
  csv->buf = b;
  return 0;
file_errno:
  free(b);
  fclose(f);
do_errno:
  snprintf(csv->data.errmsg,sizeof(csv->data.errmsg),"%s\n",strerror(errno));
  return -1;
}
