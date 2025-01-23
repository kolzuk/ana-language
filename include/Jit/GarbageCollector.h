#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <cstdint>
#include <vector>

class GarbageCollector {

  std::vector<int64_t*> Allocations;
  int64_t* BasePtr;
 public:

  int64_t* newArrayAlloc(int64_t Size) {
    auto* Ptr = new int64_t[Size];
    for (int i = 0; i < Size; i++) {
      Ptr[i] = 0;
    }
    Allocations.push_back(Ptr);
    return Ptr;
  }

  void initStack(int64_t* StackPtr) {
    BasePtr = StackPtr;
  }

  void collect(int64_t* StackPtr) {
    bool IsMarked[Allocations.size()];
    for (auto& Flag : IsMarked) {
      Flag = false;
    }
    for (auto Ptr = StackPtr; Ptr <= BasePtr; ++Ptr) {
      for (int i = 0; i < Allocations.size(); i++) {
        if (reinterpret_cast<int64_t>(Allocations[i]) == *Ptr) {
          IsMarked[i] = true;
          break;
        }
      }
    }

    std::vector<int64_t*> NewHeap;
    for (int i = 0; i < Allocations.size(); i++) {
      if (IsMarked[i]) {
        NewHeap.push_back(Allocations[i]);
      } else {
        delete[] Allocations[i];
      }
    }
    Allocations = NewHeap;
  }

  ~GarbageCollector() {
    for (auto i : Allocations) {
      delete[] i;
    }
  }

};

#endif //GARBAGE_COLLECTOR_H
