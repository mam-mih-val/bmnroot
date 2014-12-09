#ifndef _L1HitPoint_h_
#define _L1HitPoint_h_

		/// contain strips positions and coordinates of hit
#if 1
struct L1HitPoint{
L1HitPoint() :     x(0.), y(0.), z(0.), u(0.), v(0.), du(0.), dv(0.), n(0){};
L1HitPoint(fscal x_, fscal y_, fscal z_, fscal v_, fscal u_, fscal dv_ = 0.0, fscal du_ = 0.0, unsigned short int n_ = 0):
  x(x_), y(y_), z(z_), u(u_), v(v_), du(du_), dv(dv_), n(n_){};

  fscal Xs() const { return X()/Z(); }
  fscal Ys() const { return Y()/Z(); } // value to sort hits by

  fscal X() const { return x; }
  fscal Y() const { return y; }
  fscal Z() const { return z; }
  fscal U() const { return u; }
  fscal V() const { return v; }
  fscal DU() const { return du; }
  fscal DV() const { return dv; }

  unsigned short int N() const { return n; }
 private:
  
  float x, y;
  float z; // TODO: may be we should use iz
  float u, v; // x\u, v - front and back strips; x, y, z - coordinates of hits
  float du, dv; // du, dv - front and back coordinate errors 
        
  unsigned short int n; // number of event
};
#else

static const float R = 60;
static const float shortPackingConstant = 2 * R / 65535.f;
static const float MZ = 110;
static const float shortPackingConstantZ = MZ / 65535.f;

		/// contain strips positions and coordinates of hit
struct L1HitPoint{
  L1HitPoint(){};
  L1HitPoint(fscal x_, fscal y_, fscal z_, fscal v_, fscal u_, unsigned short int n_ = 0): x(f2s(x_)), y(f2s(y_)), z(f2sZ(z_)), u(f2s(u_)), v(f2s(v_)), n(n_){};

  fscal Xs() const { return X()/Z(); }
  fscal Ys() const { return Y()/Z(); } // value to sort hits by

  fscal X() const { return s2f(x); }
  fscal Y() const { return s2f(y); }
  fscal Z() const { return s2fZ(z); }
  fscal U() const { return s2f(u); }
  fscal V() const { return s2f(v); }

  unsigned short int N() const { return n; }
 private:
  //unsigned short int
  unsigned short int f2s(float f) const {
    return (f + R)/shortPackingConstant;
  }
  float s2f(unsigned short int f) const {
    return (float(f)+0.5)*shortPackingConstant - R;
  }

  unsigned short int f2sZ(float f) const {
    return (f)/shortPackingConstantZ;
  }
  float s2fZ(unsigned short int f) const {
    return (float(f)+0.5)*shortPackingConstantZ;
  }
  
  unsigned short int x, y;
  unsigned short int z; // TODO: may be we should use iz
  unsigned short int u, v; // x\u, v - front and back strips; x, y, z - coordinates of hits
        
  unsigned short int n; // number of event
};

#endif

#endif


