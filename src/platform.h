
#pragma once

class Platform
{
private:
  static std::function<void(const SDL_Event &)> handleEvent;

private:
  static void timer (void *);

public:
  enum Type { JS, SDL };

  static Platform::Type getType ();
  static void run (std::function<void(const SDL_Event &)> handleEvent);
};
