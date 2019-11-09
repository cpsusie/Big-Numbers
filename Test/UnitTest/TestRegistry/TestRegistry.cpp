#include "stdafx.h"
#include <Registry.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestRegistry {

#include <UnitTestTraits.h>

  class InitRegistry {
  public:
    InitRegistry();
    ~InitRegistry();
    static RegistryKey getRoot() {
      return RegistryKey(HKEY_CURRENT_USER, "Software").createOrOpenPath(_T("TestRegistry"));
    }
    static RegistryKey getTestKey() {
      return getRoot().createOrOpenPath(_T("Test"));
    }
  };

  InitRegistry::InitRegistry() {
    try {
      RegistryKey key = getTestKey();
      for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
          key.createOrOpenPath(format(_T("subdirI%02d\\subdirJ%02d"), i, j));
        }
      }
      INFO(_T("key.fullName:<%s>"), key.name().cstr());
    } catch (Exception e) {
      OUTPUT(_T("Exception in InitRegistry:%s"), e.what());
      verify(false);
    }
  }

  InitRegistry::~InitRegistry() {
    RegistryKey key0(HKEY_CURRENT_USER, _T("Software"));
    try {
      key0.deleteKey(_T("TestRegistry"));
      verify(false);
    }  catch (Exception e) {
      // ignore
    }
    try {
      key0.deleteKeyAndSubkeys(_T("TestRegistry"));
    } catch (Exception e) {
      OUTPUT(_T("Exception in ~InitRegistry:%s"), e.what());
      verify(false);
    }
  }

  static InitRegistry dummy;

  class RegValueArray : public Array<RegistryValue> {
  public:
    void load();
    void save();
    void listValues() const;
    void removeValues();
  };

  void RegValueArray::load() {
    try {
      RegistryKey key = InitRegistry::getTestKey();
      clear();
      for(Iterator<RegistryValue> it = key.getValueIterator(); it.hasNext();) {
        add(it.next());
      }
    } catch (Exception e) {
      OUTPUT(_T("Exception:%s"), e.what());
    }
  }

  void RegValueArray::save() {
    try {
      RegistryKey key = InitRegistry::getTestKey();
      key.deleteValues();
      for(size_t i = 0; i < size(); i++) {
        key.setValue((*this)[i]);
      }
    } catch(Exception e) {
      OUTPUT(_T("Exception:%s"), e.what());
    }
  }

  void RegValueArray::listValues() const {
    RegistryKey key = InitRegistry::getTestKey();
    for(Iterator<RegistryValue> it = key.getValueIterator(); it.hasNext();) {
      const RegistryValue &value = it.next();
      INFO(_T("type:%-10s name=%-20s. value=<%s>"), value.typeName().cstr(), value.name().cstr(), value.toString().cstr());
    }
  }

  void RegValueArray::removeValues() {
    RegistryKey key = InitRegistry::getTestKey();
    for(Iterator<RegistryValue> it = key.getValueIterator(); it.hasNext();) {
      const RegistryValue value = it.next();
      INFO(_T("removing value %-20s"), value.name().cstr());
      it.remove();
    }
  }

  static Array<String> registryErrors;

  void traverseRegistryKey(RegistryKey &key, int level = 0) {
    OUTPUT(_T("%*.*sKey:%s"), level, level, EMPTYSTRING, key.name().cstr());
    bool headerDone = false;
    for(Iterator<RegistryValue> itv = key.getValueIterator(); itv.hasNext();) {
      if(!headerDone) {
        OUTPUT(_T("  %*.*s%-20s %-12.12s %s"), level, level, EMPTYSTRING, _T("Name"), _T("Type"), _T("Data"));
        headerDone = true;
      }
      const RegistryValue &value = itv.next();
      OUTPUT(_T("  %*.*s%-20.20s %-12.12s %-70.70s"), level, level, EMPTYSTRING, value.name().cstr(), value.typeName().cstr(), value.toString().cstr());
    }
    for(Iterator<String> it = key.getSubKeyIterator(); it.hasNext();) {
      String child = it.next();
      try {
        traverseRegistryKey(key.openKey(child), level + 2);
      } catch (Exception e) {
        registryErrors.add(format(_T("%s:%s\\%s"), e.what(), key.name().cstr(), child.cstr()));
      }
    }
  }

	TEST_CLASS(TestRegistry) {
    public:

    TEST_METHOD(TestRegistryValue)  {
      unsigned char bytes[] = { 0,1,2,3,4,5,6,7,8,9,10,0,0 };
      RegistryValue binaryValue(_T("binaryFelt"), bytes, ARRAYSIZE(bytes));
      StringArray strings;
      for(int i = 0; i < 10; i++) {
        strings.add(format(_T("string %d"), i));
      }
      RegistryValue multiStringValue(_T("multiStringFelt"), strings);

      RegValueArray test;
      test.add(RegistryValue(_T("stringFelt"  ), _T("fisk")));
      test.add(RegistryValue(_T("longFelt"    ), 0x12345678));
      test.add(RegistryValue(_T("longlongFelt"), 0x123456789abcdef));
      test.add(binaryValue);
      test.add(multiStringValue);
      test.save();

      test.clear();

//      traverseRegistryKey(InitRegistry::getRoot());

      test.load();
      test.listValues();
      test.removeValues();
    }

    template<class T> class ItemField {
    private:
      const String m_name;
      const T      m_value;
    public:
      ItemField(const String &name, T value) : m_name(name), m_value(value) {
      }
      const String &name() const {
        return m_name;
      }
      const T &value() const {
        return m_value;
      }
      bool operator==(const ItemField<T> &r) const {
        return (m_name == r.m_name) && (m_value == r.m_value);
      }
      bool operator!=(const ItemField<T> &r) const {
        return !(*this == r);
      }
    };

    TEST_METHOD(TestRegistrySetGet) {
      RegistryKey key = InitRegistry::getTestKey();
      ItemField<bool>   boolValue(  "bool"  , true       );
      ItemField<short>  shortValue( "short" , -17300     );
      ItemField<USHORT> ushortValue("ushort", 45034      );
      ItemField<int>    intValue(   "int"   , -2000012300);
      ItemField<UINT>   uintValue(  "uint"  , 4003450698 );
      ItemField<INT64>  int64Value( "int64" , 0x1234567890abcdef);
      ItemField<UINT64> uint64Value("uint64", 0x890abcdef1234567);
      ItemField<String> stringValue("string", "fisk"     );

      const char str[] = "this is a string with some useless text";
      const ByteArray ba((BYTE*)str, strlen(str)+1);
      ItemField<ByteArray> byteArrayValue("byteArray", ba);
      RegistryValue regValue(_T("regval"), ba.getData(), (UINT)ba.size());

      StringArray strings;
      for (int i = 0; i < 10; i++) {
        strings.add(format(_T("string %d"), i));
      }
      RegistryValue multiStringValue(_T("multiStringFelt"), strings);

      for(int i = 1; i <= 10; i++) {
        const String itemName = format(_T("item%02d"), i);
        RegistryKey item = key.createOrOpenKey(itemName);
        item.setValue(boolValue.name()     , boolValue.value(  ));
        item.setValue(shortValue.name()    , shortValue.value( ));
        item.setValue(ushortValue.name()   , ushortValue.value());
        item.setValue(intValue.name()      , intValue.value(   ));
        item.setValue(uintValue.name()     , uintValue.value(  ));
        item.setValue(int64Value.name()    , int64Value.value() );
        item.setValue(uint64Value.name()   , uint64Value.value());
        item.setValue(stringValue.name()   , stringValue.value());
        item.setValue(byteArrayValue.name(), byteArrayValue.value().getData(), (DWORD)byteArrayValue.value().size());
        item.setValue(regValue);
        item.setValue(multiStringValue);
      }

      BitSet usedIndexSet(30);
      for(Iterator<String> it = key.getSubKeyIterator(); it.hasNext();) {
        const String name = it.next();
        if((name.length() == 6) && (substr(name,0,4) == "item")) {
          const int index = _wtoi(name.cstr() + 4);
          verify((index >= 1) && (index <= 10));
          verify(!usedIndexSet.contains(index));
          usedIndexSet.add(index);
          RegistryKey item = key.createOrOpenKey(name);
          verify(item.getBool(  boolValue.name()  , false) == true               );
          verify(item.getShort( shortValue.name() , 0)     == shortValue.value() );
          verify(item.getUshort(ushortValue.name(), 0)     == ushortValue.value());
          verify(item.getInt(   intValue.name()   , 0)     == intValue.value()   );
          verify(item.getUint(  uintValue.name()  , 0)     == uintValue.value()  );
          verify(item.getInt64( int64Value.name() , 0)     == int64Value.value() );
          verify(item.getUint64(uint64Value.name(), 0)     == uint64Value.value());
          verify(item.getString(stringValue.name(), "")    == stringValue.value());

          RegistryValue tmpByteArrayRegValue;
          item.getValue(byteArrayValue.name(), tmpByteArrayRegValue);

          verify(tmpByteArrayRegValue.name() == byteArrayValue.name()        );
          verify(tmpByteArrayRegValue.getValueInfo().type() == REG_BINARY    );
          const ByteArray tmpba = tmpByteArrayRegValue;
          verify(tmpba == byteArrayValue.value());

          RegistryValue tmpRegValue;
          verify(item.getValue(regValue.name(), tmpRegValue) == regValue);

          RegistryValue tmpMultiStringValue;
          verify(item.getValue(multiStringValue.name(), tmpMultiStringValue) == multiStringValue);
        }
      }
      verify(usedIndexSet.size() == 10);

//      traverseRegistryKey(InitRegistry::getRoot());

      for(Iterator<String> it = key.getSubKeyIterator(); it.hasNext();) {
        const String name = it.next();
        if((name.length() == 6) && (substr(name, 0, 4) == "item")) {
          it.remove();
        }
      }
    }
  };
}
