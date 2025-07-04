#include <AK/Format.h>
#include <LibWeb/HTML/Parser/Entities.h>
#include <AK/Vector.h>
#include <AK/Assertions.h>

static StringView named_character_references[2231] = { "AElig"sv, "AElig;"sv, "AMP"sv, "AMP;"sv, "Aacute"sv, "Aacute;"sv, "Abreve;"sv, "Acirc"sv, "Acirc;"sv, "Acy;"sv, "Afr;"sv, "Agrave"sv, "Agrave;"sv, "Alpha;"sv, "Amacr;"sv, "And;"sv, "Aogon;"sv, "Aopf;"sv, "ApplyFunction;"sv, "Aring"sv, "Aring;"sv, "Ascr;"sv, "Assign;"sv, "Atilde"sv, "Atilde;"sv, "Auml"sv, "Auml;"sv, "Backslash;"sv, "Barv;"sv, "Barwed;"sv, "Bcy;"sv, "Because;"sv, "Bernoullis;"sv, "Beta;"sv, "Bfr;"sv, "Bopf;"sv, "Breve;"sv, "Bscr;"sv, "Bumpeq;"sv, "CHcy;"sv, "COPY"sv, "COPY;"sv, "Cacute;"sv, "Cap;"sv, "CapitalDifferentialD;"sv, "Cayleys;"sv, "Ccaron;"sv, "Ccedil"sv, "Ccedil;"sv, "Ccirc;"sv, "Cconint;"sv, "Cdot;"sv, "Cedilla;"sv, "CenterDot;"sv, "Cfr;"sv, "Chi;"sv, "CircleDot;"sv, "CircleMinus;"sv, "CirclePlus;"sv, "CircleTimes;"sv, "ClockwiseContourIntegral;"sv, "CloseCurlyDoubleQuote;"sv, "CloseCurlyQuote;"sv, "Colon;"sv, "Colone;"sv, "Congruent;"sv, "Conint;"sv, "ContourIntegral;"sv, "Copf;"sv, "Coproduct;"sv, "CounterClockwiseContourIntegral;"sv, "Cross;"sv, "Cscr;"sv, "Cup;"sv, "CupCap;"sv, "DD;"sv, "DDotrahd;"sv, "DJcy;"sv, "DScy;"sv, "DZcy;"sv, "Dagger;"sv, "Darr;"sv, "Dashv;"sv, "Dcaron;"sv, "Dcy;"sv, "Del;"sv, "Delta;"sv, "Dfr;"sv, "DiacriticalAcute;"sv, "DiacriticalDot;"sv, "DiacriticalDoubleAcute;"sv, "DiacriticalGrave;"sv, "DiacriticalTilde;"sv, "Diamond;"sv, "DifferentialD;"sv, "Dopf;"sv, "Dot;"sv, "DotDot;"sv, "DotEqual;"sv, "DoubleContourIntegral;"sv, "DoubleDot;"sv, "DoubleDownArrow;"sv, "DoubleLeftArrow;"sv, "DoubleLeftRightArrow;"sv, "DoubleLeftTee;"sv, "DoubleLongLeftArrow;"sv, "DoubleLongLeftRightArrow;"sv, "DoubleLongRightArrow;"sv, "DoubleRightArrow;"sv, "DoubleRightTee;"sv, "DoubleUpArrow;"sv, "DoubleUpDownArrow;"sv, "DoubleVerticalBar;"sv, "DownArrow;"sv, "DownArrowBar;"sv, "DownArrowUpArrow;"sv, "DownBreve;"sv, "DownLeftRightVector;"sv, "DownLeftTeeVector;"sv, "DownLeftVector;"sv, "DownLeftVectorBar;"sv, "DownRightTeeVector;"sv, "DownRightVector;"sv, "DownRightVectorBar;"sv, "DownTee;"sv, "DownTeeArrow;"sv, "Downarrow;"sv, "Dscr;"sv, "Dstrok;"sv, "ENG;"sv, "ETH"sv, "ETH;"sv, "Eacute"sv, "Eacute;"sv, "Ecaron;"sv, "Ecirc"sv, "Ecirc;"sv, "Ecy;"sv, "Edot;"sv, "Efr;"sv, "Egrave"sv, "Egrave;"sv, "Element;"sv, "Emacr;"sv, "EmptySmallSquare;"sv, "EmptyVerySmallSquare;"sv, "Eogon;"sv, "Eopf;"sv, "Epsilon;"sv, "Equal;"sv, "EqualTilde;"sv, "Equilibrium;"sv, "Escr;"sv, "Esim;"sv, "Eta;"sv, "Euml"sv, "Euml;"sv, "Exists;"sv, "ExponentialE;"sv, "Fcy;"sv, "Ffr;"sv, "FilledSmallSquare;"sv, "FilledVerySmallSquare;"sv, "Fopf;"sv, "ForAll;"sv, "Fouriertrf;"sv, "Fscr;"sv, "GJcy;"sv, "GT"sv, "GT;"sv, "Gamma;"sv, "Gammad;"sv, "Gbreve;"sv, "Gcedil;"sv, "Gcirc;"sv, "Gcy;"sv, "Gdot;"sv, "Gfr;"sv, "Gg;"sv, "Gopf;"sv, "GreaterEqual;"sv, "GreaterEqualLess;"sv, "GreaterFullEqual;"sv, "GreaterGreater;"sv, "GreaterLess;"sv, "GreaterSlantEqual;"sv, "GreaterTilde;"sv, "Gscr;"sv, "Gt;"sv, "HARDcy;"sv, "Hacek;"sv, "Hat;"sv, "Hcirc;"sv, "Hfr;"sv, "HilbertSpace;"sv, "Hopf;"sv, "HorizontalLine;"sv, "Hscr;"sv, "Hstrok;"sv, "HumpDownHump;"sv, "HumpEqual;"sv, "IEcy;"sv, "IJlig;"sv, "IOcy;"sv, "Iacute"sv, "Iacute;"sv, "Icirc"sv, "Icirc;"sv, "Icy;"sv, "Idot;"sv, "Ifr;"sv, "Igrave"sv, "Igrave;"sv, "Im;"sv, "Imacr;"sv, "ImaginaryI;"sv, "Implies;"sv, "Int;"sv, "Integral;"sv, "Intersection;"sv, "InvisibleComma;"sv, "InvisibleTimes;"sv, "Iogon;"sv, "Iopf;"sv, "Iota;"sv, "Iscr;"sv, "Itilde;"sv, "Iukcy;"sv, "Iuml"sv, "Iuml;"sv, "Jcirc;"sv, "Jcy;"sv, "Jfr;"sv, "Jopf;"sv, "Jscr;"sv, "Jsercy;"sv, "Jukcy;"sv, "KHcy;"sv, "KJcy;"sv, "Kappa;"sv, "Kcedil;"sv, "Kcy;"sv, "Kfr;"sv, "Kopf;"sv, "Kscr;"sv, "LJcy;"sv, "LT"sv, "LT;"sv, "Lacute;"sv, "Lambda;"sv, "Lang;"sv, "Laplacetrf;"sv, "Larr;"sv, "Lcaron;"sv, "Lcedil;"sv, "Lcy;"sv, "LeftAngleBracket;"sv, "LeftArrow;"sv, "LeftArrowBar;"sv, "LeftArrowRightArrow;"sv, "LeftCeiling;"sv, "LeftDoubleBracket;"sv, "LeftDownTeeVector;"sv, "LeftDownVector;"sv, "LeftDownVectorBar;"sv, "LeftFloor;"sv, "LeftRightArrow;"sv, "LeftRightVector;"sv, "LeftTee;"sv, "LeftTeeArrow;"sv, "LeftTeeVector;"sv, "LeftTriangle;"sv, "LeftTriangleBar;"sv, "LeftTriangleEqual;"sv, "LeftUpDownVector;"sv, "LeftUpTeeVector;"sv, "LeftUpVector;"sv, "LeftUpVectorBar;"sv, "LeftVector;"sv, "LeftVectorBar;"sv, "Leftarrow;"sv, "Leftrightarrow;"sv, "LessEqualGreater;"sv, "LessFullEqual;"sv, "LessGreater;"sv, "LessLess;"sv, "LessSlantEqual;"sv, "LessTilde;"sv, "Lfr;"sv, "Ll;"sv, "Lleftarrow;"sv, "Lmidot;"sv, "LongLeftArrow;"sv, "LongLeftRightArrow;"sv, "LongRightArrow;"sv, "Longleftarrow;"sv, "Longleftrightarrow;"sv, "Longrightarrow;"sv, "Lopf;"sv, "LowerLeftArrow;"sv, "LowerRightArrow;"sv, "Lscr;"sv, "Lsh;"sv, "Lstrok;"sv, "Lt;"sv, "Map;"sv, "Mcy;"sv, "MediumSpace;"sv, "Mellintrf;"sv, "Mfr;"sv, "MinusPlus;"sv, "Mopf;"sv, "Mscr;"sv, "Mu;"sv, "NJcy;"sv, "Nacute;"sv, "Ncaron;"sv, "Ncedil;"sv, "Ncy;"sv, "NegativeMediumSpace;"sv, "NegativeThickSpace;"sv, "NegativeThinSpace;"sv, "NegativeVeryThinSpace;"sv, "NestedGreaterGreater;"sv, "NestedLessLess;"sv, "NewLine;"sv, "Nfr;"sv, "NoBreak;"sv, "NonBreakingSpace;"sv, "Nopf;"sv, "Not;"sv, "NotCongruent;"sv, "NotCupCap;"sv, "NotDoubleVerticalBar;"sv, "NotElement;"sv, "NotEqual;"sv, "NotEqualTilde;"sv, "NotExists;"sv, "NotGreater;"sv, "NotGreaterEqual;"sv, "NotGreaterFullEqual;"sv, "NotGreaterGreater;"sv, "NotGreaterLess;"sv, "NotGreaterSlantEqual;"sv, "NotGreaterTilde;"sv, "NotHumpDownHump;"sv, "NotHumpEqual;"sv, "NotLeftTriangle;"sv, "NotLeftTriangleBar;"sv, "NotLeftTriangleEqual;"sv, "NotLess;"sv, "NotLessEqual;"sv, "NotLessGreater;"sv, "NotLessLess;"sv, "NotLessSlantEqual;"sv, "NotLessTilde;"sv, "NotNestedGreaterGreater;"sv, "NotNestedLessLess;"sv, "NotPrecedes;"sv, "NotPrecedesEqual;"sv, "NotPrecedesSlantEqual;"sv, "NotReverseElement;"sv, "NotRightTriangle;"sv, "NotRightTriangleBar;"sv, "NotRightTriangleEqual;"sv, "NotSquareSubset;"sv, "NotSquareSubsetEqual;"sv, "NotSquareSuperset;"sv, "NotSquareSupersetEqual;"sv, "NotSubset;"sv, "NotSubsetEqual;"sv, "NotSucceeds;"sv, "NotSucceedsEqual;"sv, "NotSucceedsSlantEqual;"sv, "NotSucceedsTilde;"sv, "NotSuperset;"sv, "NotSupersetEqual;"sv, "NotTilde;"sv, "NotTildeEqual;"sv, "NotTildeFullEqual;"sv, "NotTildeTilde;"sv, "NotVerticalBar;"sv, "Nscr;"sv, "Ntilde"sv, "Ntilde;"sv, "Nu;"sv, "OElig;"sv, "Oacute"sv, "Oacute;"sv, "Ocirc"sv, "Ocirc;"sv, "Ocy;"sv, "Odblac;"sv, "Ofr;"sv, "Ograve"sv, "Ograve;"sv, "Omacr;"sv, "Omega;"sv, "Omicron;"sv, "Oopf;"sv, "OpenCurlyDoubleQuote;"sv, "OpenCurlyQuote;"sv, "Or;"sv, "Oscr;"sv, "Oslash"sv, "Oslash;"sv, "Otilde"sv, "Otilde;"sv, "Otimes;"sv, "Ouml"sv, "Ouml;"sv, "OverBar;"sv, "OverBrace;"sv, "OverBracket;"sv, "OverParenthesis;"sv, "PartialD;"sv, "Pcy;"sv, "Pfr;"sv, "Phi;"sv, "Pi;"sv, "PlusMinus;"sv, "Poincareplane;"sv, "Popf;"sv, "Pr;"sv, "Precedes;"sv, "PrecedesEqual;"sv, "PrecedesSlantEqual;"sv, "PrecedesTilde;"sv, "Prime;"sv, "Product;"sv, "Proportion;"sv, "Proportional;"sv, "Pscr;"sv, "Psi;"sv, "QUOT"sv, "QUOT;"sv, "Qfr;"sv, "Qopf;"sv, "Qscr;"sv, "RBarr;"sv, "REG"sv, "REG;"sv, "Racute;"sv, "Rang;"sv, "Rarr;"sv, "Rarrtl;"sv, "Rcaron;"sv, "Rcedil;"sv, "Rcy;"sv, "Re;"sv, "ReverseElement;"sv, "ReverseEquilibrium;"sv, "ReverseUpEquilibrium;"sv, "Rfr;"sv, "Rho;"sv, "RightAngleBracket;"sv, "RightArrow;"sv, "RightArrowBar;"sv, "RightArrowLeftArrow;"sv, "RightCeiling;"sv, "RightDoubleBracket;"sv, "RightDownTeeVector;"sv, "RightDownVector;"sv, "RightDownVectorBar;"sv, "RightFloor;"sv, "RightTee;"sv, "RightTeeArrow;"sv, "RightTeeVector;"sv, "RightTriangle;"sv, "RightTriangleBar;"sv, "RightTriangleEqual;"sv, "RightUpDownVector;"sv, "RightUpTeeVector;"sv, "RightUpVector;"sv, "RightUpVectorBar;"sv, "RightVector;"sv, "RightVectorBar;"sv, "Rightarrow;"sv, "Ropf;"sv, "RoundImplies;"sv, "Rrightarrow;"sv, "Rscr;"sv, "Rsh;"sv, "RuleDelayed;"sv, "SHCHcy;"sv, "SHcy;"sv, "SOFTcy;"sv, "Sacute;"sv, "Sc;"sv, "Scaron;"sv, "Scedil;"sv, "Scirc;"sv, "Scy;"sv, "Sfr;"sv, "ShortDownArrow;"sv, "ShortLeftArrow;"sv, "ShortRightArrow;"sv, "ShortUpArrow;"sv, "Sigma;"sv, "SmallCircle;"sv, "Sopf;"sv, "Sqrt;"sv, "Square;"sv, "SquareIntersection;"sv, "SquareSubset;"sv, "SquareSubsetEqual;"sv, "SquareSuperset;"sv, "SquareSupersetEqual;"sv, "SquareUnion;"sv, "Sscr;"sv, "Star;"sv, "Sub;"sv, "Subset;"sv, "SubsetEqual;"sv, "Succeeds;"sv, "SucceedsEqual;"sv, "SucceedsSlantEqual;"sv, "SucceedsTilde;"sv, "SuchThat;"sv, "Sum;"sv, "Sup;"sv, "Superset;"sv, "SupersetEqual;"sv, "Supset;"sv, "THORN"sv, "THORN;"sv, "TRADE;"sv, "TSHcy;"sv, "TScy;"sv, "Tab;"sv, "Tau;"sv, "Tcaron;"sv, "Tcedil;"sv, "Tcy;"sv, "Tfr;"sv, "Therefore;"sv, "Theta;"sv, "ThickSpace;"sv, "ThinSpace;"sv, "Tilde;"sv, "TildeEqual;"sv, "TildeFullEqual;"sv, "TildeTilde;"sv, "Topf;"sv, "TripleDot;"sv, "Tscr;"sv, "Tstrok;"sv, "Uacute"sv, "Uacute;"sv, "Uarr;"sv, "Uarrocir;"sv, "Ubrcy;"sv, "Ubreve;"sv, "Ucirc"sv, "Ucirc;"sv, "Ucy;"sv, "Udblac;"sv, "Ufr;"sv, "Ugrave"sv, "Ugrave;"sv, "Umacr;"sv, "UnderBar;"sv, "UnderBrace;"sv, "UnderBracket;"sv, "UnderParenthesis;"sv, "Union;"sv, "UnionPlus;"sv, "Uogon;"sv, "Uopf;"sv, "UpArrow;"sv, "UpArrowBar;"sv, "UpArrowDownArrow;"sv, "UpDownArrow;"sv, "UpEquilibrium;"sv, "UpTee;"sv, "UpTeeArrow;"sv, "Uparrow;"sv, "Updownarrow;"sv, "UpperLeftArrow;"sv, "UpperRightArrow;"sv, "Upsi;"sv, "Upsilon;"sv, "Uring;"sv, "Uscr;"sv, "Utilde;"sv, "Uuml"sv, "Uuml;"sv, "VDash;"sv, "Vbar;"sv, "Vcy;"sv, "Vdash;"sv, "Vdashl;"sv, "Vee;"sv, "Verbar;"sv, "Vert;"sv, "VerticalBar;"sv, "VerticalLine;"sv, "VerticalSeparator;"sv, "VerticalTilde;"sv, "VeryThinSpace;"sv, "Vfr;"sv, "Vopf;"sv, "Vscr;"sv, "Vvdash;"sv, "Wcirc;"sv, "Wedge;"sv, "Wfr;"sv, "Wopf;"sv, "Wscr;"sv, "Xfr;"sv, "Xi;"sv, "Xopf;"sv, "Xscr;"sv, "YAcy;"sv, "YIcy;"sv, "YUcy;"sv, "Yacute"sv, "Yacute;"sv, "Ycirc;"sv, "Ycy;"sv, "Yfr;"sv, "Yopf;"sv, "Yscr;"sv, "Yuml;"sv, "ZHcy;"sv, "Zacute;"sv, "Zcaron;"sv, "Zcy;"sv, "Zdot;"sv, "ZeroWidthSpace;"sv, "Zeta;"sv, "Zfr;"sv, "Zopf;"sv, "Zscr;"sv, "aacute"sv, "aacute;"sv, "abreve;"sv, "ac;"sv, "acE;"sv, "acd;"sv, "acirc"sv, "acirc;"sv, "acute"sv, "acute;"sv, "acy;"sv, "aelig"sv, "aelig;"sv, "af;"sv, "afr;"sv, "agrave"sv, "agrave;"sv, "alefsym;"sv, "aleph;"sv, "alpha;"sv, "amacr;"sv, "amalg;"sv, "amp"sv, "amp;"sv, "and;"sv, "andand;"sv, "andd;"sv, "andslope;"sv, "andv;"sv, "ang;"sv, "ange;"sv, "angle;"sv, "angmsd;"sv, "angmsdaa;"sv, "angmsdab;"sv, "angmsdac;"sv, "angmsdad;"sv, "angmsdae;"sv, "angmsdaf;"sv, "angmsdag;"sv, "angmsdah;"sv, "angrt;"sv, "angrtvb;"sv, "angrtvbd;"sv, "angsph;"sv, "angst;"sv, "angzarr;"sv, "aogon;"sv, "aopf;"sv, "ap;"sv, "apE;"sv, "apacir;"sv, "ape;"sv, "apid;"sv, "apos;"sv, "approx;"sv, "approxeq;"sv, "aring"sv, "aring;"sv, "ascr;"sv, "ast;"sv, "asymp;"sv, "asympeq;"sv, "atilde"sv, "atilde;"sv, "auml"sv, "auml;"sv, "awconint;"sv, "awint;"sv, "bNot;"sv, "backcong;"sv, "backepsilon;"sv, "backprime;"sv, "backsim;"sv, "backsimeq;"sv, "barvee;"sv, "barwed;"sv, "barwedge;"sv, "bbrk;"sv, "bbrktbrk;"sv, "bcong;"sv, "bcy;"sv, "bdquo;"sv, "becaus;"sv, "because;"sv, "bemptyv;"sv, "bepsi;"sv, "bernou;"sv, "beta;"sv, "beth;"sv, "between;"sv, "bfr;"sv, "bigcap;"sv, "bigcirc;"sv, "bigcup;"sv, "bigodot;"sv, "bigoplus;"sv, "bigotimes;"sv, "bigsqcup;"sv, "bigstar;"sv, "bigtriangledown;"sv, "bigtriangleup;"sv, "biguplus;"sv, "bigvee;"sv, "bigwedge;"sv, "bkarow;"sv, "blacklozenge;"sv, "blacksquare;"sv, "blacktriangle;"sv, "blacktriangledown;"sv, "blacktriangleleft;"sv, "blacktriangleright;"sv, "blank;"sv, "blk12;"sv, "blk14;"sv, "blk34;"sv, "block;"sv, "bne;"sv, "bnequiv;"sv, "bnot;"sv, "bopf;"sv, "bot;"sv, "bottom;"sv, "bowtie;"sv, "boxDL;"sv, "boxDR;"sv, "boxDl;"sv, "boxDr;"sv, "boxH;"sv, "boxHD;"sv, "boxHU;"sv, "boxHd;"sv, "boxHu;"sv, "boxUL;"sv, "boxUR;"sv, "boxUl;"sv, "boxUr;"sv, "boxV;"sv, "boxVH;"sv, "boxVL;"sv, "boxVR;"sv, "boxVh;"sv, "boxVl;"sv, "boxVr;"sv, "boxbox;"sv, "boxdL;"sv, "boxdR;"sv, "boxdl;"sv, "boxdr;"sv, "boxh;"sv, "boxhD;"sv, "boxhU;"sv, "boxhd;"sv, "boxhu;"sv, "boxminus;"sv, "boxplus;"sv, "boxtimes;"sv, "boxuL;"sv, "boxuR;"sv, "boxul;"sv, "boxur;"sv, "boxv;"sv, "boxvH;"sv, "boxvL;"sv, "boxvR;"sv, "boxvh;"sv, "boxvl;"sv, "boxvr;"sv, "bprime;"sv, "breve;"sv, "brvbar"sv, "brvbar;"sv, "bscr;"sv, "bsemi;"sv, "bsim;"sv, "bsime;"sv, "bsol;"sv, "bsolb;"sv, "bsolhsub;"sv, "bull;"sv, "bullet;"sv, "bump;"sv, "bumpE;"sv, "bumpe;"sv, "bumpeq;"sv, "cacute;"sv, "cap;"sv, "capand;"sv, "capbrcup;"sv, "capcap;"sv, "capcup;"sv, "capdot;"sv, "caps;"sv, "caret;"sv, "caron;"sv, "ccaps;"sv, "ccaron;"sv, "ccedil"sv, "ccedil;"sv, "ccirc;"sv, "ccups;"sv, "ccupssm;"sv, "cdot;"sv, "cedil"sv, "cedil;"sv, "cemptyv;"sv, "cent"sv, "cent;"sv, "centerdot;"sv, "cfr;"sv, "chcy;"sv, "check;"sv, "checkmark;"sv, "chi;"sv, "cir;"sv, "cirE;"sv, "circ;"sv, "circeq;"sv, "circlearrowleft;"sv, "circlearrowright;"sv, "circledR;"sv, "circledS;"sv, "circledast;"sv, "circledcirc;"sv, "circleddash;"sv, "cire;"sv, "cirfnint;"sv, "cirmid;"sv, "cirscir;"sv, "clubs;"sv, "clubsuit;"sv, "colon;"sv, "colone;"sv, "coloneq;"sv, "comma;"sv, "commat;"sv, "comp;"sv, "compfn;"sv, "complement;"sv, "complexes;"sv, "cong;"sv, "congdot;"sv, "conint;"sv, "copf;"sv, "coprod;"sv, "copy"sv, "copy;"sv, "copysr;"sv, "crarr;"sv, "cross;"sv, "cscr;"sv, "csub;"sv, "csube;"sv, "csup;"sv, "csupe;"sv, "ctdot;"sv, "cudarrl;"sv, "cudarrr;"sv, "cuepr;"sv, "cuesc;"sv, "cularr;"sv, "cularrp;"sv, "cup;"sv, "cupbrcap;"sv, "cupcap;"sv, "cupcup;"sv, "cupdot;"sv, "cupor;"sv, "cups;"sv, "curarr;"sv, "curarrm;"sv, "curlyeqprec;"sv, "curlyeqsucc;"sv, "curlyvee;"sv, "curlywedge;"sv, "curren"sv, "curren;"sv, "curvearrowleft;"sv, "curvearrowright;"sv, "cuvee;"sv, "cuwed;"sv, "cwconint;"sv, "cwint;"sv, "cylcty;"sv, "dArr;"sv, "dHar;"sv, "dagger;"sv, "daleth;"sv, "darr;"sv, "dash;"sv, "dashv;"sv, "dbkarow;"sv, "dblac;"sv, "dcaron;"sv, "dcy;"sv, "dd;"sv, "ddagger;"sv, "ddarr;"sv, "ddotseq;"sv, "deg"sv, "deg;"sv, "delta;"sv, "demptyv;"sv, "dfisht;"sv, "dfr;"sv, "dharl;"sv, "dharr;"sv, "diam;"sv, "diamond;"sv, "diamondsuit;"sv, "diams;"sv, "die;"sv, "digamma;"sv, "disin;"sv, "div;"sv, "divide"sv, "divide;"sv, "divideontimes;"sv, "divonx;"sv, "djcy;"sv, "dlcorn;"sv, "dlcrop;"sv, "dollar;"sv, "dopf;"sv, "dot;"sv, "doteq;"sv, "doteqdot;"sv, "dotminus;"sv, "dotplus;"sv, "dotsquare;"sv, "doublebarwedge;"sv, "downarrow;"sv, "downdownarrows;"sv, "downharpoonleft;"sv, "downharpoonright;"sv, "drbkarow;"sv, "drcorn;"sv, "drcrop;"sv, "dscr;"sv, "dscy;"sv, "dsol;"sv, "dstrok;"sv, "dtdot;"sv, "dtri;"sv, "dtrif;"sv, "duarr;"sv, "duhar;"sv, "dwangle;"sv, "dzcy;"sv, "dzigrarr;"sv, "eDDot;"sv, "eDot;"sv, "eacute"sv, "eacute;"sv, "easter;"sv, "ecaron;"sv, "ecir;"sv, "ecirc"sv, "ecirc;"sv, "ecolon;"sv, "ecy;"sv, "edot;"sv, "ee;"sv, "efDot;"sv, "efr;"sv, "eg;"sv, "egrave"sv, "egrave;"sv, "egs;"sv, "egsdot;"sv, "el;"sv, "elinters;"sv, "ell;"sv, "els;"sv, "elsdot;"sv, "emacr;"sv, "empty;"sv, "emptyset;"sv, "emptyv;"sv, "emsp13;"sv, "emsp14;"sv, "emsp;"sv, "eng;"sv, "ensp;"sv, "eogon;"sv, "eopf;"sv, "epar;"sv, "eparsl;"sv, "eplus;"sv, "epsi;"sv, "epsilon;"sv, "epsiv;"sv, "eqcirc;"sv, "eqcolon;"sv, "eqsim;"sv, "eqslantgtr;"sv, "eqslantless;"sv, "equals;"sv, "equest;"sv, "equiv;"sv, "equivDD;"sv, "eqvparsl;"sv, "erDot;"sv, "erarr;"sv, "escr;"sv, "esdot;"sv, "esim;"sv, "eta;"sv, "eth"sv, "eth;"sv, "euml"sv, "euml;"sv, "euro;"sv, "excl;"sv, "exist;"sv, "expectation;"sv, "exponentiale;"sv, "fallingdotseq;"sv, "fcy;"sv, "female;"sv, "ffilig;"sv, "fflig;"sv, "ffllig;"sv, "ffr;"sv, "filig;"sv, "fjlig;"sv, "flat;"sv, "fllig;"sv, "fltns;"sv, "fnof;"sv, "fopf;"sv, "forall;"sv, "fork;"sv, "forkv;"sv, "fpartint;"sv, "frac12"sv, "frac12;"sv, "frac13;"sv, "frac14"sv, "frac14;"sv, "frac15;"sv, "frac16;"sv, "frac18;"sv, "frac23;"sv, "frac25;"sv, "frac34"sv, "frac34;"sv, "frac35;"sv, "frac38;"sv, "frac45;"sv, "frac56;"sv, "frac58;"sv, "frac78;"sv, "frasl;"sv, "frown;"sv, "fscr;"sv, "gE;"sv, "gEl;"sv, "gacute;"sv, "gamma;"sv, "gammad;"sv, "gap;"sv, "gbreve;"sv, "gcirc;"sv, "gcy;"sv, "gdot;"sv, "ge;"sv, "gel;"sv, "geq;"sv, "geqq;"sv, "geqslant;"sv, "ges;"sv, "gescc;"sv, "gesdot;"sv, "gesdoto;"sv, "gesdotol;"sv, "gesl;"sv, "gesles;"sv, "gfr;"sv, "gg;"sv, "ggg;"sv, "gimel;"sv, "gjcy;"sv, "gl;"sv, "glE;"sv, "gla;"sv, "glj;"sv, "gnE;"sv, "gnap;"sv, "gnapprox;"sv, "gne;"sv, "gneq;"sv, "gneqq;"sv, "gnsim;"sv, "gopf;"sv, "grave;"sv, "gscr;"sv, "gsim;"sv, "gsime;"sv, "gsiml;"sv, "gt"sv, "gt;"sv, "gtcc;"sv, "gtcir;"sv, "gtdot;"sv, "gtlPar;"sv, "gtquest;"sv, "gtrapprox;"sv, "gtrarr;"sv, "gtrdot;"sv, "gtreqless;"sv, "gtreqqless;"sv, "gtrless;"sv, "gtrsim;"sv, "gvertneqq;"sv, "gvnE;"sv, "hArr;"sv, "hairsp;"sv, "half;"sv, "hamilt;"sv, "hardcy;"sv, "harr;"sv, "harrcir;"sv, "harrw;"sv, "hbar;"sv, "hcirc;"sv, "hearts;"sv, "heartsuit;"sv, "hellip;"sv, "hercon;"sv, "hfr;"sv, "hksearow;"sv, "hkswarow;"sv, "hoarr;"sv, "homtht;"sv, "hookleftarrow;"sv, "hookrightarrow;"sv, "hopf;"sv, "horbar;"sv, "hscr;"sv, "hslash;"sv, "hstrok;"sv, "hybull;"sv, "hyphen;"sv, "iacute"sv, "iacute;"sv, "ic;"sv, "icirc"sv, "icirc;"sv, "icy;"sv, "iecy;"sv, "iexcl"sv, "iexcl;"sv, "iff;"sv, "ifr;"sv, "igrave"sv, "igrave;"sv, "ii;"sv, "iiiint;"sv, "iiint;"sv, "iinfin;"sv, "iiota;"sv, "ijlig;"sv, "imacr;"sv, "image;"sv, "imagline;"sv, "imagpart;"sv, "imath;"sv, "imof;"sv, "imped;"sv, "in;"sv, "incare;"sv, "infin;"sv, "infintie;"sv, "inodot;"sv, "int;"sv, "intcal;"sv, "integers;"sv, "intercal;"sv, "intlarhk;"sv, "intprod;"sv, "iocy;"sv, "iogon;"sv, "iopf;"sv, "iota;"sv, "iprod;"sv, "iquest"sv, "iquest;"sv, "iscr;"sv, "isin;"sv, "isinE;"sv, "isindot;"sv, "isins;"sv, "isinsv;"sv, "isinv;"sv, "it;"sv, "itilde;"sv, "iukcy;"sv, "iuml"sv, "iuml;"sv, "jcirc;"sv, "jcy;"sv, "jfr;"sv, "jmath;"sv, "jopf;"sv, "jscr;"sv, "jsercy;"sv, "jukcy;"sv, "kappa;"sv, "kappav;"sv, "kcedil;"sv, "kcy;"sv, "kfr;"sv, "kgreen;"sv, "khcy;"sv, "kjcy;"sv, "kopf;"sv, "kscr;"sv, "lAarr;"sv, "lArr;"sv, "lAtail;"sv, "lBarr;"sv, "lE;"sv, "lEg;"sv, "lHar;"sv, "lacute;"sv, "laemptyv;"sv, "lagran;"sv, "lambda;"sv, "lang;"sv, "langd;"sv, "langle;"sv, "lap;"sv, "laquo"sv, "laquo;"sv, "larr;"sv, "larrb;"sv, "larrbfs;"sv, "larrfs;"sv, "larrhk;"sv, "larrlp;"sv, "larrpl;"sv, "larrsim;"sv, "larrtl;"sv, "lat;"sv, "latail;"sv, "late;"sv, "lates;"sv, "lbarr;"sv, "lbbrk;"sv, "lbrace;"sv, "lbrack;"sv, "lbrke;"sv, "lbrksld;"sv, "lbrkslu;"sv, "lcaron;"sv, "lcedil;"sv, "lceil;"sv, "lcub;"sv, "lcy;"sv, "ldca;"sv, "ldquo;"sv, "ldquor;"sv, "ldrdhar;"sv, "ldrushar;"sv, "ldsh;"sv, "le;"sv, "leftarrow;"sv, "leftarrowtail;"sv, "leftharpoondown;"sv, "leftharpoonup;"sv, "leftleftarrows;"sv, "leftrightarrow;"sv, "leftrightarrows;"sv, "leftrightharpoons;"sv, "leftrightsquigarrow;"sv, "leftthreetimes;"sv, "leg;"sv, "leq;"sv, "leqq;"sv, "leqslant;"sv, "les;"sv, "lescc;"sv, "lesdot;"sv, "lesdoto;"sv, "lesdotor;"sv, "lesg;"sv, "lesges;"sv, "lessapprox;"sv, "lessdot;"sv, "lesseqgtr;"sv, "lesseqqgtr;"sv, "lessgtr;"sv, "lesssim;"sv, "lfisht;"sv, "lfloor;"sv, "lfr;"sv, "lg;"sv, "lgE;"sv, "lhard;"sv, "lharu;"sv, "lharul;"sv, "lhblk;"sv, "ljcy;"sv, "ll;"sv, "llarr;"sv, "llcorner;"sv, "llhard;"sv, "lltri;"sv, "lmidot;"sv, "lmoust;"sv, "lmoustache;"sv, "lnE;"sv, "lnap;"sv, "lnapprox;"sv, "lne;"sv, "lneq;"sv, "lneqq;"sv, "lnsim;"sv, "loang;"sv, "loarr;"sv, "lobrk;"sv, "longleftarrow;"sv, "longleftrightarrow;"sv, "longmapsto;"sv, "longrightarrow;"sv, "looparrowleft;"sv, "looparrowright;"sv, "lopar;"sv, "lopf;"sv, "loplus;"sv, "lotimes;"sv, "lowast;"sv, "lowbar;"sv, "loz;"sv, "lozenge;"sv, "lozf;"sv, "lpar;"sv, "lparlt;"sv, "lrarr;"sv, "lrcorner;"sv, "lrhar;"sv, "lrhard;"sv, "lrm;"sv, "lrtri;"sv, "lsaquo;"sv, "lscr;"sv, "lsh;"sv, "lsim;"sv, "lsime;"sv, "lsimg;"sv, "lsqb;"sv, "lsquo;"sv, "lsquor;"sv, "lstrok;"sv, "lt"sv, "lt;"sv, "ltcc;"sv, "ltcir;"sv, "ltdot;"sv, "lthree;"sv, "ltimes;"sv, "ltlarr;"sv, "ltquest;"sv, "ltrPar;"sv, "ltri;"sv, "ltrie;"sv, "ltrif;"sv, "lurdshar;"sv, "luruhar;"sv, "lvertneqq;"sv, "lvnE;"sv, "mDDot;"sv, "macr"sv, "macr;"sv, "male;"sv, "malt;"sv, "maltese;"sv, "map;"sv, "mapsto;"sv, "mapstodown;"sv, "mapstoleft;"sv, "mapstoup;"sv, "marker;"sv, "mcomma;"sv, "mcy;"sv, "mdash;"sv, "measuredangle;"sv, "mfr;"sv, "mho;"sv, "micro"sv, "micro;"sv, "mid;"sv, "midast;"sv, "midcir;"sv, "middot"sv, "middot;"sv, "minus;"sv, "minusb;"sv, "minusd;"sv, "minusdu;"sv, "mlcp;"sv, "mldr;"sv, "mnplus;"sv, "models;"sv, "mopf;"sv, "mp;"sv, "mscr;"sv, "mstpos;"sv, "mu;"sv, "multimap;"sv, "mumap;"sv, "nGg;"sv, "nGt;"sv, "nGtv;"sv, "nLeftarrow;"sv, "nLeftrightarrow;"sv, "nLl;"sv, "nLt;"sv, "nLtv;"sv, "nRightarrow;"sv, "nVDash;"sv, "nVdash;"sv, "nabla;"sv, "nacute;"sv, "nang;"sv, "nap;"sv, "napE;"sv, "napid;"sv, "napos;"sv, "napprox;"sv, "natur;"sv, "natural;"sv, "naturals;"sv, "nbsp"sv, "nbsp;"sv, "nbump;"sv, "nbumpe;"sv, "ncap;"sv, "ncaron;"sv, "ncedil;"sv, "ncong;"sv, "ncongdot;"sv, "ncup;"sv, "ncy;"sv, "ndash;"sv, "ne;"sv, "neArr;"sv, "nearhk;"sv, "nearr;"sv, "nearrow;"sv, "nedot;"sv, "nequiv;"sv, "nesear;"sv, "nesim;"sv, "nexist;"sv, "nexists;"sv, "nfr;"sv, "ngE;"sv, "nge;"sv, "ngeq;"sv, "ngeqq;"sv, "ngeqslant;"sv, "nges;"sv, "ngsim;"sv, "ngt;"sv, "ngtr;"sv, "nhArr;"sv, "nharr;"sv, "nhpar;"sv, "ni;"sv, "nis;"sv, "nisd;"sv, "niv;"sv, "njcy;"sv, "nlArr;"sv, "nlE;"sv, "nlarr;"sv, "nldr;"sv, "nle;"sv, "nleftarrow;"sv, "nleftrightarrow;"sv, "nleq;"sv, "nleqq;"sv, "nleqslant;"sv, "nles;"sv, "nless;"sv, "nlsim;"sv, "nlt;"sv, "nltri;"sv, "nltrie;"sv, "nmid;"sv, "nopf;"sv, "not"sv, "not;"sv, "notin;"sv, "notinE;"sv, "notindot;"sv, "notinva;"sv, "notinvb;"sv, "notinvc;"sv, "notni;"sv, "notniva;"sv, "notnivb;"sv, "notnivc;"sv, "npar;"sv, "nparallel;"sv, "nparsl;"sv, "npart;"sv, "npolint;"sv, "npr;"sv, "nprcue;"sv, "npre;"sv, "nprec;"sv, "npreceq;"sv, "nrArr;"sv, "nrarr;"sv, "nrarrc;"sv, "nrarrw;"sv, "nrightarrow;"sv, "nrtri;"sv, "nrtrie;"sv, "nsc;"sv, "nsccue;"sv, "nsce;"sv, "nscr;"sv, "nshortmid;"sv, "nshortparallel;"sv, "nsim;"sv, "nsime;"sv, "nsimeq;"sv, "nsmid;"sv, "nspar;"sv, "nsqsube;"sv, "nsqsupe;"sv, "nsub;"sv, "nsubE;"sv, "nsube;"sv, "nsubset;"sv, "nsubseteq;"sv, "nsubseteqq;"sv, "nsucc;"sv, "nsucceq;"sv, "nsup;"sv, "nsupE;"sv, "nsupe;"sv, "nsupset;"sv, "nsupseteq;"sv, "nsupseteqq;"sv, "ntgl;"sv, "ntilde"sv, "ntilde;"sv, "ntlg;"sv, "ntriangleleft;"sv, "ntrianglelefteq;"sv, "ntriangleright;"sv, "ntrianglerighteq;"sv, "nu;"sv, "num;"sv, "numero;"sv, "numsp;"sv, "nvDash;"sv, "nvHarr;"sv, "nvap;"sv, "nvdash;"sv, "nvge;"sv, "nvgt;"sv, "nvinfin;"sv, "nvlArr;"sv, "nvle;"sv, "nvlt;"sv, "nvltrie;"sv, "nvrArr;"sv, "nvrtrie;"sv, "nvsim;"sv, "nwArr;"sv, "nwarhk;"sv, "nwarr;"sv, "nwarrow;"sv, "nwnear;"sv, "oS;"sv, "oacute"sv, "oacute;"sv, "oast;"sv, "ocir;"sv, "ocirc"sv, "ocirc;"sv, "ocy;"sv, "odash;"sv, "odblac;"sv, "odiv;"sv, "odot;"sv, "odsold;"sv, "oelig;"sv, "ofcir;"sv, "ofr;"sv, "ogon;"sv, "ograve"sv, "ograve;"sv, "ogt;"sv, "ohbar;"sv, "ohm;"sv, "oint;"sv, "olarr;"sv, "olcir;"sv, "olcross;"sv, "oline;"sv, "olt;"sv, "omacr;"sv, "omega;"sv, "omicron;"sv, "omid;"sv, "ominus;"sv, "oopf;"sv, "opar;"sv, "operp;"sv, "oplus;"sv, "or;"sv, "orarr;"sv, "ord;"sv, "order;"sv, "orderof;"sv, "ordf"sv, "ordf;"sv, "ordm"sv, "ordm;"sv, "origof;"sv, "oror;"sv, "orslope;"sv, "orv;"sv, "oscr;"sv, "oslash"sv, "oslash;"sv, "osol;"sv, "otilde"sv, "otilde;"sv, "otimes;"sv, "otimesas;"sv, "ouml"sv, "ouml;"sv, "ovbar;"sv, "par;"sv, "para"sv, "para;"sv, "parallel;"sv, "parsim;"sv, "parsl;"sv, "part;"sv, "pcy;"sv, "percnt;"sv, "period;"sv, "permil;"sv, "perp;"sv, "pertenk;"sv, "pfr;"sv, "phi;"sv, "phiv;"sv, "phmmat;"sv, "phone;"sv, "pi;"sv, "pitchfork;"sv, "piv;"sv, "planck;"sv, "planckh;"sv, "plankv;"sv, "plus;"sv, "plusacir;"sv, "plusb;"sv, "pluscir;"sv, "plusdo;"sv, "plusdu;"sv, "pluse;"sv, "plusmn"sv, "plusmn;"sv, "plussim;"sv, "plustwo;"sv, "pm;"sv, "pointint;"sv, "popf;"sv, "pound"sv, "pound;"sv, "pr;"sv, "prE;"sv, "prap;"sv, "prcue;"sv, "pre;"sv, "prec;"sv, "precapprox;"sv, "preccurlyeq;"sv, "preceq;"sv, "precnapprox;"sv, "precneqq;"sv, "precnsim;"sv, "precsim;"sv, "prime;"sv, "primes;"sv, "prnE;"sv, "prnap;"sv, "prnsim;"sv, "prod;"sv, "profalar;"sv, "profline;"sv, "profsurf;"sv, "prop;"sv, "propto;"sv, "prsim;"sv, "prurel;"sv, "pscr;"sv, "psi;"sv, "puncsp;"sv, "qfr;"sv, "qint;"sv, "qopf;"sv, "qprime;"sv, "qscr;"sv, "quaternions;"sv, "quatint;"sv, "quest;"sv, "questeq;"sv, "quot"sv, "quot;"sv, "rAarr;"sv, "rArr;"sv, "rAtail;"sv, "rBarr;"sv, "rHar;"sv, "race;"sv, "racute;"sv, "radic;"sv, "raemptyv;"sv, "rang;"sv, "rangd;"sv, "range;"sv, "rangle;"sv, "raquo"sv, "raquo;"sv, "rarr;"sv, "rarrap;"sv, "rarrb;"sv, "rarrbfs;"sv, "rarrc;"sv, "rarrfs;"sv, "rarrhk;"sv, "rarrlp;"sv, "rarrpl;"sv, "rarrsim;"sv, "rarrtl;"sv, "rarrw;"sv, "ratail;"sv, "ratio;"sv, "rationals;"sv, "rbarr;"sv, "rbbrk;"sv, "rbrace;"sv, "rbrack;"sv, "rbrke;"sv, "rbrksld;"sv, "rbrkslu;"sv, "rcaron;"sv, "rcedil;"sv, "rceil;"sv, "rcub;"sv, "rcy;"sv, "rdca;"sv, "rdldhar;"sv, "rdquo;"sv, "rdquor;"sv, "rdsh;"sv, "real;"sv, "realine;"sv, "realpart;"sv, "reals;"sv, "rect;"sv, "reg"sv, "reg;"sv, "rfisht;"sv, "rfloor;"sv, "rfr;"sv, "rhard;"sv, "rharu;"sv, "rharul;"sv, "rho;"sv, "rhov;"sv, "rightarrow;"sv, "rightarrowtail;"sv, "rightharpoondown;"sv, "rightharpoonup;"sv, "rightleftarrows;"sv, "rightleftharpoons;"sv, "rightrightarrows;"sv, "rightsquigarrow;"sv, "rightthreetimes;"sv, "ring;"sv, "risingdotseq;"sv, "rlarr;"sv, "rlhar;"sv, "rlm;"sv, "rmoust;"sv, "rmoustache;"sv, "rnmid;"sv, "roang;"sv, "roarr;"sv, "robrk;"sv, "ropar;"sv, "ropf;"sv, "roplus;"sv, "rotimes;"sv, "rpar;"sv, "rpargt;"sv, "rppolint;"sv, "rrarr;"sv, "rsaquo;"sv, "rscr;"sv, "rsh;"sv, "rsqb;"sv, "rsquo;"sv, "rsquor;"sv, "rthree;"sv, "rtimes;"sv, "rtri;"sv, "rtrie;"sv, "rtrif;"sv, "rtriltri;"sv, "ruluhar;"sv, "rx;"sv, "sacute;"sv, "sbquo;"sv, "sc;"sv, "scE;"sv, "scap;"sv, "scaron;"sv, "sccue;"sv, "sce;"sv, "scedil;"sv, "scirc;"sv, "scnE;"sv, "scnap;"sv, "scnsim;"sv, "scpolint;"sv, "scsim;"sv, "scy;"sv, "sdot;"sv, "sdotb;"sv, "sdote;"sv, "seArr;"sv, "searhk;"sv, "searr;"sv, "searrow;"sv, "sect"sv, "sect;"sv, "semi;"sv, "seswar;"sv, "setminus;"sv, "setmn;"sv, "sext;"sv, "sfr;"sv, "sfrown;"sv, "sharp;"sv, "shchcy;"sv, "shcy;"sv, "shortmid;"sv, "shortparallel;"sv, "shy"sv, "shy;"sv, "sigma;"sv, "sigmaf;"sv, "sigmav;"sv, "sim;"sv, "simdot;"sv, "sime;"sv, "simeq;"sv, "simg;"sv, "simgE;"sv, "siml;"sv, "simlE;"sv, "simne;"sv, "simplus;"sv, "simrarr;"sv, "slarr;"sv, "smallsetminus;"sv, "smashp;"sv, "smeparsl;"sv, "smid;"sv, "smile;"sv, "smt;"sv, "smte;"sv, "smtes;"sv, "softcy;"sv, "sol;"sv, "solb;"sv, "solbar;"sv, "sopf;"sv, "spades;"sv, "spadesuit;"sv, "spar;"sv, "sqcap;"sv, "sqcaps;"sv, "sqcup;"sv, "sqcups;"sv, "sqsub;"sv, "sqsube;"sv, "sqsubset;"sv, "sqsubseteq;"sv, "sqsup;"sv, "sqsupe;"sv, "sqsupset;"sv, "sqsupseteq;"sv, "squ;"sv, "square;"sv, "squarf;"sv, "squf;"sv, "srarr;"sv, "sscr;"sv, "ssetmn;"sv, "ssmile;"sv, "sstarf;"sv, "star;"sv, "starf;"sv, "straightepsilon;"sv, "straightphi;"sv, "strns;"sv, "sub;"sv, "subE;"sv, "subdot;"sv, "sube;"sv, "subedot;"sv, "submult;"sv, "subnE;"sv, "subne;"sv, "subplus;"sv, "subrarr;"sv, "subset;"sv, "subseteq;"sv, "subseteqq;"sv, "subsetneq;"sv, "subsetneqq;"sv, "subsim;"sv, "subsub;"sv, "subsup;"sv, "succ;"sv, "succapprox;"sv, "succcurlyeq;"sv, "succeq;"sv, "succnapprox;"sv, "succneqq;"sv, "succnsim;"sv, "succsim;"sv, "sum;"sv, "sung;"sv, "sup1"sv, "sup1;"sv, "sup2"sv, "sup2;"sv, "sup3"sv, "sup3;"sv, "sup;"sv, "supE;"sv, "supdot;"sv, "supdsub;"sv, "supe;"sv, "supedot;"sv, "suphsol;"sv, "suphsub;"sv, "suplarr;"sv, "supmult;"sv, "supnE;"sv, "supne;"sv, "supplus;"sv, "supset;"sv, "supseteq;"sv, "supseteqq;"sv, "supsetneq;"sv, "supsetneqq;"sv, "supsim;"sv, "supsub;"sv, "supsup;"sv, "swArr;"sv, "swarhk;"sv, "swarr;"sv, "swarrow;"sv, "swnwar;"sv, "szlig"sv, "szlig;"sv, "target;"sv, "tau;"sv, "tbrk;"sv, "tcaron;"sv, "tcedil;"sv, "tcy;"sv, "tdot;"sv, "telrec;"sv, "tfr;"sv, "there4;"sv, "therefore;"sv, "theta;"sv, "thetasym;"sv, "thetav;"sv, "thickapprox;"sv, "thicksim;"sv, "thinsp;"sv, "thkap;"sv, "thksim;"sv, "thorn"sv, "thorn;"sv, "tilde;"sv, "times"sv, "times;"sv, "timesb;"sv, "timesbar;"sv, "timesd;"sv, "tint;"sv, "toea;"sv, "top;"sv, "topbot;"sv, "topcir;"sv, "topf;"sv, "topfork;"sv, "tosa;"sv, "tprime;"sv, "trade;"sv, "triangle;"sv, "triangledown;"sv, "triangleleft;"sv, "trianglelefteq;"sv, "triangleq;"sv, "triangleright;"sv, "trianglerighteq;"sv, "tridot;"sv, "trie;"sv, "triminus;"sv, "triplus;"sv, "trisb;"sv, "tritime;"sv, "trpezium;"sv, "tscr;"sv, "tscy;"sv, "tshcy;"sv, "tstrok;"sv, "twixt;"sv, "twoheadleftarrow;"sv, "twoheadrightarrow;"sv, "uArr;"sv, "uHar;"sv, "uacute"sv, "uacute;"sv, "uarr;"sv, "ubrcy;"sv, "ubreve;"sv, "ucirc"sv, "ucirc;"sv, "ucy;"sv, "udarr;"sv, "udblac;"sv, "udhar;"sv, "ufisht;"sv, "ufr;"sv, "ugrave"sv, "ugrave;"sv, "uharl;"sv, "uharr;"sv, "uhblk;"sv, "ulcorn;"sv, "ulcorner;"sv, "ulcrop;"sv, "ultri;"sv, "umacr;"sv, "uml"sv, "uml;"sv, "uogon;"sv, "uopf;"sv, "uparrow;"sv, "updownarrow;"sv, "upharpoonleft;"sv, "upharpoonright;"sv, "uplus;"sv, "upsi;"sv, "upsih;"sv, "upsilon;"sv, "upuparrows;"sv, "urcorn;"sv, "urcorner;"sv, "urcrop;"sv, "uring;"sv, "urtri;"sv, "uscr;"sv, "utdot;"sv, "utilde;"sv, "utri;"sv, "utrif;"sv, "uuarr;"sv, "uuml"sv, "uuml;"sv, "uwangle;"sv, "vArr;"sv, "vBar;"sv, "vBarv;"sv, "vDash;"sv, "vangrt;"sv, "varepsilon;"sv, "varkappa;"sv, "varnothing;"sv, "varphi;"sv, "varpi;"sv, "varpropto;"sv, "varr;"sv, "varrho;"sv, "varsigma;"sv, "varsubsetneq;"sv, "varsubsetneqq;"sv, "varsupsetneq;"sv, "varsupsetneqq;"sv, "vartheta;"sv, "vartriangleleft;"sv, "vartriangleright;"sv, "vcy;"sv, "vdash;"sv, "vee;"sv, "veebar;"sv, "veeeq;"sv, "vellip;"sv, "verbar;"sv, "vert;"sv, "vfr;"sv, "vltri;"sv, "vnsub;"sv, "vnsup;"sv, "vopf;"sv, "vprop;"sv, "vrtri;"sv, "vscr;"sv, "vsubnE;"sv, "vsubne;"sv, "vsupnE;"sv, "vsupne;"sv, "vzigzag;"sv, "wcirc;"sv, "wedbar;"sv, "wedge;"sv, "wedgeq;"sv, "weierp;"sv, "wfr;"sv, "wopf;"sv, "wp;"sv, "wr;"sv, "wreath;"sv, "wscr;"sv, "xcap;"sv, "xcirc;"sv, "xcup;"sv, "xdtri;"sv, "xfr;"sv, "xhArr;"sv, "xharr;"sv, "xi;"sv, "xlArr;"sv, "xlarr;"sv, "xmap;"sv, "xnis;"sv, "xodot;"sv, "xopf;"sv, "xoplus;"sv, "xotime;"sv, "xrArr;"sv, "xrarr;"sv, "xscr;"sv, "xsqcup;"sv, "xuplus;"sv, "xutri;"sv, "xvee;"sv, "xwedge;"sv, "yacute"sv, "yacute;"sv, "yacy;"sv, "ycirc;"sv, "ycy;"sv, "yen"sv, "yen;"sv, "yfr;"sv, "yicy;"sv, "yopf;"sv, "yscr;"sv, "yucy;"sv, "yuml"sv, "yuml;"sv, "zacute;"sv, "zcaron;"sv, "zcy;"sv, "zdot;"sv, "zeetrf;"sv, "zeta;"sv, "zfr;"sv, "zhcy;"sv, "zigrarr;"sv, "zopf;"sv, "zscr;"sv, "zwj;"sv, "zwnj;"sv };

