class Alloc {};

template<typename T>
class nsTArray {
public:
  nsTArray() : myT(T()) {}
  nsTArray(const T& aT) : myT(aT) {}

  template<typename Foo = T>
  bool Contains(T aT, Foo foo) {
    return myT == aT || myT == foo;
  }

  template<typename U, typename Alloc = Alloc>
  bool AppendElement(U) {
    return false;
  }
private:
  T myT;
};
