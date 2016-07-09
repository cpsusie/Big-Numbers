#pragma once

typedef enum {
  PP_2DFUNCTION
 ,PP_ISOSURFACE
 ,PP_ISOCURVE
} PersistentParameterType;

class PersistentParameter {
private:
  String m_name;
protected:
  static String readString( FILE *f);
  static void   writeString(FILE *f, const String &str);
  static String readLine(FILE *f);
public:
  PersistentParameter() : m_name("Untitled") {
  }
  void load(const String &fileName);
  void save(const String &fileName);
  virtual void read( FILE *f) = 0;
  virtual void write(FILE *f) = 0;
  String getName() const {
    return m_name;
  }
  void setName(const String &name) {
    m_name = name;
  }
  bool hasDefaultName() const {
    return m_name == "Untitled";
  }
  String getDisplayName() const;
  virtual PersistentParameterType getType() const = 0;
};
