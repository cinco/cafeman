#include <fox-1.6/fx.h>
#include <fox-1.6/FXRex.h>

int
isDate(char *string)
{
  FXRex notdate("([^0-9/])|(\\d{3,})|(\\d*/\\d*/\\d*/)", REX_NORMAL);

  if (notdate.match(string))
    return 0;

  return 1;
}

int
isTime(char *string)
{
  FXRex nottime("([^0-9:])|(\\d{3,})|(\\d*:\\d*:\\d*)", REX_NORMAL);

  if (nottime.match(string))
    return 0;

  return 1;
}

int
isPrice(char *string)
{
  FXRex notprice("[^0-9.]");

  if (notprice.match(string))
    return 0;

  return 1;
}
