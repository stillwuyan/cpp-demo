#!/bin/bash

if [ "$(basename $0)" == "b.bash" ];then
  tail -n +$[ $(grep -n '^BINARY' $0|cut -d ':' -f 1) + 1 ] $0 | base64 -d > a2.out
  chmod +x a2.out
  ./a2.out
  echo $?
  exit
fi

cat "$0" > b.bash
echo "BINARY" >> b.bash
cat > a.c << EOF
int     main(){
        return 12;
}
EOF
gcc a.c 
base64 a.out >> b.bash
BINARY
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
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAsARAAAAAAACQBEAAAAAAAAAAAAAAAAAAAQAAAAAAAAABAAAAAAAAAAwAAAAAAAAA
kANAAAAAAAANAAAAAAAAAGQFQAAAAAAAGQAAAAAAAAAQDmAAAAAAABsAAAAAAAAACAAAAAAAAAAa
AAAAAAAAABgOYAAAAAAAHAAAAAAAAAAIAAAAAAAAAPX+/28AAAAAmAJAAAAAAAAFAAAAAAAAAAAD
QAAAAAAABgAAAAAAAAC4AkAAAAAAAAoAAAAAAAAAOAAAAAAAAAALAAAAAAAAABgAAAAAAAAAFQAA
AAAAAAAAAAAAAAAAAAMAAAAAAAAAABBgAAAAAAACAAAAAAAAABgAAAAAAAAAFAAAAAAAAAAHAAAA
AAAAABcAAAAAAAAAeANAAAAAAAAHAAAAAAAAAGADQAAAAAAACAAAAAAAAAAYAAAAAAAAAAkAAAAA
AAAAGAAAAAAAAAD+//9vAAAAAEADQAAAAAAA////bwAAAAABAAAAAAAAAPD//28AAAAAOANAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACgOYAAAAAAA
AAAAAAAAAAAAAAAAAAAAAMYDQAAAAAAAAAAAAAAAAAAAAAAAAAAAAEdDQzogKFVidW50dSA1LjQu
MC02dWJ1bnR1MX4xNi4wNC4xMikgNS40LjAgMjAxNjA2MDkAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAMAAQA4AkAAAAAAAAAAAAAAAAAAAAAAAAMAAgBUAkAAAAAAAAAAAAAAAAAAAAAA
AAMAAwB0AkAAAAAAAAAAAAAAAAAAAAAAAAMABACYAkAAAAAAAAAAAAAAAAAAAAAAAAMABQC4AkAA
AAAAAAAAAAAAAAAAAAAAAAMABgAAA0AAAAAAAAAAAAAAAAAAAAAAAAMABwA4A0AAAAAAAAAAAAAA
AAAAAAAAAAMACABAA0AAAAAAAAAAAAAAAAAAAAAAAAMACQBgA0AAAAAAAAAAAAAAAAAAAAAAAAMA
CgB4A0AAAAAAAAAAAAAAAAAAAAAAAAMACwCQA0AAAAAAAAAAAAAAAAAAAAAAAAMADACwA0AAAAAA
AAAAAAAAAAAAAAAAAAMADQDQA0AAAAAAAAAAAAAAAAAAAAAAAAMADgDgA0AAAAAAAAAAAAAAAAAA
AAAAAAMADwBkBUAAAAAAAAAAAAAAAAAAAAAAAAMAEABwBUAAAAAAAAAAAAAAAAAAAAAAAAMAEQB0
BUAAAAAAAAAAAAAAAAAAAAAAAAMAEgCoBUAAAAAAAAAAAAAAAAAAAAAAAAMAEwAQDmAAAAAAAAAA
AAAAAAAAAAAAAAMAFAAYDmAAAAAAAAAAAAAAAAAAAAAAAAMAFQAgDmAAAAAAAAAAAAAAAAAAAAAA
AAMAFgAoDmAAAAAAAAAAAAAAAAAAAAAAAAMAFwD4D2AAAAAAAAAAAAAAAAAAAAAAAAMAGAAAEGAA
AAAAAAAAAAAAAAAAAAAAAAMAGQAgEGAAAAAAAAAAAAAAAAAAAAAAAAMAGgAwEGAAAAAAAAAAAAAA
AAAAAAAAAAMAGwAAAAAAAAAAAAAAAAAAAAAAAQAAAAQA8f8AAAAAAAAAAAAAAAAAAAAADAAAAAEA
FQAgDmAAAAAAAAAAAAAAAAAAGQAAAAIADgAQBEAAAAAAAAAAAAAAAAAAGwAAAAIADgBQBEAAAAAA
AAAAAAAAAAAALgAAAAIADgCQBEAAAAAAAAAAAAAAAAAARAAAAAEAGgAwEGAAAAAAAAEAAAAAAAAA
UwAAAAEAFAAYDmAAAAAAAAAAAAAAAAAAegAAAAIADgCwBEAAAAAAAAAAAAAAAAAAhgAAAAEAEwAQ
DmAAAAAAAAAAAAAAAAAApQAAAAQA8f8AAAAAAAAAAAAAAAAAAAAAAQAAAAQA8f8AAAAAAAAAAAAA
AAAAAAAAqQAAAAEAEgCYBkAAAAAAAAAAAAAAAAAAtwAAAAEAFQAgDmAAAAAAAAAAAAAAAAAAAAAA
AAQA8f8AAAAAAAAAAAAAAAAAAAAAwwAAAAAAEwAYDmAAAAAAAAAAAAAAAAAA1AAAAAEAFgAoDmAA
AAAAAAAAAAAAAAAA3QAAAAAAEwAQDmAAAAAAAAAAAAAAAAAA8AAAAAAAEQB0BUAAAAAAAAAAAAAA
AAAAAwEAAAEAGAAAEGAAAAAAAAAAAAAAAAAAGQEAABIADgBgBUAAAAAAAAIAAAAAAAAAKQEAACAA
AAAAAAAAAAAAAAAAAAAAAAAAbQEAACAAGQAgEGAAAAAAAAAAAAAAAAAARQEAABAAGQAwEGAAAAAA
AAAAAAAAAAAAIwEAABIADwBkBUAAAAAAAAAAAAAAAAAATAEAABIAAAAAAAAAAAAAAAAAAAAAAAAA
awEAABAAGQAgEGAAAAAAAAAAAAAAAAAAeAEAACAAAAAAAAAAAAAAAAAAAAAAAAAAhwEAABECGQAo
EGAAAAAAAAAAAAAAAAAAlAEAABEAEABwBUAAAAAAAAQAAAAAAAAAowEAABIADgDwBEAAAAAAAGUA
AAAAAAAAzwAAABAAGgA4EGAAAAAAAAAAAAAAAAAAcQEAABIADgDgA0AAAAAAACoAAAAAAAAAswEA
ABAAGgAwEGAAAAAAAAAAAAAAAAAAvwEAABIADgDWBEAAAAAAAAsAAAAAAAAAxAEAACAAAAAAAAAA
AAAAAAAAAAAAAAAA2AEAABECGQAwEGAAAAAAAAAAAAAAAAAA5AEAACAAAAAAAAAAAAAAAAAAAAAA
AAAArQEAABIACwCQA0AAAAAAAAAAAAAAAAAAAGNydHN0dWZmLmMAX19KQ1JfTElTVF9fAGRlcmVn
aXN0ZXJfdG1fY2xvbmVzAF9fZG9fZ2xvYmFsX2R0b3JzX2F1eABjb21wbGV0ZWQuNzU5NABfX2Rv
X2dsb2JhbF9kdG9yc19hdXhfZmluaV9hcnJheV9lbnRyeQBmcmFtZV9kdW1teQBfX2ZyYW1lX2R1
bW15X2luaXRfYXJyYXlfZW50cnkAYS5jAF9fRlJBTUVfRU5EX18AX19KQ1JfRU5EX18AX19pbml0
X2FycmF5X2VuZABfRFlOQU1JQwBfX2luaXRfYXJyYXlfc3RhcnQAX19HTlVfRUhfRlJBTUVfSERS
AF9HTE9CQUxfT0ZGU0VUX1RBQkxFXwBfX2xpYmNfY3N1X2ZpbmkAX0lUTV9kZXJlZ2lzdGVyVE1D
bG9uZVRhYmxlAF9lZGF0YQBfX2xpYmNfc3RhcnRfbWFpbkBAR0xJQkNfMi4yLjUAX19kYXRhX3N0
YXJ0AF9fZ21vbl9zdGFydF9fAF9fZHNvX2hhbmRsZQBfSU9fc3RkaW5fdXNlZABfX2xpYmNfY3N1
X2luaXQAX19ic3Nfc3RhcnQAbWFpbgBfSnZfUmVnaXN0ZXJDbGFzc2VzAF9fVE1DX0VORF9fAF9J
VE1fcmVnaXN0ZXJUTUNsb25lVGFibGUAAC5zeW10YWIALnN0cnRhYgAuc2hzdHJ0YWIALmludGVy
cAAubm90ZS5BQkktdGFnAC5ub3RlLmdudS5idWlsZC1pZAAuZ251Lmhhc2gALmR5bnN5bQAuZHlu
c3RyAC5nbnUudmVyc2lvbgAuZ251LnZlcnNpb25fcgAucmVsYS5keW4ALnJlbGEucGx0AC5pbml0
AC5wbHQuZ290AC50ZXh0AC5maW5pAC5yb2RhdGEALmVoX2ZyYW1lX2hkcgAuZWhfZnJhbWUALmlu
aXRfYXJyYXkALmZpbmlfYXJyYXkALmpjcgAuZHluYW1pYwAuZ290LnBsdAAuZGF0YQAuYnNzAC5j
b21tZW50AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAbAAAAAQAAAAIAAAAAAAAAOAJAAAAAAAA4AgAAAAAAABwAAAAA
AAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAIwAAAAcAAAACAAAAAAAAAFQCQAAAAAAAVAIAAAAA
AAAgAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAADEAAAAHAAAAAgAAAAAAAAB0AkAAAAAA
AHQCAAAAAAAAJAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAABEAAAA9v//bwIAAAAAAAAA
mAJAAAAAAACYAgAAAAAAABwAAAAAAAAABQAAAAAAAAAIAAAAAAAAAAAAAAAAAAAATgAAAAsAAAAC
AAAAAAAAALgCQAAAAAAAuAIAAAAAAABIAAAAAAAAAAYAAAABAAAACAAAAAAAAAAYAAAAAAAAAFYA
AAADAAAAAgAAAAAAAAAAA0AAAAAAAAADAAAAAAAAOAAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAA
AAAAAABeAAAA////bwIAAAAAAAAAOANAAAAAAAA4AwAAAAAAAAYAAAAAAAAABQAAAAAAAAACAAAA
AAAAAAIAAAAAAAAAawAAAP7//28CAAAAAAAAAEADQAAAAAAAQAMAAAAAAAAgAAAAAAAAAAYAAAAB
AAAACAAAAAAAAAAAAAAAAAAAAHoAAAAEAAAAAgAAAAAAAABgA0AAAAAAAGADAAAAAAAAGAAAAAAA
AAAFAAAAAAAAAAgAAAAAAAAAGAAAAAAAAACEAAAABAAAAEIAAAAAAAAAeANAAAAAAAB4AwAAAAAA
ABgAAAAAAAAABQAAABgAAAAIAAAAAAAAABgAAAAAAAAAjgAAAAEAAAAGAAAAAAAAAJADQAAAAAAA
kAMAAAAAAAAaAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAIkAAAABAAAABgAAAAAAAACw
A0AAAAAAALADAAAAAAAAIAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAEAAAAAAAAACUAAAAAQAAAAYA
AAAAAAAA0ANAAAAAAADQAwAAAAAAAAgAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAAnQAA
AAEAAAAGAAAAAAAAAOADQAAAAAAA4AMAAAAAAACCAQAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAA
AAAAAKMAAAABAAAABgAAAAAAAABkBUAAAAAAAGQFAAAAAAAACQAAAAAAAAAAAAAAAAAAAAQAAAAA
AAAAAAAAAAAAAACpAAAAAQAAABIAAAAAAAAAcAVAAAAAAABwBQAAAAAAAAQAAAAAAAAAAAAAAAAA
AAAEAAAAAAAAAAQAAAAAAAAAsQAAAAEAAAACAAAAAAAAAHQFQAAAAAAAdAUAAAAAAAA0AAAAAAAA
AAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAL8AAAABAAAAAgAAAAAAAACoBUAAAAAAAKgFAAAAAAAA
9AAAAAAAAAAAAAAAAAAAAAgAAAAAAAAAAAAAAAAAAADJAAAADgAAAAMAAAAAAAAAEA5gAAAAAAAQ
DgAAAAAAAAgAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAA1QAAAA8AAAADAAAAAAAAABgO
YAAAAAAAGA4AAAAAAAAIAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAOEAAAABAAAAAwAA
AAAAAAAgDmAAAAAAACAOAAAAAAAACAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAAAAAAAAAAAADmAAAA
BgAAAAMAAAAAAAAAKA5gAAAAAAAoDgAAAAAAANABAAAAAAAABgAAAAAAAAAIAAAAAAAAABAAAAAA
AAAAmAAAAAEAAAADAAAAAAAAAPgPYAAAAAAA+A8AAAAAAAAIAAAAAAAAAAAAAAAAAAAACAAAAAAA
AAAIAAAAAAAAAO8AAAABAAAAAwAAAAAAAAAAEGAAAAAAAAAQAAAAAAAAIAAAAAAAAAAAAAAAAAAA
AAgAAAAAAAAACAAAAAAAAAD4AAAAAQAAAAMAAAAAAAAAIBBgAAAAAAAgEAAAAAAAABAAAAAAAAAA
AAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAA/gAAAAgAAAADAAAAAAAAADAQYAAAAAAAMBAAAAAAAAAI
AAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAMBAAABAAAAMAAAAAAAAAAAAAAAAAAAADAQ
AAAAAAAANQAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAQAAAAAAAAARAAAAAwAAAAAAAAAAAAAAAAAA
AAAAAACWGAAAAAAAAAwBAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAQAAAAIAAAAAAAAA
AAAAAAAAAAAAAAAAaBAAAAAAAAAwBgAAAAAAAB4AAAAvAAAACAAAAAAAAAAYAAAAAAAAAAkAAAAD
AAAAAAAAAAAAAAAAAAAAAAAAAJgWAAAAAAAA/gEAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAA
AAA=
