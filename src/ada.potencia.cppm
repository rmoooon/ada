export module ada.potencia;

import std;

export import ada.tipos;

export namespace ada {

u128 potencia_secuencial(u128 base, unsigned exponente) {
  u128 resultado = 1;
  for (unsigned i = 0; i < exponente; ++i) {
    resultado *= base;
  }
  return resultado;
}

u128 potencia_logaritmica(u128 base, unsigned exponente) {
  if (exponente == 0) {
    return 1;
  }
  const u128 mitad = potencia_logaritmica(base, exponente / 2);
  const u128 cuadrado = mitad * mitad;
  return exponente % 2 == 0 ? cuadrado : cuadrado * base;
}

}  // namespace ada
