#include "config.h"
#include "mutt/lib.h"

struct NeoMutt
{
  int dummy;
};

struct NeoMutt *NeoMutt = NULL;

struct ConfigSubset;
const char *cs_subset_path(const struct ConfigSubset *sub, const char *name)
{
  return NULL;
}

struct Fruit
{
  const char *apple;
  const char *banana;
};

struct Animal
{
  const char *cat;
  const char *dog;
};

typedef bool (*opt_callback_t)(const char *option, void *data, struct Buffer *buf);

struct Option
{
  const char *option; // "a" to represent `%a`, "algorithm" to represent `%{algorithm}`
  opt_callback_t callback; // Callback function(struct option *opt)
  void *data;              // Private data for callback
};

bool callback_fruit(const char *option, void *data, struct Buffer *buf)
{
  struct Fruit *fruit = data;
  if (mutt_str_equal(option, "a"))
  {
    buf_addstr(buf, fruit->apple);
    return true;
  }

  if (mutt_str_equal(option, "b"))
  {
    buf_addstr(buf, fruit->banana);
    return true;
  }

  return false;
}

bool callback_animal(const char *option, void *data, struct Buffer *buf)
{
  struct Animal *animal = data;
  if (mutt_str_equal(option, "c"))
  {
    buf_addstr(buf, animal->cat);
    return true;
  }

  if (mutt_str_equal(option, "d"))
  {
    buf_addstr(buf, animal->dog);
    return true;
  }

  return false;
}

void expando(struct Buffer *buf, const char *fmt, struct Option *callback)
{
  struct Buffer buf_callback = buf_make(1024);

  for (const char *ch = fmt; *ch; ch++)
  {
    if (*ch == '%')
    {
      bool result = false;
      ch++;
      for (size_t i = 0; callback[i].option; i++)
      {
        if (*ch == callback[i].option[0])
        {
          buf_reset(&buf_callback);
          result = callback[i].callback(callback[i].option, callback[i].data, &buf_callback);
          if (result)
          {
            buf_addstr(buf, "\033[1;32m");
            buf_addstr(buf, buf_callback.data);
            buf_addstr(buf, "\033[m");
          }

          break;
        }
      }

      if (!result)
      {
        buf_addch(buf, '%');
        buf_addch(buf, *ch);
      }
      continue;
    }

    buf_addch(buf, *ch);
  }

  buf_dealloc(&buf_callback);
}

int main(int argc, char *argv[])
{
  struct Fruit fruit = { "apple", "banana" };
  struct Animal animal = { "cat", "dog" };

  if (argc > 1)
    fruit.apple = argv[1];
  if (argc > 2)
    fruit.banana = argv[2];
  if (argc > 3)
    animal.cat = argv[3];
  if (argc > 4)
    animal.dog = argv[4];

  struct Option opts[] = { { "a", callback_fruit, &fruit },
                           { "b", callback_fruit, &fruit },
                           { "c", callback_animal, &animal },
                           { "d", callback_animal, &animal },
                           { NULL } };

  struct Buffer buf = buf_make(1024);

  const char *fmt = "pre %a %b %x %c %d post";
  expando(&buf, fmt, (struct Option *) &opts);

  printf("format: %s\n", fmt);
  printf("result: %s\n", buf.data);

  buf_dealloc(&buf);
  return 0;
}
