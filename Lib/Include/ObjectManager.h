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
    return new E(*(E*)e);
  }
  void  deleteObject(const void *e) const {
    delete (E*)e;
  }
  AbstractObjectManager *clone() const {
    return new ObjectManager<E>();
  }

};

