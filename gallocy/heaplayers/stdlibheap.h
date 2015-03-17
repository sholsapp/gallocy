#ifndef _STDLIBHEAP_H_
#define _STDLIBHEAP_H_


namespace HL {

template <class Super>
class StdlibHeap : public Super {

public:

  void *malloc(size_t sz) {
    return Super::malloc(sz);
  }

  void free(void *ptr) {
    Super::free(ptr);
  }

  void* realloc(void* ptr, size_t sz) {
    if (ptr == NULL) {
      return Super::malloc(sz);
    }
    size_t min_size = Super::getSize(ptr);
    void* buf = Super::malloc(sz);
    if (buf != NULL) {
      memcpy(buf, ptr, min_size);
      Super::free(ptr);
    }
    return buf;
  }

  char *strdup(const char *s1) {
    char *s2 = (char *) Super::malloc(sizeof(char) * strlen(s1) + 1);
    memcpy(s2, s1, strlen(s1) + 1);
    return s2;
  }

  void *calloc(size_t count, size_t size) {
    void *ptr = Super::malloc(count * size);
    memset(ptr, 0, count * size);
    return ptr;
  }

};

}

#endif
