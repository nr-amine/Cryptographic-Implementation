import os

class Curve:
    def __init__(self, a, b, p):
        self.a = a
        self.b = b
        self.p = p

def normaliser(val, mod):
    res = val % mod
    return res + mod if res < 0 else res

def add_mod(x, y, mod):
    return normaliser(x + y, mod)

def sub_mod(x, y, mod):
    return normaliser(x - y, mod)

def mult_mod(x, y, mod):
    return normaliser(x * y, mod)

def bezout(a, b):
    r0, r1 = a, b
    u0, u1 = 1, 0
    v0, v1 = 0, 1

    while r1 != 0:
        q = r0 // r1
        r0, r1 = r1, r0 - q * r1
        u0, u1 = u1, u0 - q * u1
        v0, v1 = v1, v0 - q * v1

    return r0, u0, v0

def inv_mod(n, mod):
    pgcd, u, _ = bezout(n, mod)
    if pgcd != 1:
        raise Exception("Erreur: pas d'inverse modulaire")
    return normaliser(u, mod)

def is_on_curve(point, curve):
    if point is None:
        return True
    
    x, y = point
    gauche = pow(y, 2, curve.p)
    droite = add_mod(add_mod(pow(x, 3, curve.p), mult_mod(curve.a, x, curve.p), curve.p), curve.b, curve.p)
    return gauche == droite

def add_points(curve, P, Q):
    if P is None:
        return Q
    if Q is None:
        return P

    x1, y1 = P
    x2, y2 = Q
    mod = curve.p

    # P + (-P) = Infini
    if x1 == x2 and y2 == sub_mod(0, y1, mod):
        return None
    
    # P + P (Doublage)
    elif x1 == x2 and y1 == y2:
        if y1 == 0:
            return None
        
        num = add_mod(mult_mod(3, pow(x1, 2, mod), mod), curve.a, mod)
        den = inv_mod(mult_mod(2, y1, mod), mod)
        lam = mult_mod(num, den, mod)
        
    # P + Q
    else:
        num = sub_mod(y2, y1, mod)
        den = inv_mod(sub_mod(x2, x1, mod), mod)
        lam = mult_mod(num, den, mod)

    x3 = sub_mod(sub_mod(pow(lam, 2, mod), x1, mod), x2, mod)
    y3 = sub_mod(mult_mod(lam, sub_mod(x1, x3, mod), mod), y1, mod)
    
    return (x3, y3)

def mul_scalaire(curve, k, point):
    res = None
    temp = point

    while k > 0:
        if k & 1:
            res = add_points(curve, res, temp)
        temp = add_points(curve, temp, temp)
        k >>= 1

    return res
    
def generate_keys(curve, base_point):
    print("ECC key gen...")
    alea = int.from_bytes(os.urandom(32), "big")
    priv_key = (alea % (curve.p - 1)) + 1
    pub_key = mul_scalaire(curve, priv_key, base_point)
    return priv_key, pub_key