enum class TransformationChoice : u8 {
    valid,
    character_swap,
    truncated,
    insert_character,
    append_character,
    ascii_char_followed_by_random_bytes,
    random_bytes,
    MAX_VALUE,
};

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

struct SplitMix64 {
    uint64_t s;

    SplitMix64(uint64_t seed) : s(seed) {}

    uint64_t next() {
        s += 0x9e3779b97f4a7c15;

        uint64_t z = s;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }
};

struct Xoshiro256 {
    uint64_t s[4];

    Xoshiro256(uint64_t seed) {
        auto gen = SplitMix64(seed);

        s[0] = gen.next();
        s[1] = gen.next();
        s[2] = gen.next();
        s[3] = gen.next();
    }

    uint64_t next() {
        const uint64_t result = rotl(s[0] + s[3], 23) + s[0];

        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result;
    }

    template<typename T>
    T uintLessThan(T less_than) {
        T x = integer<T>();
        unsigned __int128 m = (unsigned __int128)x * less_than;
        T l = static_cast<T>(m);
        if (l < less_than) {
            T t = -less_than;

            if (t >= less_than) {
                t -= less_than;
                if (t >= less_than) {
                    t %= less_than;
                }
            }
            while (l < t) {
                x = integer<T>();
                m = (unsigned __int128)x * less_than;
                l = static_cast<T>(m);
            }
        }
        return static_cast<T>(m >> sizeof(T) * 8);
    }

