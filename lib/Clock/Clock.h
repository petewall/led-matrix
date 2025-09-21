#pragma once

#include "Visualization.h"
#include "Display.h"

class Clock : public Visualization {
public:
  // Refresh every 500ms so the colon can blink
  explicit Clock(Display* display);

protected:
  bool run() override;
  void render() override;

private:
  bool colonOn;
  bool timeInitialized;

  void initTimeOnce();
  void drawString(const char* s);
};

