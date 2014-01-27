#line 1
#include "StaticWarning.h"
STATIC_WARNING(1==2, "Failed with 1 and 2");
STATIC_WARNING(1<2, "Succeeded with 1 and 2");

struct Foo
{
  STATIC_WARNING(2==3, "2 and 3: oops");
  STATIC_WARNING(2<3, "2 and 3 worked");
};

void func()
{
  STATIC_WARNING(3==4, "Not so good on 3 and 4");
  STATIC_WARNING(3<4, "3 and 4, check");
}

template <typename T> struct wrap
{
  typedef T type;
  STATIC_WARNING(4==5, "Bad with 4 and 5");
  STATIC_WARNING(4<5, "Good on 4 and 5");
  STATIC_WARNING_TEMPLATE(WRAP_WARNING1, 4==5, "A template warning");
};

template struct wrap<int>;

