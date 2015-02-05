
.PHONY:all list clean

RM=rm -rf
AR=ar rcus
STATIC_LIB=./lib/libcsvparse.lib
SHARED_LIB=./lib/libcsvparse.so
TEST=./test
INCLUDE=include/
OBJDIR=obj/

CFLAGS=-Werror -O2 -Wall -g -I$(INCLUDE)
SHARED=-fPIC --shared


SRC_LIB=src/csvparse.c src/transfer.c
OBJS_LIB=$(addprefix $(OBJDIR),$(patsubst %.c,%.o,$(SRC_LIB)))
DEPS_LIB=$(patsubst %.o,%.d,$(OBJS_LIB))


all:$(STATIC_LIB) $(SHARED_LIB)

list:
	@echo $(SRC_LIB)
	@echo $(OBJS_LIB)
	@echo $(DEPS_LIB)

$(STATIC_LIB):$(OBJS_LIB)
	$(AR) $(STATIC_LIB) $(OBJS_LIB)
$(SHARED_LIB):$(OBJS_LIB)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

ifneq ($(MAKECMDGOALS),clean)
$(OBJDIR)%.o:%.c $(OBJDIR)%.d
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)%.d:%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM $< -MT $(@:.d=.o) -MF $@

$(shell mkdir -p $(dir $(STATIC_LIB)))
$(shell mkdir -p $(dir $(SHARED_LIB)))
$(foreach d,$(OBJS_LIB),$(shell mkdir -p $(dir $(d))))
-include $(DEPS_LIB)
endif

clean:
	$(RM) $(STATIC_LIB)
	$(RM) $(SHARED_LIB)
	$(RM) $(TEST)
	$(RM) $(OBJDIR)

cleanall:clean
	$(RM) $(STATIC_LIB) $(SHARED_LIB)


