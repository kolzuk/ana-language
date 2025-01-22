#ifndef JIT_JIT_GARBAGECOLLECTOR_H_
#define JIT_JIT_GARBAGECOLLECTOR_H_

#include <cstdint>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

class GarbageCollector {

  struct Variable {
    int64_t Value;
    char* Name;
  };

  std::vector<Variable> Stack;
  std::vector<int64_t*> Allocations;
 public:

  int64_t* getVariablePtr(char* Name) {
    for (int i = Stack.size() - 1; i >= 0; i--) {
      if (strcmp(Stack[i].Name, Name) == 0) {
        return &Stack[i].Value;
      }
    }
    return nullptr;
  }

  void newInt(char* Name) {
    Stack.push_back(Variable{0, Name});
  }

  void newPtr(char* Name) {
    Stack.push_back(Variable{0, Name});
  }

  void newArrayAlloc(char* Name, int64_t Size) {
    auto* Ptr = new int64_t[Size];
    Allocations.push_back(Ptr);
    Stack.push_back(Variable{reinterpret_cast<int64_t>(Ptr), Name});
  }

  // TODO
  void newScope() {
  }

  // TODO
  void destroyScope() {
  }

  ~GarbageCollector() {
    Stack.clear();
    for (auto i : Allocations) {
      delete[] i;
    }
  }

};

#endif //JIT_JIT_GARBAGECOLLECTOR_H_
