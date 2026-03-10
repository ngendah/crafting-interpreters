#include <assert.h>
#include <stdlib.h>

#include "string.h"

typedef struct {
  double value;
  bool err;
} strtd_r;

extern strtd_r strtd(const char *str, size_t length);

void test_strtd() {
  {
    auto sval = _("");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == 0);
    assert(val.err == true);
  }
  {
    auto sval = _("1231");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == 1231);
    assert(val.err == false);
  }
  {
    auto sval = _("-1231");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == -1231);
    assert(val.err == false);
  }
  {
    auto sval = _("1231.");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == 1231);
    assert(val.err == false);
  }
  {
    auto sval = _(".1231");
    auto val = strtd(sval.str, sval.length);
    assert((float)val.value == (float)0.1231);
    assert(val.err == false);
  }
  {
    auto sval = _("123.13");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == 123.13);
    assert(val.err == false);
  }
  {
    auto sval = _("12.31");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == 12.31);
    assert(val.err == false);
  }
  {
    auto sval = _("1.231");
    auto val = strtd(sval.str, sval.length);
    assert((float)val.value == (float)1.231);
    assert(val.err == false);
  }
  {
    auto sval = _("123.1");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == 123.1);
    assert(val.err == false);
  }
  {
    auto sval = _("-0.001231");
    auto val = strtd(sval.str, sval.length);
    assert((float)val.value == (float)-0.001231);
    assert(val.err == false);
  }
  {
    auto sval = _("-0.00.1231");
    auto val = strtd(sval.str, sval.length);
    assert((float)val.value == (float)-0.1231);
    assert(val.err == true);
  }
  {
    auto sval = _("-0.avbc1231");
    auto val = strtd(sval.str, sval.length);
    assert((float)val.value == (float)-0.00001231);
    assert(val.err == true);
  }
  {
    auto sval = _("avbc1231");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == 1231);
    assert(val.err == true);
  }
  {
    auto sval = _("1231adad");
    auto val = strtd(sval.str, sval.length);
    assert(val.value == 12310000);
    assert(val.err == true);
  }
}

int main() {
  test_strtd();
  return EXIT_SUCCESS;
}
