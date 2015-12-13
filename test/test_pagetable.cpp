#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "libgallocy.h"
#include "pagetable.h"


TEST(PagetableTests, EntryCount) {
#if 0
  void* ptr = custom_malloc(16);
  ASSERT_GE(pt.get_page_table_entry_count(), 1);
  custom_free(ptr);
#endif
}
