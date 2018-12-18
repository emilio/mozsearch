#include "templates5.h"

namespace {

struct Bar {
  int m_baz = 0;
};

}

static nsTArray<nsTArray<Bar>>* FOO;

bool somethingElse() {
  nsTArray<Bar> foo;
  return foo.AppendElement(45.5);
}

bool func() {
  nsTArray<int> intarray(3);
  nsTArray<float> floatarray(3.5);
  return intarray.Contains(5, 3) ||
         floatarray.Contains(3.5, 5);
}
