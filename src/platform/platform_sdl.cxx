
#include "../blupi.h"
#include "../event.h"
#include "../platform.h"

std::function<void(const SDL_Event &)> Platform::handleEvent;

Platform::Type
Platform::getType ()
{
  return SDL;
}

void
Platform::run (std::function<void(const SDL_Event &)> handleEvent)
{
  SDL_TimerID updateTimer = SDL_AddTimer (
    g_timerInterval,
    [](Uint32 interval, void * param) -> Uint32 {
      CEvent::PushUserEvent (EV_UPDATE);
      return interval;
    },
    nullptr);

  SDL_Event event;
  while (SDL_WaitEvent (&event))
  {
    handleEvent (event);
    if (event.type == SDL_QUIT)
      break;
  }

  SDL_RemoveTimer (updateTimer);
}

void
Platform::timer (void *)
{
}
