export module ada.fibonacci;

import std;

export import ada.tipos;
export import ada.matriz;

export namespace ada {

u128 fib_secuencial(unsigned n) {
  u128 anterior = 0;
  u128 actual = 1;
  for (unsigned i = 1; i < n; ++i) {
    const u128 siguiente = anterior + actual;
    anterior = actual;
    actual = siguiente;
  }
  return (n == 0) ? 0 : actual;
}

u128 fib_logaritmico(unsigned n) {
  Mat2 base;
  base.en(0, 0) = 1;
  base.en(0, 1) = 1;
  base.en(1, 0) = 1;
  return potencia_mat2(base, n).en(0, 1);
}

}  // namespace ada
