#ifndef _FIRSTFITHEAP_H_
#define _FIRSTFITHEAP_H_

#include <cstdio>
#include <assert.h>

template <class Super>
class FirstFitHeap : public Super {
   public:

      FirstFitHeap (void)
         : myFreeList (NULL), nObjects (0)
      {
         assert (classInvariant());
      }

      ~FirstFitHeap (void)
      {
#if 1
         // Delete everything on the free list.
         void * ptr = myFreeList;
         while (ptr != NULL) {
            // assert (nObjects > 0);
            assert (ptr != NULL);
            void * oldptr = ptr;
            ptr = (void *) ((freeObject *) ptr)->next;
            Super::free (oldptr);
            --nObjects;
         }
#endif
      }

      inline void * malloc (size_t sz) {
         // Check the free list first.
         assert (classInvariant());
         void * ptr = myFreeList;
         if (ptr == NULL) {
            assert (nObjects == 0);
            ptr = Super::malloc (sz);
         } else {
            assert (nObjects > 0);
            freeObject * p = myFreeList;
            freeObject * prev = NULL;
            while ((p != NULL) && (Super::getSize((void *) p) < sz)) {
               prev = p;
               p = p->next;
            }
            if (p == NULL) {
               ptr = Super::malloc (sz);
            } else {
               assert (Super::getSize((void *) p) >= sz);
               if (prev == NULL) {
                  myFreeList = p->next;
               } else {
                  assert (prev->next == p);
                  prev->next = p->next;
               }
               nObjects--;
               ptr = p;
            }
         }
         assert (classInvariant());
         return ptr;
      }

      inline void free (void * ptr) {

         if (!ptr)
           return;

         for (int i = 0; i < Super::getSize((void*) ptr); i++) {
            ((unsigned char*) ptr)[i] = 0xED;
         }

         // Add this object to the free list.
         assert (ptr != NULL);
         assert (classInvariant());
         nObjects++;
         freeObject * p = myFreeList;
         freeObject * prev = NULL;
         // Insert the object "in order".
#if 1
         while ((p != NULL) & (p <= ptr)) {
            prev = p;
            p = p->next;
         }
#endif
         if (prev == NULL) {
            ((freeObject *) ptr)->next = myFreeList;
            myFreeList = (freeObject *) ptr;
         } else {
            ((freeObject *) ptr)->next = prev->next;
            prev->next = (freeObject *) ptr;
         }
         assert (classInvariant());
      }

   private:

      int classInvariant (void) {
         return (((myFreeList == NULL) && (nObjects == 0))
               || ((myFreeList != NULL) && (nObjects > 0)));
      }

      class freeObject {
         public:
            freeObject * next;
      };

      freeObject * myFreeList;
      int nObjects;

};

#endif
