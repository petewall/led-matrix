#pragma once

#include "StaticVisualization.h"

class Text : public StaticVisualization {
public:
  Text(const char* text, Display* display);

protected:
  virtual void render() override;

private:
  const char* text;
};
