#ifndef L1Portion_H
#define L1Portion_H

#include <vector>
using std::vector;
// class for portions organization

template <typename T> class L1Portion;

template <>
class L1Portion<L1TrackPar>
{
  public:
    typedef L1TrackPar T;
//     typedef vector<T> vType;
    typedef nsL1::vector<T>::TSimd vType;

    L1Portion():a(),dataSize(0){};
    L1Portion(int size):a(),dataSize(0) { reserve(size); };
    L1Portion(int size, int size2):a(),dataSize(size2) {
      reserve(size);
//     reserve2(size2);
    };
    vType& operator[] (int i) { return a[i];}
    void resize(int size) { a.resize(size); };
    void reserve(int size) { a.reserve(size); };
    void reserve2(int size) { for(unsigned int i = 0; i < a.size(); i++) a[i].reserve(size); };
    void push_back(vType &v) { a.push_back(v); };
    void add_void(){
      vType v;
//     v.resize(dataSize);
      a.push_back(v);
      a[a.size()-1].reserve(dataSize);
    };
    void add_void(int i) {
      T t;
      a[i].push_back(t);
    };

    int CalcSize() {
      int size = 0;
      for(unsigned int i = 0; i < a.size(); i++) size += a[i].size();
      return size*sizeof(T);
    };

  private:
    vector<vType> a;
//   int mainSize; // size of a
    int dataSize; // size of vType
};

template <>
    class L1Portion<L1FieldRegion>
{
  public:
    typedef L1FieldRegion T;
    typedef nsL1::vector<T>::TSimd vType;
//     typedef std::vector<T, nsL1::SimdAlloc<T> > vType;

    L1Portion():a(),dataSize(0){};
    L1Portion(int size):a(),dataSize(0) { reserve(size); };
    L1Portion(int size, int size2):a(),dataSize(size2) {
      reserve(size);
//     reserve2(size2);
    };

    vType& operator[] (int i) { return a[i];}
    void resize(int size) { a.resize(size); };
    void reserve(int size) { a.reserve(size); };
    void reserve2(int size) { for(unsigned int i = 0; i < a.size(); i++) a[i].reserve(size); };
    void push_back(vType &v) { a.push_back(v); };
    void add_void(){
      vType v;
//     v.resize(dataSize);
      a.push_back(v);
      a[a.size()-1].reserve(dataSize);
    };
    void add_void(int i) {
      T t;
      a[i].push_back(t);
    };

    int CalcSize() {
      int size = 0;
      for(unsigned int i = 0; i < a.size(); i++) size += a[i].size();
      return size*sizeof(T);
    };

  private:
    vector<vType> a;
//   int mainSize; // size of a
    int dataSize; // size of vType
};


template <typename T>
class L1Portion
{
 public:
  typedef vector<T> vType;

  L1Portion():a(),dataSize(0) { };
  L1Portion(int size):a(),dataSize(0) { reserve(size); };
  L1Portion(int size, int size2):a(),dataSize(size2) {
    reserve(size);
//     reserve2(size2);
  };
  vType& operator[] (int i) { return a[i];}
  void resize(int size) { a.resize(size); };
  void reserve(int size) { a.reserve(size); };
  void reserve2(int size) { for(int i = 0; i < a.size(); i++) a[i].reserve(size); };
  void push_back(vType &v) { a.push_back(v); };
  void add_void(){
    vType v;
//     v.resize(dataSize);
    a.push_back(v);
    a[a.size()-1].reserve(dataSize);
  };
  void add_void(int i) {
    T t;
    a[i].push_back(t);
  };

  int CalcSize() {
    int size = 0;
    for(int i = 0; i < a.size(); i++) size += a[i].size();
    return size*sizeof(T);
  };

 private:
  vector<vType> a;
//   int mainSize; // size of a
  int dataSize; // size of vType
};

#endif // L1Portion_H
