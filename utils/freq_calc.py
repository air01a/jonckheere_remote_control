import math

# Définition des constantes
XTAL_FREQ = 25e6  # 25 MHz
PLL_FREQ_MIN = 600e6  # 600 MHz
PLL_FREQ_MAX = 900e6  # 900 MHz

# Options pour RDiv
RDiv_options = [1, 2, 4, 8, 16, 32, 64, 128]

# Plage pour MultiSynth
MS_MIN = 4
MS_MAX = 900

def approximate_fraction(fraction, max_denominator=1048575):
    a = math.floor(fraction)
    frac = fraction - a

    if frac < 1e-12:
        return 0, 1

    lower_n, lower_d = 0, 1
    upper_n, upper_d = 1, 1

    while True:
        mediant_n = lower_n + upper_n
        mediant_d = lower_d + upper_d

        if mediant_d > max_denominator:
            break

        mediant = mediant_n / mediant_d

        if abs(mediant - frac) < 1e-12:
            return mediant_n, mediant_d
        elif mediant < frac:
            lower_n, lower_d = mediant_n, mediant_d
        else:
            upper_n, upper_d = mediant_n, mediant_d

    # Choisir le plus proche entre lower et upper
    lower = lower_n / lower_d
    upper = upper_n / upper_d
    if abs(lower - frac) < abs(upper - frac):
        return lower_n, lower_d
    else:
        return upper_n, upper_d

def find_best_parameters(f_out):
    best = {
        'error': float('inf'),
        'f_out_calc': None,
        'RDiv': None,
        'MS': None,
        'a': None,
        'b': None,
        'c': None
    }

    # Itération sur toutes les options de RDiv
    for RDiv in RDiv_options:
        # Itération sur toutes les valeurs de MS
        for MS in range(MS_MIN, MS_MAX + 1):
            # Calcul de la fréquence PLL requise
            f_PLL = f_out * MS * RDiv

            # Vérification des limites de la PLL
            if f_PLL < PLL_FREQ_MIN or f_PLL > PLL_FREQ_MAX:
                continue

            # Calcul de (a + b/c)
            a_plus_bc = f_PLL / XTAL_FREQ

            # Vérification que a_plus_bc est dans les limites
            if a_plus_bc < 15.0 or a_plus_bc > 90.0:
                continue

            # Extraction de a
            a = math.floor(a_plus_bc)
            frac = a_plus_bc - a

            # Approximation de b/c
            b, c = 0, 1
            if frac > 1e-12:  # Si il y a une fraction
                b, c = approximate_fraction(frac)

            # Calcul de la fréquence PLL réelle
            f_PLL_calc = XTAL_FREQ * (a + (b / c))

            # Calcul de la fréquence de sortie réelle
            f_out_calc = f_PLL_calc / (MS * RDiv)

            # Calcul de l'erreur
            error = abs(f_out_calc - f_out)

            # Mise à jour de la meilleure solution
            if error < best['error']:
                best.update({
                    'error': error,
                    'f_out_calc': f_out_calc,
                    'RDiv': RDiv,
                    'MS': MS,
                    'a': a,
                    'b': b,
                    'c': c
                })

                # Si l'erreur est très petite, on peut arrêter
                if error < 1e-6:
                    return best

    return best

if __name__ == "__main__":
    desired_f_out = float(input("Entrez la fréquence de sortie désirée en Hz (ex: 10000000 pour 10 MHz): "))
    
    solution = find_best_parameters(desired_f_out)
    
    if solution['error'] < float('inf'):
        print("\nMeilleure solution trouvée:")
        print(f"RDiv : {solution['RDiv']}")
        print(f"MultiSynth (MS) : {solution['MS']}")
        print(f"PLL : a = {solution['a']}, b = {solution['b']}, c = {solution['c']}")
        print(f"Fréquence de sortie calculée : {solution['f_out_calc']} Hz")
        print(f"Erreur : {solution['error']} Hz ({(solution['error'] / desired_f_out) * 100.0:.10f}%)")
    else:
        print("Aucune solution trouvée pour la fréquence désirée.")


    print("Arduino code")
    print("_____________")
    arduino_code="void setup(void)\n  /* Initialise the sensor */\n  if (clockgen.begin() != ERROR_NONE)\n  {\n    Serial.print(...);\n    while(1);\n  }"
    arduino_code += "  clockgen.setupPLL(SI5351_PLL_A,"+str(solution['a'])+","+str(solution['b'])+','+str(solution['c'])+");\n"
    arduino_code +=  "  clockgen.setupMultisynth(2, SI5351_PLL_A, "+str(solution['MS'])+", 0, 1);\n"
    arduino_code +="  clockgen.setupRdiv(0, SI5351_R_DIV_"+str(solution['RDiv'])+");\n"
    arduino_code += "  clockgen.enableOutputs(true);\n}"
    print(arduino_code)