    template<typename T>
    T integer() {
        return static_cast<T>(next());
    }

    template<typename T>
    T uintAtMost(T at_most) {
        return uintLessThan<T>(at_most + 1);
    }

    bool boolean() {
        return next() & 1;
    }
};

static char randAlphabeticAscii(Xoshiro256& rand) {
    u8 c = rand.uintAtMost<u8>('Z' - 'A') + 'A';
    if (rand.boolean()) c += 'a' - 'A';
    return static_cast<char>(c);
}

int main(int argc, char* argv[]) {
    //Vector<char, 64> buf;
    Array<char, 64> buf;
    size_t buf_size = 0;
    auto rand = Xoshiro256(0);

    Web::HTML::NamedCharacterReferenceMatcherDafsa dafsa;
    Web::HTML::NamedCharacterReferenceMatcherDafsaBinarySearch dafsa_binary_search;
    Web::HTML::NamedCharacterReferenceMatcherGecko gecko;
    Web::HTML::NamedCharacterReferenceMatcherBlink blink;
    Web::HTML::NamedCharacterReferenceMatcherWebKit webkit;
    Web::HTML::NamedCharacterReferenceMatcher* matcher = &dafsa;
    if (argc > 1) {
        if (strcmp(argv[1], "dafsa") == 0) {
            matcher = &dafsa;
        } else if (strcmp(argv[1], "dafsa-binary-search") == 0) {
            matcher = &dafsa_binary_search;
        } else if (strcmp(argv[1], "gecko") == 0) {
            matcher = &gecko;
        } else if (strcmp(argv[1], "blink") == 0) {
            matcher = &blink;
        } else if (strcmp(argv[1], "webkit") == 0) {
            matcher = &webkit;
        }
    }

    size_t num_matches = 0;
    size_t max_iterations = 1000000;
    for (size_t i=0; i<max_iterations; i++) {
        buf_size = 0;
        auto ref = named_character_references[rand.uintLessThan<size_t>(2231)];
        VERIFY(ref.copy_characters_to_buffer(buf.data(), buf.size()));
        buf_size += ref.length();

        TransformationChoice choice = static_cast<TransformationChoice>(rand.uintLessThan<size_t>((size_t)TransformationChoice::MAX_VALUE));

        size_t tmp;
        char* first;
        char* last;
        char* dest_last;
        switch (choice) {
        case TransformationChoice::valid:
            break;
        case TransformationChoice::character_swap:
            tmp = rand.uintLessThan<size_t>(buf_size);
            buf[tmp] = randAlphabeticAscii(rand);
            break;
        case TransformationChoice::truncated:
            buf_size -= rand.uintLessThan<size_t>(buf_size);
            break;
        case TransformationChoice::insert_character:
            tmp = rand.uintLessThan<size_t>(buf_size);
            first = buf.data() + tmp;
            last = buf.data() + buf_size;
            dest_last = last + 1;
            while (first != last)
                *(--dest_last) = *(--last);

            buf[tmp] = randAlphabeticAscii(rand);
            buf_size++;
            break;
        case TransformationChoice::append_character:
            buf[buf_size] = randAlphabeticAscii(rand);
            buf_size++;
            break;
        case TransformationChoice::ascii_char_followed_by_random_bytes:
            buf_size = 0;
            buf[buf_size++] = randAlphabeticAscii(rand);
            tmp = rand.uintLessThan<size_t>(10);
            for (size_t j=0; j<tmp; j++) {
                buf[buf_size++] = static_cast<char>(rand.integer<u8>());
            }
            break;
        case TransformationChoice::random_bytes:
            buf_size = 0;
            tmp = rand.uintLessThan<size_t>(20);
            for (size_t j=0; j<tmp; j++) {
                buf[buf_size++] = static_cast<char>(rand.integer<u8>());
            }
            break;
        default:
            VERIFY_NOT_REACHED();
        }

        matcher->reset();
        for (const auto& c : buf.span().slice(0, buf_size)) {
            if (!matcher->try_consume_code_point(c)) break;
        }

        if (matcher->code_points().has_value()) {
            num_matches++;
        }
    }

    dbgln("{}", num_matches);
    return 0;
}
