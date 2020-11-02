#pragma once

class String;
class TemplateWriter;

class KeywordHandler {
public:
  virtual void handleKeyword(TemplateWriter &writer, String &line) const = 0;
  virtual ~KeywordHandler() {
  }
};
