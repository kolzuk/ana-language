//
// Created by kolzuk on 1/25/25.
//

#ifndef HEAP_H
#define HEAP_H

#include <cstdint>
#include <iostream>

struct Heap {
  struct HeapMemoryUnit {
    bool isAllocated = false;
    int64_t value = 0;
  };

  HeapMemoryUnit* heap;
  int64_t size;

  explicit Heap(int64_t size) {
    this->size = size;
    this->heap = new HeapMemoryUnit[size];
  }

  [[nodiscard]] int64_t AllocateMemory(int64_t neededMemory) const {
    neededMemory++;
    int64_t it1 = 0;
    while (it1 < size) {
      int64_t it2 = it1;

      while (it2 < size && !heap[it2].isAllocated && (it2 - it1) != neededMemory)
        it2++;

      if (it2 - it1 == neededMemory) {
        for (int64_t it = it1; it < it2; ++it)
          heap[it].isAllocated = true;
        heap[it1].value = neededMemory;
        return it1 + 1;
      }

      it1 = it2 + 1;
    }

    return -1;
  }

  void freeMemory(int64_t index) const {
    if (index <= 0 || index >= size) {
      std::cerr << "Index bound of heap size";
      return;
    }

    index--;
    int64_t blockSize = heap[index].value;

    for (int64_t it = index; it < index + blockSize; ++it) {
      heap[it].isAllocated = false;
      heap[it].value = 0;
    }
  }

  int64_t GetValueByIndex(int64_t index) const {
    if (index <= 0 || index >= size) {
      std::cerr << "Segfault";
      return -1;
    }

    return heap[index].value;
  }

  void SetValueByIndex(int64_t index, int64_t value) const {
    if (index <= 0 || index >= size) {
      std::cerr << "Segfault" << std::endl;
      return;
    }

    heap[index].value = value;
  }
};

#endif //HEAP_H
