#define EV_STR_IMPLEMENTATION
#include "ev_str.h"

evstring global_str = evstr("Global 'Hello, World!'");

int main()
{
  const evstring stack_str = evstr("Stack 'Hello, World!'");
  printf("Stack String: %s, Length: %llu\n", stack_str, evstring_getLength(stack_str));
  printf("Global String: %s, Length: %llu\n", global_str, evstring_getLength(global_str));

  evstring heap_str = evstring_new("Heap 'Hello, World!'");
  printf("Heap String: %s, Length: %llu\n", heap_str, evstring_getLength(heap_str));

  evstring_view view = evstring_slice(stack_str, 0, -1);
  printf("String View: %.*s\n", (i32)view.len, view.data + view.offset);
  printf("View length: %llu\n", view.len);

  evstring heap_str2 = evstring_new(view);
  printf("Heap String 2: %s, Length: %llu\n", heap_str2, evstring_getLength(heap_str2));
  evstring_free(heap_str2);


  evstring_error_t push_str_res = evstring_push(&heap_str, "Hello, Sisyphus! %f", 0.001f);
  printf("Push char*: %s, New Length: %llu\n", heap_str, evstring_getLength(heap_str));
  assert(push_str_res == EV_STR_ERR_NONE);

  evstring_error_t push_char_res = evstring_push(&heap_str, (char)'X');
  printf("Push char: %s, New Length: %llu\n", heap_str, evstring_getLength(heap_str));
  assert(push_char_res == EV_STR_ERR_NONE);

  evstring_error_t push_view_res = evstring_push(&heap_str, view);
  printf("Push view: %s, New Length: %llu\n", heap_str, evstring_getLength(heap_str));
  assert(push_view_res == EV_STR_ERR_NONE);

  evstring str_fmt = evstring_new("%d, %d, %.*s", 1, 0, view.len, view.data + view.offset);
  printf("Formatted String: %s\n", str_fmt);

  evstring rep_str = evstring_replaceFirst(heap_str, evstr("Hello"), evstr("Bye"));
  printf("Replaced String: %s\n", rep_str);
  evstring_free(rep_str);

  evstring_free(str_fmt);
  evstring_free(heap_str);

  evstring_view search_results[8];
  evstring search_string = evstr("Hello, this is me saying `Hello` like someone who says 'Hello'");
  assert(evstring_findAll(search_string, evstr("Hello"), search_results) == 3);
  assert(search_results[0].data == search_string);
  assert(search_results[1].data == search_string);
  assert(search_results[2].data == search_string);
  assert(search_results[0].len == 5);
  assert(search_results[1].len == 5);
  assert(search_results[2].len == 5);
  assert(search_results[0].offset == 0);
  assert(search_results[1].offset == 26);
  assert(search_results[2].offset == 56);

  return 0;
}
