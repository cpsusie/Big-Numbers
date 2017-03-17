#include "stdafx.h"

static bool itemsEqual(const KeyFileDefinition &keydef, const KeyPageItem &i1, const KeyPageItem &i2) {
  return keydef.keyCmp(i1.m_key,i2.m_key) == 0 && i1.m_child == i2.m_child;
}

static void testKeyPageBasic() {
  _tprintf(_T("testKeyPageBasic\n"));

  KeyPage page(10);
  page.init(false);

  verify(page.getItemCount() == 0);
  verify(page.getItemSize() > 0);
  verify(page.getHalfSize() > 0);
  verify(page.getPageSize() > 0);
  verify(page.getPageSize() <= KEYPAGESIZE);
  verify(page.getMaxItemCount() > 0);
  page.setItemCount(10);
  verify(page.getItemCount() == 10);
  page.setItemCount(page.getMaxItemCount());
  verify(page.getItemCount() == page.getMaxItemCount());

  for(int i = 0; i <= page.getItemCount(); i++) {
    page.setChild(i,i+1);
    verify(page.getChild(i) == i+1);
  }
#ifdef _DEBUG
  try {
    page.setItemCount(page.getMaxItemCount()+1);
    verify(false);
  } catch(sqlca ca) {
    verify(ca.sqlcode == SQL_FATAL_ERROR);
  }
  try {
    page.setChild(page.getMaxItemCount()+1,10);
    verify(false);
  } catch(sqlca ca) {
    verify(ca.sqlcode == SQL_FATAL_ERROR);
  }
#endif
}

static void testKeyPageSetGetItem() {
  _tprintf(_T("testKeyPageSetGetItem\n"));

  TestFields fields;

  KeyPage keyPage(fields.getSize());

  CompactArray<KeyPageItem> itemList;
  keyPage.setItemCount(keyPage.getMaxItemCount()-1);
  UINT i;
  for(i = 1; i <= keyPage.getItemCount(); i++) {
    KeyPageItem item;
    item.m_key = fields.getKey();
    item.m_child = i;
    itemList.add(item);
    keyPage.setItem(i,item);
    fields.incrAll();
  }

#ifdef _DEBUG
  try {
    KeyPageItem item;
    item.m_key = fields.getKey();
    item.m_child = i;
    keyPage.setItem(i,item);

    verify(false);
  } catch(sqlca ca) {
    verify(ca.sqlcode == SQL_FATAL_ERROR);
  }
#endif

  keyPage.itemCountIncr();
  KeyPageItem item;
  item.m_key = fields.getKey();
  item.m_child = i;
  keyPage.setItem(i,item);

#ifdef _DEBUG
  try {
    keyPage.itemCountIncr();
    verify(false);
  } catch(sqlca ca) {
    verify(ca.sqlcode == SQL_FATAL_ERROR);
  }
#endif

  for(i = 0; i < itemList.size(); i++) {
    KeyPageItem item;
    keyPage.getItem(i+1, item);
    if (!itemsEqual(fields, item, itemList[i])) {
      _tprintf(_T("%s"), keyPage.toString(fields).cstr());
      _tprintf(_T("pageitem[%u]:%s\n"), i+1, item.toString(fields).cstr());
      _tprintf(_T("itemList[%u]:%s\n"), i  , itemList[i].toString(fields).cstr());
      PAUSE();
    }
//  verify( itemsEqual(fields,item,itemList[i]) );
  }

#ifdef _DEBUG
  try {
    KeyPageItem item;
    keyPage.setItem(0, item);
    verify(false);
  } catch(sqlca ca) {
    verify(ca.sqlcode == SQL_FATAL_ERROR);
  }
  try {
    KeyPageItem item;
    keyPage.setItem(keyPage.getMaxItemCount()+1,item);
    verify(false);
  } catch(sqlca ca) {
    verify(ca.sqlcode == SQL_FATAL_ERROR);
  }
#endif
}

