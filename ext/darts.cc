#include <ruby.h>
#include <sys/mman.h>
#include "darts.h"


static const int RESULT_SIZE = 100;
static size_t _m_size = 0;
static void *_m_ptr = NULL;

static VALUE mDarts;
static VALUE cTrie;

typedef struct {
  Darts::DoubleArray *da;
  size_t m_size;
  void* m_ptr;
} darts_t;


static void
darts_free(darts_t *ptr) {
  delete ptr->da;
  munmap(ptr->m_ptr, ptr->m_size);
}


static VALUE
darts_trie_allocate(VALUE klass) {
  darts_t *ptr;

  ptr = (darts_t*)malloc(sizeof(darts_t));
  if (ptr == NULL) {
    rb_raise(rb_eNoMemError, "No Mem Error");
  }

  ptr->m_size = 0;
  ptr->m_ptr = NULL;
  ptr->da = new Darts::DoubleArray();

  return Data_Wrap_Struct(klass, -1, darts_free, ptr);
}


static VALUE
rb_darts_trie_initialize(VALUE self, VALUE rb_db_path) {
  darts_t *ptr;
  char *db_path;
  Data_Get_Struct(self, darts_t, ptr);

  db_path = StringValuePtr(rb_db_path);
  FILE *fp = fopen(db_path, "rb");

  if (fp == NULL) {
    rb_sys_fail("open(2) - fail to open file.");
  }

  if (fseek(fp, 0, SEEK_END) != 0) {
    fclose(fp);
    rb_raise(rb_eIOError, "Darts IOError");
  }

  _m_size = ftell(fp);
  _m_ptr = mmap(NULL, _m_size, PROT_READ, MAP_SHARED, fileno(fp), 0);

  fclose(fp);

  if (_m_ptr == MAP_FAILED) {
    rb_sys_fail("mmap(2) - MAP_FAILED");
  }

  ptr->da->set_array(_m_ptr);

  return Qnil;
}


static VALUE
rb_darts_longest_match(VALUE self, VALUE rb_text) {
  darts_t *ptr;
  char *text;
  Darts::result_pair_type darts_results[RESULT_SIZE];
  size_t da_size;
  int i=0, maxlen=0;

  Data_Get_Struct(self, darts_t, ptr);
  
  text = StringValuePtr(rb_text);
  da_size = ptr->da->commonPrefixSearch(text, darts_results, RESULT_SIZE);

  for(i;i<da_size;++i) {
    if (darts_results[i].length > maxlen) {
      maxlen = darts_results[i].length;
    }
  }

  if (maxlen > 0) {
    return rb_str_new(text, maxlen);
  }
  return Qnil;
}


static VALUE
rb_darts_common_prefix_search(VALUE self, VALUE rb_text) {
  darts_t *ptr;
  char *text;
  Darts::result_pair_type darts_results[RESULT_SIZE];
  size_t da_size;
  int i = 0;
  VALUE ret_arr;

  Data_Get_Struct(self, darts_t, ptr);

  text = StringValuePtr(rb_text);
  da_size = ptr->da->commonPrefixSearch(text, darts_results, RESULT_SIZE);

  ret_arr = rb_ary_new();
  for (i;i<da_size;++i) {
    rb_ary_push(ret_arr, rb_str_new(text, darts_results[i].length));
  }
  return ret_arr;
}


extern "C" void
Init_darts(void) {
  mDarts = rb_define_module("Darts");
  cTrie = rb_define_class_under(mDarts, "Trie", rb_cObject);

  rb_define_alloc_func(cTrie, darts_trie_allocate);

  rb_define_private_method(cTrie, "initialize", RUBY_METHOD_FUNC(rb_darts_trie_initialize), 1);
  rb_define_method(cTrie, "longest_match", RUBY_METHOD_FUNC(rb_darts_longest_match), 1);
  rb_define_method(cTrie, "common_prefix_search", RUBY_METHOD_FUNC(rb_darts_common_prefix_search), 1);
}
