#pragma  once

class AbstractObjectManager {
public:
  virtual void *cloneObject(      const void *e) const = 0;
  virtual void  deleteObject(     const void *e) const = 0;
  virtual AbstractObjectManager *clone() const = 0;
  virtual ~AbstractObjectManager() {}
};

template <class E> class ObjectManager : public AbstractObjectManager {
public:
  void *cloneObject(const void *e) const {
    E *e1 = new E(*(E*)e); TRACE_NEW(e1); return e1;
  }
  void  deleteObject(const void *e) const {
    E *e1 = (E*)e; SAFEDELETE(e1);
  }
  AbstractObjectManager *clone() const {
    return new ObjectManager<E>();
  }
};
