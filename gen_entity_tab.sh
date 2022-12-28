#!/bin/bash

# See https://www.codeproject.com/Articles/2995/The-Complete-Guide-to-C-Strings-Part-I-Win32-Chara
gen_entity () {
  # BOM=Byte Order Mark

  # UTF-8 BOM
  printf '\xEF\xBB\xBF'

  # UTF-16 big-endian BOM
  #printf '\xFE\xFF'

  # UTF-16 little-endian BOM, that's what Windows use for its values stored in memory (mais ça marche pô ...)
  #printf '\xFF\xFE'

  echo "std::vector<std::pair<wchar_t, std::string>> ent_sym = {"
  echo -n "  { L'\0', \"&;\"    }, // First one must of that kind (don't ask)"
  let n=0
  while read sy ch;
  do
    (( n == 0 )) || echo -n ", "
    if (( n % 3 == 0 )); then
      echo; echo -n "  "
    fi
    let n++
    let l=8-${#sy}
    s=""
    for ((i=0; i < $l; i++)); do s+=" "; done
    echo -n "{ L'${ch}', \"&${sy};\" ${s}}"
  done <<EOF
acute ´
alpha α
Alpha Α
amp &
and ∧
ang ∠
asymp ≈
bdquo „
beta β
Beta Β
brvbar ¦
bull •
cap ∩
cedil ¸
cent ¢
chi χ
Chi Χ
circ ˆ
clubs ♣
cong ≅
copy ©
crarr ↵
cup ∪
curren ¤
dagger †
Dagger ‡
darr ↓
deg °
delta δ
Delta Δ
diams ♦
divide ÷
empty ∅
epsilon ε
Epsilon Ε
equiv ≡
eta η
Eta Η
euro €
exist ∃
fnof ƒ
forall ∀
frac12 ½
frac14 ¼
frac34 ¾
gamma γ
Gamma Γ
ge ≥
gt >
harr ↔
hearts ♥
hellip …
iexcl ¡
infin ∞
int ∫
iota ι
Iota Ι
iquest ¿
isin ∈
kappa κ
Kappa Κ
lambda λ
Lambda Λ
laquo «
larr ←
lceil ⌈
ldquo “
le ≤
lfloor ⌊
lowast ∗
loz ◊
lsaquo ‹
lsquo ‘
lt <
macr ¯
mdash —
micro µ
minus −
mu μ
Mu Μ
nabla ∇
ndash –
ne ≠
ni ∋
not ¬
notin ∉
nsub ⊄
nu ν
Nu Ν
oelig œ
OElig Œ
oline ‾
omega ω
Omega Ω
omicron ο
Omicron Ο
oplus ⊕
or ∨
ordf ª
ordm º
otimes ⊗
para ¶
part ∂
permil ‰
perp ⊥
phi φ
Phi Φ
pi π
Pi Π
piv ϖ
plusmn ±
pound £
prime ′
Prime ″
prod ∏
prop ∝
psi ψ
Psi Ψ
radic √
raquo »
rarr →
rceil ⌉
rdquo ”
reg ®
rfloor ⌋
rho ρ
Rho Ρ
rsaquo ›
rsquo ’
sbquo ‚
scaron š
Scaron Š
sdot ⋅
sect §
shy ­
sigma σ
Sigma Σ
sigmaf ς
sim ∼
spades ♠
sub ⊂
sube ⊆
sum ∑
sup ⊃
sup1 ¹
sup2 ²
sup3 ³
supe ⊇
tau τ
Tau Τ
there4 ∴
theta θ
Theta Θ
thetasym ϑ
tilde ˜
times ×
trade ™
uarr ↑
uml ¨
upsih ϒ
upsilon υ
Upsilon Υ
xi ξ
Xi Ξ
yen ¥
Yuml Ÿ
zeta ζ
Zeta Ζ
EOF

  echo
  echo "};"
}

gen_entity >ent_sym.h

