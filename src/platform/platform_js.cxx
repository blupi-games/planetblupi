
#include <emscripten.h>
#include <functional>

#include "../blupi.h"
#include "../event.h"
#include "../platform.h"

std::function<void(const SDL_Event &)> Platform::handleEvent;

Platform::Type
Platform::getType ()
{
  return JS;
}

void
Platform::run (std::function<void(const SDL_Event &)> handleEvent)
{
  Platform::handleEvent = handleEvent;
  Platform::timer (nullptr);
  emscripten_set_main_loop (
    []() {
      SDL_Event event;
      while (SDL_PollEvent (&event))
      {
        Platform::handleEvent (event);
        if (event.type == SDL_QUIT)
          break;
      }
    },
    0, 1);
}

void
Platform::timer (void *)
{
  CEvent::PushUserEvent (EV_UPDATE);
  emscripten_async_call (Platform::timer, nullptr, g_timerInterval);
}
