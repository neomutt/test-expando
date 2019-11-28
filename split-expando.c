#include "config.h"
#include "mutt/mutt.h"

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
  if (mutt_str_strcmp(option, "a") == 0)
  {
    mutt_buffer_addstr(buf, fruit->apple);
    return true;
  }

  if (mutt_str_strcmp(option, "b") == 0)
  {
    mutt_buffer_addstr(buf, fruit->banana);
    return true;
  }

  return false;
}

bool callback_animal(const char *option, void *data, struct Buffer *buf)
{
  struct Animal *animal = data;
  if (mutt_str_strcmp(option, "c") == 0)
  {
    mutt_buffer_addstr(buf, animal->cat);
    return true;
  }

  if (mutt_str_strcmp(option, "d") == 0)
  {
    mutt_buffer_addstr(buf, animal->dog);
    return true;
  }

  return false;
}

void expando(struct Buffer *buf, const char *fmt, struct Option *callback)
{
  struct Buffer buf_callback = mutt_buffer_make(1024);

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
          mutt_buffer_reset(&buf_callback);
          result = callback[i].callback(callback[i].option, callback[i].data, &buf_callback);
          if (result)
          {
            mutt_buffer_addstr(buf, "\033[1;32m");
            mutt_buffer_addstr(buf, buf_callback.data);
            mutt_buffer_addstr(buf, "\033[m");
          }

          break;
        }
      }

      if (!result)
      {
        mutt_buffer_addch(buf, '%');
        mutt_buffer_addch(buf, *ch);
      }
      continue;
    }

    mutt_buffer_addch(buf, *ch);
  }

  mutt_buffer_dealloc(&buf_callback);
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

  struct Buffer buf = mutt_buffer_make(1024);

  const char *fmt = "pre %a %b %x %c %d post";
  expando(&buf, fmt, (struct Option *) &opts);

  printf("format: %s\n", fmt);
  printf("result: %s\n", buf.data);

  mutt_buffer_dealloc(&buf);
  return 0;
}
