/* GIMP RGBA C-Source image dump (dirtTile.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[32 * 32 * 4 + 1];
} dirtTile = {
  32, 32, 4,
  "V\061!\352X\060\032\352a\065\034\352j<$\352e;%\352Z\063\"\352Z\065#\352T/\035\352"
  "X\061\"\352\\\063!\352|M\071\352zJ\064\352\200P\071\352f\071\"\352Z\063\034\352W"
  "\063\035\352X\062!\352qE\062\352\200L\063\352m:\"\352\\/\032\352X\062\"\352^\071"
  "(\352a\067#\352n?&\352~M\062\352oB*\352\\\061\034\352\\\063\"\352V/\040\352W\062"
  "#\352X\062$\352Y\063\040\377d\071#\377sE+\377k=#\377wK\064\377b\070%\377X/\036\377"
  "\\\064\040\377\\\062\040\377\\\060\034\377}M\067\377k:$\377n>'\377h<&\377\\\066\""
  "\377U\063\037\377Y\063\"\377pE\060\377\200L\063\377~K\062\377n@*\377]\066%\377]"
  "\067&\377j@*\377pC+\377nA)\377nB,\377\\\061\036\377[\062\040\377Z\061\040\377Z\060"
  "\037\377Z\060\036\377^\065\040\377|N\065\377|J\060\377m<!\377}M\066\377j=(\377_\063"
  "\036\377yL\065\377wJ\063\377~O\070\377|J\063\377\177M\066\377n>(\377h;'\377[\063"
  "!\377W\062\"\377W\062\040\377qF\060\377~L\062\377~L\062\377o@)\377b\070%\377a\067"
  "%\377n@+\377nA,\377^\065!\377h?+\377d;'\377c\067$\377oB-\377}O\071\377yI\062"
  "\377wM\071\377zL\064\377|J\061\377o>%\377yK\064\377l?*\377b\065\040\377f\070\"\377"
  "e\070\"\377xJ\063\377e\065\037\377p>(\377yG\062\377yJ\066\377i<+\377^\064$\377["
  "\063\040\377a\065\040\377g\066\035\377wE+\377o?'\377c\066\"\377i;(\377h\070#\377"
  "n@,\377oE\062\377f;'\377i=(\377i='\377m?)\377xH\062\377m=&\377b;(\377c\067!"
  "\377h\071\"\377i:#\377e:&\377_\065\"\377g<)\377uG\061\377[\061\034\377uJ\064\377"
  "h\071%\377b\062\034\377yG\062\377\201N\071\377h\067$\377e\066!\377oD/\377tH\061\377"
  "xH/\377|J\061\377{K\063\377qC-\377j;&\377j\070#\377k:%\377m>)\377f\067!\377a"
  "\063\035\377h:%\377tG\060\377i<'\377vJ\064\377]\066$\377nE\061\377`\063\036\377h"
  ";%\377]\063!\377Z\061\037\377i=*\377}O\070\377X\061\034\377kB.\377e\070#\377g\070"
  "#\377o?)\377sA,\377l:$\377wE.\377pC,\377oB+\377qA*\377p@(\377qA*\377tD.\377"
  "pA+\377j;&\377rA+\377wF-\377j\071!\377f\066\036\377m>'\377tG\061\377_\065!\377"
  "d<)\377a\070%\377yM\070\377qC-\377i<%\377d\070%\377_\063\040\377j;%\377o>'\377"
  "c\070!\377i='\377a\063\035\377j;'\377k;'\377d\065\037\377sE/\377pB+\377k<&\377"
  "k<&\377sD.\377yJ\063\377sC,\377l=&\377j<&\377h;'\377l>)\377n>'\377sC,\377"
  "zJ\063\377vF\060\377j='\377Z\060\035\377W\061\037\377c\067\"\377yI\062\377p?'\377"
  "k<%\377j<&\377i;%\377|I\061\377~G-\377k:\040\377wI/\377wG\061\377vH\062\377l"
  ">)\377`\064\040\377b\071$\377e=(\377j<&\377j:%\377g\070$\377uG\061\377sD-\377"
  "i;$\377d\070!\377]\064\040\377b\070$\377g\071%\377h:&\377sD/\377zK\064\377tG\061"
  "\377c\067#\377Y\062\036\377a\064\036\377nA*\377n>&\377vD/\377l>'\377_\065\037\377"
  "j>(\377tA-\377`\065\037\377g<'\377zN\071\377k>)\377h:&\377pC/\377pD\060\377k"
  "?+\377tE\060\377n?)\377c\065\040\377pD/\377wK\065\377oA+\377h\071!\377h\067\040"
  "\377g\071\"\377d\071%\377a\067#\377f:&\377nA+\377xJ\063\377g:$\377Z\060\037\377"
  "l@)\377c\070\040\377l=&\377vC.\377h;$\377X\062\033\377Y\063\036\377i\071)\377Y\060"
  "\035\377lA.\377qD\060\377g\070#\377qA,\377rB-\377zK\066\377n?*\377m>)\377h:&"
  "\377_\063\036\377d;&\377kA-\377qE\060\377tE.\377vD-\377rA*\377g:$\377b\067\""
  "\377b\066!\377l>(\377yI\062\377g\070\"\377[\060\036\377zM\067\377pG/\377tF\060\377"
  "yF\061\377_\060\032\377[\065\036\377lD.\377l;+\377\\\060\033\377xK\065\377k<%\377"
  "vE-\377~K\063\377{H\060\377|J\062\377r@(\377b\064\040\377]\061\037\377Y\060\035\377"
  "Z\062\037\377Z\062\037\377b\070%\377i<(\377tD.\377vE.\377m;#\377j:$\377l=(\377"
  "vF/\377vD+\377wG.\377l@+\377k@,\377kB+\377oA+\377\177L\067\377e\066\040\377"
  "a:\"\377lC-\377m;)\377e\071$\377oB,\377j;$\377tC+\377r@(\377|I\061\377vC+\377"
  "p=&\377oA.\377nB\060\377d\071'\377`\067$\377a\070$\377^\063\037\377f\071%\377uF"
  "\061\377uD-\377m:\"\377p?*\377sD.\377pA*\377n>%\377sC*\377l@+\377Y\060\034\377"
  "\\\063\036\377b\065\040\377}K\066\377l<&\377_\067\037\377a\070!\377vB\060\377qF\063"
  "\377c\067$\377e\070\"\377j\071$\377f\066\037\377vF/\377n>(\377k:%\377qB.\377s"
  "E\061\377k=)\377h;&\377i;&\377k>(\377n?*\377vF\061\377wE/\377l:$\377l=*\377"
  "i<*\377d\067\"\377h\071#\377f\071!\377`\067\"\377]\066\"\377Z\063\036\377b\066!\377"
  "}K\066\377k;$\377g@'\377^\063\033\377\177J\067\377jB\061\377^\064\"\377^\062\036"
  "\377c\065!\377h:%\377qC-\377k<(\377i;'\377k;%\377q@*\377tC,\377tD,\377tD,"
  "\377yH\060\377wF/\377xF\060\377wE/\377j:'\377b\067%\377\\\063\"\377`\067%\377"
  "g;'\377e\071#\377\\\063\035\377^\070&\377^\067$\377f:%\377wD\060\377k;$\377pG."
  "\377k?'\377t?+\377X\060\036\377c\071&\377i>*\377pC-\377k<'\377qB-\377h\071$\377"
  "h:%\377k;%\377qA*\377wE.\377vD,\377yG/\377wF.\377xG\060\377wF\060\377n>(\377"
  "i;&\377b\070&\377_\067%\377c:(\377pD\060\377i;%\377m@*\377Z\063\"\377^\070%\377"
  "d\070$\377k\070#\377l<%\377i?&\377yM\064\377s<(\377b\067#\377g;'\377oA,\377{"
  "L\064\377p?(\377yG\060\377sB+\377m<%\377k;'\377j:&\377l<&\377k;$\377p@(\377"
  "m='\377m>)\377l>*\377g\067#\377k;$\377e:%\377^\064\"\377b\067$\377qB-\377n<"
  "%\377wF/\377X\062\037\377Y\061\036\377^\062\036\377k;%\377qB+\377i>&\377|P\067\377"
  "sA+\377j=(\377g<%\377^\063\034\377pC,\377rC-\377wE/\377yH\060\377n>&\377qA+"
  "\377n>(\377o@(\377pB*\377m?'\377h:#\377m>)\377qA.\377q@,\377q?)\377g\071$"
  "\377`\063\037\377e\070#\377n?(\377m;#\377p>&\377^\066\"\377a\070%\377`\065\037\377"
  "k>'\377tG\060\377j=&\377|O\070\377_\064\034\377e\067!\377g?(\377S\062\033\377Z\066"
  "\"\377rH\067\377l>,\377i\071$\377h;#\377vE,\377q@&\377uF+\377{L\062\377oA(\377"
  "e\066\036\377qA*\377zI\064\377{I\065\377uB-\377l:$\377l<&\377pB+\377qC-\377g"
  "\071\"\377i;#\377f:(\377sF\063\377rD/\377p@*\377l<'\377sC-\377uG\061\377j;%"
  "\377a/\035\377f:%\377[\062\035\377f<'\377vI\065\377g\071%\377Y-\027\377d;$\377"
  "h;$\377j;%\377j<&\377nB,\377k@+\377f;'\377b\067#\377h<*\377n@-\377l;&\377"
  "k:$\377pA+\377wH\062\377l=(\377g\071$\377g\071\"\377^\062\037\377k<*\377zJ\066"
  "\377wE\060\377\177L\067\377xF\062\377m=(\377f\066\040\377i\070(\377i\071(\377h;"
  "&\377{L\065\377|L\065\377m?)\377`\064\037\377c;%\377g:%\377k<(\377h;'\377b\067"
  "$\377jA-\377kC\060\377_\066$\377Z\061\037\377g;'\377sC-\377j<%\377oB+\377zK\065"
  "\377vD/\377tC.\377m?*\377\\\061\036\377e\071%\377uH\063\377d\065\037\377{L\066\377"
  "b\063\036\377yJ\064\377f\071#\377sF\066\377h:(\377wH\062\377~L\065\377zI\062\377"
  "xH\061\377m?*\377h<(\377o@,\377l<(\377i:'\377g:&\377e;&\377a\067\"\377]\063"
  "\037\377_\065!\377e;&\377j>(\377j;$\377tC-\377xG\061\377g\071#\377m?*\377{K\070"
  "\377Y\061\035\377`\067#\377qG\061\377c\067\040\377xK\064\377f\071\"\377h<%\377xK"
  "\064\377[\065\"\377Z\060\035\377l<&\377zH\062\377i\070\040\377k\071#\377o>*\377n"
  "=+\377zG\063\377r?*\377l;%\377l=&\377e\070!\377b\064\036\377j<%\377m?)\377rD"
  "\060\377sE\061\377e\070!\377m@)\377{L\064\377}J\064\377xF\062\377rE\062\377Z\063"
  "\036\377d;'\377f<&\377g<$\377l?(\377g:$\377c\070!\377h=%\377[\065\036\377^\065"
  "\036\377j<&\377zJ\065\377n?)\377_\061\033\377rA,\377~J\067\377r>*\377p>(\377m"
  ";%\377i\071\"\377m='\377n>'\377vF/\377\177M\066\377qB,\377h;&\377sA,\377n:"
  "$\377rA*\377uH\060\377_\065\037\377[\060\036\377e;(\377sH\064\377sF\061\377k=&\377"
  "yK\064\377wI\062\377yK\064\377i<%\377h=%\377n@*\377m<)\377h:&\377c\071#\377`"
  "\067\040\377a\065\035\377wE.\377o?)\377rC-\377g\071#\377g\071#\377rC-\377rB-\377"
  "sB+\377|I\063\377xE\060\377n<(\377m='\377m=%\377o>&\377xG\061\377g:&\377V/\034"
  "\377vJ\067\377`\062\037\377c\064\037\377h\070!\377n<'\377m;&\377i:$\377j:$\377"
  "j<$\377{I\063\377}J\067\377c\066#\377Y\063\036\377_<$\377e<#\377k;\"\377l='\377"
  "l?*\377i<'\377h;&\377i;&\377j;&\377l;&\377l\071$\377rA-\377uE\061\377vC.\377"
  "zG\061\377m;#\377i;$\377c\071$\377Z\064!\377yM:\377{K\063\377n;\036\377m;!\377"
  "}P;\377V-\033\377\\\063!\377g='\377e;#\377l;\"\377e/\025\377xH\062\377a\071&\377"
  "V\063\040\377Y\061\035\377e\066!\377|J\064\377}L\065\377g\071#\377f\071\"\377j=&\377"
  "i<%\377i:$\377k:$\377k\071%\377g\070$\377k@+\377rH\062\377j?)\377h\071%\377g"
  "\067$\377^/\035\377b\066#\377sB,\377n;!\377n=$\377{M\066\377`\065\"\377e:'\377"
  "a\063\036\377f<$\377k<%\377\177K\063\377\177N\071\377a\070%\377[\066#\377a\067$"
  "\377o?)\377k:$\377l<&\377k<&\377i;$\377j<%\377j<%\377j;$\377m<&\377m<&\377"
  "i:#\377a\065\036\377`\065\035\377g:#\377n=)\377k\071&\377h:&\377\\\060\037\377c"
  "\064\037\377k\071\"\377p>'\377wI\062\377h\071%\377j<%\377}M\066\377[\061\033\377"
  "yJ\064\377\177K\066\377}M\071\377c\067%\377a\070$\377i;'\377yG\061\377pA*\377h"
  ":#\377g\067!\377xI\063\377uF\060\377i\071#\377j:$\377tE/\377vF\060\377l;$\377"
  "e\070!\377a\064\035\377h:#\377j\071#\377n<'\377|M\066\377a\066%\377yK\070\377j:"
  "&\377j:$\377\201Q;\377k;%\377b\060\031\377|J\061\377\\/\032\377yJ\066\377c\060"
  "\035\377{K\067\377m?,\377j=(\377l='\377|G\062\377g\071#\377e\071\"\377zK\065\377"
  "~M\067\377\177L\067\377xE\060\377l<&\377i;$\377rC,\377~L\065\377tF/\377h<#\377"
  "rD-\377zI\064\377q@+\377xI\064\377d\070%\377vI\066\377h;'\377j;'\377\200P:\377"
  "xF/\377j\070\037\377m;\"\377k=&\377h\071%\377j\071(\377h\071'\377_\061\035\377g"
  ":#\377yI\062\377j\070\"\377e:$\377f;$\377zK\065\377{I\064\377o<'\377h\065\037\377"
  "k:#\377vG\060\377\177P\070\377\177N\067\377sE.\377g:\"\377n?(\377wG\061\377q"
  "@*\377rE.\377tG\061\377^\064!\377a\067%\377h=)\377j<%\377p>%\377p>%\377i\071"
  "\"\377h\071\"\377k<(\377i:(\377k>+\377h;&\377k=&\377i;$\377j;&\377f:%\377"
  "g;&\377e\067!\377tE\060\377}L\067\377uB-\377k\071\"\377tC-\377}L\065\377vE/\377"
  "f\071\"\377pD-\377tG\060\377d\065\037\377o@*\377{O\067\377xJ\061\377Z\061\034\377"
  "V\060\035\377]\064!\377h;$\377g\070\036\377h\070!\377j=&\377p>#\377i\071\"\377j"
  "=*\377]\060\036\377^\061\034\377^\061\033\377sF\061\377nA/\377b\065\"\377^\061\034"
  "\377sF\061\377h<'\377oC,\377tF/\377uD.\377\201N\067\377tB-\377d\063\036\377h"
  "<%\377b\070\040\377]\062\033\377a\063\037\377h:&\377X/\031\377",
};