static void testKeyPageInsertDeleteItem() {
  _tprintf(_T("testKeyPageInsertDeleteItem\n"));

  TestFields fields;

  KeyPage keyPage(fields.getSize());
  keyPage.init(false);

  CompactArray<KeyPageItem> itemList;
  int insertPoint = 1;
  for(UINT i = 0; i < keyPage.getMaxItemCount(); i++) {
    KeyPageItem item;
    item.m_key = fields.getKey();
    item.m_child = i + 1;
    itemList.add(insertPoint-1,item);
    keyPage.insertItem(insertPoint,item);
    fields.incrAll();
    if(i % 3 == 0)
      insertPoint++;
  }

  for(UINT i = 0; i < itemList.size(); i++) {
    KeyPageItem item;
    keyPage.getItem(i+1, item);

    if (!itemsEqual(fields, item, itemList[i])) {
      _tprintf(_T("%s"), keyPage.toString(fields).cstr());
      _tprintf(_T("pageitem[%u]:%s\n"), i+1, item.toString(fields).cstr());
      _tprintf(_T("itemList[%u]:%s\n"), i  , itemList[i].toString(fields).cstr());
      PAUSE();
    }
    verify( itemsEqual(fields,item,itemList[i]) );
  }

#ifdef _DEBUG
  try {
    KeyPageItem item;
    item.m_key = fields.getKey();
    keyPage.insertItem(1,item);
    verify(false);
  } catch(sqlca ca) {
    verify(ca.sqlcode == SQL_FATAL_ERROR);
  }
#endif

  while(keyPage.getItemCount() > 10) {
    keyPage.removeItem(1);
    itemList.remove(0);
    KeyPageItem item;
    keyPage.getItem(1, item);
    verify( itemsEqual(fields,item,itemList[0]));
    keyPage.getItem(keyPage.getItemCount(), item);
    verify( itemsEqual(fields,item,itemList.last()));
  }
  while(keyPage.getItemCount() > 5) {
    keyPage.removeItem(3);
    itemList.remove(2);
    KeyPageItem item;
    keyPage.getItem(1, item);
    verify( itemsEqual(fields,item,itemList[0]));
    keyPage.getItem(keyPage.getItemCount(), item);
    verify( itemsEqual(fields,item,itemList.last()));
  }
  keyPage.removeItem(keyPage.getItemCount());
  itemList.removeLast();
  KeyPageItem item;
  keyPage.getItem(1, item);
  verify( itemsEqual(fields,item,itemList[0]));
  keyPage.getItem(keyPage.getItemCount(), item);
  verify( itemsEqual(fields,item,itemList.last()));

  while(keyPage.getItemCount() > 0) {
    keyPage.removeItem(1);
  }

#ifdef _DEBUG
  try {
    keyPage.removeItem(1);
    verify(false);
  } catch(sqlca ca) {
    verify(ca.sqlcode == SQL_FATAL_ERROR);
  }
#endif
}

static void testKeyPageCopyItems() {
  _tprintf(_T("testKeyPageCopyItems\n"));

  TestFields fields;

  KeyPage keyPage1(fields.getSize());

  for(UINT i = 0; i < keyPage1.getMaxItemCount(); i++) {
    KeyPageItem item;
    item.m_key = fields.getKey();
    item.m_child = i + 1;
    keyPage1.insertItem(1, item);
    fields.incrAll();
  }
  KeyPage keyPage2(fields.getSize());
  keyPage2.setItemCount(10);
  keyPage2.copyItems(1,10,keyPage1,5);
  for(UINT i = 1; i < 10; i++) {
    KeyPageItem item1, item2;
    verify(itemsEqual(fields, keyPage2.getItem(i, item2), keyPage1.getItem(i+4, item1)));
  }
}

void testKeyPage() {
  _tprintf(_T("testKeyPage\n"));

  testKeyPageBasic();
  testKeyPageSetGetItem();
  testKeyPageInsertDeleteItem();
  testKeyPageCopyItems();
}
