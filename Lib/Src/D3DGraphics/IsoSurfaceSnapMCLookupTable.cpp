#include "pch.h"
#include <D3DGraphics/IsoSurfacePolygonizerSnapMC.h>

namespace ISOSURFACE_POLYGONIZER_SNAPMC {
                                                                                               // --------
                                                                                               // -------=
static const char vlist0002[] = { 1, 8, 9,12};                                                 // -------+
                                                                                               // ------=-
                                                                                               // ------==
static const char vlist0005[] = { 1, 1, 9,12};                                                 // ------=+
static const char vlist0006[] = { 1,10, 8,13};                                                 // ------+-
static const char vlist0007[] = { 1,10, 0,13};                                                 // ------+=
static const char vlist0008[] = { 2,10, 9,12,13,10,12};                                        // ------++
                                                                                               // -----=--
                                                                                               // -----=-=
static const char vlist0011[] = { 1, 8, 2,12};                                                 // -----=-+
                                                                                               // -----==-
static const char vlist0013[] = { 1, 1, 2, 0};                                                 // -----===
static const char vlist0014[] = { 1, 1, 2,12};                                                 // -----==+
static const char vlist0015[] = { 2, 2, 8,13,10, 2,13};                                        // -----=+-
static const char vlist0016[] = { 2, 2, 0,13,10, 2,13};                                        // -----=+=
static const char vlist0017[] = { 2,13, 2,12,10, 2,13};                                        // -----=++
static const char vlist0018[] = { 1, 9,11,14};                                                 // -----+--
static const char vlist0019[] = { 1, 0,11,14};                                                 // -----+-=
static const char vlist0020[] = { 2, 8,11,12,11,14,12};                                        // -----+-+
static const char vlist0021[] = { 2, 1,11,14, 9, 1,14};                                        // -----+=-
static const char vlist0022[] = { 2, 1,11,14, 0, 1,14};                                        // -----+==
static const char vlist0023[] = { 2, 1,11,14, 1,14,12};                                        // -----+=+
static const char vlist0024[] = { 4,10,11,13,11,14,13,14, 9,13, 9, 8,13};                      // -----++-
static const char vlist0025[] = { 3,14, 0,13,10,11,13,11,14,13};                               // -----++=
static const char vlist0026[] = { 3,10,11,13,13,11,14,13,14,12};                               // -----+++
                                                                                               // ----=---
                                                                                               // ----=--=
static const char vlist0029[] = { 2, 8, 3,12, 3, 9,12};                                        // ----=--+
                                                                                               // ----=-=-
static const char vlist0031[] = { 1, 1, 3, 0};                                                 // ----=-==
static const char vlist0032[] = { 2, 1, 3,12, 3, 9,12};                                        // ----=-=+
static const char vlist0033[] = { 1, 3, 8,13};                                                 // ----=-+-
static const char vlist0034[] = { 1, 3, 0,13};                                                 // ----=-+=
static const char vlist0035[] = { 2, 3, 9,12,13, 3,12};                                        // ----=-++
                                                                                               // ----==--
static const char vlist0037[] = { 1, 3, 2, 0};                                                 // ----==-=
static const char vlist0038[] = { 2, 3, 2,12, 8, 3,12};                                        // ----==-+
static const char vlist0039[] = { 1, 3, 2, 1};                                                 // ----===-
static const char vlist0040[] = { 2, 1, 2, 0, 2, 1, 3};                                        // ----====
static const char vlist0041[] = { 2, 1, 3,12, 3, 2,12};                                        // ----===+
static const char vlist0042[] = { 2, 3, 2,13, 2, 8,13};                                        // ----==+-
static const char vlist0043[] = { 2, 2, 0,13, 3, 2,13};                                        // ----==+=
static const char vlist0044[] = { 2, 3, 2,12,13, 3,12};                                        // ----==++
static const char vlist0045[] = { 1, 9, 3,14};                                                 // ----=+--
static const char vlist0046[] = { 1, 0, 3,14};                                                 // ----=+-=
static const char vlist0047[] = { 2, 8, 3,12, 3,14,12};                                        // ----=+-+
static const char vlist0048[] = { 2, 9, 1,14, 1, 3,14};                                        // ----=+=-
static const char vlist0049[] = { 2, 1, 3,14, 0, 1,14};                                        // ----=+==
static const char vlist0050[] = { 2, 1, 3,12, 3,14,12};                                        // ----=+=+
static const char vlist0051[] = { 3, 9, 8,13,14, 9,13, 3,14,13};                               // ----=++-
static const char vlist0052[] = { 2,14, 0,13, 3,14,13};                                        // ----=++=
static const char vlist0053[] = { 2,13, 3,14,13,14,12};                                        // ----=+++
static const char vlist0054[] = { 1,11,10,15};                                                 // ----+---
static const char vlist0055[] = { 2,11, 0,15, 0,10,15};                                        // ----+--=
static const char vlist0056[] = { 4,11, 9,12,15,11,12,10,15,12, 8,10,12};                      // ----+--+
static const char vlist0057[] = { 1,11, 1,15};                                                 // ----+-=-
static const char vlist0058[] = { 2, 0, 1,15,11, 0,15};                                        // ----+-==
static const char vlist0059[] = { 3, 1,15,12,11, 9,12,15,11,12};                               // ----+-=+
static const char vlist0060[] = { 2,11, 8,13,15,11,13};                                        // ----+-+-
static const char vlist0061[] = { 2,15, 0,13,11, 0,15};                                        // ----+-+=
static const char vlist0062[] = { 3,11, 9,12,15,11,12,13,15,12};                               // ----+-++
static const char vlist0063[] = { 1, 2,10,15};                                                 // ----+=--
static const char vlist0064[] = { 2, 2, 0,15, 0,10,15};                                        // ----+=-=
static const char vlist0065[] = { 3,15, 2,12, 8,10,12,10,15,12};                               // ----+=-+
static const char vlist0066[] = { 1, 2, 1,15};                                                 // ----+==-
static const char vlist0067[] = { 2, 0, 1,15, 2, 0,15};                                        // ----+===
static const char vlist0068[] = { 2, 1,15,12,15, 2,12};                                        // ----+==+
static const char vlist0069[] = { 2,15, 2,13, 2, 8,13};                                        // ----+=+-
static const char vlist0070[] = { 2, 2, 0,13,15, 2,13};                                        // ----+=+=
static const char vlist0071[] = { 2,15, 2,12,13,15,12};                                        // ----+=++
static const char vlist0072[] = { 2, 9,10,14,10,15,14};                                        // ----++--
static const char vlist0073[] = { 2, 0,15,14, 0,10,15};                                        // ----++-=
static const char vlist0074[] = { 3, 8,10,12,10,15,12,15,14,12};                               // ----++-+
static const char vlist0075[] = { 2, 9, 1,14, 1,15,14};                                        // ----++=-
static const char vlist0076[] = { 2, 1,15,14, 0, 1,14};                                        // ----++==
static const char vlist0077[] = { 2, 1,15,12,15,14,12};                                        // ----++=+
static const char vlist0078[] = { 3, 9, 8,13,14, 9,13,15,14,13};                               // ----+++-
static const char vlist0079[] = { 2,14, 0,13,15,14,13};                                        // ----+++=
static const char vlist0080[] = { 2,13,15,14,13,14,12};                                        // ----++++
                                                                                               // ---=----
                                                                                               // ---=---=
static const char vlist0083[] = { 1, 4, 8, 9};                                                 // ---=---+
                                                                                               // ---=--=-
static const char vlist0085[] = { 1, 4, 1, 0};                                                 // ---=--==
static const char vlist0086[] = { 1, 4, 1, 9};                                                 // ---=--=+
static const char vlist0087[] = { 2, 8, 4,10, 4,13,10};                                        // ---=--+-
static const char vlist0088[] = { 2, 0, 4,10, 4,13,10};                                        // ---=--+=
static const char vlist0089[] = { 2, 4,10, 9, 4,13,10};                                        // ---=--++
                                                                                               // ---=-=--
static const char vlist0091[] = { 1, 2, 4, 0};                                                 // ---=-=-=
static const char vlist0092[] = { 1, 2, 4, 8};                                                 // ---=-=-+
                                                                                               // ---=-==-
static const char vlist0094[] = { 1, 1, 2, 4};                                                 // ---=-===
static const char vlist0095[] = { 1, 1, 2, 4};                                                 // ---=-==+
static const char vlist0096[] = { 3, 2, 4,10, 2, 4, 8,10, 4,13};                               // ---=-=+-
static const char vlist0097[] = { 2,10, 2, 4,13,10, 4};                                        // ---=-=+=
static const char vlist0098[] = { 2,10, 2, 4,13,10, 4};                                        // ---=-=++
static const char vlist0099[] = { 2, 4, 9,11,14, 4,11};                                        // ---=-+--
static const char vlist0100[] = { 2, 4, 0,11,14, 4,11};                                        // ---=-+-=
static const char vlist0101[] = { 2,11, 4, 8,14, 4,11};                                        // ---=-+-+
static const char vlist0102[] = { 3,14, 4,11,11, 4, 1, 4, 1, 9};                               // ---=-+=-
static const char vlist0103[] = { 2, 1,11, 4,11,14, 4};                                        // ---=-+==
static const char vlist0104[] = { 2, 1,11, 4,11,14, 4};                                        // ---=-+=+
static const char vlist0105[] = { 4,11,14,13,11,13,10,14, 4,13, 9, 4, 8};                      // ---=-++-
static const char vlist0106[] = { 3,10,11,13,13,11,14,13,14, 4};                               // ---=-++=
static const char vlist0107[] = { 3,10,11,13,13,11,14,13,14, 4};                               // ---=-+++
                                                                                               // ---==---
                                                                                               // ---==--=
static const char vlist0110[] = { 2, 4, 3, 9, 3, 4, 8};                                        // ---==--+
                                                                                               // ---==-=-
static const char vlist0112[] = { 2, 1, 3, 4, 3, 0, 4};                                        // ---==-==
static const char vlist0113[] = { 2, 1, 3, 4, 3, 9, 4};                                        // ---==-=+
static const char vlist0114[] = { 2, 3, 4,13, 4, 3, 8};                                        // ---==-+-
static const char vlist0115[] = { 2, 3, 0, 4,13, 3, 4};                                        // ---==-+=
static const char vlist0116[] = { 2, 3, 9, 4,13, 3, 4};                                        // ---==-++
                                                                                               // ---===--
static const char vlist0118[] = { 2, 3, 2, 4, 0, 3, 4};                                        // ---===-=
static const char vlist0119[] = { 2, 3, 2, 4, 8, 3, 4};                                        // ---===-+
static const char vlist0120[] = { 3, 3, 2, 4, 1, 2, 4, 1, 3, 4};                               // ---====-
static const char vlist0121[] = { 2, 1, 3, 4, 3, 2, 4};                                        // ---=====
static const char vlist0122[] = { 2, 1, 3, 4, 3, 2, 4};                                        // ---====+
static const char vlist0123[] = { 3, 3, 2, 4, 8, 2, 4,13, 3, 4};                               // ---===+-
static const char vlist0124[] = { 2, 3, 2, 4,13, 3, 4};                                        // ---===+=
static const char vlist0125[] = { 2, 3, 2, 4,13, 3, 4};                                        // ---===++
static const char vlist0126[] = { 2, 4, 3,14, 3, 4, 9};                                        // ---==+--
static const char vlist0127[] = { 2, 0, 3, 4, 3,14, 4};                                        // ---==+-=
static const char vlist0128[] = { 2, 8, 3, 4, 3,14, 4};                                        // ---==+-+
static const char vlist0129[] = { 3, 1, 9, 4, 1, 3, 4, 3,14, 4};                               // ---==+=-
static const char vlist0130[] = { 2, 1, 3, 4, 3,14, 4};                                        // ---==+==
static const char vlist0131[] = { 2, 1, 3, 4, 3,14, 4};                                        // ---==+=+
static const char vlist0132[] = { 3, 8, 9, 4,13, 3, 4, 3,14, 4};                               // ---==++-
static const char vlist0133[] = { 2, 3,14, 4,13, 3, 4};                                        // ---==++=
static const char vlist0134[] = { 2, 3,14, 4,13, 3, 4};                                        // ---==+++
static const char vlist0135[] = { 3, 4,10,15,10, 4,11, 4,15,11};                               // ---=+---
static const char vlist0136[] = { 4, 4, 0,10, 4,11, 0, 4,15,11,15, 4,10};                      // ---=+--=
static const char vlist0137[] = { 4,10, 4, 8,15, 4,10, 4,15,11, 4,11, 9};                      // ---=+--+
static const char vlist0138[] = { 3, 4, 1,11, 4,15, 1, 4,15,11};                               // ---=+-=-
static const char vlist0139[] = { 3, 1,15, 4,11, 0, 4,15,11, 4};                               // ---=+-==
static const char vlist0140[] = { 3, 1,15, 4,11, 9, 4,15,11, 4};                               // ---=+-=+
static const char vlist0141[] = { 3, 4,11, 8, 4,15,11,15, 4,13};                               // ---=+-+-
static const char vlist0142[] = { 3,11, 0, 4,15,11, 4,13,15, 4};                               // ---=+-+=
static const char vlist0143[] = { 3,11, 9, 4,15,11, 4,13,15, 4};                               // ---=+-++
static const char vlist0144[] = { 3, 4, 2,10, 4,15, 2,15, 4,10};                               // ---=+=--
static const char vlist0145[] = { 3,15, 2, 4, 0,10, 4,10,15, 4};                               // ---=+=-=
static const char vlist0146[] = { 3,15, 2, 4, 8,10, 4,10,15, 4};                               // ---=+=-+
static const char vlist0147[] = { 3,15, 2, 4, 1, 2, 4, 1,15, 4};                               // ---=+==-
static const char vlist0148[] = { 2, 1,15, 4,15, 2, 4};                                        // ---=+===
static const char vlist0149[] = { 2, 1,15, 4,15, 2, 4};                                        // ---=+==+
static const char vlist0150[] = { 3,15, 2, 4, 8, 2, 4,13,15, 4};                               // ---=+=+-
static const char vlist0151[] = { 2,15, 2, 4,13,15, 4};                                        // ---=+=+=
static const char vlist0152[] = { 2,15, 2, 4,13,15, 4};                                        // ---=+=++
static const char vlist0153[] = { 3,10, 4, 9,15, 4,10, 4,15,14};                               // ---=++--
static const char vlist0154[] = { 3, 0,10, 4,10,15, 4,15,14, 4};                               // ---=++-=
static const char vlist0155[] = { 3, 8,10, 4,10,15, 4,15,14, 4};                               // ---=++-+
static const char vlist0156[] = { 3, 1, 9, 4, 1,15, 4,15,14, 4};                               // ---=++=-
static const char vlist0157[] = { 2, 1,15, 4,15,14, 4};                                        // ---=++==
static const char vlist0158[] = { 2, 1,15, 4,15,14, 4};                                        // ---=++=+
static const char vlist0159[] = { 3, 9, 8, 4,13,15, 4,15,14, 4};                               // ---=+++-
static const char vlist0160[] = { 2,13,15, 4,15,14, 4};                                        // ---=+++=
static const char vlist0161[] = { 2,13,15, 4,15,14, 4};                                        // ---=++++
static const char vlist0162[] = { 1,16,12,17};                                                 // ---+----
static const char vlist0163[] = { 1,16, 0,17};                                                 // ---+---=
static const char vlist0164[] = { 2,16, 8, 9,17,16, 9};                                        // ---+---+
static const char vlist0165[] = { 2,16, 1,17, 1,12,17};                                        // ---+--=-
static const char vlist0166[] = { 2,16, 1,17, 1, 0,17};                                        // ---+--==
static const char vlist0167[] = { 2,16, 1,17,17, 1, 9};                                        // ---+--=+
static const char vlist0168[] = { 4,16,13,10,17,16,10,12,17,10, 8,12,10};                      // ---+--+-
static const char vlist0169[] = { 3, 0,17,10,16,13,10,17,16,10};                               // ---+--+=
static const char vlist0170[] = { 3,16,13,10,17,16,10,17,10, 9};                               // ---+--++
static const char vlist0171[] = { 2, 2,17,16,12, 2,16};                                        // ---+-=--
static const char vlist0172[] = { 2, 2,17,16, 0, 2,16};                                        // ---+-=-=
static const char vlist0173[] = { 2, 2,17,16, 2,16, 8};                                        // ---+-=-+
static const char vlist0174[] = { 3, 1, 2,12, 2,16, 1, 2,17,16};                               // ---+-==-
static const char vlist0175[] = { 2, 1, 2,16,16, 2,17};                                        // ---+-===
static const char vlist0176[] = { 2, 1, 2,16,16, 2,17};                                        // ---+-==+
static const char vlist0177[] = { 4, 2,12, 8, 2,17,10,17,16,10,10,16,13};                      // ---+-=+-
static const char vlist0178[] = { 3,10, 2,17,10,17,16,13,10,16};                               // ---+-=+=
static const char vlist0179[] = { 3,10, 2,17,10,17,16,13,10,16};                               // ---+-=++
static const char vlist0180[] = { 4,14,16,11,14,17,16,16,12,11,12, 9,11};                      // ---+-+--
static const char vlist0181[] = { 3,16, 0,11,14,17,16,14,16,11};                               // ---+-+-=
static const char vlist0182[] = { 3,14,17,16,14,16,11,11,16, 8};                               // ---+-+-+
static const char vlist0183[] = { 4,14,16,11,14,17,16,11,16, 1,12, 1, 9};                      // ---+-+=-
static const char vlist0184[] = { 3, 1,11,16,11,14,16,16,14,17};                               // ---+-+==
static const char vlist0185[] = { 3, 1,11,16,11,14,16,16,14,17};                               // ---+-+=+
static const char vlist0186[] = { 5,11,14,13,11,13,10,14,17,16,14,16,13,12, 9, 8};             // ---+-++-
static const char vlist0187[] = { 4,10,11,13,13,11,14,13,14,16,16,14,17};                      // ---+-++=
static const char vlist0188[] = { 4,10,11,13,13,11,14,13,14,16,16,14,17};                      // ---+-+++
static const char vlist0189[] = { 3, 3,17,16,12,17, 3,12, 3,16};                               // ---+=---
static const char vlist0190[] = { 3, 3,17,16,17, 3, 0, 0, 3,16};                               // ---+=--=
static const char vlist0191[] = { 3, 3,17,16,17, 3, 9, 3,16, 8};                               // ---+=--+
static const char vlist0192[] = { 4,17, 3,12, 3,17,16,16, 3, 1, 1, 3,12};                      // ---+=-=-
static const char vlist0193[] = { 3, 1, 3,16, 3, 0,17,16, 3,17};                               // ---+=-==
static const char vlist0194[] = { 3, 1, 3,16, 3, 9,17,16, 3,17};                               // ---+=-=+
static const char vlist0195[] = { 4,17, 3,12, 3,17,16, 3,16,13,12, 3, 8};                      // ---+=-+-
static const char vlist0196[] = { 3, 3, 0,17,16, 3,17,13, 3,16};                               // ---+=-+=
static const char vlist0197[] = { 3, 3, 9,17,16, 3,17,13, 3,16};                               // ---+=-++
static const char vlist0198[] = { 4, 3,17,16,17, 3, 2, 2, 3,12, 3,16,12};                      // ---+==--
static const char vlist0199[] = { 3, 3, 2,17, 0, 3,16,16, 3,17};                               // ---+==-=
static const char vlist0200[] = { 3, 3, 2,17, 8, 3,16,16, 3,17};                               // ---+==-+
static const char vlist0201[] = { 4, 3, 2,17, 1, 2,12, 1, 3,16,16, 3,17};                      // ---+===-
static const char vlist0202[] = { 3, 1, 3,16, 3, 2,17,16, 3,17};                               // ---+====
static const char vlist0203[] = { 3, 1, 3,16, 3, 2,17,16, 3,17};                               // ---+===+
static const char vlist0204[] = { 4, 3, 2,17, 8, 2,12,13, 3,16,16, 3,17};                      // ---+==+-
static const char vlist0205[] = { 3, 3, 2,17,16, 3,17,13, 3,16};                               // ---+==+=
static const char vlist0206[] = { 3, 3, 2,17,16, 3,17,13, 3,16};                               // ---+==++
static const char vlist0207[] = { 4, 3,16,12,16, 3,17,17, 3,14, 3,12, 9};                      // ---+=+--
static const char vlist0208[] = { 3, 0, 3,16, 3,17,16, 3,14,17};                               // ---+=+-=
static const char vlist0209[] = { 3, 8, 3,16, 3,17,16, 3,14,17};                               // ---+=+-+
static const char vlist0210[] = { 4, 1, 9,12, 1, 3,16, 3,17,16, 3,14,17};                      // ---+=+=-
static const char vlist0211[] = { 3, 1, 3,16, 3,14,17, 3,17,16};                               // ---+=+==
static const char vlist0212[] = { 3, 1, 3,16, 3,14,17, 3,17,16};                               // ---+=+=+
static const char vlist0213[] = { 4, 9, 8,12,13, 3,16, 3,17,16, 3,14,17};                      // ---+=++-
static const char vlist0214[] = { 3, 3,14,17, 3,17,16,13, 3,16};                               // ---+=++=
static const char vlist0215[] = { 3, 3,14,17, 3,17,16,13, 3,16};                               // ---+=+++
static const char vlist0216[] = { 6,12,17,11,17,15,11,17,16,15,16,10,15,10,16,12,10,12,11};    // ---++---
static const char vlist0217[] = { 5,16, 0,10,15,17,16,17,15,11,17,11, 0,15,16,10};             // ---++--=
static const char vlist0218[] = { 5,10,16, 8,15,16,10,15,17,16,17,15,11,17,11, 9};             // ---++--+
static const char vlist0219[] = { 5, 1,12,11,12,17,11,17,15,11,15,17,16,15,16, 1};             // ---++-=-
static const char vlist0220[] = { 4, 1,15,16,11, 0,17,15,11,17,16,15,17};                      // ---++-==
static const char vlist0221[] = { 4, 1,15,16,11, 9,17,16,15,17,15,11,17};                      // ---++-=+
static const char vlist0222[] = { 5,12,11, 8,17,11,12,17,15,11,15,17,16,15,16,13};             // ---++-+-
static const char vlist0223[] = { 4,11, 0,17,15,11,17,16,15,17,13,15,16};                      // ---++-+=
static const char vlist0224[] = { 4,11, 9,17,16,15,17,13,15,16,15,11,17};                      // ---++-++
static const char vlist0225[] = { 5,12, 2,10,15,17,16,17,15, 2,15,16,10,16,12,10};             // ---++=--
static const char vlist0226[] = { 4,15, 2,17, 0,10,16,10,15,16,16,15,17};                      // ---++=-=
static const char vlist0227[] = { 4,15, 2,17, 8,10,16,16,15,17,10,15,16};                      // ---++=-+
static const char vlist0228[] = { 4,15, 2,17, 1, 2,12, 1,15,16,16,15,17};                      // ---++==-
static const char vlist0229[] = { 3, 1,15,16,15, 2,17,16,15,17};                               // ---++===
static const char vlist0230[] = { 3, 1,15,16,15, 2,17,16,15,17};                               // ---++==+
static const char vlist0231[] = { 4,15, 2,17, 2, 8,12,13,15,16,16,15,17};                      // ---++=+-
static const char vlist0232[] = { 3,15, 2,17,13,15,16,16,15,17};                               // ---++=+=
static const char vlist0233[] = { 3,15, 2,17,13,15,16,16,15,17};                               // ---++=++
static const char vlist0234[] = { 5,10,12, 9,10,16,12,15,16,10,15,17,16,17,15,14};             // ---+++--
static const char vlist0235[] = { 4, 0,10,16,10,15,16,16,15,17,15,14,17};                      // ---+++-=
static const char vlist0236[] = { 4, 8,10,16,16,15,17,10,15,16,15,14,17};                      // ---+++-+
static const char vlist0237[] = { 4, 9, 1,12, 1,15,16,15,14,17,16,15,17};                      // ---+++=-
static const char vlist0238[] = { 3, 1,15,16,15,14,17,16,15,17};                               // ---+++==
static const char vlist0239[] = { 3, 1,15,16,15,14,17,16,15,17};                               // ---+++=+
static const char vlist0240[] = { 4, 9, 8,12,13,15,16,16,15,17,15,14,17};                      // ---++++-
static const char vlist0241[] = { 3,13,15,16,16,15,17,15,14,17};                               // ---++++=
static const char vlist0242[] = { 3,13,15,16,16,15,17,15,14,17};                               // ---+++++
                                                                                               // --=-----
                                                                                               // --=----=
static const char vlist0245[] = { 2, 5, 8, 9,12, 5, 9};                                        // --=----+
                                                                                               // --=---=-
static const char vlist0247[] = { 1, 5, 1, 0};                                                 // --=---==
static const char vlist0248[] = { 2, 5, 1, 9,12, 5, 9};                                        // --=---=+
static const char vlist0249[] = { 1, 8, 5,10};                                                 // --=---+-
static const char vlist0250[] = { 1, 0, 5,10};                                                 // --=---+=
static const char vlist0251[] = { 2,12, 5, 9, 5,10, 9};                                        // --=---++
                                                                                               // --=--=--
                                                                                               // --=--=-=
static const char vlist0254[] = { 2, 5, 2,12, 2, 5, 8};                                        // --=--=-+
                                                                                               // --=--==-
static const char vlist0256[] = { 2, 1, 2, 5, 2, 0, 5};                                        // --=--===
static const char vlist0257[] = { 2, 1, 2, 5, 2,12, 5};                                        // --=--==+
static const char vlist0258[] = { 2, 2, 5,10, 5, 2, 8};                                        // --=--=+-
static const char vlist0259[] = { 2, 2, 0, 5,10, 2, 5};                                        // --=--=+=
static const char vlist0260[] = { 2, 2,12, 5,10, 2, 5};                                        // --=--=++
static const char vlist0261[] = { 3, 9, 5,14, 5, 9,11,14, 5,11};                               // --=--+--
static const char vlist0262[] = { 3, 5, 0,11, 5,14, 0,14, 5,11};                               // --=--+-=
static const char vlist0263[] = { 3,11, 5, 8,14, 5,11, 5,14,12};                               // --=--+-+
static const char vlist0264[] = { 4, 5, 1, 9, 5,11, 1,14, 5,11, 5,14, 9};                      // --=--+=-
static const char vlist0265[] = { 3, 1,11, 5,14, 0, 5,11,14, 5};                               // --=--+==
static const char vlist0266[] = { 3, 1,11, 5,14,12, 5,11,14, 5};                               // --=--+=+
static const char vlist0267[] = { 4, 5, 9, 8, 5,14, 9,14, 5,11,11, 5,10};                      // --=--++-
static const char vlist0268[] = { 3,14, 0, 5,10,11, 5,11,14, 5};                               // --=--++=
static const char vlist0269[] = { 3,14,12, 5,11,14, 5,10,11, 5};                               // --=--+++
                                                                                               // --=-=---
                                                                                               // --=-=--=
static const char vlist0272[] = { 3, 5, 3, 9, 5, 3, 8, 5, 9,12};                               // --=-=--+
static const char vlist0273[] = { 1, 5, 3, 1};                                                 // --=-=-=-
static const char vlist0274[] = { 1, 3, 0, 5};                                                 // --=-=-==
static const char vlist0275[] = { 2, 3, 9, 5, 9,12, 5};                                        // --=-=-=+
static const char vlist0276[] = { 1, 5, 3, 8};                                                 // --=-=-+-
static const char vlist0277[] = { 1, 3, 0, 5};                                                 // --=-=-+=
static const char vlist0278[] = { 2, 3, 9, 5, 9,12, 5};                                        // --=-=-++
                                                                                               // --=-==--
static const char vlist0280[] = { 3, 3, 2, 5, 2, 0, 5, 3, 0, 5};                               // --=-==-=
static const char vlist0281[] = { 3, 3, 2, 5, 2,12, 5, 3, 8, 5};                               // --=-==-+
static const char vlist0282[] = { 2, 3, 2, 5, 2, 1, 5};                                        // --=-===-
static const char vlist0283[] = { 2, 2, 0, 5, 3, 2, 5};                                        // --=-====
static const char vlist0284[] = { 2, 2,12, 5, 3, 2, 5};                                        // --=-===+
static const char vlist0285[] = { 2, 3, 2, 5, 2, 8, 5};                                        // --=-==+-
static const char vlist0286[] = { 2, 2, 0, 5, 3, 2, 5};                                        // --=-==+=
static const char vlist0287[] = { 2, 2,12, 5, 3, 2, 5};                                        // --=-==++
static const char vlist0288[] = { 3, 5, 3,14, 5, 3, 9, 5,14, 9};                               // --=-=+--
static const char vlist0289[] = { 3,14, 0, 5, 3,14, 5, 3, 0, 5};                               // --=-=+-=
static const char vlist0290[] = { 3,14,12, 5, 3,14, 5, 3, 8, 5};                               // --=-=+-+
static const char vlist0291[] = { 3, 9, 1, 5,14, 9, 5, 3,14, 5};                               // --=-=+=-
static const char vlist0292[] = { 2,14, 0, 5, 3,14, 5};                                        // --=-=+==
static const char vlist0293[] = { 2,14,12, 5, 3,14, 5};                                        // --=-=+=+
static const char vlist0294[] = { 3, 9, 8, 5,14, 9, 5, 3,14, 5};                               // --=-=++-
static const char vlist0295[] = { 2,14, 0, 5, 3,14, 5};                                        // --=-=++=
static const char vlist0296[] = { 2,14,12, 5, 3,14, 5};                                        // --=-=+++
static const char vlist0297[] = { 2,10, 5,11, 5,15,11};                                        // --=-+---
static const char vlist0298[] = { 3, 5,15,11, 5,11, 0, 0, 5,10};                               // --=-+--=
static const char vlist0299[] = { 4,15,11,12,12,11, 9, 5,15,12, 5,10, 8};                      // --=-+--+
static const char vlist0300[] = { 2, 1, 5,11, 5,15,11};                                        // --=-+-=-
static const char vlist0301[] = { 2,11, 0, 5,15,11, 5};                                        // --=-+-==
static const char vlist0302[] = { 3,11, 9,12,11,12,15,15,12, 5};                               // --=-+-=+
static const char vlist0303[] = { 2, 5,11, 8, 5,15,11};                                        // --=-+-+-
static const char vlist0304[] = { 2,11, 0, 5,15,11, 5};                                        // --=-+-+=
static const char vlist0305[] = { 3,11, 9,12,11,12,15,15,12, 5};                               // --=-+-++
static const char vlist0306[] = { 2, 5,15, 2, 5, 2,10};                                        // --=-+=--
static const char vlist0307[] = { 3,15, 2, 5, 2, 0, 5,10, 0, 5};                               // --=-+=-=
static const char vlist0308[] = { 3,15, 2, 5, 2,12, 5,10, 8, 5};                               // --=-+=-+
static const char vlist0309[] = { 2,15, 2, 5, 2, 1, 5};                                        // --=-+==-
static const char vlist0310[] = { 2, 2, 0, 5,15, 2, 5};                                        // --=-+===
static const char vlist0311[] = { 2, 2,12, 5,15, 2, 5};                                        // --=-+==+
static const char vlist0312[] = { 2,15, 2, 5, 2, 8, 5};                                        // --=-+=+-
static const char vlist0313[] = { 2, 2, 0, 5,15, 2, 5};                                        // --=-+=+=
static const char vlist0314[] = { 2, 2,12, 5,15, 2, 5};                                        // --=-+=++
static const char vlist0315[] = { 3, 5,15,14, 5,14, 9,10, 5, 9};                               // --=-++--
static const char vlist0316[] = { 3,14, 0, 5,15,14, 5,10, 0, 5};                               // --=-++-=
static const char vlist0317[] = { 3,14,12, 5,15,14, 5, 8,10, 5};                               // --=-++-+
static const char vlist0318[] = { 3, 9, 1, 5,14, 9, 5,15,14, 5};                               // --=-++=-
static const char vlist0319[] = { 2,14, 0, 5,15,14, 5};                                        // --=-++==
static const char vlist0320[] = { 2,14,12, 5,15,14, 5};                                        // --=-++=+
static const char vlist0321[] = { 3, 9, 8, 5,14, 9, 5,15,14, 5};                               // --=-+++-
static const char vlist0322[] = { 2,14, 0, 5,15,14, 5};                                        // --=-+++=
static const char vlist0323[] = { 2,14,12, 5,15,14, 5};                                        // --=-++++
                                                                                               // --==----
static const char vlist0325[] = { 1, 4, 5, 0};                                                 // --==---=
static const char vlist0326[] = { 2, 4, 5, 9, 5, 8, 9};                                        // --==---+
static const char vlist0327[] = { 1, 4, 5, 1};                                                 // --==--=-
static const char vlist0328[] = { 2, 4, 1, 0, 1, 4, 5};                                        // --==--==
static const char vlist0329[] = { 2, 5, 1, 9, 4, 5, 9};                                        // --==--=+
static const char vlist0330[] = { 2, 4, 5,10, 8, 4,10};                                        // --==--+-
static const char vlist0331[] = { 2, 0, 4,10, 4, 5,10};                                        // --==--+=
static const char vlist0332[] = { 2, 4, 5, 9, 5,10, 9};                                        // --==--++
                                                                                               // --==-=--
static const char vlist0334[] = { 2, 2, 4, 5, 0, 2, 5};                                        // --==-=-=
static const char vlist0335[] = { 2, 2, 4, 5, 8, 2, 5};                                        // --==-=-+
static const char vlist0336[] = { 3, 2, 4, 1, 2, 4, 5, 1, 2, 5};                               // --==-==-
static const char vlist0337[] = { 2, 1, 2, 5, 5, 2, 4};                                        // --==-===
static const char vlist0338[] = { 2, 1, 2, 5, 5, 2, 4};                                        // --==-==+
static const char vlist0339[] = { 3, 2, 4, 8, 2, 4, 5,10, 2, 5};                               // --==-=+-
static const char vlist0340[] = { 2, 5, 2, 4,10, 2, 5};                                        // --==-=+=
static const char vlist0341[] = { 2, 5, 2, 4,10, 2, 5};                                        // --==-=++
static const char vlist0342[] = { 4, 5, 4,14, 5, 4, 9,11, 5, 9,14, 5,11};                      // --==-+--
static const char vlist0343[] = { 3, 0,11, 5,11,14, 5,14, 4, 5};                               // --==-+-=
static const char vlist0344[] = { 3, 8,11, 5,11,14, 5,14, 4, 5};                               // --==-+-+
static const char vlist0345[] = { 4, 9, 4, 1, 1,11, 5,11,14, 5,14, 4, 5};                      // --==-+=-
static const char vlist0346[] = { 3, 1,11, 5,11,14, 5, 5,14, 4};                               // --==-+==
static const char vlist0347[] = { 3, 1,11, 5,11,14, 5, 5,14, 4};                               // --==-+=+
static const char vlist0348[] = { 4, 4, 9, 8,10,11, 5,11,14, 5,14, 4, 5};                      // --==-++-
static const char vlist0349[] = { 3,10,11, 5,11,14, 5, 5,14, 4};                               // --==-++=
static const char vlist0350[] = { 3,10,11, 5,11,14, 5, 5,14, 4};                               // --==-+++
                                                                                               // --===---
static const char vlist0352[] = { 3, 4, 3, 0, 3, 4, 5, 3, 0, 5};                               // --===--=
static const char vlist0353[] = { 3, 4, 3, 9, 3, 4, 5, 3, 8, 5};                               // --===--+
static const char vlist0354[] = { 2, 4, 3, 1, 3, 4, 5};                                        // --===-=-
static const char vlist0355[] = { 2, 3, 0, 4, 5, 3, 4};                                        // --===-==
static const char vlist0356[] = { 2, 3, 9, 4, 5, 3, 4};                                        // --===-=+
static const char vlist0357[] = { 2, 4, 3, 8, 3, 4, 5};                                        // --===-+-
static const char vlist0358[] = { 2, 3, 0, 4, 5, 3, 4};                                        // --===-+=
static const char vlist0359[] = { 2, 3, 9, 4, 5, 3, 4};                                        // --===-++
                                                                                               // --====--
static const char vlist0361[] = { 3, 3, 2, 4, 3, 0, 5, 5, 3, 4};                               // --====-=
static const char vlist0362[] = { 3, 3, 2, 4, 3, 8, 5, 5, 3, 4};                               // --====-+
static const char vlist0363[] = { 3, 3, 2, 4, 1, 2, 4, 5, 3, 4};                               // --=====-
static const char vlist0364[] = { 2, 3, 2, 4, 5, 3, 4};                                        // --======
static const char vlist0365[] = { 2, 3, 2, 4, 5, 3, 4};                                        // --=====+
static const char vlist0366[] = { 3, 3, 2, 4, 8, 2, 4, 5, 3, 4};                               // --====+-
static const char vlist0367[] = { 2, 3, 2, 4, 5, 3, 4};                                        // --====+=
static const char vlist0368[] = { 2, 3, 2, 4, 5, 3, 4};                                        // --====++
static const char vlist0369[] = { 4, 3,14, 5, 3, 5, 9, 5, 4, 9,14, 4, 5};                      // --===+--
static const char vlist0370[] = { 3, 3, 0, 5, 5, 3,14, 5,14, 4};                               // --===+-=
static const char vlist0371[] = { 3, 8, 3, 5, 5, 3,14, 5,14, 4};                               // --===+-+
static const char vlist0372[] = { 3, 1, 9, 4, 5, 3,14, 5,14, 4};                               // --===+=-
static const char vlist0373[] = { 2, 5, 3,14, 5,14, 4};                                        // --===+==
static const char vlist0374[] = { 2, 5, 3,14, 5,14, 4};                                        // --===+=+
static const char vlist0375[] = { 3, 9, 8, 4, 5, 3,14, 5,14, 4};                               // --===++-
static const char vlist0376[] = { 2, 5, 3,14, 5,14, 4};                                        // --===++=
static const char vlist0377[] = { 2, 5, 3,14, 5,14, 4};                                        // --===+++
static const char vlist0378[] = { 4, 5, 4,15, 5, 4,10, 4,11,10, 4,15,11};                      // --==+---
static const char vlist0379[] = { 4,10, 0, 5, 4,11, 0, 4,15,11,15, 4, 5};                      // --==+--=
static const char vlist0380[] = { 4, 8,10, 5, 4,11, 9, 4,15,11,15, 4, 5};                      // --==+--+
static const char vlist0381[] = { 3, 4,11, 1, 4,15,11,15, 4, 5};                               // --==+-=-
static const char vlist0382[] = { 3,11, 0, 4,15,11, 4, 5,15, 4};                               // --==+-==
static const char vlist0383[] = { 3,11, 9, 4,15,11, 4, 5,15, 4};                               // --==+-=+
static const char vlist0384[] = { 3, 4,11, 8, 4,15,11,15, 4, 5};                               // --==+-+-
static const char vlist0385[] = { 3,11, 0, 4,15,11, 4, 5,15, 4};                               // --==+-+=
static const char vlist0386[] = { 3,11, 9, 4,15,11, 4, 5,15, 4};                               // --==+-++
static const char vlist0387[] = { 4, 5, 4,10,15, 4, 5, 4,15, 2, 4, 2,10};                      // --==+=--
static const char vlist0388[] = { 3,15, 2, 4,10, 0, 5, 5,15, 4};                               // --==+=-=
static const char vlist0389[] = { 3,15, 2, 4, 8,10, 5, 5,15, 4};                               // --==+=-+
static const char vlist0390[] = { 3,15, 2, 4, 1, 2, 4, 5,15, 4};                               // --==+==-
static const char vlist0391[] = { 2,15, 2, 4, 5,15, 4};                                        // --==+===
static const char vlist0392[] = { 2,15, 2, 4, 5,15, 4};                                        // --==+==+
static const char vlist0393[] = { 3,15, 2, 4, 2, 8, 4, 5,15, 4};                               // --==+=+-
static const char vlist0394[] = { 2,15, 2, 4, 5,15, 4};                                        // --==+=+=
static const char vlist0395[] = { 2,15, 2, 4, 5,15, 4};                                        // --==+=++
static const char vlist0396[] = { 4,15,14, 5,14, 4, 5, 5, 4, 9,10, 5, 9};                      // --==++--
static const char vlist0397[] = { 3, 0,10, 5, 5,15,14, 5,14, 4};                               // --==++-=
static const char vlist0398[] = { 3, 8,10, 5, 5,15,14, 5,14, 4};                               // --==++-+
static const char vlist0399[] = { 3, 9, 1, 4, 5,15,14, 5,14, 4};                               // --==++=-
static const char vlist0400[] = { 2, 5,15,14, 5,14, 4};                                        // --==++==
static const char vlist0401[] = { 2, 5,15,14, 5,14, 4};                                        // --==++=+
static const char vlist0402[] = { 3, 9, 8, 4, 5,15,14, 5,14, 4};                               // --==+++-
static const char vlist0403[] = { 2, 5,15,14, 5,14, 4};                                        // --==+++=
static const char vlist0404[] = { 2, 5,15,14, 5,14, 4};                                        // --==++++
static const char vlist0405[] = { 1, 5,12,17};                                                 // --=+----
static const char vlist0406[] = { 1, 5, 0,17};                                                 // --=+---=
static const char vlist0407[] = { 2, 5, 8, 9,17, 5, 9};                                        // --=+---+
static const char vlist0408[] = { 2, 1,12,17, 5, 1,17};                                        // --=+--=-
static const char vlist0409[] = { 2, 5, 1,17, 1, 0,17};                                        // --=+--==
static const char vlist0410[] = { 2, 5, 1, 9,17, 5, 9};                                        // --=+--=+
static const char vlist0411[] = { 3, 8,12,10,12,17,10,17, 5,10};                               // --=+--+-
static const char vlist0412[] = { 2, 0,17,10,17, 5,10};                                        // --=+--+=
static const char vlist0413[] = { 2, 5,10,17,17,10, 9};                                        // --=+--++
static const char vlist0414[] = { 2,17, 5, 2, 2, 5,12};                                        // --=+-=--
static const char vlist0415[] = { 2, 2,17, 5, 0, 2, 5};                                        // --=+-=-=
static const char vlist0416[] = { 2, 2,17, 5, 8, 2, 5};                                        // --=+-=-+
static const char vlist0417[] = { 3, 2,17, 5, 1, 2, 5, 2,12, 1};                               // --=+-==-
static const char vlist0418[] = { 2, 1, 2, 5, 5, 2,17};                                        // --=+-===
static const char vlist0419[] = { 2, 1, 2, 5, 5, 2,17};                                        // --=+-==+
static const char vlist0420[] = { 3, 2,17, 5,10, 2, 5,12, 2, 8};                               // --=+-=+-
static const char vlist0421[] = { 2, 5, 2,17,10, 2, 5};                                        // --=+-=+=
static const char vlist0422[] = { 2, 5, 2,17,10, 2, 5};                                        // --=+-=++
static const char vlist0423[] = { 4,17, 5,14,14, 5,11,11, 5, 9, 5,12, 9};                      // --=+-+--
static const char vlist0424[] = { 3, 0,11, 5,11,14, 5,14,17, 5};                               // --=+-+-=
static const char vlist0425[] = { 3, 8,11, 5,11,14, 5,14,17, 5};                               // --=+-+-+
static const char vlist0426[] = { 4,12, 9, 1, 1,11, 5,11,14, 5,14,17, 5};                      // --=+-+=-
static const char vlist0427[] = { 3, 1,11, 5,11,14, 5, 5,14,17};                               // --=+-+==
static const char vlist0428[] = { 3, 1,11, 5,11,14, 5, 5,14,17};                               // --=+-+=+
static const char vlist0429[] = { 4,12, 9, 8,10,11, 5,11,14, 5,14,17, 5};                      // --=+-++-
static const char vlist0430[] = { 3,10,11, 5,11,14, 5, 5,14,17};                               // --=+-++=
static const char vlist0431[] = { 3,10,11, 5,11,14, 5, 5,14,17};                               // --=+-+++
static const char vlist0432[] = { 3,17, 3,12, 5,17, 3, 5, 3,12};                               // --=+=---
static const char vlist0433[] = { 3, 3,17, 5,17, 3, 0, 3, 0, 5};                               // --=+=--=
static const char vlist0434[] = { 3, 3,17, 5,17, 3, 9, 8, 3, 5};                               // --=+=--+
static const char vlist0435[] = { 3, 3,17, 5,17, 3,12,12, 3, 1};                               // --=+=-=-
static const char vlist0436[] = { 2, 3, 0,17, 5, 3,17};                                        // --=+=-==
static const char vlist0437[] = { 2, 3, 9,17, 5, 3,17};                                        // --=+=-=+
static const char vlist0438[] = { 3, 3,17, 5,17, 3,12,12, 3, 8};                               // --=+=-+-
static const char vlist0439[] = { 2, 3, 0,17, 5, 3,17};                                        // --=+=-+=
static const char vlist0440[] = { 2, 3, 9,17, 5, 3,17};                                        // --=+=-++
static const char vlist0441[] = { 4, 3,17, 5,17, 3, 2, 2, 3,12, 3, 5,12};                      // --=+==--
static const char vlist0442[] = { 3, 3, 2,17, 3, 0, 5, 5, 3,17};                               // --=+==-=
static const char vlist0443[] = { 3, 3, 2,17, 8, 3, 5, 5, 3,17};                               // --=+==-+
static const char vlist0444[] = { 3, 3, 2,17, 1, 2,12, 5, 3,17};                               // --=+===-
static const char vlist0445[] = { 2, 3, 2,17, 5, 3,17};                                        // --=+====
static const char vlist0446[] = { 2, 3, 2,17, 5, 3,17};                                        // --=+===+
static const char vlist0447[] = { 3, 3, 2,17, 2, 8,12, 5, 3,17};                               // --=+==+-
static const char vlist0448[] = { 2, 3, 2,17, 5, 3,17};                                        // --=+==+=
static const char vlist0449[] = { 2, 3, 2,17, 5, 3,17};                                        // --=+==++
static const char vlist0450[] = { 4, 3,14, 5, 3, 5, 9, 5,12, 9,14,17, 5};                      // --=+=+--
static const char vlist0451[] = { 3, 0, 3, 5, 5, 3,14, 5,14,17};                               // --=+=+-=
static const char vlist0452[] = { 3, 8, 3, 5, 5, 3,14, 5,14,17};                               // --=+=+-+
static const char vlist0453[] = { 3, 9, 1,12, 5, 3,14, 5,14,17};                               // --=+=+=-
static const char vlist0454[] = { 2, 5, 3,14, 5,14,17};                                        // --=+=+==
static const char vlist0455[] = { 2, 5, 3,14, 5,14,17};                                        // --=+=+=+
static const char vlist0456[] = { 3, 9, 8,12, 5, 3,14, 5,14,17};                               // --=+=++-
static const char vlist0457[] = { 2, 5, 3,14, 5,14,17};                                        // --=+=++=
static const char vlist0458[] = { 2, 5, 3,14, 5,14,17};                                        // --=+=+++
static const char vlist0459[] = { 5,17,11,12,17,15,11,17, 5,15,12,11,10, 5,12,10};             // --=++---
static const char vlist0460[] = { 4, 0,10, 5,15,17, 5,17,15,11,17,11, 0};                      // --=++--=
static const char vlist0461[] = { 4, 8,10, 5,15,17, 5,17,15,11,17,11, 9};                      // --=++--+
static const char vlist0462[] = { 4,15,17, 5,17,15,11,17,11,12,12,11, 1};                      // --=++-=-
static const char vlist0463[] = { 3,11, 0,17,15,11,17, 5,15,17};                               // --=++-==
static const char vlist0464[] = { 3,11, 9,17, 5,15,17,15,11,17};                               // --=++-=+
static const char vlist0465[] = { 4,15,17, 5,17,15,11,17,11,12,12,11, 8};                      // --=++-+-
static const char vlist0466[] = { 3,11, 0,17,15,11,17, 5,15,17};                               // --=++-+=
static const char vlist0467[] = { 3,11, 9,17, 5,15,17,15,11,17};                               // --=++-++
static const char vlist0468[] = { 4, 5,12,10,17,15, 2,15,17, 5,12, 2,10};                      // --=++=--
static const char vlist0469[] = { 3,15, 2,17, 0,10, 5, 5,15,17};                               // --=++=-=
static const char vlist0470[] = { 3,15, 2,17, 8,10, 5, 5,15,17};                               // --=++=-+
static const char vlist0471[] = { 3,15, 2,17, 2, 1,12, 5,15,17};                               // --=++==-
static const char vlist0472[] = { 2,15, 2,17, 5,15,17};                                        // --=++===
static const char vlist0473[] = { 2,15, 2,17, 5,15,17};                                        // --=++==+
static const char vlist0474[] = { 3,15, 2,17, 2, 8,12, 5,15,17};                               // --=++=+-
static const char vlist0475[] = { 2,15, 2,17, 5,15,17};                                        // --=++=+=
static const char vlist0476[] = { 2,15, 2,17, 5,15,17};                                        // --=++=++
static const char vlist0477[] = { 4,15,17, 5,15,14,17,10, 5,12,10,12, 9};                      // --=+++--
static const char vlist0478[] = { 3, 0,10, 5, 5,15,17,17,15,14};                               // --=+++-=
static const char vlist0479[] = { 3, 8,10, 5, 5,15,17,17,15,14};                               // --=+++-+
static const char vlist0480[] = { 3, 9, 1,12, 5,15,17,17,15,14};                               // --=+++=-
static const char vlist0481[] = { 2, 5,15,17,17,15,14};                                        // --=+++==
static const char vlist0482[] = { 2, 5,15,17,17,15,14};                                        // --=+++=+
static const char vlist0483[] = { 3, 9, 8,12, 5,15,17,17,15,14};                               // --=++++-
static const char vlist0484[] = { 2, 5,15,17,17,15,14};                                        // --=++++=
static const char vlist0485[] = { 2, 5,15,17,17,15,14};                                        // --=+++++
static const char vlist0486[] = { 1,13,16,18};                                                 // --+-----
static const char vlist0487[] = { 2, 0,16,18,13, 0,18};                                        // --+----=
static const char vlist0488[] = { 4,12,16, 9,16,18, 9,18,13, 9,13, 8, 9};                      // --+----+
static const char vlist0489[] = { 1, 1,16,18};                                                 // --+---=-
static const char vlist0490[] = { 2, 1, 0,18, 0,16,18};                                        // --+---==
static const char vlist0491[] = { 3,18, 1, 9,12,16, 9,16,18, 9};                               // --+---=+
static const char vlist0492[] = { 2, 8,16,10,16,18,10};                                        // --+---+-
static const char vlist0493[] = { 2, 0,18,10, 0,16,18};                                        // --+---+=
static const char vlist0494[] = { 3,12,16, 9,16,18, 9,18,10, 9};                               // --+---++
static const char vlist0495[] = { 3,18, 2,16,18,13, 2, 2,13,16};                               // --+--=--
static const char vlist0496[] = { 4, 2,18,13,18, 2,16,16, 2, 0, 0, 2,13};                      // --+--=-=
static const char vlist0497[] = { 4, 2,18,13,18, 2,16,16, 2,12, 2,13, 8};                      // --+--=-+
static const char vlist0498[] = { 3,18, 2,16,18, 2, 1, 1, 2,16};                               // --+--==-
static const char vlist0499[] = { 3, 1, 2,18, 2, 0,16, 2,16,18};                               // --+--===
static const char vlist0500[] = { 3, 1, 2,18, 2,12,16, 2,16,18};                               // --+--==+
static const char vlist0501[] = { 3,18, 2,16, 2,18,10,16, 2, 8};                               // --+--=+-
static const char vlist0502[] = { 3, 2, 0,16, 2,16,18,10, 2,18};                               // --+--=+=
static const char vlist0503[] = { 3, 2,12,16, 2,16,18,10, 2,18};                               // --+--=++
static const char vlist0504[] = { 6,18,13,11,14,18,11,16,18,14, 9,16,14,16, 9,13,13, 9,11};    // --+--+--
static const char vlist0505[] = { 5,13, 0,11,18,13,11,14,18,11,18,14,16,16,14, 0};             // --+--+-=
static const char vlist0506[] = { 5,11,13, 8,11,18,13,14,18,11,18,14,16,16,14,12};             // --+--+-+
static const char vlist0507[] = { 5, 1,16, 9,18,14,16,14,18,11,11,18, 1,16,14, 9};             // --+--+=-
static const char vlist0508[] = { 4, 1,11,18,14, 0,16,11,14,18,14,16,18};                      // --+--+==
static const char vlist0509[] = { 4, 1,11,18,14,12,16,14,16,18,11,14,18};                      // --+--+=+
static const char vlist0510[] = { 5,16, 9, 8,16,14, 9,18,14,16,14,18,11,11,18,10};             // --+--++-
static const char vlist0511[] = { 4,14, 0,16,10,11,18,14,16,18,11,14,18};                      // --+--++=
static const char vlist0512[] = { 4,14,12,16,14,16,18,11,14,18,10,11,18};                      // --+--+++
static const char vlist0513[] = { 2,18, 3,16, 3,13,16};                                        // --+-=---
static const char vlist0514[] = { 3, 3, 0,13,16, 3, 0,18, 3,16};                               // --+-=--=
static const char vlist0515[] = { 4,13, 3, 8,18, 3, 9,16,18, 9,16, 9,12};                      // --+-=--+
static const char vlist0516[] = { 2,18, 3,16, 3, 1,16};                                        // --+-=-=-
static const char vlist0517[] = { 2, 3, 0,16, 3,16,18};                                        // --+-=-==
static const char vlist0518[] = { 3, 3, 9,18,18, 9,16, 9,12,16};                               // --+-=-=+
static const char vlist0519[] = { 2,18, 3,16,16, 3, 8};                                        // --+-=-+-
static const char vlist0520[] = { 2, 3, 0,16, 3,16,18};                                        // --+-=-+=
static const char vlist0521[] = { 3, 3, 9,18,18, 9,16, 9,12,16};                               // --+-=-++
static const char vlist0522[] = { 4, 2, 3,13,18, 3, 2,18, 2,16,16, 2,13};                      // --+-==--
static const char vlist0523[] = { 4, 3, 2,18, 2,16,18, 2, 0,16, 3, 0,13};                      // --+-==-=
static const char vlist0524[] = { 4, 3, 2,18, 2,16,18, 2,12,16, 3, 8,13};                      // --+-==-+
static const char vlist0525[] = { 3, 3, 2,18, 2,16,18, 2, 1,16};                               // --+-===-
static const char vlist0526[] = { 3, 2, 0,16, 2,16,18, 3, 2,18};                               // --+-====
static const char vlist0527[] = { 3, 2,12,16, 2,16,18, 3, 2,18};                               // --+-===+
static const char vlist0528[] = { 3, 3, 2,18, 2,16,18, 2, 8,16};                               // --+-==+-
static const char vlist0529[] = { 3, 2, 0,16, 2,16,18, 3, 2,18};                               // --+-==+=
static const char vlist0530[] = { 3, 2,12,16, 2,16,18, 3, 2,18};                               // --+-==++
static const char vlist0531[] = { 5, 3,13, 9,18, 3,14,18,14,16,16,14, 9,13,16, 9};             // --+-=+--
static const char vlist0532[] = { 4,14, 0,16,14,16,18, 3,14,18, 3, 0,13};                      // --+-=+-=
static const char vlist0533[] = { 4,14,12,16,14,16,18, 3,14,18, 8, 3,13};                      // --+-=+-+
static const char vlist0534[] = { 4, 9, 1,16,14, 9,16,14,16,18, 3,14,18};                      // --+-=+=-
static const char vlist0535[] = { 3,14, 0,16, 3,14,18,14,16,18};                               // --+-=+==
static const char vlist0536[] = { 3,14,12,16,14,16,18, 3,14,18};                               // --+-=+=+
static const char vlist0537[] = { 4, 9, 8,16,14, 9,16,14,16,18, 3,14,18};                      // --+-=++-
static const char vlist0538[] = { 3,14, 0,16, 3,14,18,14,16,18};                               // --+-=++=
static const char vlist0539[] = { 3,14,12,16,14,16,18, 3,14,18};                               // --+-=+++
static const char vlist0540[] = { 4,16,15,11,18,15,16,13,16,11,10,13,11};                      // --+-+---
static const char vlist0541[] = { 4,16,15,11,18,15,16,16,11, 0, 0,13,10};                      // --+-+--=
static const char vlist0542[] = { 5,15,11,12,12,11, 9,18,15,16,16,15,12,13,10, 8};             // --+-+--+
static const char vlist0543[] = { 3, 1,16,11,18,15,16,16,15,11};                               // --+-+-=-
static const char vlist0544[] = { 3,11, 0,16,15,11,16,15,16,18};                               // --+-+-==
static const char vlist0545[] = { 4,11, 9,12,11,12,15,15,12,16,15,16,18};                      // --+-+-=+
static const char vlist0546[] = { 3,18,15,16,16,15,11,16,11, 8};                               // --+-+-+-
static const char vlist0547[] = { 3,11, 0,16,15,11,16,15,16,18};                               // --+-+-+=
static const char vlist0548[] = { 4,11, 9,12,11,12,15,15,12,16,15,16,18};                      // --+-+-++
static const char vlist0549[] = { 4,18,15, 2,16,18, 2,16, 2,13,13, 2,10};                      // --+-+=--
static const char vlist0550[] = { 4,15, 2,18,18, 2,16, 2, 0,16,10, 0,13};                      // --+-+=-=
static const char vlist0551[] = { 4,15, 2,18,18, 2,16, 2,12,16, 8,10,13};                      // --+-+=-+
static const char vlist0552[] = { 3,15, 2,18,18, 2,16, 2, 1,16};                               // --+-+==-
static const char vlist0553[] = { 3, 2, 0,16,18, 2,16,15, 2,18};                               // --+-+===
static const char vlist0554[] = { 3, 2,12,16,18, 2,16,15, 2,18};                               // --+-+==+
static const char vlist0555[] = { 3,15, 2,18,18, 2,16, 2, 8,16};                               // --+-+=+-
static const char vlist0556[] = { 3, 2, 0,16,18, 2,16,15, 2,18};                               // --+-+=+=
static const char vlist0557[] = { 3, 2,12,16,18, 2,16,15, 2,18};                               // --+-+=++
static const char vlist0558[] = { 5,18,15,14,18,14,16,16,14, 9,13,16, 9,10,13, 9};             // --+-++--
static const char vlist0559[] = { 4,14, 0,16,14,16,18,15,14,18, 0,10,13};                      // --+-++-=
static const char vlist0560[] = { 4,14,12,16,14,16,18,15,14,18, 8,10,13};                      // --+-++-+
static const char vlist0561[] = { 4, 9, 1,16,14, 9,16,14,16,18,15,14,18};                      // --+-++=-
static const char vlist0562[] = { 3,14, 0,16,15,14,18,14,16,18};                               // --+-++==
static const char vlist0563[] = { 3,14,12,16,14,16,18,15,14,18};                               // --+-++=+
static const char vlist0564[] = { 4, 9, 8,16,14, 9,16,14,16,18,15,14,18};                      // --+-+++-
static const char vlist0565[] = { 3,14, 0,16,15,14,18,14,16,18};                               // --+-+++=
static const char vlist0566[] = { 3,14,12,16,14,16,18,15,14,18};                               // --+-++++
static const char vlist0567[] = { 1,13, 4,18};                                                 // --+=----
static const char vlist0568[] = { 2, 0, 4,18,13, 0,18};                                        // --+=---=
static const char vlist0569[] = { 3, 4,18, 9,13, 8, 9,18,13, 9};                               // --+=---+
static const char vlist0570[] = { 1, 1, 4,18};                                                 // --+=--=-
static const char vlist0571[] = { 2, 1, 0,18, 0, 4,18};                                        // --+=--==
static const char vlist0572[] = { 2,18, 1, 9, 4,18, 9};                                        // --+=--=+
static const char vlist0573[] = { 2, 4,18,10, 8, 4,10};                                        // --+=--+-
static const char vlist0574[] = { 2, 0, 4,10, 4,18,10};                                        // --+=--+=
static const char vlist0575[] = { 2, 4,18, 9,18,10, 9};                                        // --+=--++
static const char vlist0576[] = { 3, 2, 4,13,18, 4, 2, 2,18,13};                               // --+=-=--
static const char vlist0577[] = { 3, 2, 4,18,13, 2,18, 0, 2,13};                               // --+=-=-=
static const char vlist0578[] = { 3, 2, 4,18,13, 2,18, 8, 2,13};                               // --+=-=-+
static const char vlist0579[] = { 3, 2, 4, 1, 2, 4,18, 1, 2,18};                               // --+=-==-
static const char vlist0580[] = { 2, 1, 2,18,18, 2, 4};                                        // --+=-===
static const char vlist0581[] = { 2, 1, 2,18,18, 2, 4};                                        // --+=-==+
static const char vlist0582[] = { 3, 4, 2, 8, 2, 4,18,10, 2,18};                               // --+=-=+-
static const char vlist0583[] = { 2,18, 2, 4,10, 2,18};                                        // --+=-=+=
static const char vlist0584[] = { 2,18, 2, 4,10, 2,18};                                        // --+=-=++
static const char vlist0585[] = { 5,13, 4, 9, 4,18,14,14,18,11,11,13, 9,11,18,13};             // --+=-+--
static const char vlist0586[] = { 4, 0,11,13,13,11,18,11,14,18,14, 4,18};                      // --+=-+-=
static const char vlist0587[] = { 4, 8,11,13,13,11,18,11,14,18,14, 4,18};                      // --+=-+-+
static const char vlist0588[] = { 4, 4, 9, 1, 1,11,18,11,14,18,14, 4,18};                      // --+=-+=-
static const char vlist0589[] = { 3, 1,11,18,11,14,18,18,14, 4};                               // --+=-+==
static const char vlist0590[] = { 3, 1,11,18,11,14,18,18,14, 4};                               // --+=-+=+
static const char vlist0591[] = { 4, 4, 9, 8,10,11,18,11,14,18,14, 4,18};                      // --+=-++-
static const char vlist0592[] = { 3,10,11,18,18,14, 4,11,14,18};                               // --+=-++=
static const char vlist0593[] = { 3,10,11,18,18,14, 4,11,14,18};                               // --+=-+++
static const char vlist0594[] = { 2, 4, 3,13, 4,18, 3};                                        // --+==---
static const char vlist0595[] = { 3, 4, 3, 0, 3, 4,18, 3, 0,13};                               // --+==--=
static const char vlist0596[] = { 3, 4, 3, 9, 3, 4,18, 8, 3,13};                               // --+==--+
static const char vlist0597[] = { 2, 4, 3, 1, 3, 4,18};                                        // --+==-=-
static const char vlist0598[] = { 2, 3, 0, 4,18, 3, 4};                                        // --+==-==
static const char vlist0599[] = { 2, 3, 9, 4,18, 3, 4};                                        // --+==-=+
static const char vlist0600[] = { 2, 4, 3, 8, 3, 4,18};                                        // --+==-+-
static const char vlist0601[] = { 2, 3, 0, 4,18, 3, 4};                                        // --+==-+=
static const char vlist0602[] = { 2, 3, 9, 4,18, 3, 4};                                        // --+==-++
static const char vlist0603[] = { 4,18, 3, 2, 2, 3,13, 4,18, 2, 4, 2,13};                      // --+===--
static const char vlist0604[] = { 3, 3, 2,18,18, 2, 4, 3, 0,13};                               // --+===-=
static const char vlist0605[] = { 3, 3, 2,18,18, 2, 4, 8, 3,13};                               // --+===-+
static const char vlist0606[] = { 3, 3, 2,18,18, 2, 4, 1, 2, 4};                               // --+====-
static const char vlist0607[] = { 2,18, 2, 4, 3, 2,18};                                        // --+=====
static const char vlist0608[] = { 2,18, 2, 4, 3, 2,18};                                        // --+====+
static const char vlist0609[] = { 3, 3, 2,18,18, 2, 4, 2, 8, 4};                               // --+===+-
static const char vlist0610[] = { 2,18, 2, 4, 3, 2,18};                                        // --+===+=
static const char vlist0611[] = { 2,18, 2, 4, 3, 2,18};                                        // --+===++
static const char vlist0612[] = { 4, 3,14,18, 3,13, 9,14, 4,18,13, 4, 9};                      // --+==+--
static const char vlist0613[] = { 3, 0, 3,13,18,14, 4,18, 3,14};                               // --+==+-=
static const char vlist0614[] = { 3, 8, 3,13,18,14, 4,18, 3,14};                               // --+==+-+
static const char vlist0615[] = { 3, 9, 1, 4,18,14, 4,18, 3,14};                               // --+==+=-
static const char vlist0616[] = { 2,18, 3,14,18,14, 4};                                        // --+==+==
static const char vlist0617[] = { 2,18, 3,14,18,14, 4};                                        // --+==+=+
static const char vlist0618[] = { 3, 9, 8, 4,18,14, 4,18, 3,14};                               // --+==++-
static const char vlist0619[] = { 2,18, 3,14,18,14, 4};                                        // --+==++=
static const char vlist0620[] = { 2,18, 3,14,18,14, 4};                                        // --+==+++
static const char vlist0621[] = { 4,13, 4,10, 4,11,10, 4,15,11, 4,18,15};                      // --+=+---
static const char vlist0622[] = { 4, 0,10,13, 4,11, 0, 4,15,11,15, 4,18};                      // --+=+--=
static const char vlist0623[] = { 4, 8,10,13, 4,11, 9, 4,15,11,15, 4,18};                      // --+=+--+
static const char vlist0624[] = { 3, 4,11, 1, 4,15,11,15, 4,18};                               // --+=+-=-
static const char vlist0625[] = { 3,11, 0, 4,15,11, 4,18,15, 4};                               // --+=+-==
static const char vlist0626[] = { 3,11, 9, 4,15,11, 4,18,15, 4};                               // --+=+-=+
static const char vlist0627[] = { 3, 4,11, 8, 4,15,11,15, 4,18};                               // --+=+-+-
static const char vlist0628[] = { 3,11, 0, 4,15,11, 4,18,15, 4};                               // --+=+-+=
static const char vlist0629[] = { 3,11, 9, 4,15,11, 4,18,15, 4};                               // --+=+-++
static const char vlist0630[] = { 4,15, 4,18, 4,15, 2, 4, 2,10,13, 4,10};                      // --+=+=--
static const char vlist0631[] = { 3,15, 2, 4, 0,10,13,18,15, 4};                               // --+=+=-=
static const char vlist0632[] = { 3,15, 2, 4, 8,10,13,18,15, 4};                               // --+=+=-+
static const char vlist0633[] = { 3,15, 2, 4, 2, 1, 4,18,15, 4};                               // --+=+==-
static const char vlist0634[] = { 2,15, 2, 4,18,15, 4};                                        // --+=+===
static const char vlist0635[] = { 2,15, 2, 4,18,15, 4};                                        // --+=+==+
static const char vlist0636[] = { 3,15, 2, 4, 2, 8, 4,18,15, 4};                               // --+=+=+-
static const char vlist0637[] = { 2,15, 2, 4,18,15, 4};                                        // --+=+=+=
static const char vlist0638[] = { 2,15, 2, 4,18,15, 4};                                        // --+=+=++
static const char vlist0639[] = { 4,15,14,18,14, 4,18,13, 4, 9,10,13, 9};                      // --+=++--
static const char vlist0640[] = { 3, 0,10,13,18,14, 4,18,15,14};                               // --+=++-=
static const char vlist0641[] = { 3, 8,10,13,18,14, 4,18,15,14};                               // --+=++-+
static const char vlist0642[] = { 3, 9, 1, 4,18,14, 4,18,15,14};                               // --+=++=-
static const char vlist0643[] = { 2,18,14, 4,18,15,14};                                        // --+=++==
static const char vlist0644[] = { 2,18,14, 4,18,15,14};                                        // --+=++=+
static const char vlist0645[] = { 3, 9, 8, 4,18,14, 4,18,15,14};                               // --+=+++-
static const char vlist0646[] = { 2,18,14, 4,18,15,14};                                        // --+=+++=
static const char vlist0647[] = { 2,18,14, 4,18,15,14};                                        // --+=++++
static const char vlist0648[] = { 2,13,12,17,18,13,17};                                        // --++----
static const char vlist0649[] = { 2,18, 0,17,13, 0,18};                                        // --++---=
static const char vlist0650[] = { 3,13, 8, 9,18,13, 9,17,18, 9};                               // --++---+
static const char vlist0651[] = { 2, 1,12,17,18, 1,17};                                        // --++--=-
static const char vlist0652[] = { 2,18, 1,17, 1, 0,17};                                        // --++--==
static const char vlist0653[] = { 2,18, 1, 9,17,18, 9};                                        // --++--=+
static const char vlist0654[] = { 3, 8,12,10,12,17,10,17,18,10};                               // --++--+-
static const char vlist0655[] = { 2, 0,17,10,17,18,10};                                        // --++--+=
static const char vlist0656[] = { 2,18,10,17,17,10, 9};                                        // --++--++
static const char vlist0657[] = { 3,17,18, 2, 2,18,13, 2,13,12};                               // --++-=--
static const char vlist0658[] = { 3, 2,17,18,13, 2,18, 0, 2,13};                               // --++-=-=
static const char vlist0659[] = { 3, 2,17,18,13, 2,18, 8, 2,13};                               // --++-=-+
static const char vlist0660[] = { 3, 2,17,18, 1, 2,18,12, 2, 1};                               // --++-==-
static const char vlist0661[] = { 2, 1, 2,18,18, 2,17};                                        // --++-===
static const char vlist0662[] = { 2, 1, 2,18,18, 2,17};                                        // --++-==+
static const char vlist0663[] = { 3, 2,17,18,10, 2,18,12, 2, 8};                               // --++-=+-
static const char vlist0664[] = { 2,18, 2,17,10, 2,18};                                        // --++-=+=
static const char vlist0665[] = { 2,18, 2,17,10, 2,18};                                        // --++-=++
static const char vlist0666[] = { 5,17,18,14,11,18,13,14,18,11,11,13, 9,13,12, 9};             // --++-+--
static const char vlist0667[] = { 4, 0,11,13,13,11,18,11,14,18,14,17,18};                      // --++-+-=
static const char vlist0668[] = { 4, 8,11,13,13,11,18,11,14,18,14,17,18};                      // --++-+-+
static const char vlist0669[] = { 4,12, 9, 1, 1,11,18,11,14,18,14,17,18};                      // --++-+=-
static const char vlist0670[] = { 3, 1,11,18,11,14,18,18,14,17};                               // --++-+==
static const char vlist0671[] = { 3, 1,11,18,11,14,18,18,14,17};                               // --++-+=+
static const char vlist0672[] = { 4,12, 9, 8,10,11,18,11,14,18,14,17,18};                      // --++-++-
static const char vlist0673[] = { 3,10,11,18,18,14,17,11,14,18};                               // --++-++=
static const char vlist0674[] = { 3,10,11,18,18,14,17,11,14,18};                               // --++-+++
static const char vlist0675[] = { 3,17, 3,12,17,18, 3, 3,13,12};                               // --++=---
static const char vlist0676[] = { 3, 3,17,18,17, 3, 0, 0, 3,13};                               // --++=--=
static const char vlist0677[] = { 3, 3,17,18,17, 3, 9, 8, 3,13};                               // --++=--+
static const char vlist0678[] = { 3, 3,17,18,17, 3,12,12, 3, 1};                               // --++=-=-
static const char vlist0679[] = { 2, 3, 0,17,18, 3,17};                                        // --++=-==
static const char vlist0680[] = { 2, 3, 9,17,18, 3,17};                                        // --++=-=+
static const char vlist0681[] = { 3, 3,17,18,17, 3,12,12, 3, 8};                               // --++=-+-
static const char vlist0682[] = { 2, 3, 0,17,18, 3,17};                                        // --++=-+=
static const char vlist0683[] = { 2, 3, 9,17,18, 3,17};                                        // --++=-++
static const char vlist0684[] = { 4, 3,17,18,17, 3, 2, 2, 3,12, 3,13,12};                      // --++==--
static const char vlist0685[] = { 3, 3, 2,17, 0, 3,13,18, 3,17};                               // --++==-=
static const char vlist0686[] = { 3, 3, 2,17, 8, 3,13,18, 3,17};                               // --++==-+
static const char vlist0687[] = { 3, 3, 2,17, 2, 1,12,18, 3,17};                               // --++===-
static const char vlist0688[] = { 2, 3, 2,17,18, 3,17};                                        // --++====
static const char vlist0689[] = { 2, 3, 2,17,18, 3,17};                                        // --++===+
static const char vlist0690[] = { 3, 3, 2,17, 2, 8,12,18, 3,17};                               // --++==+-
static const char vlist0691[] = { 2, 3, 2,17,18, 3,17};                                        // --++==+=
static const char vlist0692[] = { 2, 3, 2,17,18, 3,17};                                        // --++==++
static const char vlist0693[] = { 4, 3,14,18, 3,13, 9,14,17,18,13,12, 9};                      // --++=+--
static const char vlist0694[] = { 3, 0, 3,13,18,14,17,18, 3,14};                               // --++=+-=
static const char vlist0695[] = { 3, 8, 3,13,18,14,17,18, 3,14};                               // --++=+-+
static const char vlist0696[] = { 3, 9, 1,12,18,14,17,18, 3,14};                               // --++=+=-
static const char vlist0697[] = { 2,18, 3,14,18,14,17};                                        // --++=+==
static const char vlist0698[] = { 2,18, 3,14,18,14,17};                                        // --++=+=+
static const char vlist0699[] = { 3, 9, 8,12,18,14,17,18, 3,14};                               // --++=++-
static const char vlist0700[] = { 2,18, 3,14,18,14,17};                                        // --++=++=
static const char vlist0701[] = { 2,18, 3,14,18,14,17};                                        // --++=+++
static const char vlist0702[] = { 5,17,11,12,17,15,11,17,18,15,12,11,10,13,12,10};             // --+++---
static const char vlist0703[] = { 4, 0,10,13,15,17,18,17,15,11,17,11, 0};                      // --+++--=
static const char vlist0704[] = { 4, 8,10,13,15,17,18,17,15,11,17,11, 9};                      // --+++--+
static const char vlist0705[] = { 4,15,17,18,17,15,11,17,11,12,12,11, 1};                      // --+++-=-
static const char vlist0706[] = { 3,11, 0,17,15,11,17,18,15,17};                               // --+++-==
static const char vlist0707[] = { 3,11, 9,17,15,11,17,18,15,17};                               // --+++-=+
static const char vlist0708[] = { 4,15,17,18,17,15,11,17,11,12,12,11, 8};                      // --+++-+-
static const char vlist0709[] = { 3,11, 0,17,15,11,17,18,15,17};                               // --+++-+=
static const char vlist0710[] = { 3,11, 9,17,15,11,17,18,15,17};                               // --+++-++
static const char vlist0711[] = { 4,17,15, 2,15,17,18,12, 2,10,13,12,10};                      // --+++=--
static const char vlist0712[] = { 3,15, 2,17, 0,10,13,18,15,17};                               // --+++=-=
static const char vlist0713[] = { 3,15, 2,17, 8,10,13,18,15,17};                               // --+++=-+
static const char vlist0714[] = { 3,15, 2,17, 2, 1,12,18,15,17};                               // --+++==-
static const char vlist0715[] = { 2,15, 2,17,18,15,17};                                        // --+++===
static const char vlist0716[] = { 2,15, 2,17,18,15,17};                                        // --+++==+
static const char vlist0717[] = { 3,15, 2,17, 2, 8,12,18,15,17};                               // --+++=+-
static const char vlist0718[] = { 2,15, 2,17,18,15,17};                                        // --+++=+=
static const char vlist0719[] = { 2,15, 2,17,18,15,17};                                        // --+++=++
static const char vlist0720[] = { 4,15,17,18,15,14,17,10,13,12,10,12, 9};                      // --++++--
static const char vlist0721[] = { 3, 0,10,13,17,15,14,18,15,17};                               // --++++-=
static const char vlist0722[] = { 3, 8,10,13,17,15,14,18,15,17};                               // --++++-+
static const char vlist0723[] = { 3, 9, 1,12,17,15,14,18,15,17};                               // --++++=-
static const char vlist0724[] = { 2,17,15,14,18,15,17};                                        // --++++==
static const char vlist0725[] = { 2,17,15,14,18,15,17};                                        // --++++=+
static const char vlist0726[] = { 3, 9, 8,12,17,15,14,18,15,17};                               // --+++++-
static const char vlist0727[] = { 2,17,15,14,18,15,17};                                        // --+++++=
static const char vlist0728[] = { 2,17,15,14,18,15,17};                                        // --++++++
                                                                                               // -=------
                                                                                               // -=-----=
static const char vlist0731[] = { 2, 8, 6,12, 6, 8, 9};                                        // -=-----+
                                                                                               // -=----=-
                                                                                               // -=----==
static const char vlist0734[] = { 2, 1, 6,12, 6, 1, 9};                                        // -=----=+
static const char vlist0735[] = { 3, 6, 8,13, 8, 6,10, 6,13,10};                               // -=----+-
static const char vlist0736[] = { 3, 6, 0,10, 6,13, 0, 6,13,10};                               // -=----+=
static const char vlist0737[] = { 3, 6,10, 9, 6,13,10,13, 6,12};                               // -=----++
                                                                                               // -=---=--
static const char vlist0739[] = { 1, 2, 6, 0};                                                 // -=---=-=
static const char vlist0740[] = { 2, 2, 6, 8, 6,12, 8};                                        // -=---=-+
                                                                                               // -=---==-
static const char vlist0742[] = { 2, 1, 2, 6, 0, 1, 6};                                        // -=---===
static const char vlist0743[] = { 2, 1, 2, 6,12, 1, 6};                                        // -=---==+
static const char vlist0744[] = { 4, 6, 2,10, 6, 2, 8,10, 6,13,13, 6, 8};                      // -=---=+-
static const char vlist0745[] = { 3, 0,13, 6,13,10, 6,10, 2, 6};                               // -=---=+=
static const char vlist0746[] = { 3,12,13, 6,13,10, 6,10, 2, 6};                               // -=---=++
static const char vlist0747[] = { 1, 6, 9,11};                                                 // -=---+--
static const char vlist0748[] = { 1, 6, 0,11};                                                 // -=---+-=
static const char vlist0749[] = { 2, 6,12, 8,11, 6, 8};                                        // -=---+-+
static const char vlist0750[] = { 2,11, 6, 1, 1, 6, 9};                                        // -=---+=-
static const char vlist0751[] = { 2, 1,11, 6, 0, 1, 6};                                        // -=---+==
static const char vlist0752[] = { 2, 1,11, 6,12, 1, 6};                                        // -=---+=+
static const char vlist0753[] = { 4,11, 6,10,10, 6,13,13, 6, 8, 6, 9, 8};                      // -=---++-
static const char vlist0754[] = { 3, 0,13, 6,13,10, 6,10,11, 6};                               // -=---++=
static const char vlist0755[] = { 3,12,13, 6,13,10, 6,10,11, 6};                               // -=---+++
                                                                                               // -=--=---
                                                                                               // -=--=--=
static const char vlist0758[] = { 3, 3, 6, 9, 3, 6, 8, 6,12, 8};                               // -=--=--+
                                                                                               // -=--=-=-
static const char vlist0760[] = { 3, 1, 3, 6, 0, 1, 6, 0, 3, 6};                               // -=--=-==
static const char vlist0761[] = { 3, 1, 3, 6,12, 1, 6, 9, 3, 6};                               // -=--=-=+
static const char vlist0762[] = { 3, 6, 3,13, 6, 3, 8,13, 6, 8};                               // -=--=-+-
static const char vlist0763[] = { 3, 0,13, 6,13, 3, 6, 0, 3, 6};                               // -=--=-+=
static const char vlist0764[] = { 3,12,13, 6,13, 3, 6, 3, 9, 6};                               // -=--=-++
static const char vlist0765[] = { 1, 3, 6, 2};                                                 // -=--==--
static const char vlist0766[] = { 1, 0, 3, 6};                                                 // -=--==-=
static const char vlist0767[] = { 2, 8, 3, 6,12, 8, 6};                                        // -=--==-+
static const char vlist0768[] = { 2, 2, 1, 6, 1, 3, 6};                                        // -=--===-
static const char vlist0769[] = { 2, 1, 3, 6, 0, 1, 6};                                        // -=--====
static const char vlist0770[] = { 2, 1, 3, 6,12, 1, 6};                                        // -=--===+
static const char vlist0771[] = { 3, 2, 8, 6, 8,13, 6,13, 3, 6};                               // -=--==+-
static const char vlist0772[] = { 2, 0,13, 6,13, 3, 6};                                        // -=--==+=
static const char vlist0773[] = { 2,12,13, 6,13, 3, 6};                                        // -=--==++
static const char vlist0774[] = { 1, 3, 6, 9};                                                 // -=--=+--
static const char vlist0775[] = { 1, 0, 3, 6};                                                 // -=--=+-=
static const char vlist0776[] = { 2, 8, 3, 6,12, 8, 6};                                        // -=--=+-+
static const char vlist0777[] = { 2, 9, 1, 6, 1, 3, 6};                                        // -=--=+=-
static const char vlist0778[] = { 2, 1, 3, 6, 0, 1, 6};                                        // -=--=+==
static const char vlist0779[] = { 2, 1, 3, 6,12, 1, 6};                                        // -=--=+=+
static const char vlist0780[] = { 3, 8,13, 6, 9, 8, 6,13, 3, 6};                               // -=--=++-
static const char vlist0781[] = { 2, 0,13, 6,13, 3, 6};                                        // -=--=++=
static const char vlist0782[] = { 2,12,13, 6,13, 3, 6};                                        // -=--=+++
static const char vlist0783[] = { 2, 6,10,15,10, 6,11};                                        // -=--+---
static const char vlist0784[] = { 3,11, 6, 0, 6, 0,10,15, 6,10};                               // -=--+--=
static const char vlist0785[] = { 4,11, 6, 9,10,12, 8,10,15,12,15, 6,12};                      // -=--+--+
static const char vlist0786[] = { 2,15, 6, 1, 1, 6,11};                                        // -=--+-=-
static const char vlist0787[] = { 3, 1,15, 6, 0, 1, 6, 0,11, 6};                               // -=--+-==
static const char vlist0788[] = { 3, 1,15, 6,12, 1, 6,11, 9, 6};                               // -=--+-=+
static const char vlist0789[] = { 3,15, 6,13,13, 6, 8, 6,11, 8};                               // -=--+-+-
static const char vlist0790[] = { 3, 0,13, 6,13,15, 6,11, 0, 6};                               // -=--+-+=
static const char vlist0791[] = { 3,12,13, 6,13,15, 6,11, 9, 6};                               // -=--+-++
static const char vlist0792[] = { 2, 6, 2,10,15, 6,10};                                        // -=--+=--
static const char vlist0793[] = { 2, 0,10, 6,10,15, 6};                                        // -=--+=-=
static const char vlist0794[] = { 3, 8,10,12,12,10,15,12,15, 6};                               // -=--+=-+
static const char vlist0795[] = { 2, 2, 1, 6, 1,15, 6};                                        // -=--+==-
static const char vlist0796[] = { 2, 1,15, 6, 0, 1, 6};                                        // -=--+===
static const char vlist0797[] = { 2, 1,15, 6,12, 1, 6};                                        // -=--+==+
static const char vlist0798[] = { 3, 2, 8, 6, 8,13, 6,13,15, 6};                               // -=--+=+-
static const char vlist0799[] = { 2, 0,13, 6,13,15, 6};                                        // -=--+=+=
static const char vlist0800[] = { 2,12,13, 6,13,15, 6};                                        // -=--+=++
static const char vlist0801[] = { 2,10, 6, 9,15, 6,10};                                        // -=--++--
static const char vlist0802[] = { 2, 0,10, 6,10,15, 6};                                        // -=--++-=
static const char vlist0803[] = { 3, 8,10,12,12,10,15,12,15, 6};                               // -=--++-+
static const char vlist0804[] = { 2, 9, 1, 6, 1,15, 6};                                        // -=--++=-
static const char vlist0805[] = { 2, 1,15, 6, 0, 1, 6};                                        // -=--++==
static const char vlist0806[] = { 2, 1,15, 6,12, 1, 6};                                        // -=--++=+
static const char vlist0807[] = { 3, 8,13, 6, 9, 8, 6,13,15, 6};                               // -=--+++-
static const char vlist0808[] = { 2, 0,13, 6,13,15, 6};                                        // -=--+++=
static const char vlist0809[] = { 2,12,13, 6,13,15, 6};                                        // -=--++++
                                                                                               // -=-=----
static const char vlist0811[] = { 1, 6, 4, 0};                                                 // -=-=---=
static const char vlist0812[] = { 2, 6, 4, 8, 9, 6, 8};                                        // -=-=---+
                                                                                               // -=-=--=-
static const char vlist0814[] = { 2, 4, 1, 6, 1, 0, 6};                                        // -=-=--==
static const char vlist0815[] = { 2, 4, 1, 6, 1, 9, 6};                                        // -=-=--=+
static const char vlist0816[] = { 4, 6, 4,13, 6, 4, 8, 6,10, 8, 6,13,10};                      // -=-=--+-
static const char vlist0817[] = { 3,10, 0, 6,13,10, 6, 4,13, 6};                               // -=-=--+=
static const char vlist0818[] = { 3,10, 9, 6,13,10, 6, 4,13, 6};                               // -=-=--++
static const char vlist0819[] = { 1, 6, 4, 2};                                                 // -=-=-=--
static const char vlist0820[] = { 2, 2, 4, 0, 4, 2, 6};                                        // -=-=-=-=
static const char vlist0821[] = { 2, 2, 6, 8, 6, 4, 8};                                        // -=-=-=-+
static const char vlist0822[] = { 3, 2, 4, 1, 1, 2, 6, 4, 1, 6};                               // -=-=-==-
static const char vlist0823[] = { 2, 1, 2, 6, 1, 6, 4};                                        // -=-=-===
static const char vlist0824[] = { 2, 1, 2, 6, 1, 6, 4};                                        // -=-=-==+
static const char vlist0825[] = { 4, 2, 4, 8,10, 2, 6,13,10, 6, 4,13, 6};                      // -=-=-=+-
static const char vlist0826[] = { 3,10, 2, 6,13,10, 6,13, 6, 4};                               // -=-=-=+=
static const char vlist0827[] = { 3,10, 2, 6,13,10, 6,13, 6, 4};                               // -=-=-=++
static const char vlist0828[] = { 2, 6, 4,11, 4, 9,11};                                        // -=-=-+--
static const char vlist0829[] = { 2, 4, 0,11, 6, 4,11};                                        // -=-=-+-=
static const char vlist0830[] = { 2, 6, 4, 8,11, 6, 8};                                        // -=-=-+-+
static const char vlist0831[] = { 3, 9, 4, 1, 4, 1, 6, 1,11, 6};                               // -=-=-+=-
static const char vlist0832[] = { 2, 1,11, 6, 1, 6, 4};                                        // -=-=-+==
static const char vlist0833[] = { 2, 1,11, 6, 1, 6, 4};                                        // -=-=-+=+
static const char vlist0834[] = { 4, 4, 9, 8,13,10, 6,10,11, 6, 4,13, 6};                      // -=-=-++-
static const char vlist0835[] = { 3,10,11, 6,13,10, 6,13, 6, 4};                               // -=-=-++=
static const char vlist0836[] = { 3,10,11, 6,13,10, 6,13, 6, 4};                               // -=-=-+++
                                                                                               // -=-==---
static const char vlist0838[] = { 3, 3, 4, 0, 4, 3, 6, 0, 3, 6};                               // -=-==--=
static const char vlist0839[] = { 3, 3, 4, 8, 4, 3, 6, 9, 3, 6};                               // -=-==--+
                                                                                               // -=-==-=-
static const char vlist0841[] = { 3, 1, 3, 4, 0, 3, 6, 3, 6, 4};                               // -=-==-==
static const char vlist0842[] = { 3, 1, 3, 4, 3, 9, 6, 3, 6, 4};                               // -=-==-=+
static const char vlist0843[] = { 4,13, 3, 6, 6, 3, 8, 4, 6, 8, 4,13, 6};                      // -=-==-+-
static const char vlist0844[] = { 3, 3, 0, 6, 3, 6,13,13, 6, 4};                               // -=-==-+=
static const char vlist0845[] = { 3, 3, 9, 6, 3, 6,13,13, 6, 4};                               // -=-==-++
static const char vlist0846[] = { 2, 3, 4, 2, 4, 3, 6};                                        // -=-===--
static const char vlist0847[] = { 2, 0, 3, 4, 3, 6, 4};                                        // -=-===-=
static const char vlist0848[] = { 2, 8, 3, 4, 3, 6, 4};                                        // -=-===-+
static const char vlist0849[] = { 3, 1, 2, 4, 1, 3, 4, 3, 6, 4};                               // -=-====-
static const char vlist0850[] = { 2, 1, 3, 4, 3, 6, 4};                                        // -=-=====
static const char vlist0851[] = { 2, 1, 3, 4, 3, 6, 4};                                        // -=-====+
static const char vlist0852[] = { 3, 8, 2, 4, 3, 6,13,13, 6, 4};                               // -=-===+-
static const char vlist0853[] = { 2, 3, 6,13,13, 6, 4};                                        // -=-===+=
static const char vlist0854[] = { 2, 3, 6,13,13, 6, 4};                                        // -=-===++
static const char vlist0855[] = { 2, 3, 4, 9, 4, 3, 6};                                        // -=-==+--
static const char vlist0856[] = { 2, 0, 3, 4, 3, 6, 4};                                        // -=-==+-=
static const char vlist0857[] = { 2, 8, 3, 4, 3, 6, 4};                                        // -=-==+-+
static const char vlist0858[] = { 3, 1, 9, 4, 1, 3, 4, 3, 6, 4};                               // -=-==+=-
static const char vlist0859[] = { 2, 1, 3, 4, 3, 6, 4};                                        // -=-==+==
static const char vlist0860[] = { 2, 1, 3, 4, 3, 6, 4};                                        // -=-==+=+
static const char vlist0861[] = { 3, 9, 8, 4, 3, 6,13,13, 6, 4};                               // -=-==++-
static const char vlist0862[] = { 2, 3, 6,13,13, 6, 4};                                        // -=-==++=
static const char vlist0863[] = { 2, 3, 6,13,13, 6, 4};                                        // -=-==+++
static const char vlist0864[] = { 4, 6, 4,15, 6, 4,11,10, 4,11,15, 4,10};                      // -=-=+---
static const char vlist0865[] = { 4, 0,11, 6,10, 4, 0,15, 4,10, 4,15, 6};                      // -=-=+--=
static const char vlist0866[] = { 4, 9,11, 6,10, 4, 8,15, 4,10, 4,15, 6};                      // -=-=+--+
static const char vlist0867[] = { 4, 4, 6,11, 4,15, 6,15, 4, 1, 1, 4,11};                      // -=-=+-=-
static const char vlist0868[] = { 3, 1,15, 4,11, 0, 6,15, 6, 4};                               // -=-=+-==
static const char vlist0869[] = { 3, 1,15, 4,11, 9, 6,15, 6, 4};                               // -=-=+-=+
static const char vlist0870[] = { 4,13,15, 6, 4,13, 6, 4, 6, 8, 6,11, 8};                      // -=-=+-+-
static const char vlist0871[] = { 3,11, 0, 6,15, 6,13,13, 6, 4};                               // -=-=+-+=
static const char vlist0872[] = { 3,11, 9, 6,15, 6,13,13, 6, 4};                               // -=-=+-++
static const char vlist0873[] = { 3,10, 4, 2,15, 4,10, 4,15, 6};                               // -=-=+=--
static const char vlist0874[] = { 3, 0,10, 4,10,15, 4,15, 6, 4};                               // -=-=+=-=
static const char vlist0875[] = { 3, 8,10, 4,10,15, 4,15, 6, 4};                               // -=-=+=-+
static const char vlist0876[] = { 3, 1, 2, 4, 1,15, 4,15, 6, 4};                               // -=-=+==-
static const char vlist0877[] = { 2, 1,15, 4,15, 6, 4};                                        // -=-=+===
static const char vlist0878[] = { 2, 1,15, 4,15, 6, 4};                                        // -=-=+==+
static const char vlist0879[] = { 3, 2, 8, 4,15, 6,13,13, 6, 4};                               // -=-=+=+-
static const char vlist0880[] = { 2,15, 6,13,13, 6, 4};                                        // -=-=+=+=
static const char vlist0881[] = { 2,15, 6,13,13, 6, 4};                                        // -=-=+=++
static const char vlist0882[] = { 3,10, 4, 9,15, 4,10, 4,15, 6};                               // -=-=++--
static const char vlist0883[] = { 3, 0,10, 4,10,15, 4,15, 6, 4};                               // -=-=++-=
static const char vlist0884[] = { 3, 8,10, 4,10,15, 4,15, 6, 4};                               // -=-=++-+
static const char vlist0885[] = { 3, 9, 1, 4, 1,15, 4,15, 6, 4};                               // -=-=++=-
static const char vlist0886[] = { 2, 1,15, 4,15, 6, 4};                                        // -=-=++==
static const char vlist0887[] = { 2, 1,15, 4,15, 6, 4};                                        // -=-=++=+
static const char vlist0888[] = { 3, 9, 8, 4,15, 6,13,13, 6, 4};                               // -=-=+++-
static const char vlist0889[] = { 2,15, 6,13,13, 6, 4};                                        // -=-=+++=
static const char vlist0890[] = { 2,15, 6,13,13, 6, 4};                                        // -=-=++++
static const char vlist0891[] = { 1,12, 6,16};                                                 // -=-+----
static const char vlist0892[] = { 1, 0, 6,16};                                                 // -=-+---=
static const char vlist0893[] = { 2, 9, 6, 8, 6,16, 8};                                        // -=-+---+
static const char vlist0894[] = { 2, 6,16, 1, 6, 1,12};                                        // -=-+--=-
static const char vlist0895[] = { 2,16, 1, 6, 1, 0, 6};                                        // -=-+--==
static const char vlist0896[] = { 2,16, 1, 6, 1, 9, 6};                                        // -=-+--=+
static const char vlist0897[] = { 4, 6,16,13, 6,13,10, 6,10, 8,12, 6, 8};                      // -=-+--+-
static const char vlist0898[] = { 3,10, 0, 6,13,10, 6,16,13, 6};                               // -=-+--+=
static const char vlist0899[] = { 3,10, 9, 6,13,10, 6,16,13, 6};                               // -=-+--++
static const char vlist0900[] = { 2,12, 2,16, 2, 6,16};                                        // -=-+-=--
static const char vlist0901[] = { 2, 2, 6,16, 0, 2,16};                                        // -=-+-=-=
static const char vlist0902[] = { 2, 2, 6, 8, 6,16, 8};                                        // -=-+-=-+
static const char vlist0903[] = { 3, 1, 2, 6, 2,12, 1,16, 1, 6};                               // -=-+-==-
static const char vlist0904[] = { 2, 1, 2, 6, 1, 6,16};                                        // -=-+-===
static const char vlist0905[] = { 2, 1, 2, 6, 1, 6,16};                                        // -=-+-==+
static const char vlist0906[] = { 4,10, 2, 6,12, 2, 8,13,10, 6,16,13, 6};                      // -=-+-=+-
static const char vlist0907[] = { 3,10, 2, 6,13,10, 6,13, 6,16};                               // -=-+-=+=
static const char vlist0908[] = { 3,10, 2, 6,13,10, 6,13, 6,16};                               // -=-+-=++
static const char vlist0909[] = { 3,12, 9,11,16,12,11, 6,16,11};                               // -=-+-+--
static const char vlist0910[] = { 2,16, 0,11, 6,16,11};                                        // -=-+-+-=
static const char vlist0911[] = { 2,11, 6,16,11,16, 8};                                        // -=-+-+-+
static const char vlist0912[] = { 3,12, 9, 1,16, 1, 6, 1,11, 6};                               // -=-+-+=-
static const char vlist0913[] = { 2, 1,11, 6, 1, 6,16};                                        // -=-+-+==
static const char vlist0914[] = { 2, 1,11, 6, 1, 6,16};                                        // -=-+-+=+
static const char vlist0915[] = { 4,12, 9, 8,13,10, 6,10,11, 6,16,13, 6};                      // -=-+-++-
static const char vlist0916[] = { 3,10,11, 6,13,10, 6,13, 6,16};                               // -=-+-++=
static const char vlist0917[] = { 3,10,11, 6,13,10, 6,13, 6,16};                               // -=-+-+++
static const char vlist0918[] = { 3, 3,16,12, 6,16, 3, 6, 3,12};                               // -=-+=---
static const char vlist0919[] = { 3,16, 3, 6, 3,16, 0, 0, 3, 6};                               // -=-+=--=
static const char vlist0920[] = { 3,16, 3, 6, 3,16, 8, 9, 3, 6};                               // -=-+=--+
static const char vlist0921[] = { 4,16, 3, 6, 3,16, 1, 3, 1,12, 6, 3,12};                      // -=-+=-=-
static const char vlist0922[] = { 3, 1, 3,16, 3, 0, 6, 3, 6,16};                               // -=-+=-==
static const char vlist0923[] = { 3, 1, 3,16, 3, 9, 6, 3, 6,16};                               // -=-+=-=+
static const char vlist0924[] = { 4,13, 3, 6, 6, 3, 8,12, 6, 8,16,13, 6};                      // -=-+=-+-
static const char vlist0925[] = { 3, 3, 0, 6, 3, 6,13,13, 6,16};                               // -=-+=-+=
static const char vlist0926[] = { 3, 3, 9, 6, 3, 6,13,13, 6,16};                               // -=-+=-++
static const char vlist0927[] = { 3,16, 3, 6, 3,16,12, 3,12, 2};                               // -=-+==--
static const char vlist0928[] = { 2, 0, 3,16, 3, 6,16};                                        // -=-+==-=
static const char vlist0929[] = { 2, 8, 3,16, 3, 6,16};                                        // -=-+==-+
static const char vlist0930[] = { 3, 1, 2,12, 1, 3,16, 3, 6,16};                               // -=-+===-
static const char vlist0931[] = { 2, 1, 3,16, 3, 6,16};                                        // -=-+====
static const char vlist0932[] = { 2, 1, 3,16, 3, 6,16};                                        // -=-+===+
static const char vlist0933[] = { 3, 2, 8,12, 3, 6,13,13, 6,16};                               // -=-+==+-
static const char vlist0934[] = { 2, 3, 6,13,13, 6,16};                                        // -=-+==+=
static const char vlist0935[] = { 2, 3, 6,13,13, 6,16};                                        // -=-+==++
static const char vlist0936[] = { 3,16, 3, 6, 3,16,12, 3,12, 9};                               // -=-+=+--
static const char vlist0937[] = { 2, 0, 3,16, 3, 6,16};                                        // -=-+=+-=
static const char vlist0938[] = { 2, 8, 3,16, 3, 6,16};                                        // -=-+=+-+
static const char vlist0939[] = { 3, 9, 1,12, 1, 3,16, 3, 6,16};                               // -=-+=+=-
static const char vlist0940[] = { 2, 1, 3,16, 3, 6,16};                                        // -=-+=+==
static const char vlist0941[] = { 2, 1, 3,16, 3, 6,16};                                        // -=-+=+=+
static const char vlist0942[] = { 3, 9, 8,12, 3, 6,13,13, 6,16};                               // -=-+=++-
static const char vlist0943[] = { 2, 3, 6,13,13, 6,16};                                        // -=-+=++=
static const char vlist0944[] = { 2, 3, 6,13,13, 6,16};                                        // -=-+=+++
static const char vlist0945[] = { 5,10,16,12,15,16,10, 6,16,15,10,12,11,12, 6,11};             // -=-++---
static const char vlist0946[] = { 4, 0,11, 6,16,15, 6,15,16,10,10,16, 0};                      // -=-++--=
static const char vlist0947[] = { 4,11, 9, 6,16,15, 6,15,16,10,10,16, 8};                      // -=-++--+
static const char vlist0948[] = { 4,12, 6,11,15,16, 1,16,15, 6, 1,12,11};                      // -=-++-=-
static const char vlist0949[] = { 3, 1,15,16,11, 0, 6,15, 6,16};                               // -=-++-==
static const char vlist0950[] = { 3, 1,15,16,11, 9, 6,15, 6,16};                               // -=-++-=+
static const char vlist0951[] = { 4,16,15, 6,13,15,16, 6,11,12,12,11, 8};                      // -=-++-+-
static const char vlist0952[] = { 3,11, 0, 6,15, 6,16,15,16,13};                               // -=-++-+=
static const char vlist0953[] = { 3,11, 9, 6,15, 6,16,15,16,13};                               // -=-++-++
static const char vlist0954[] = { 4,16,15, 6,15,16,10,10,16,12,10,12, 2};                      // -=-++=--
static const char vlist0955[] = { 3, 0,10,16,10,15,16,15, 6,16};                               // -=-++=-=
static const char vlist0956[] = { 3, 8,10,16,15, 6,16,10,15,16};                               // -=-++=-+
static const char vlist0957[] = { 3, 2, 1,12, 1,15,16,15, 6,16};                               // -=-++==-
static const char vlist0958[] = { 2, 1,15,16,15, 6,16};                                        // -=-++===
static const char vlist0959[] = { 2, 1,15,16,15, 6,16};                                        // -=-++==+
static const char vlist0960[] = { 3, 2, 8,12,15, 6,16,15,16,13};                               // -=-++=+-
static const char vlist0961[] = { 2,15,16,13,15, 6,16};                                        // -=-++=+=
static const char vlist0962[] = { 2,15,16,13,15, 6,16};                                        // -=-++=++
static const char vlist0963[] = { 4,16,15, 6,15,16,10,10,16,12,10,12, 9};                      // -=-+++--
static const char vlist0964[] = { 3, 0,10,16,10,15,16,15, 6,16};                               // -=-+++-=
static const char vlist0965[] = { 3, 8,10,16,15, 6,16,10,15,16};                               // -=-+++-+
static const char vlist0966[] = { 3, 9, 1,12, 1,15,16,15, 6,16};                               // -=-+++=-
static const char vlist0967[] = { 2, 1,15,16,15, 6,16};                                        // -=-+++==
static const char vlist0968[] = { 2, 1,15,16,15, 6,16};                                        // -=-+++=+
static const char vlist0969[] = { 3, 9, 8,12,15, 6,16,15,16,13};                               // -=-++++-
static const char vlist0970[] = { 2,15,16,13,15, 6,16};                                        // -=-++++=
static const char vlist0971[] = { 2,15,16,13,15, 6,16};                                        // -=-+++++
                                                                                               // -==-----
                                                                                               // -==----=
static const char vlist0974[] = { 3, 6, 5,12, 6, 5, 8, 9, 6, 8};                               // -==----+
                                                                                               // -==---=-
static const char vlist0976[] = { 3, 5, 1, 6, 1, 0, 6, 5, 0, 6};                               // -==---==
static const char vlist0977[] = { 3, 5, 1, 6, 1, 9, 6, 5,12, 6};                               // -==---=+
static const char vlist0978[] = { 3, 6, 5,10, 6, 5, 8, 6,10, 8};                               // -==---+-
static const char vlist0979[] = { 3,10, 0, 6, 5,10, 6, 5, 0, 6};                               // -==---+=
static const char vlist0980[] = { 3,10, 9, 6, 5,10, 6, 5,12, 6};                               // -==---++
                                                                                               // -==--=--
static const char vlist0982[] = { 3, 2, 5, 0, 5, 2, 6, 5, 0, 6};                               // -==--=-=
static const char vlist0983[] = { 3, 2, 5, 8, 5, 2, 6, 5,12, 6};                               // -==--=-+
                                                                                               // -==--==-
static const char vlist0985[] = { 3, 1, 2, 5, 0, 6, 5, 2, 6, 5};                               // -==--===
static const char vlist0986[] = { 3, 1, 2, 5, 2, 6, 5, 6,12, 5};                               // -==--==+
static const char vlist0987[] = { 4,10, 2, 6, 6, 2, 8, 5, 6, 8, 5,10, 6};                      // -==--=+-
static const char vlist0988[] = { 3, 6, 0, 5, 2, 6,10,10, 6, 5};                               // -==--=+=
static const char vlist0989[] = { 3, 2, 6,10,10, 6, 5, 6,12, 5};                               // -==--=++
static const char vlist0990[] = { 3, 6, 5,11, 6, 5, 9,11, 5, 9};                               // -==--+--
static const char vlist0991[] = { 3, 5, 0, 6,11, 5, 0, 5,11, 6};                               // -==--+-=
static const char vlist0992[] = { 3,11, 5, 8, 5,11, 6, 5,12, 6};                               // -==--+-+
static const char vlist0993[] = { 4, 5, 6, 9, 5,11, 6,11, 5, 1, 1, 5, 9};                      // -==--+=-
static const char vlist0994[] = { 3, 1,11, 5, 6, 0, 5,11, 6, 5};                               // -==--+==
static const char vlist0995[] = { 3, 1,11, 5,11, 6, 5, 6,12, 5};                               // -==--+=+
static const char vlist0996[] = { 4,10,11, 6, 5,10, 6, 5, 6, 8, 6, 9, 8};                      // -==--++-
static const char vlist0997[] = { 3, 6, 0, 5,11, 6,10,10, 6, 5};                               // -==--++=
static const char vlist0998[] = { 3,11, 6,10,10, 6, 5, 6,12, 5};                               // -==--+++
                                                                                               // -==-=---
static const char vlist1000[] = { 4, 5, 3, 0, 3, 5, 6, 0, 3, 6, 5, 0, 6};                      // -==-=--=
static const char vlist1001[] = { 4, 5, 3, 8, 3, 5, 6, 9, 3, 6, 5,12, 6};                      // -==-=--+
static const char vlist1002[] = { 3, 3, 5, 6, 3, 1, 6, 1, 5, 6};                               // -==-=-=-
static const char vlist1003[] = { 3, 0, 6, 5, 0, 3, 6, 6, 3, 5};                               // -==-=-==
static const char vlist1004[] = { 3, 3, 9, 6, 3, 6, 5, 6,12, 5};                               // -==-=-=+
static const char vlist1005[] = { 3, 3, 5, 6, 3, 8, 6, 8, 5, 6};                               // -==-=-+-
static const char vlist1006[] = { 3, 6, 0, 5, 3, 0, 6, 3, 6, 5};                               // -==-=-+=
static const char vlist1007[] = { 3, 3, 9, 6, 3, 6, 5, 6,12, 5};                               // -==-=-++
static const char vlist1008[] = { 3, 3, 5, 2, 3, 5, 6, 2, 5, 6};                               // -==-==--
static const char vlist1009[] = { 3, 0, 6, 5, 6, 3, 5, 3, 0, 5};                               // -==-==-=
static const char vlist1010[] = { 3,12, 6, 5, 3, 6, 5, 3, 8, 5};                               // -==-==-+
static const char vlist1011[] = { 3, 6, 2, 5, 2, 1, 5, 6, 3, 5};                               // -==-===-
static const char vlist1012[] = { 2, 6, 0, 5, 6, 3, 5};                                        // -==-====
static const char vlist1013[] = { 2, 3, 6, 5, 6,12, 5};                                        // -==-===+
static const char vlist1014[] = { 3, 6, 2, 8, 6, 8, 5, 3, 6, 5};                               // -==-==+-
static const char vlist1015[] = { 2, 6, 0, 5, 3, 6, 5};                                        // -==-==+=
static const char vlist1016[] = { 2, 3, 6, 5, 6,12, 5};                                        // -==-==++
static const char vlist1017[] = { 3, 3, 5, 9, 3, 5, 6, 9, 5, 6};                               // -==-=+--
static const char vlist1018[] = { 3, 0, 6, 5, 3, 6, 5, 3, 0, 5};                               // -==-=+-=
static const char vlist1019[] = { 3, 6,12, 5, 3, 6, 5, 8, 3, 5};                               // -==-=+-+
static const char vlist1020[] = { 3, 9, 1, 5, 6, 9, 5, 3, 6, 5};                               // -==-=+=-
static const char vlist1021[] = { 2, 6, 0, 5, 3, 6, 5};                                        // -==-=+==
static const char vlist1022[] = { 2, 3, 6, 5, 6,12, 5};                                        // -==-=+=+
static const char vlist1023[] = { 3, 6, 8, 5, 6, 9, 8, 3, 6, 5};                               // -==-=++-
static const char vlist1024[] = { 2, 6, 0, 5, 3, 6, 5};                                        // -==-=++=
static const char vlist1025[] = { 2, 3, 6, 5, 6,12, 5};                                        // -==-=+++
static const char vlist1026[] = { 3, 5, 6,15, 5, 6,10, 6,11,10};                               // -==-+---
static const char vlist1027[] = { 4, 5,10, 0, 5, 0, 6, 0,11, 6,15, 5, 6};                      // -==-+--=
static const char vlist1028[] = { 4, 5,10, 8, 9,11, 6, 5,12, 6, 5,15, 6};                      // -==-+--+
static const char vlist1029[] = { 3,11, 1, 6, 1, 5, 6,15, 5, 6};                               // -==-+-=-
static const char vlist1030[] = { 3, 6, 0, 5,11, 0, 6,15, 6, 5};                               // -==-+-==
static const char vlist1031[] = { 3,11, 9, 6,15, 6, 5, 6,12, 5};                               // -==-+-=+
static const char vlist1032[] = { 3, 8, 5, 6,11, 8, 6, 5,15, 6};                               // -==-+-+-
static const char vlist1033[] = { 3, 6, 0, 5,11, 0, 6,15, 6, 5};                               // -==-+-+=
static const char vlist1034[] = { 3,11, 9, 6,15, 6, 5, 6,12, 5};                               // -==-+-++
static const char vlist1035[] = { 3,10, 5, 2, 2, 5, 6,15, 5, 6};                               // -==-+=--
static const char vlist1036[] = { 3, 0, 6, 5,15, 6, 5,10, 0, 5};                               // -==-+=-=
static const char vlist1037[] = { 3, 6,12, 5,15, 6, 5, 8,10, 5};                               // -==-+=-+
static const char vlist1038[] = { 3, 6, 2, 5, 2, 1, 5,15, 6, 5};                               // -==-+==-
static const char vlist1039[] = { 2, 6, 0, 5,15, 6, 5};                                        // -==-+===
static const char vlist1040[] = { 2,15, 6, 5, 6,12, 5};                                        // -==-+==+
static const char vlist1041[] = { 3, 6, 2, 8, 6, 8, 5,15, 6, 5};                               // -==-+=+-
static const char vlist1042[] = { 2, 6, 0, 5,15, 6, 5};                                        // -==-+=+=
static const char vlist1043[] = { 2,15, 6, 5, 6,12, 5};                                        // -==-+=++
static const char vlist1044[] = { 3,10, 5, 9, 9, 5, 6, 5,15, 6};                               // -==-++--
static const char vlist1045[] = { 3, 6, 0, 5,15, 6, 5, 0,10, 5};                               // -==-++-=
static const char vlist1046[] = { 3, 6,12, 5,15, 6, 5, 8,10, 5};                               // -==-++-+
static const char vlist1047[] = { 3, 9, 1, 5, 6, 9, 5,15, 6, 5};                               // -==-++=-
static const char vlist1048[] = { 2, 6, 0, 5,15, 6, 5};                                        // -==-++==
static const char vlist1049[] = { 2,15, 6, 5, 6,12, 5};                                        // -==-++=+
static const char vlist1050[] = { 3, 6, 8, 5, 6, 9, 8,15, 6, 5};                               // -==-+++-
static const char vlist1051[] = { 2, 6, 0, 5,15, 6, 5};                                        // -==-+++=
static const char vlist1052[] = { 2,15, 6, 5, 6,12, 5};                                        // -==-++++
static const char vlist1053[] = { 1, 6, 5, 4};                                                 // -===----
static const char vlist1054[] = { 1, 5, 0, 6};                                                 // -===---=
static const char vlist1055[] = { 2, 5, 8, 6, 8, 9, 6};                                        // -===---+
static const char vlist1056[] = { 2, 1, 4, 6, 5, 1, 6};                                        // -===--=-
static const char vlist1057[] = { 2, 5, 1, 6, 1, 0, 6};                                        // -===--==
static const char vlist1058[] = { 2, 5, 1, 6, 1, 9, 6};                                        // -===--=+
static const char vlist1059[] = { 3, 8, 4, 6,10, 8, 6, 5,10, 6};                               // -===--+-
static const char vlist1060[] = { 2,10, 0, 6, 5,10, 6};                                        // -===--+=
static const char vlist1061[] = { 2,10, 9, 6, 5,10, 6};                                        // -===--++
static const char vlist1062[] = { 2, 5, 4, 2, 5, 2, 6};                                        // -===-=--
static const char vlist1063[] = { 2, 2, 6, 5, 0, 2, 5};                                        // -===-=-=
static const char vlist1064[] = { 2, 2, 6, 5, 8, 2, 5};                                        // -===-=-+
static const char vlist1065[] = { 3, 2, 4, 1, 2, 6, 5, 1, 2, 5};                               // -===-==-
static const char vlist1066[] = { 2, 1, 2, 5, 5, 2, 6};                                        // -===-===
static const char vlist1067[] = { 2, 1, 2, 5, 5, 2, 6};                                        // -===-==+
static const char vlist1068[] = { 3, 4, 2, 8, 2, 6,10,10, 6, 5};                               // -===-=+-
static const char vlist1069[] = { 2,10, 2, 6, 5,10, 6};                                        // -===-=+=
static const char vlist1070[] = { 2,10, 2, 6, 5,10, 6};                                        // -===-=++
static const char vlist1071[] = { 3, 5, 4, 9, 5, 9,11, 5,11, 6};                               // -===-+--
static const char vlist1072[] = { 2, 0,11, 5,11, 6, 5};                                        // -===-+-=
static const char vlist1073[] = { 2, 8,11, 5,11, 6, 5};                                        // -===-+-+
static const char vlist1074[] = { 3, 4, 9, 1, 1,11, 5,11, 6, 5};                               // -===-+=-
static const char vlist1075[] = { 2, 1,11, 5, 5,11, 6};                                        // -===-+==
static const char vlist1076[] = { 2, 1,11, 5, 5,11, 6};                                        // -===-+=+
static const char vlist1077[] = { 3, 4, 9, 8,10, 6, 5,11, 6,10};                               // -===-++-
static const char vlist1078[] = { 2,10,11, 6, 5,10, 6};                                        // -===-++=
static const char vlist1079[] = { 2,10,11, 6, 5,10, 6};                                        // -===-+++
static const char vlist1080[] = { 3, 5, 4, 3, 3, 4, 6, 3, 5, 6};                               // -====---
static const char vlist1081[] = { 3, 6, 3, 5, 3, 6, 0, 3, 0, 5};                               // -====--=
static const char vlist1082[] = { 3, 6, 3, 5, 6, 3, 9, 8, 3, 5};                               // -====--+
static const char vlist1083[] = { 3, 4, 3, 1, 3, 4, 6, 6, 3, 5};                               // -====-=-
static const char vlist1084[] = { 2, 3, 0, 6, 3, 5, 6};                                        // -====-==
static const char vlist1085[] = { 2, 3, 9, 6, 5, 3, 6};                                        // -====-=+
static const char vlist1086[] = { 3, 3, 8, 6, 6, 3, 5, 8, 4, 6};                               // -====-+-
static const char vlist1087[] = { 2, 3, 0, 6, 5, 3, 6};                                        // -====-+=
static const char vlist1088[] = { 2, 3, 9, 6, 5, 3, 6};                                        // -====-++
static const char vlist1089[] = { 3, 3, 4, 2, 4, 3, 5, 3, 5, 6};                               // -=====--
static const char vlist1090[] = { 2, 3, 0, 5, 3, 5, 6};                                        // -=====-=
static const char vlist1091[] = { 2, 8, 3, 5, 5, 3, 6};                                        // -=====-+
static const char vlist1092[] = { 2, 1, 2, 4, 3, 5, 6};                                        // -======-
static const char vlist1093[] = { 1, 5, 3, 6};                                                 // -=======
static const char vlist1094[] = { 1, 5, 3, 6};                                                 // -======+
static const char vlist1095[] = { 2, 2, 8, 4, 5, 3, 6};                                        // -=====+-
static const char vlist1096[] = { 1, 5, 3, 6};                                                 // -=====+=
static const char vlist1097[] = { 1, 5, 3, 6};                                                 // -=====++
static const char vlist1098[] = { 3, 9, 3, 5, 5, 3, 6, 4, 9, 5};                               // -====+--
static const char vlist1099[] = { 2, 0, 3, 5, 5, 3, 6};                                        // -====+-=
static const char vlist1100[] = { 2, 8, 3, 5, 5, 3, 6};                                        // -====+-+
static const char vlist1101[] = { 2, 9, 1, 4, 5, 3, 6};                                        // -====+=-
static const char vlist1102[] = { 1, 5, 3, 6};                                                 // -====+==
static const char vlist1103[] = { 1, 5, 3, 6};                                                 // -====+=+
static const char vlist1104[] = { 2, 9, 8, 4, 5, 3, 6};                                        // -====++-
static const char vlist1105[] = { 1, 5, 3, 6};                                                 // -====++=
static const char vlist1106[] = { 1, 5, 3, 6};                                                 // -====+++
static const char vlist1107[] = { 4, 5, 4,10,11, 4, 6,10, 4,11,15, 5, 6};                      // -===+---
static const char vlist1108[] = { 3, 6,15, 5, 6,11, 0, 0,10, 5};                               // -===+--=
static const char vlist1109[] = { 3,15, 6, 5, 6,11, 9, 8,10, 5};                               // -===+--+
static const char vlist1110[] = { 3, 6,15, 5,11, 4, 6, 4,11, 1};                               // -===+-=-
static const char vlist1111[] = { 2,11, 0, 6, 5,15, 6};                                        // -===+-==
static const char vlist1112[] = { 2,11, 9, 6, 5,15, 6};                                        // -===+-=+
static const char vlist1113[] = { 3,11, 8, 6, 8, 4, 6,15, 6, 5};                               // -===+-+-
static const char vlist1114[] = { 2,11, 0, 6, 5,15, 6};                                        // -===+-+=
static const char vlist1115[] = { 2,11, 9, 6, 5,15, 6};                                        // -===+-++
static const char vlist1116[] = { 3, 5,15, 6, 4,10, 5,10, 4, 2};                               // -===+=--
static const char vlist1117[] = { 2, 0,10, 5, 5,15, 6};                                        // -===+=-=
static const char vlist1118[] = { 2, 8,10, 5, 5,15, 6};                                        // -===+=-+
static const char vlist1119[] = { 2, 2, 1, 4, 5,15, 6};                                        // -===+==-
static const char vlist1120[] = { 1, 5,15, 6};                                                 // -===+===
static const char vlist1121[] = { 1, 5,15, 6};                                                 // -===+==+
static const char vlist1122[] = { 2, 2, 8, 4, 5,15, 6};                                        // -===+=+-
static const char vlist1123[] = { 1, 5,15, 6};                                                 // -===+=+=
static const char vlist1124[] = { 1, 5,15, 6};                                                 // -===+=++
static const char vlist1125[] = { 3, 9,10, 5, 4, 9, 5, 5,15, 6};                               // -===++--
static const char vlist1126[] = { 2, 0,10, 5, 5,15, 6};                                        // -===++-=
static const char vlist1127[] = { 2, 8,10, 5, 5,15, 6};                                        // -===++-+
static const char vlist1128[] = { 2, 9, 1, 4, 5,15, 6};                                        // -===++=-
static const char vlist1129[] = { 1, 5,15, 6};                                                 // -===++==
static const char vlist1130[] = { 1, 5,15, 6};                                                 // -===++=+
static const char vlist1131[] = { 2, 9, 8, 4, 5,15, 6};                                        // -===+++-
static const char vlist1132[] = { 1, 5,15, 6};                                                 // -===+++=
static const char vlist1133[] = { 1, 5,15, 6};                                                 // -===++++
static const char vlist1134[] = { 1, 6, 5,12};                                                 // -==+----
static const char vlist1135[] = { 1, 5, 0, 6};                                                 // -==+---=
static const char vlist1136[] = { 2, 5, 8, 6, 8, 9, 6};                                        // -==+---+
static const char vlist1137[] = { 2, 1,12, 6, 5, 1, 6};                                        // -==+--=-
static const char vlist1138[] = { 2, 5, 1, 6, 1, 0, 6};                                        // -==+--==
static const char vlist1139[] = { 2, 5, 1, 6, 1, 9, 6};                                        // -==+--=+
static const char vlist1140[] = { 3,10, 8, 6, 8,12, 6, 5,10, 6};                               // -==+--+-
static const char vlist1141[] = { 2,10, 0, 6, 5,10, 6};                                        // -==+--+=
static const char vlist1142[] = { 2,10, 9, 6, 5,10, 6};                                        // -==+--++
static const char vlist1143[] = { 2, 5,12, 2, 5, 2, 6};                                        // -==+-=--
static const char vlist1144[] = { 2, 2, 6, 5, 0, 2, 5};                                        // -==+-=-=
static const char vlist1145[] = { 2, 2, 6, 5, 8, 2, 5};                                        // -==+-=-+
static const char vlist1146[] = { 3, 2, 6, 5, 1, 2, 5,12, 2, 1};                               // -==+-==-
static const char vlist1147[] = { 2, 1, 2, 5, 5, 2, 6};                                        // -==+-===
static const char vlist1148[] = { 2, 1, 2, 5, 5, 2, 6};                                        // -==+-==+
static const char vlist1149[] = { 3, 2, 6,10,12, 2, 8,10, 6, 5};                               // -==+-=+-
static const char vlist1150[] = { 2,10, 2, 6, 5,10, 6};                                        // -==+-=+=
static const char vlist1151[] = { 2,10, 2, 6, 5,10, 6};                                        // -==+-=++
static const char vlist1152[] = { 3, 5,12, 9, 5, 9,11, 5,11, 6};                               // -==+-+--
static const char vlist1153[] = { 2, 0,11, 5,11, 6, 5};                                        // -==+-+-=
static const char vlist1154[] = { 2, 8,11, 5,11, 6, 5};                                        // -==+-+-+
static const char vlist1155[] = { 3,12, 9, 1, 1,11, 5,11, 6, 5};                               // -==+-+=-
static const char vlist1156[] = { 2, 1,11, 5, 5,11, 6};                                        // -==+-+==
static const char vlist1157[] = { 2, 1,11, 5, 5,11, 6};                                        // -==+-+=+
static const char vlist1158[] = { 3,12, 9, 8,10, 6, 5,11, 6,10};                               // -==+-++-
static const char vlist1159[] = { 2,10,11, 6, 5,10, 6};                                        // -==+-++=
static const char vlist1160[] = { 2,10,11, 6, 5,10, 6};                                        // -==+-+++
static const char vlist1161[] = { 3, 3,12, 6, 5,12, 3, 3, 5, 6};                               // -==+=---
static const char vlist1162[] = { 3, 6, 3, 5, 6, 3, 0, 0, 3, 5};                               // -==+=--=
static const char vlist1163[] = { 3, 3, 6, 5, 6, 3, 9, 8, 3, 5};                               // -==+=--+
static const char vlist1164[] = { 3, 3,12, 6,12, 3, 1, 6, 3, 5};                               // -==+=-=-
static const char vlist1165[] = { 2, 3, 0, 6, 5, 3, 6};                                        // -==+=-==
static const char vlist1166[] = { 2, 3, 9, 6, 5, 3, 6};                                        // -==+=-=+
static const char vlist1167[] = { 3, 3, 8, 6, 3, 6, 5, 8,12, 6};                               // -==+=-+-
static const char vlist1168[] = { 2, 3, 0, 6, 5, 3, 6};                                        // -==+=-+=
static const char vlist1169[] = { 2, 3, 9, 6, 5, 3, 6};                                        // -==+=-++
static const char vlist1170[] = { 3,12, 3, 5, 3,12, 2, 5, 3, 6};                               // -==+==--
static const char vlist1171[] = { 2, 0, 3, 5, 5, 3, 6};                                        // -==+==-=
static const char vlist1172[] = { 2, 8, 3, 5, 5, 3, 6};                                        // -==+==-+
static const char vlist1173[] = { 2, 2, 1,12, 5, 3, 6};                                        // -==+===-
static const char vlist1174[] = { 1, 5, 3, 6};                                                 // -==+====
static const char vlist1175[] = { 1, 5, 3, 6};                                                 // -==+===+
static const char vlist1176[] = { 2, 2, 8,12, 5, 3, 6};                                        // -==+==+-
static const char vlist1177[] = { 1, 5, 3, 6};                                                 // -==+==+=
static const char vlist1178[] = { 1, 5, 3, 6};                                                 // -==+==++
static const char vlist1179[] = { 3, 9, 3, 5, 5, 3, 6,12, 9, 5};                               // -==+=+--
static const char vlist1180[] = { 2, 0, 3, 5, 5, 3, 6};                                        // -==+=+-=
static const char vlist1181[] = { 2, 8, 3, 5, 5, 3, 6};                                        // -==+=+-+
static const char vlist1182[] = { 2, 9, 1,12, 5, 3, 6};                                        // -==+=+=-
static const char vlist1183[] = { 1, 5, 3, 6};                                                 // -==+=+==
static const char vlist1184[] = { 1, 5, 3, 6};                                                 // -==+=+=+
static const char vlist1185[] = { 2, 9, 8,12, 5, 3, 6};                                        // -==+=++-
static const char vlist1186[] = { 1, 5, 3, 6};                                                 // -==+=++=
static const char vlist1187[] = { 1, 5, 3, 6};                                                 // -==+=+++
static const char vlist1188[] = { 4,11,12, 6,10,12,11, 5,12,10, 5,15, 6};                      // -==++---
static const char vlist1189[] = { 3,15, 6, 5, 6,11, 0, 0,10, 5};                               // -==++--=
static const char vlist1190[] = { 3,15, 6, 5, 6,11, 9, 8,10, 5};                               // -==++--+
static const char vlist1191[] = { 3,15, 6, 5,12,11, 1,11,12, 6};                               // -==++-=-
static const char vlist1192[] = { 2,11, 0, 6, 5,15, 6};                                        // -==++-==
static const char vlist1193[] = { 2,11, 9, 6, 5,15, 6};                                        // -==++-=+
static const char vlist1194[] = { 3,11,12, 6,11, 8,12,15, 6, 5};                               // -==++-+-
static const char vlist1195[] = { 2,11, 0, 6, 5,15, 6};                                        // -==++-+=
static const char vlist1196[] = { 2,11, 9, 6, 5,15, 6};                                        // -==++-++
static const char vlist1197[] = { 3, 5,15, 6,10,12, 2,12,10, 5};                               // -==++=--
static const char vlist1198[] = { 2, 0,10, 5, 5,15, 6};                                        // -==++=-=
static const char vlist1199[] = { 2, 8,10, 5, 5,15, 6};                                        // -==++=-+
static const char vlist1200[] = { 2, 2, 1,12, 5,15, 6};                                        // -==++==-
static const char vlist1201[] = { 1, 5,15, 6};                                                 // -==++===
static const char vlist1202[] = { 1, 5,15, 6};                                                 // -==++==+
static const char vlist1203[] = { 2, 2, 8,12, 5,15, 6};                                        // -==++=+-
static const char vlist1204[] = { 1, 5,15, 6};                                                 // -==++=+=
static const char vlist1205[] = { 1, 5,15, 6};                                                 // -==++=++
static const char vlist1206[] = { 3,12,10, 5, 9,10,12, 5,15, 6};                               // -==+++--
static const char vlist1207[] = { 2, 0,10, 5, 5,15, 6};                                        // -==+++-=
static const char vlist1208[] = { 2, 8,10, 5, 5,15, 6};                                        // -==+++-+
static const char vlist1209[] = { 2, 9, 1,12, 5,15, 6};                                        // -==+++=-
static const char vlist1210[] = { 1, 5,15, 6};                                                 // -==+++==
static const char vlist1211[] = { 1, 5,15, 6};                                                 // -==+++=+
static const char vlist1212[] = { 2, 9, 8,12, 5,15, 6};                                        // -==++++-
static const char vlist1213[] = { 1, 5,15, 6};                                                 // -==++++=
static const char vlist1214[] = { 1, 5,15, 6};                                                 // -==+++++
static const char vlist1215[] = { 2,13, 6,18, 6,13,16};                                        // -=+-----
static const char vlist1216[] = { 3, 6,16, 0, 0, 6,13, 6,18,13};                               // -=+----=
static const char vlist1217[] = { 4, 6,16,12, 9,13, 8, 9,18,13, 9, 6,18};                      // -=+----+
static const char vlist1218[] = { 2, 6,18, 1, 6, 1,16};                                        // -=+---=-
static const char vlist1219[] = { 3,18, 1, 6, 1, 0, 6,16, 0, 6};                               // -=+---==
static const char vlist1220[] = { 3,18, 1, 6, 1, 9, 6,16,12, 6};                               // -=+---=+
static const char vlist1221[] = { 3, 6,18,10, 6,10, 8,16, 6, 8};                               // -=+---+-
static const char vlist1222[] = { 3,10, 0, 6,18,10, 6,16, 0, 6};                               // -=+---+=
static const char vlist1223[] = { 3,10, 9, 6,18,10, 6,12,16, 6};                               // -=+---++
static const char vlist1224[] = { 4, 2,18,13, 6,18, 2, 6, 2,16, 2,13,16};                      // -=+--=--
static const char vlist1225[] = { 4,18, 2, 6, 2,18,13, 2,13, 0,16, 0, 6};                      // -=+--=-=
static const char vlist1226[] = { 4,18, 2, 6, 2,18,13, 2,13, 8,16,12, 6};                      // -=+--=-+
static const char vlist1227[] = { 4,18, 2, 6, 2,18, 1, 2, 1,16, 6, 2,16};                      // -=+--==-
static const char vlist1228[] = { 3, 1, 2,18, 6, 0,16, 2, 6,18};                               // -=+--===
static const char vlist1229[] = { 3, 1, 2,18, 2, 6,18, 6,12,16};                               // -=+--==+
static const char vlist1230[] = { 4,10, 2, 6, 6, 2, 8,16, 6, 8,18,10, 6};                      // -=+--=+-
static const char vlist1231[] = { 3, 6, 0,16, 2, 6,10,10, 6,18};                               // -=+--=+=
static const char vlist1232[] = { 3, 2, 6,10,10, 6,18, 6,12,16};                               // -=+--=++
static const char vlist1233[] = { 5,11,18,13, 6,18,11,11,13, 9,13,16, 9,16, 6, 9};             // -=+--+--
static const char vlist1234[] = { 4,16, 0, 6,18,11, 6,11,18,13,11,13, 0};                      // -=+--+-=
static const char vlist1235[] = { 4,18,11, 6,11,18,13,11,13, 8,12,16, 6};                      // -=+--+-+
static const char vlist1236[] = { 4,16, 6, 9,11,18, 1,18,11, 6, 1,16, 9};                      // -=+--+=-
static const char vlist1237[] = { 3, 1,11,18, 6, 0,16,11, 6,18};                               // -=+--+==
static const char vlist1238[] = { 3, 1,11,18,11, 6,18, 6,12,16};                               // -=+--+=+
static const char vlist1239[] = { 4,18,11, 6,10,11,18, 6, 9,16,16, 9, 8};                      // -=+--++-
static const char vlist1240[] = { 3, 6, 0,16,11, 6,18,11,18,10};                               // -=+--++=
static const char vlist1241[] = { 3,11, 6,18, 6,12,16,11,18,10};                               // -=+--+++
static const char vlist1242[] = { 3,18, 6, 3, 6, 3,13,16, 6,13};                               // -=+-=---
static const char vlist1243[] = { 4, 3,18, 6, 0, 3, 6,13, 3, 0,16, 0, 6};                      // -=+-=--=
static const char vlist1244[] = { 4,18, 3, 6, 9, 3, 6,13, 3, 8,16,12, 6};                      // -=+-=--+
static const char vlist1245[] = { 3, 3,18, 6, 3, 1, 6, 1,16, 6};                               // -=+-=-=-
static const char vlist1246[] = { 3, 6, 0,16, 3, 0, 6, 3, 6,18};                               // -=+-=-==
static const char vlist1247[] = { 3, 3, 9, 6, 3, 6,18, 6,12,16};                               // -=+-=-=+
static const char vlist1248[] = { 3,18, 3, 6, 3, 8, 6, 8,16, 6};                               // -=+-=-+-
static const char vlist1249[] = { 3, 6, 0,16, 3, 0, 6, 3, 6,18};                               // -=+-=-+=
static const char vlist1250[] = { 3, 3, 9, 6, 3, 6,18, 6,12,16};                               // -=+-=-++
static const char vlist1251[] = { 4, 3,18, 6, 3,13, 2,13,16, 2, 2,16, 6};                      // -=+-==--
static const char vlist1252[] = { 3, 0, 6,16, 3, 6,18, 3, 0,13};                               // -=+-==-=
static const char vlist1253[] = { 3, 6,12,16, 3, 6,18, 8, 3,13};                               // -=+-==-+
static const char vlist1254[] = { 3, 6, 2,16, 2, 1,16, 3, 6,18};                               // -=+-===-
static const char vlist1255[] = { 2, 6, 0,16, 3, 6,18};                                        // -=+-====
static const char vlist1256[] = { 2, 3, 6,18, 6,12,16};                                        // -=+-===+
static const char vlist1257[] = { 3, 6, 2, 8, 6, 8,16, 3, 6,18};                               // -=+-==+-
static const char vlist1258[] = { 2, 6, 0,16, 3, 6,18};                                        // -=+-==+=
static const char vlist1259[] = { 2, 3, 6,18, 6,12,16};                                        // -=+-==++
static const char vlist1260[] = { 4,18, 3, 6, 3,13, 9,13,16, 9, 9,16, 6};                      // -=+-=+--
static const char vlist1261[] = { 3, 6, 0,16, 3, 6,18, 0, 3,13};                               // -=+-=+-=
static const char vlist1262[] = { 3, 6,12,16, 3, 6,18, 8, 3,13};                               // -=+-=+-+
static const char vlist1263[] = { 3, 9, 1,16, 6, 9,16, 3, 6,18};                               // -=+-=+=-
static const char vlist1264[] = { 2, 6, 0,16, 3, 6,18};                                        // -=+-=+==
static const char vlist1265[] = { 2, 3, 6,18, 6,12,16};                                        // -=+-=+=+
static const char vlist1266[] = { 3,16, 9, 8, 6, 9,16, 3, 6,18};                               // -=+-=++-
static const char vlist1267[] = { 2, 6, 0,16, 3, 6,18};                                        // -=+-=++=
static const char vlist1268[] = { 2, 3, 6,18, 6,12,16};                                        // -=+-=+++
static const char vlist1269[] = { 4,18, 6,15,13,11,10,16,11,13, 6,11,16};                      // -=+-+---
static const char vlist1270[] = { 4,13,10, 0,16, 0, 6, 0,11, 6,18,15, 6};                      // -=+-+--=
static const char vlist1271[] = { 4,10,13, 8,11, 9, 6,12,16, 6,18,15, 6};                      // -=+-+--+
static const char vlist1272[] = { 3,11, 1, 6, 1,16, 6,18,15, 6};                               // -=+-+-=-
static const char vlist1273[] = { 3, 6, 0,16,11, 0, 6,15, 6,18};                               // -=+-+-==
static const char vlist1274[] = { 3,11, 9, 6,15, 6,18, 6,12,16};                               // -=+-+-=+
static const char vlist1275[] = { 3, 8,16, 6,11, 8, 6,18,15, 6};                               // -=+-+-+-
static const char vlist1276[] = { 3, 6, 0,16,11, 0, 6,15, 6,18};                               // -=+-+-+=
static const char vlist1277[] = { 3,11, 9, 6,15, 6,18, 6,12,16};                               // -=+-+-++
static const char vlist1278[] = { 4,10,13, 2,13,16, 2, 2,16, 6,18,15, 6};                      // -=+-+=--
static const char vlist1279[] = { 3, 6, 0,16,15, 6,18, 0,10,13};                               // -=+-+=-=
static const char vlist1280[] = { 3, 6,12,16,15, 6,18, 8,10,13};                               // -=+-+=-+
static const char vlist1281[] = { 3, 6, 2,16, 2, 1,16,15, 6,18};                               // -=+-+==-
static const char vlist1282[] = { 2, 6, 0,16,15, 6,18};                                        // -=+-+===
static const char vlist1283[] = { 2,15, 6,18, 6,12,16};                                        // -=+-+==+
static const char vlist1284[] = { 3, 6, 2, 8, 6, 8,16,15, 6,18};                               // -=+-+=+-
static const char vlist1285[] = { 2, 6, 0,16,15, 6,18};                                        // -=+-+=+=
static const char vlist1286[] = { 2,15, 6,18, 6,12,16};                                        // -=+-+=++
static const char vlist1287[] = { 4,10,13, 9,13,16, 9, 9,16, 6,18,15, 6};                      // -=+-++--
static const char vlist1288[] = { 3, 6, 0,16,15, 6,18, 0,10,13};                               // -=+-++-=
static const char vlist1289[] = { 3, 6,12,16,15, 6,18, 8,10,13};                               // -=+-++-+
static const char vlist1290[] = { 3, 9, 1,16, 6, 9,16,15, 6,18};                               // -=+-++=-
static const char vlist1291[] = { 2, 6, 0,16,15, 6,18};                                        // -=+-++==
static const char vlist1292[] = { 2,15, 6,18, 6,12,16};                                        // -=+-++=+
static const char vlist1293[] = { 3,16, 9, 8, 6, 9,16,15, 6,18};                               // -=+-+++-
static const char vlist1294[] = { 2, 6, 0,16,15, 6,18};                                        // -=+-+++=
static const char vlist1295[] = { 2,15, 6,18, 6,12,16};                                        // -=+-++++
static const char vlist1296[] = { 2, 4, 6,13, 6,18,13};                                        // -=+=----
static const char vlist1297[] = { 2,13, 0, 6,18,13, 6};                                        // -=+=---=
static const char vlist1298[] = { 3, 8, 9,13,13, 9,18,18, 9, 6};                               // -=+=---+
static const char vlist1299[] = { 2, 1, 4, 6,18, 1, 6};                                        // -=+=--=-
static const char vlist1300[] = { 2,18, 1, 6, 1, 0, 6};                                        // -=+=--==
static const char vlist1301[] = { 2,18, 1, 6, 1, 9, 6};                                        // -=+=--=+
static const char vlist1302[] = { 3, 8, 4, 6,10, 8, 6,18,10, 6};                               // -=+=--+-
static const char vlist1303[] = { 2,10, 0, 6,18,10, 6};                                        // -=+=--+=
static const char vlist1304[] = { 2,10, 9, 6,18,10, 6};                                        // -=+=--++
static const char vlist1305[] = { 3,13, 4, 2,18,13, 2,18, 2, 6};                               // -=+=-=--
static const char vlist1306[] = { 3, 2, 6,18,13, 2,18, 0, 2,13};                               // -=+=-=-=
static const char vlist1307[] = { 3, 2, 6,18,13, 2,18, 8, 2,13};                               // -=+=-=-+
static const char vlist1308[] = { 3, 4, 2, 1, 2, 6,18, 1, 2,18};                               // -=+=-==-
static const char vlist1309[] = { 2, 1, 2,18,18, 2, 6};                                        // -=+=-===
static const char vlist1310[] = { 2, 1, 2,18,18, 2, 6};                                        // -=+=-==+
static const char vlist1311[] = { 3, 4, 2, 8, 2, 6,10,10, 6,18};                               // -=+=-=+-
static const char vlist1312[] = { 2,10, 2, 6,18,10, 6};                                        // -=+=-=+=
static const char vlist1313[] = { 2,10, 2, 6,18,10, 6};                                        // -=+=-=++
static const char vlist1314[] = { 4,13, 4, 9,13, 9,11,18,11, 6,18,13,11};                      // -=+=-+--
static const char vlist1315[] = { 3, 0,11,13,13,11,18,11, 6,18};                               // -=+=-+-=
static const char vlist1316[] = { 3, 8,11,13,13,11,18,11, 6,18};                               // -=+=-+-+
static const char vlist1317[] = { 3, 4, 9, 1, 1,11,18,11, 6,18};                               // -=+=-+=-
static const char vlist1318[] = { 2, 1,11,18,18,11, 6};                                        // -=+=-+==
static const char vlist1319[] = { 2, 1,11,18,18,11, 6};                                        // -=+=-+=+
static const char vlist1320[] = { 3, 4, 9, 8,11,18,10,11, 6,18};                               // -=+=-++-
static const char vlist1321[] = { 2,10,11,18,18,11, 6};                                        // -=+=-++=
static const char vlist1322[] = { 2,10,11,18,18,11, 6};                                        // -=+=-+++
static const char vlist1323[] = { 3, 3, 4, 6,13, 4, 3, 3,18, 6};                               // -=+==---
static const char vlist1324[] = { 3, 6, 3,18, 6, 3, 0, 0, 3,13};                               // -=+==--=
static const char vlist1325[] = { 3, 3, 6,18, 6, 3, 9, 8, 3,13};                               // -=+==--+
static const char vlist1326[] = { 3, 4, 3, 1, 3, 4, 6, 6, 3,18};                               // -=+==-=-
static const char vlist1327[] = { 2, 3, 0, 6,18, 3, 6};                                        // -=+==-==
static const char vlist1328[] = { 2, 3, 9, 6,18, 3, 6};                                        // -=+==-=+
static const char vlist1329[] = { 3, 3, 8, 6, 3, 6,18, 8, 4, 6};                               // -=+==-+-
static const char vlist1330[] = { 2, 3, 0, 6,18, 3, 6};                                        // -=+==-+=
static const char vlist1331[] = { 2, 3, 9, 6,18, 3, 6};                                        // -=+==-++
static const char vlist1332[] = { 3, 3,13, 2,18, 3, 6,13, 4, 2};                               // -=+===--
static const char vlist1333[] = { 2, 0, 3,13,18, 3, 6};                                        // -=+===-=
static const char vlist1334[] = { 2, 8, 3,13,18, 3, 6};                                        // -=+===-+
static const char vlist1335[] = { 2, 2, 1, 4,18, 3, 6};                                        // -=+====-
static const char vlist1336[] = { 1,18, 3, 6};                                                 // -=+=====
static const char vlist1337[] = { 1,18, 3, 6};                                                 // -=+====+
static const char vlist1338[] = { 2, 2, 8, 4,18, 3, 6};                                        // -=+===+-
static const char vlist1339[] = { 1,18, 3, 6};                                                 // -=+===+=
static const char vlist1340[] = { 1,18, 3, 6};                                                 // -=+===++
static const char vlist1341[] = { 3, 9, 3,13,18, 3, 6, 4, 9,13};                               // -=+==+--
static const char vlist1342[] = { 2, 0, 3,13,18, 3, 6};                                        // -=+==+-=
static const char vlist1343[] = { 2, 8, 3,13,18, 3, 6};                                        // -=+==+-+
static const char vlist1344[] = { 2, 9, 1, 4,18, 3, 6};                                        // -=+==+=-
static const char vlist1345[] = { 1,18, 3, 6};                                                 // -=+==+==
static const char vlist1346[] = { 1,18, 3, 6};                                                 // -=+==+=+
static const char vlist1347[] = { 2, 9, 8, 4,18, 3, 6};                                        // -=+==++-
static const char vlist1348[] = { 1,18, 3, 6};                                                 // -=+==++=
static const char vlist1349[] = { 1,18, 3, 6};                                                 // -=+==+++
static const char vlist1350[] = { 4,11, 4, 6,10, 4,11,13, 4,10,18,15, 6};                      // -=+=+---
static const char vlist1351[] = { 3,15, 6,18, 6,11, 0, 0,10,13};                               // -=+=+--=
static const char vlist1352[] = { 3,15, 6,18, 6,11, 9, 8,10,13};                               // -=+=+--+
static const char vlist1353[] = { 3,15, 6,18,11, 4, 6, 4,11, 1};                               // -=+=+-=-
static const char vlist1354[] = { 2,11, 0, 6,18,15, 6};                                        // -=+=+-==
static const char vlist1355[] = { 2,11, 9, 6,18,15, 6};                                        // -=+=+-=+
static const char vlist1356[] = { 3,11, 8, 6, 8, 4, 6,15, 6,18};                               // -=+=+-+-
static const char vlist1357[] = { 2,11, 0, 6,18,15, 6};                                        // -=+=+-+=
static const char vlist1358[] = { 2,11, 9, 6,18,15, 6};                                        // -=+=+-++
static const char vlist1359[] = { 3,18,15, 6, 4,10,13,10, 4, 2};                               // -=+=+=--
static const char vlist1360[] = { 2, 0,10,13,18,15, 6};                                        // -=+=+=-=
static const char vlist1361[] = { 2, 8,10,13,18,15, 6};                                        // -=+=+=-+
static const char vlist1362[] = { 2, 2, 1, 4,18,15, 6};                                        // -=+=+==-
static const char vlist1363[] = { 1,18,15, 6};                                                 // -=+=+===
static const char vlist1364[] = { 1,18,15, 6};                                                 // -=+=+==+
static const char vlist1365[] = { 2, 2, 8, 4,18,15, 6};                                        // -=+=+=+-
static const char vlist1366[] = { 1,18,15, 6};                                                 // -=+=+=+=
static const char vlist1367[] = { 1,18,15, 6};                                                 // -=+=+=++
static const char vlist1368[] = { 3, 9,10,13, 4, 9,13,18,15, 6};                               // -=+=++--
static const char vlist1369[] = { 2, 0,10,13,18,15, 6};                                        // -=+=++-=
static const char vlist1370[] = { 2, 8,10,13,18,15, 6};                                        // -=+=++-+
static const char vlist1371[] = { 2, 9, 1, 4,18,15, 6};                                        // -=+=++=-
static const char vlist1372[] = { 1,18,15, 6};                                                 // -=+=++==
static const char vlist1373[] = { 1,18,15, 6};                                                 // -=+=++=+
static const char vlist1374[] = { 2, 9, 8, 4,18,15, 6};                                        // -=+=+++-
static const char vlist1375[] = { 1,18,15, 6};                                                 // -=+=+++=
static const char vlist1376[] = { 1,18,15, 6};                                                 // -=+=++++
static const char vlist1377[] = { 2, 6,13,12, 6,18,13};                                        // -=++----
static const char vlist1378[] = { 2,13, 0, 6,18,13, 6};                                        // -=++---=
static const char vlist1379[] = { 3, 8, 9,13,13, 9,18,18, 9, 6};                               // -=++---+
static const char vlist1380[] = { 2, 1,12, 6,18, 1, 6};                                        // -=++--=-
static const char vlist1381[] = { 2,18, 1, 6, 1, 0, 6};                                        // -=++--==
static const char vlist1382[] = { 2,18, 1, 6, 1, 9, 6};                                        // -=++--=+
static const char vlist1383[] = { 3,10, 8, 6, 8,12, 6,18,10, 6};                               // -=++--+-
static const char vlist1384[] = { 2,10, 0, 6,18,10, 6};                                        // -=++--+=
static const char vlist1385[] = { 2,10, 9, 6,18,10, 6};                                        // -=++--++
static const char vlist1386[] = { 3,13,12, 2,18,13, 2,18, 2, 6};                               // -=++-=--
static const char vlist1387[] = { 3, 2, 6,18,13, 2,18, 0, 2,13};                               // -=++-=-=
static const char vlist1388[] = { 3, 2, 6,18,13, 2,18, 8, 2,13};                               // -=++-=-+
static const char vlist1389[] = { 3, 2, 6,18, 1, 2,18,12, 2, 1};                               // -=++-==-
static const char vlist1390[] = { 2, 1, 2,18,18, 2, 6};                                        // -=++-===
static const char vlist1391[] = { 2, 1, 2,18,18, 2, 6};                                        // -=++-==+
static const char vlist1392[] = { 3, 2, 6,10,12, 2, 8,10, 6,18};                               // -=++-=+-
static const char vlist1393[] = { 2,10, 2, 6,18,10, 6};                                        // -=++-=+=
static const char vlist1394[] = { 2,10, 2, 6,18,10, 6};                                        // -=++-=++
static const char vlist1395[] = { 4,13,12, 9,18,13,11,13, 9,11,18,11, 6};                      // -=++-+--
static const char vlist1396[] = { 3, 0,11,13,13,11,18,11, 6,18};                               // -=++-+-=
static const char vlist1397[] = { 3, 8,11,13,13,11,18,11, 6,18};                               // -=++-+-+
static const char vlist1398[] = { 3,12, 9, 1, 1,11,18,11, 6,18};                               // -=++-+=-
static const char vlist1399[] = { 2, 1,11,18,18,11, 6};                                        // -=++-+==
static const char vlist1400[] = { 2, 1,11,18,18,11, 6};                                        // -=++-+=+
static const char vlist1401[] = { 3,12, 9, 8,11,18,10,11, 6,18};                               // -=++-++-
static const char vlist1402[] = { 2,10,11,18,18,11, 6};                                        // -=++-++=
static const char vlist1403[] = { 2,10,11,18,18,11, 6};                                        // -=++-+++
static const char vlist1404[] = { 3, 3,12, 6,13,12, 3,18, 3, 6};                               // -=++=---
static const char vlist1405[] = { 3, 3, 6,18, 6, 3, 0, 0, 3,13};                               // -=++=--=
static const char vlist1406[] = { 3, 3, 6,18, 6, 3, 9, 8, 3,13};                               // -=++=--+
static const char vlist1407[] = { 3, 3,12, 6,12, 3, 1, 3, 6,18};                               // -=++=-=-
static const char vlist1408[] = { 2, 3, 0, 6,18, 3, 6};                                        // -=++=-==
static const char vlist1409[] = { 2, 3, 9, 6,18, 3, 6};                                        // -=++=-=+
static const char vlist1410[] = { 3, 3, 8, 6, 3, 6,18, 8,12, 6};                               // -=++=-+-
static const char vlist1411[] = { 2, 3, 0, 6,18, 3, 6};                                        // -=++=-+=
static const char vlist1412[] = { 2, 3, 9, 6,18, 3, 6};                                        // -=++=-++
static const char vlist1413[] = { 3,12, 3,13, 3,12, 2,18, 3, 6};                               // -=++==--
static const char vlist1414[] = { 2, 0, 3,13,18, 3, 6};                                        // -=++==-=
static const char vlist1415[] = { 2, 8, 3,13,18, 3, 6};                                        // -=++==-+
static const char vlist1416[] = { 2, 2, 1,12,18, 3, 6};                                        // -=++===-
static const char vlist1417[] = { 1,18, 3, 6};                                                 // -=++====
static const char vlist1418[] = { 1,18, 3, 6};                                                 // -=++===+
static const char vlist1419[] = { 2, 2, 8,12,18, 3, 6};                                        // -=++==+-
static const char vlist1420[] = { 1,18, 3, 6};                                                 // -=++==+=
static const char vlist1421[] = { 1,18, 3, 6};                                                 // -=++==++
static const char vlist1422[] = { 3, 9, 3,13,18, 3, 6,12, 9,13};                               // -=++=+--
static const char vlist1423[] = { 2, 0, 3,13,18, 3, 6};                                        // -=++=+-=
static const char vlist1424[] = { 2, 8, 3,13,18, 3, 6};                                        // -=++=+-+
static const char vlist1425[] = { 2, 9, 1,12,18, 3, 6};                                        // -=++=+=-
static const char vlist1426[] = { 1,18, 3, 6};                                                 // -=++=+==
static const char vlist1427[] = { 1,18, 3, 6};                                                 // -=++=+=+
static const char vlist1428[] = { 2, 9, 8,12,18, 3, 6};                                        // -=++=++-
static const char vlist1429[] = { 1,18, 3, 6};                                                 // -=++=++=
static const char vlist1430[] = { 1,18, 3, 6};                                                 // -=++=+++
static const char vlist1431[] = { 4,11,12, 6,10,12,11,13,12,10,18,15, 6};                      // -=+++---
static const char vlist1432[] = { 3,15, 6,18, 6,11, 0, 0,10,13};                               // -=+++--=
static const char vlist1433[] = { 3,15, 6,18, 6,11, 9, 8,10,13};                               // -=+++--+
static const char vlist1434[] = { 3,15, 6,18,12,11, 1,11,12, 6};                               // -=+++-=-
static const char vlist1435[] = { 2,11, 0, 6,18,15, 6};                                        // -=+++-==
static const char vlist1436[] = { 2,11, 9, 6,18,15, 6};                                        // -=+++-=+
static const char vlist1437[] = { 3,11,12, 6,11, 8,12,15, 6,18};                               // -=+++-+-
static const char vlist1438[] = { 2,11, 0, 6,18,15, 6};                                        // -=+++-+=
static const char vlist1439[] = { 2,11, 9, 6,18,15, 6};                                        // -=+++-++
static const char vlist1440[] = { 3,18,15, 6,10,12, 2,12,10,13};                               // -=+++=--
static const char vlist1441[] = { 2, 0,10,13,18,15, 6};                                        // -=+++=-=
static const char vlist1442[] = { 2, 8,10,13,18,15, 6};                                        // -=+++=-+
static const char vlist1443[] = { 2, 2, 1,12,18,15, 6};                                        // -=+++==-
static const char vlist1444[] = { 1,18,15, 6};                                                 // -=+++===
static const char vlist1445[] = { 1,18,15, 6};                                                 // -=+++==+
static const char vlist1446[] = { 2, 2, 8,12,18,15, 6};                                        // -=+++=+-
static const char vlist1447[] = { 1,18,15, 6};                                                 // -=+++=+=
static const char vlist1448[] = { 1,18,15, 6};                                                 // -=+++=++
static const char vlist1449[] = { 3,12,10,13, 9,10,12,18,15, 6};                               // -=++++--
static const char vlist1450[] = { 2, 0,10,13,18,15, 6};                                        // -=++++-=
static const char vlist1451[] = { 2, 8,10,13,18,15, 6};                                        // -=++++-+
static const char vlist1452[] = { 2, 9, 1,12,18,15, 6};                                        // -=++++=-
static const char vlist1453[] = { 1,18,15, 6};                                                 // -=++++==
static const char vlist1454[] = { 1,18,15, 6};                                                 // -=++++=+
static const char vlist1455[] = { 2, 9, 8,12,18,15, 6};                                        // -=+++++-
static const char vlist1456[] = { 1,18,15, 6};                                                 // -=+++++=
static const char vlist1457[] = { 1,18,15, 6};                                                 // -=++++++
static const char vlist1458[] = { 1,17,14,19};                                                 // -+------
static const char vlist1459[] = { 2,17, 0,19, 0,14,19};                                        // -+-----=
static const char vlist1460[] = { 4,14, 8, 9,19, 8,14, 8,19,17, 8,17,12};                      // -+-----+
static const char vlist1461[] = { 3, 1,19,17,14,19, 1,14, 1,17};                               // -+----=-
static const char vlist1462[] = { 4,19, 1,14, 1,19,17,17, 1, 0, 0, 1,14};                      // -+----==
static const char vlist1463[] = { 4,19, 1,14, 1,19,17, 1,17,12,14, 1, 9};                      // -+----=+
static const char vlist1464[] = { 6,14,19,10,19,13,10,19,17,13,17, 8,13, 8,17,14, 8,14,10};    // -+----+-
static const char vlist1465[] = { 5, 0,14,10,14,19,10,19,13,10,13,19,17,13,17, 0};             // -+----+=
static const char vlist1466[] = { 5,14,10, 9,19,10,14,19,13,10,13,19,17,13,17,12};             // -+----++
static const char vlist1467[] = { 1,17, 2,19};                                                 // -+---=--
static const char vlist1468[] = { 2, 0, 2,19,17, 0,19};                                        // -+---=-=
static const char vlist1469[] = { 3, 2,19, 8,17,12, 8,19,17, 8};                               // -+---=-+
static const char vlist1470[] = { 3, 1, 2,17,19, 2, 1, 1,19,17};                               // -+---==-
static const char vlist1471[] = { 3, 1, 2,19,17, 1,19, 0, 1,17};                               // -+---===
static const char vlist1472[] = { 3, 1, 2,19,17, 1,19,12, 1,17};                               // -+---==+
static const char vlist1473[] = { 5,17, 2, 8, 2,19,10,10,19,13,13,17, 8,13,19,17};             // -+---=+-
static const char vlist1474[] = { 4, 0,13,17,17,13,19,13,10,19,10, 2,19};                      // -+---=+=
static const char vlist1475[] = { 4,12,13,17,17,13,19,13,10,19,10, 2,19};                      // -+---=++
static const char vlist1476[] = { 2,17, 9,11,19,17,11};                                        // -+---+--
static const char vlist1477[] = { 2,19, 0,11,17, 0,19};                                        // -+---+-=
static const char vlist1478[] = { 3,17,12, 8,19,17, 8,11,19, 8};                               // -+---+-+
static const char vlist1479[] = { 3,11,19, 1, 1,19,17, 1,17, 9};                               // -+---+=-
static const char vlist1480[] = { 3, 1,11,19,17, 1,19, 0, 1,17};                               // -+---+==
static const char vlist1481[] = { 3, 1,11,19,17, 1,19,12, 1,17};                               // -+---+=+
static const char vlist1482[] = { 5,11,19,10,13,19,17,10,19,13,13,17, 8,17, 9, 8};             // -+---++-
static const char vlist1483[] = { 4, 0,13,17,17,13,19,13,10,19,10,11,19};                      // -+---++=
static const char vlist1484[] = { 4,12,13,17,17,13,19,13,10,19,10,11,19};                      // -+---+++
static const char vlist1485[] = { 2, 3,19,17,14, 3,17};                                        // -+--=---
static const char vlist1486[] = { 3, 0, 3,14, 3,17, 0, 3,19,17};                               // -+--=--=
static const char vlist1487[] = { 4, 3,19, 8, 3,14, 9,17,12, 8,19,17, 8};                      // -+--=--+
static const char vlist1488[] = { 4, 1, 3,14,19, 3, 1, 1,19,17, 1,17,14};                      // -+--=-=-
static const char vlist1489[] = { 4, 1, 3,19,17, 1,19, 0, 1,17, 0, 3,14};                      // -+--=-==
static const char vlist1490[] = { 4, 1, 3,19,17, 1,19,12, 1,17, 3, 9,14};                      // -+--=-=+
static const char vlist1491[] = { 5,14, 3, 8, 3,19,13,13,19,17,13,17, 8,17,14, 8};             // -+--=-+-
static const char vlist1492[] = { 4, 0,13,17,17,13,19,13, 3,19, 3, 0,14};                      // -+--=-+=
static const char vlist1493[] = { 4,12,13,17,17,13,19,13, 3,19, 3, 9,14};                      // -+--=-++
static const char vlist1494[] = { 2, 3,19,17, 2, 3,17};                                        // -+--==--
static const char vlist1495[] = { 2, 0, 3,17,17, 3,19};                                        // -+--==-=
static const char vlist1496[] = { 3, 8, 3,19, 8,19,17,12, 8,17};                               // -+--==-+
static const char vlist1497[] = { 3, 2, 1,17,17, 1,19, 1, 3,19};                               // -+--===-
static const char vlist1498[] = { 3, 1, 3,19,17, 1,19, 0, 1,17};                               // -+--====
static const char vlist1499[] = { 3, 1, 3,19,17, 1,19,12, 1,17};                               // -+--===+
static const char vlist1500[] = { 4, 2, 8,17, 8,13,17,17,13,19,13, 3,19};                      // -+--==+-
static const char vlist1501[] = { 3, 0,13,17,17,13,19,13, 3,19};                               // -+--==+=
static const char vlist1502[] = { 3,12,13,17,17,13,19,13, 3,19};                               // -+--==++
static const char vlist1503[] = { 2, 3,19,17, 3,17, 9};                                        // -+--=+--
static const char vlist1504[] = { 2, 0, 3,17,17, 3,19};                                        // -+--=+-=
static const char vlist1505[] = { 3, 8, 3,19, 8,19,17,12, 8,17};                               // -+--=+-+
static const char vlist1506[] = { 3, 9, 1,17,17, 1,19, 1, 3,19};                               // -+--=+=-
static const char vlist1507[] = { 3, 1, 3,19,17, 1,19, 0, 1,17};                               // -+--=+==
static const char vlist1508[] = { 3, 1, 3,19,17, 1,19,12, 1,17};                               // -+--=+=+
static const char vlist1509[] = { 4, 8,13,17, 9, 8,17,17,13,19,13, 3,19};                      // -+--=++-
static const char vlist1510[] = { 3, 0,13,17,17,13,19,13, 3,19};                               // -+--=++=
static const char vlist1511[] = { 3,12,13,17,17,13,19,13, 3,19};                               // -+--=+++
static const char vlist1512[] = { 4,10,14,11,10,17,14,17,10,15,19,17,15};                      // -+--+---
static const char vlist1513[] = { 4,11,14, 0,17, 0,10,15,17,10,15,19,17};                      // -+--+--=
static const char vlist1514[] = { 5,14,11, 9,10,12, 8,10,15,12,15,17,12,15,19,17};             // -+--+--+
static const char vlist1515[] = { 4,15,19, 1,19,17, 1, 1,17,14, 1,14,11};                      // -+--+-=-
static const char vlist1516[] = { 4, 1,15,19, 1,19,17, 0, 1,17,11, 0,14};                      // -+--+-==
static const char vlist1517[] = { 4, 1,15,19, 1,19,17,12, 1,17,11, 9,14};                      // -+--+-=+
static const char vlist1518[] = { 5,15,19,13,13,19,17,13,17, 8,17,14, 8,14,11, 8};             // -+--+-+-
static const char vlist1519[] = { 4, 0,13,17,17,13,19,13,15,19,11, 0,14};                      // -+--+-+=
static const char vlist1520[] = { 4,12,13,17,17,13,19,13,15,19,11, 9,14};                      // -+--+-++
static const char vlist1521[] = { 3,17, 2,10,15,19,17,15,17,10};                               // -+--+=--
static const char vlist1522[] = { 3, 0,10,17,10,15,17,17,15,19};                               // -+--+=-=
static const char vlist1523[] = { 4, 8,10,12,12,10,15,12,15,17,17,15,19};                      // -+--+=-+
static const char vlist1524[] = { 3, 2, 1,17, 1,19,17, 1,15,19};                               // -+--+==-
static const char vlist1525[] = { 3, 1,15,19, 1,19,17, 0, 1,17};                               // -+--+===
static const char vlist1526[] = { 3, 1,15,19, 1,19,17,12, 1,17};                               // -+--+==+
static const char vlist1527[] = { 4, 2, 8,17, 8,13,17,17,13,19,13,15,19};                      // -+--+=+-
static const char vlist1528[] = { 3, 0,13,17,17,13,19,13,15,19};                               // -+--+=+=
static const char vlist1529[] = { 3,12,13,17,17,13,19,13,15,19};                               // -+--+=++
static const char vlist1530[] = { 3,15,19,17,15,17,10,10,17, 9};                               // -+--++--
static const char vlist1531[] = { 3, 0,10,17,10,15,17,17,15,19};                               // -+--++-=
static const char vlist1532[] = { 4, 8,10,12,12,10,15,12,15,17,17,15,19};                      // -+--++-+
static const char vlist1533[] = { 3, 9, 1,17, 1,19,17, 1,15,19};                               // -+--++=-
static const char vlist1534[] = { 3, 1,15,19, 1,19,17, 0, 1,17};                               // -+--++==
static const char vlist1535[] = { 3, 1,15,19, 1,19,17,12, 1,17};                               // -+--++=+
static const char vlist1536[] = { 4, 8,13,17, 9, 8,17,17,13,19,13,15,19};                      // -+--+++-
static const char vlist1537[] = { 3, 0,13,17,17,13,19,13,15,19};                               // -+--+++=
static const char vlist1538[] = { 3,12,13,17,17,13,19,13,15,19};                               // -+--++++
static const char vlist1539[] = { 1, 4,14,19};                                                 // -+-=----
static const char vlist1540[] = { 2, 4, 0,19, 0,14,19};                                        // -+-=---=
static const char vlist1541[] = { 3,19, 4, 8, 9,14, 8,14,19, 8};                               // -+-=---+
static const char vlist1542[] = { 3, 1, 4,14,19, 4, 1,19, 1,14};                               // -+-=--=-
static const char vlist1543[] = { 3, 4, 1,19, 1,14,19, 1, 0,14};                               // -+-=--==
static const char vlist1544[] = { 3, 4, 1,19, 1,14,19, 1, 9,14};                               // -+-=--=+
static const char vlist1545[] = { 5, 4,14, 8,19, 4,13,19,13,10,14,10, 8,19,10,14};             // -+-=--+-
static const char vlist1546[] = { 4,10, 0,14,10,14,19,13,10,19, 4,13,19};                      // -+-=--+=
static const char vlist1547[] = { 4,10, 9,14,10,14,19,13,10,19, 4,13,19};                      // -+-=--++
static const char vlist1548[] = { 1, 4, 2,19};                                                 // -+-=-=--
static const char vlist1549[] = { 2, 0, 2,19, 4, 0,19};                                        // -+-=-=-=
static const char vlist1550[] = { 2, 2,19, 8,19, 4, 8};                                        // -+-=-=-+
static const char vlist1551[] = { 3, 2, 4, 1, 1, 2,19, 4, 1,19};                               // -+-=-==-
static const char vlist1552[] = { 2, 1, 2,19, 1,19, 4};                                        // -+-=-===
static const char vlist1553[] = { 2, 1, 2,19, 1,19, 4};                                        // -+-=-==+
static const char vlist1554[] = { 4, 4, 2, 8,10, 2,19,13,10,19, 4,13,19};                      // -+-=-=+-
static const char vlist1555[] = { 3,10, 2,19,13,10,19,13,19, 4};                               // -+-=-=+=
static const char vlist1556[] = { 3,10, 2,19,13,10,19,13,19, 4};                               // -+-=-=++
static const char vlist1557[] = { 2,19, 4,11, 4, 9,11};                                        // -+-=-+--
static const char vlist1558[] = { 2, 4, 0,11,19, 4,11};                                        // -+-=-+-=
static const char vlist1559[] = { 2,19, 4, 8,11,19, 8};                                        // -+-=-+-+
static const char vlist1560[] = { 3, 4, 9, 1, 4, 1,19, 1,11,19};                               // -+-=-+=-
static const char vlist1561[] = { 2, 1,11,19, 1,19, 4};                                        // -+-=-+==
static const char vlist1562[] = { 2, 1,11,19, 1,19, 4};                                        // -+-=-+=+
static const char vlist1563[] = { 4, 4, 9, 8,13,10,19,10,11,19, 4,13,19};                      // -+-=-++-
static const char vlist1564[] = { 3,10,11,19,13,10,19,13,19, 4};                               // -+-=-++=
static const char vlist1565[] = { 3,10,11,19,13,10,19,13,19, 4};                               // -+-=-+++
static const char vlist1566[] = { 2, 3, 4,14,19, 4, 3};                                        // -+-==---
static const char vlist1567[] = { 3, 3, 4, 0, 4, 3,19, 0, 3,14};                               // -+-==--=
static const char vlist1568[] = { 3, 3, 4, 8, 4, 3,19, 9, 3,14};                               // -+-==--+
static const char vlist1569[] = { 4, 3,19, 1, 3, 1,14,19, 4, 1, 1, 4,14};                      // -+-==-=-
static const char vlist1570[] = { 3, 1, 3,19, 1,19, 4, 3, 0,14};                               // -+-==-==
static const char vlist1571[] = { 3, 1, 3,19, 1,19, 4, 3, 9,14};                               // -+-==-=+
static const char vlist1572[] = { 4,13, 3,19,14, 3, 8, 4,13,19, 4,14, 8};                      // -+-==-+-
static const char vlist1573[] = { 3, 3, 0,14,13,19, 4, 3,19,13};                               // -+-==-+=
static const char vlist1574[] = { 3, 3, 9,14,13,19, 4, 3,19,13};                               // -+-==-++
static const char vlist1575[] = { 2, 3, 4, 2, 4, 3,19};                                        // -+-===--
static const char vlist1576[] = { 2, 0, 3, 4, 3,19, 4};                                        // -+-===-=
static const char vlist1577[] = { 2, 8, 3, 4, 3,19, 4};                                        // -+-===-+
static const char vlist1578[] = { 3, 1, 2, 4, 1,19, 4, 1, 3,19};                               // -+-====-
static const char vlist1579[] = { 2, 1, 3,19, 1,19, 4};                                        // -+-=====
static const char vlist1580[] = { 2, 1, 3,19, 1,19, 4};                                        // -+-====+
static const char vlist1581[] = { 3, 2, 8, 4, 3,19,13,13,19, 4};                               // -+-===+-
static const char vlist1582[] = { 2, 3,19,13,13,19, 4};                                        // -+-===+=
static const char vlist1583[] = { 2, 3,19,13,13,19, 4};                                        // -+-===++
static const char vlist1584[] = { 2, 3, 4, 9, 4, 3,19};                                        // -+-==+--
static const char vlist1585[] = { 2, 0, 3, 4, 3,19, 4};                                        // -+-==+-=
static const char vlist1586[] = { 2, 8, 3, 4, 3,19, 4};                                        // -+-==+-+
static const char vlist1587[] = { 3, 9, 1, 4, 1,19, 4, 1, 3,19};                               // -+-==+=-
static const char vlist1588[] = { 2, 1, 3,19, 1,19, 4};                                        // -+-==+==
static const char vlist1589[] = { 2, 1, 3,19, 1,19, 4};                                        // -+-==+=+
static const char vlist1590[] = { 3, 9, 8, 4, 3,19,13,13,19, 4};                               // -+-==++-
static const char vlist1591[] = { 2, 3,19,13,13,19, 4};                                        // -+-==++=
static const char vlist1592[] = { 2, 3,19,13,13,19, 4};                                        // -+-==+++
static const char vlist1593[] = { 4, 4,14,11,10, 4,11,15, 4,10,19, 4,15};                      // -+-=+---
static const char vlist1594[] = { 4, 0,11,14,10, 4, 0,15, 4,10, 4,15,19};                      // -+-=+--=
static const char vlist1595[] = { 4,11, 9,14,10, 4, 8,15, 4,10, 4,15,19};                      // -+-=+--+
static const char vlist1596[] = { 4, 4,15,19,15, 4, 1, 1, 4,11, 4,14,11};                      // -+-=+-=-
static const char vlist1597[] = { 3, 1,15, 4,11, 0,14,15,19, 4};                               // -+-=+-==
static const char vlist1598[] = { 3, 1,15, 4,11, 9,14,15,19, 4};                               // -+-=+-=+
static const char vlist1599[] = { 4,13,15,19, 4,13,19, 4,14, 8,14,11, 8};                      // -+-=+-+-
static const char vlist1600[] = { 3,11, 0,14,13,19, 4,15,19,13};                               // -+-=+-+=
static const char vlist1601[] = { 3,11, 9,14,13,19, 4,15,19,13};                               // -+-=+-++
static const char vlist1602[] = { 3,10, 4, 2,15, 4,10, 4,15,19};                               // -+-=+=--
static const char vlist1603[] = { 3, 0,10, 4,10,15, 4,15,19, 4};                               // -+-=+=-=
static const char vlist1604[] = { 3, 8,10, 4,10,15, 4,15,19, 4};                               // -+-=+=-+
static const char vlist1605[] = { 3, 2, 1, 4, 1,15, 4,15,19, 4};                               // -+-=+==-
static const char vlist1606[] = { 2, 1,15, 4,15,19, 4};                                        // -+-=+===
static const char vlist1607[] = { 2, 1,15, 4,15,19, 4};                                        // -+-=+==+
static const char vlist1608[] = { 3, 2, 8, 4,13,19, 4,15,19,13};                               // -+-=+=+-
static const char vlist1609[] = { 2,15,19,13,13,19, 4};                                        // -+-=+=+=
static const char vlist1610[] = { 2,15,19,13,13,19, 4};                                        // -+-=+=++
static const char vlist1611[] = { 3,10, 4, 9,15, 4,10, 4,15,19};                               // -+-=++--
static const char vlist1612[] = { 3, 0,10, 4,10,15, 4,15,19, 4};                               // -+-=++-=
static const char vlist1613[] = { 3, 8,10, 4,10,15, 4,15,19, 4};                               // -+-=++-+
static const char vlist1614[] = { 3, 9, 1, 4, 1,15, 4,15,19, 4};                               // -+-=++=-
static const char vlist1615[] = { 2, 1,15, 4,15,19, 4};                                        // -+-=++==
static const char vlist1616[] = { 2, 1,15, 4,15,19, 4};                                        // -+-=++=+
static const char vlist1617[] = { 3, 9, 8, 4,13,19, 4,15,19,13};                               // -+-=+++-
static const char vlist1618[] = { 2,15,19,13,13,19, 4};                                        // -+-=+++=
static const char vlist1619[] = { 2,15,19,13,13,19, 4};                                        // -+-=++++
static const char vlist1620[] = { 2,12,14,16,14,19,16};                                        // -+-+----
static const char vlist1621[] = { 2, 0,19,16, 0,14,19};                                        // -+-+---=
static const char vlist1622[] = { 3, 9,14, 8,14,19, 8,19,16, 8};                               // -+-+---+
static const char vlist1623[] = { 3,19,16, 1,19, 1,14,14, 1,12};                               // -+-+--=-
static const char vlist1624[] = { 3,16, 1,19, 1,14,19, 1, 0,14};                               // -+-+--==
static const char vlist1625[] = { 3,16, 1,19, 1,14,19, 1, 9,14};                               // -+-+--=+
static const char vlist1626[] = { 5,19,16,13,19,10,14,19,13,10,14,10, 8,12,14, 8};             // -+-+--+-
static const char vlist1627[] = { 4,10, 0,14,10,14,19,13,10,19,16,13,19};                      // -+-+--+=
static const char vlist1628[] = { 4,10, 9,14,10,14,19,13,10,19,16,13,19};                      // -+-+--++
static const char vlist1629[] = { 2,12, 2,16, 2,19,16};                                        // -+-+-=--
static const char vlist1630[] = { 2, 2,19,16, 0, 2,16};                                        // -+-+-=-=
static const char vlist1631[] = { 2, 2,19, 8,19,16, 8};                                        // -+-+-=-+
static const char vlist1632[] = { 3, 1, 2,19,12, 2, 1,16, 1,19};                               // -+-+-==-
static const char vlist1633[] = { 2, 1, 2,19, 1,19,16};                                        // -+-+-===
static const char vlist1634[] = { 2, 1, 2,19, 1,19,16};                                        // -+-+-==+
static const char vlist1635[] = { 4,10, 2,19,12, 2, 8,13,10,19,16,13,19};                      // -+-+-=+-
static const char vlist1636[] = { 3,10, 2,19,13,10,19,13,19,16};                               // -+-+-=+=
static const char vlist1637[] = { 3,10, 2,19,13,10,19,13,19,16};                               // -+-+-=++
static const char vlist1638[] = { 3,12, 9,11,16,12,11,19,16,11};                               // -+-+-+--
static const char vlist1639[] = { 2,16, 0,11,19,16,11};                                        // -+-+-+-=
static const char vlist1640[] = { 2,11,19,16,11,16, 8};                                        // -+-+-+-+
static const char vlist1641[] = { 3,12, 9, 1,16, 1,19, 1,11,19};                               // -+-+-+=-
static const char vlist1642[] = { 2, 1,11,19, 1,19,16};                                        // -+-+-+==
static const char vlist1643[] = { 2, 1,11,19, 1,19,16};                                        // -+-+-+=+
static const char vlist1644[] = { 4,12, 9, 8,13,10,19,10,11,19,16,13,19};                      // -+-+-++-
static const char vlist1645[] = { 3,10,11,19,13,10,19,13,19,16};                               // -+-+-++=
static const char vlist1646[] = { 3,10,11,19,13,10,19,13,19,16};                               // -+-+-+++
static const char vlist1647[] = { 3, 3,16,12,19,16, 3,14, 3,12};                               // -+-+=---
static const char vlist1648[] = { 3,16, 3,19, 3,16, 0, 0, 3,14};                               // -+-+=--=
static const char vlist1649[] = { 3,16, 3,19, 3,16, 8, 3, 9,14};                               // -+-+=--+
static const char vlist1650[] = { 4,16, 3,19, 3,16, 1, 3, 1,12,14, 3,12};                      // -+-+=-=-
static const char vlist1651[] = { 3, 1, 3,16, 3, 0,14, 3,19,16};                               // -+-+=-==
static const char vlist1652[] = { 3, 1, 3,16, 3, 9,14, 3,19,16};                               // -+-+=-=+
static const char vlist1653[] = { 4,13, 3,19,14, 3, 8,16,13,19,12,14, 8};                      // -+-+=-+-
static const char vlist1654[] = { 3, 3, 0,14,13,19,16, 3,19,13};                               // -+-+=-+=
static const char vlist1655[] = { 3, 3, 9,14,13,19,16, 3,19,13};                               // -+-+=-++
static const char vlist1656[] = { 3,16, 3,19, 3,16,12, 3,12, 2};                               // -+-+==--
static const char vlist1657[] = { 2, 0, 3,16, 3,19,16};                                        // -+-+==-=
static const char vlist1658[] = { 2, 8, 3,16, 3,19,16};                                        // -+-+==-+
static const char vlist1659[] = { 3, 2, 1,12, 1, 3,16, 3,19,16};                               // -+-+===-
static const char vlist1660[] = { 2, 1, 3,16, 3,19,16};                                        // -+-+====
static const char vlist1661[] = { 2, 1, 3,16, 3,19,16};                                        // -+-+===+
static const char vlist1662[] = { 3, 2, 8,12, 3,19,13,13,19,16};                               // -+-+==+-
static const char vlist1663[] = { 2, 3,19,13,13,19,16};                                        // -+-+==+=
static const char vlist1664[] = { 2, 3,19,13,13,19,16};                                        // -+-+==++
static const char vlist1665[] = { 3,16, 3,19, 3,16,12, 3,12, 9};                               // -+-+=+--
static const char vlist1666[] = { 2, 0, 3,16, 3,19,16};                                        // -+-+=+-=
static const char vlist1667[] = { 2, 8, 3,16, 3,19,16};                                        // -+-+=+-+
static const char vlist1668[] = { 3, 9, 1,12, 1, 3,16, 3,19,16};                               // -+-+=+=-
static const char vlist1669[] = { 2, 1, 3,16, 3,19,16};                                        // -+-+=+==
static const char vlist1670[] = { 2, 1, 3,16, 3,19,16};                                        // -+-+=+=+
static const char vlist1671[] = { 3, 9, 8,12, 3,19,13,13,19,16};                               // -+-+=++-
static const char vlist1672[] = { 2, 3,19,13,13,19,16};                                        // -+-+=++=
static const char vlist1673[] = { 2, 3,19,13,13,19,16};                                        // -+-+=+++
static const char vlist1674[] = { 5,10,16,12,15,16,10,19,16,15,10,12,11,12,14,11};             // -+-++---
static const char vlist1675[] = { 4,11, 0,14,16,15,19,15,16,10,10,16, 0};                      // -+-++--=
static const char vlist1676[] = { 4,11, 9,14,16,15,19,15,16,10,10,16, 8};                      // -+-++--+
static const char vlist1677[] = { 4,15,16, 1,16,15,19, 1,12,11,12,14,11};                      // -+-++-=-
static const char vlist1678[] = { 3, 1,15,16,11, 0,14,15,19,16};                               // -+-++-==
static const char vlist1679[] = { 3, 1,15,16,11, 9,14,15,19,16};                               // -+-++-=+
static const char vlist1680[] = { 4,16,15,19,13,15,16,14,11,12,12,11, 8};                      // -+-++-+-
static const char vlist1681[] = { 3,11, 0,14,15,16,13,15,19,16};                               // -+-++-+=
static const char vlist1682[] = { 3,11, 9,14,15,16,13,15,19,16};                               // -+-++-++
static const char vlist1683[] = { 4,16,15,19,15,16,10,10,16,12,10,12, 2};                      // -+-++=--
static const char vlist1684[] = { 3, 0,10,16,10,15,16,15,19,16};                               // -+-++=-=
static const char vlist1685[] = { 3, 8,10,16,10,15,16,15,19,16};                               // -+-++=-+
static const char vlist1686[] = { 3, 2, 1,12, 1,15,16,15,19,16};                               // -+-++==-
static const char vlist1687[] = { 2, 1,15,16,15,19,16};                                        // -+-++===
static const char vlist1688[] = { 2, 1,15,16,15,19,16};                                        // -+-++==+
static const char vlist1689[] = { 3, 2, 8,12,15,16,13,15,19,16};                               // -+-++=+-
static const char vlist1690[] = { 2,15,16,13,15,19,16};                                        // -+-++=+=
static const char vlist1691[] = { 2,15,16,13,15,19,16};                                        // -+-++=++
static const char vlist1692[] = { 4,16,15,19,15,16,10,10,16,12,10,12, 9};                      // -+-+++--
static const char vlist1693[] = { 3, 0,10,16,10,15,16,15,19,16};                               // -+-+++-=
static const char vlist1694[] = { 3, 8,10,16,10,15,16,15,19,16};                               // -+-+++-+
static const char vlist1695[] = { 3, 9, 1,12, 1,15,16,15,19,16};                               // -+-+++=-
static const char vlist1696[] = { 2, 1,15,16,15,19,16};                                        // -+-+++==
static const char vlist1697[] = { 2, 1,15,16,15,19,16};                                        // -+-+++=+
static const char vlist1698[] = { 3, 9, 8,12,15,16,13,15,19,16};                               // -+-++++-
static const char vlist1699[] = { 2,15,16,13,15,19,16};                                        // -+-++++=
static const char vlist1700[] = { 2,15,16,13,15,19,16};                                        // -+-+++++
static const char vlist1701[] = { 2,14,19, 5,14, 5,17};                                        // -+=-----
static const char vlist1702[] = { 3,19, 5,14, 5, 0,14,17, 5, 0};                               // -+=----=
static const char vlist1703[] = { 4,19, 5, 8,17, 5,12, 9,14, 8,14,19, 8};                      // -+=----+
static const char vlist1704[] = { 4, 1, 5,17,19, 5, 1,19, 1,14,14, 1,17};                      // -+=---=-
static const char vlist1705[] = { 4, 5, 1,19, 1,14,19, 1, 0,14, 5, 0,17};                      // -+=---==
static const char vlist1706[] = { 4, 5, 1,19, 1,14,19, 1, 9,14, 5,12,17};                      // -+=---=+
static const char vlist1707[] = { 5, 5,17, 8,19, 5,10,19,10,14,14,10, 8,17,14, 8};             // -+=---+-
static const char vlist1708[] = { 4,10, 0,14,10,14,19, 5,10,19, 5, 0,17};                      // -+=---+=
static const char vlist1709[] = { 4,10, 9,14,10,14,19, 5,10,19,12, 5,17};                      // -+=---++
static const char vlist1710[] = { 2, 2, 5,17,19, 5, 2};                                        // -+=--=--
static const char vlist1711[] = { 3, 2, 5, 0, 5, 2,19, 5, 0,17};                               // -+=--=-=
static const char vlist1712[] = { 3, 2, 5, 8, 5, 2,19, 5,12,17};                               // -+=--=-+
static const char vlist1713[] = { 4, 2,19, 1, 2, 1,17,19, 5, 1, 1, 5,17};                      // -+=--==-
static const char vlist1714[] = { 3, 1, 2,19, 1,19, 5,17, 0, 5};                               // -+=--===
static const char vlist1715[] = { 3, 1, 2,19, 1,19, 5,17,12, 5};                               // -+=--==+
static const char vlist1716[] = { 4,10, 2,19,17, 2, 8, 5,10,19, 5,17, 8};                      // -+=--=+-
static const char vlist1717[] = { 3,17, 0, 5,10,19, 5, 2,19,10};                               // -+=--=+=
static const char vlist1718[] = { 3,17,12, 5,10,19, 5, 2,19,10};                               // -+=--=++
static const char vlist1719[] = { 3, 5,17, 9,11, 5, 9,19, 5,11};                               // -+=--+--
static const char vlist1720[] = { 3, 5, 0,17,11, 5, 0, 5,11,19};                               // -+=--+-=
static const char vlist1721[] = { 3,11, 5, 8, 5,11,19,12, 5,17};                               // -+=--+-+
static const char vlist1722[] = { 4, 5,11,19,11, 5, 1, 1, 5, 9, 5,17, 9};                      // -+=--+=-
static const char vlist1723[] = { 3, 1,11, 5,17, 0, 5,11,19, 5};                               // -+=--+==
static const char vlist1724[] = { 3, 1,11, 5,17,12, 5,11,19, 5};                               // -+=--+=+
static const char vlist1725[] = { 4,10,11,19, 5,10,19, 5,17, 8,17, 9, 8};                      // -+=--++-
static const char vlist1726[] = { 3,17, 0, 5,10,19, 5,11,19,10};                               // -+=--++=
static const char vlist1727[] = { 3,17,12, 5,10,19, 5,11,19,10};                               // -+=--+++
static const char vlist1728[] = { 3, 5,17,14, 3, 5,14, 5,19, 3};                               // -+=-=---
static const char vlist1729[] = { 4, 5, 3, 0, 3, 5,19, 0, 3,14, 5, 0,17};                      // -+=-=--=
static const char vlist1730[] = { 4, 5, 3, 8, 5, 3,19, 9, 3,14, 5,12,17};                      // -+=-=--+
static const char vlist1731[] = { 4, 3, 5,19, 3, 1,14,14, 1,17, 1, 5,17};                      // -+=-=-=-
static const char vlist1732[] = { 3,17, 0, 5, 3, 0,14, 3,19, 5};                               // -+=-=-==
static const char vlist1733[] = { 3, 3, 9,14,17,12, 5, 3,19, 5};                               // -+=-=-=+
static const char vlist1734[] = { 4, 5, 3,19, 3, 8,14, 8, 5,17,14, 8,17};                      // -+=-=-+-
static const char vlist1735[] = { 3,17, 0, 5, 3, 0,14, 3,19, 5};                               // -+=-=-+=
static const char vlist1736[] = { 3, 3, 9,14,17,12, 5, 3,19, 5};                               // -+=-=-++
static const char vlist1737[] = { 3, 3, 5, 2, 3, 5,19, 2, 5,17};                               // -+=-==--
static const char vlist1738[] = { 3, 3,19, 5, 3, 0, 5, 0,17, 5};                               // -+=-==-=
static const char vlist1739[] = { 3, 3,19, 5, 8, 3, 5,17,12, 5};                               // -+=-==-+
static const char vlist1740[] = { 3, 2, 1,17,17, 1, 5, 3,19, 5};                               // -+=-===-
static const char vlist1741[] = { 2,17, 0, 5, 3,19, 5};                                        // -+=-====
static const char vlist1742[] = { 2, 3,19, 5,17,12, 5};                                        // -+=-===+
static const char vlist1743[] = { 3,17, 2, 8,17, 8, 5, 3,19, 5};                               // -+=-==+-
static const char vlist1744[] = { 2,17, 0, 5, 3,19, 5};                                        // -+=-==+=
static const char vlist1745[] = { 2, 3,19, 5,17,12, 5};                                        // -+=-==++
static const char vlist1746[] = { 3, 3, 5, 9, 5, 3,19, 9, 5,17};                               // -+=-=+--
static const char vlist1747[] = { 3, 3,19, 5, 0, 3, 5,17, 0, 5};                               // -+=-=+-=
static const char vlist1748[] = { 3, 3,19, 5, 8, 3, 5,17,12, 5};                               // -+=-=+-+
static const char vlist1749[] = { 3, 9, 1, 5,17, 9, 5, 3,19, 5};                               // -+=-=+=-
static const char vlist1750[] = { 2,17, 0, 5, 3,19, 5};                                        // -+=-=+==
static const char vlist1751[] = { 2, 3,19, 5,17,12, 5};                                        // -+=-=+=+
static const char vlist1752[] = { 3,17, 8, 5,17, 9, 8, 3,19, 5};                               // -+=-=++-
static const char vlist1753[] = { 2,17, 0, 5, 3,19, 5};                                        // -+=-=++=
static const char vlist1754[] = { 2, 3,19, 5,17,12, 5};                                        // -+=-=+++
static const char vlist1755[] = { 4, 5,17,10, 5,19,15,14,11,10,17,14,10};                      // -+=-+---
static const char vlist1756[] = { 4, 5,10, 0, 5, 0,17, 0,11,14, 5,15,19};                      // -+=-+--=
static const char vlist1757[] = { 4,10, 5, 8,11, 9,14,12, 5,17, 5,15,19};                      // -+=-+--+
static const char vlist1758[] = { 4,11, 1,14,14, 1,17, 1, 5,17, 5,15,19};                      // -+=-+-=-
static const char vlist1759[] = { 3,17, 0, 5,11, 0,14,15,19, 5};                               // -+=-+-==
static const char vlist1760[] = { 3,11, 9,14,17,12, 5,15,19, 5};                               // -+=-+-=+
static const char vlist1761[] = { 4, 8, 5,17,14, 8,17,11, 8,14, 5,15,19};                      // -+=-+-+-
static const char vlist1762[] = { 3,17, 0, 5,11, 0,14,15,19, 5};                               // -+=-+-+=
static const char vlist1763[] = { 3,11, 9,14,17,12, 5,15,19, 5};                               // -+=-+-++
static const char vlist1764[] = { 3,10, 5, 2, 2, 5,17, 5,15,19};                               // -+=-+=--
static const char vlist1765[] = { 3,15,19, 5, 0,10, 5,17, 0, 5};                               // -+=-+=-=
static const char vlist1766[] = { 3,15,19, 5,17,12, 5, 8,10, 5};                               // -+=-+=-+
static const char vlist1767[] = { 3, 2, 1,17,17, 1, 5,15,19, 5};                               // -+=-+==-
static const char vlist1768[] = { 2,17, 0, 5,15,19, 5};                                        // -+=-+===
static const char vlist1769[] = { 2,15,19, 5,17,12, 5};                                        // -+=-+==+
static const char vlist1770[] = { 3,17, 2, 8,17, 8, 5,15,19, 5};                               // -+=-+=+-
static const char vlist1771[] = { 2,17, 0, 5,15,19, 5};                                        // -+=-+=+=
static const char vlist1772[] = { 2,15,19, 5,17,12, 5};                                        // -+=-+=++
static const char vlist1773[] = { 3,10, 5, 9, 9, 5,17, 5,15,19};                               // -+=-++--
static const char vlist1774[] = { 3,15,19, 5, 0,10, 5,17, 0, 5};                               // -+=-++-=
static const char vlist1775[] = { 3,15,19, 5,17,12, 5, 8,10, 5};                               // -+=-++-+
static const char vlist1776[] = { 3, 9, 1, 5,17, 9, 5,15,19, 5};                               // -+=-++=-
static const char vlist1777[] = { 2,17, 0, 5,15,19, 5};                                        // -+=-++==
static const char vlist1778[] = { 2,15,19, 5,17,12, 5};                                        // -+=-++=+
static const char vlist1779[] = { 3,17, 8, 5,17, 9, 8,15,19, 5};                               // -+=-+++-
static const char vlist1780[] = { 2,17, 0, 5,15,19, 5};                                        // -+=-+++=
static const char vlist1781[] = { 2,15,19, 5,17,12, 5};                                        // -+=-++++
static const char vlist1782[] = { 2,19, 5,14, 5, 4,14};                                        // -+==----
static const char vlist1783[] = { 2, 5, 0,14, 5,14,19};                                        // -+==---=
static const char vlist1784[] = { 3, 5, 8,19,19, 8,14, 8, 9,14};                               // -+==---+
static const char vlist1785[] = { 3, 1, 4,14, 1,14,19, 5, 1,19};                               // -+==--=-
static const char vlist1786[] = { 3, 5, 1,19, 1,14,19, 1, 0,14};                               // -+==--==
static const char vlist1787[] = { 3, 5, 1,19, 1,14,19, 1, 9,14};                               // -+==--=+
static const char vlist1788[] = { 4, 8, 4,14,10, 8,14,10,14,19, 5,10,19};                      // -+==--+-
static const char vlist1789[] = { 3,10, 0,14,10,14,19, 5,10,19};                               // -+==--+=
static const char vlist1790[] = { 3,10, 9,14,10,14,19, 5,10,19};                               // -+==--++
static const char vlist1791[] = { 2, 5, 4, 2, 5, 2,19};                                        // -+==-=--
static const char vlist1792[] = { 2, 2,19, 5, 0, 2, 5};                                        // -+==-=-=
static const char vlist1793[] = { 2, 2,19, 5, 8, 2, 5};                                        // -+==-=-+
static const char vlist1794[] = { 3, 4, 2, 1, 1, 2,19, 1,19, 5};                               // -+==-==-
static const char vlist1795[] = { 2, 1, 2,19, 1,19, 5};                                        // -+==-===
static const char vlist1796[] = { 2, 1, 2,19, 1,19, 5};                                        // -+==-==+
static const char vlist1797[] = { 3, 4, 2, 8, 2,19,10,10,19, 5};                               // -+==-=+-
static const char vlist1798[] = { 2,10, 2,19, 5,10,19};                                        // -+==-=+=
static const char vlist1799[] = { 2,10, 2,19, 5,10,19};                                        // -+==-=++
static const char vlist1800[] = { 3, 5, 4, 9, 5, 9,11, 5,11,19};                               // -+==-+--
static const char vlist1801[] = { 2, 0,11, 5,11,19, 5};                                        // -+==-+-=
static const char vlist1802[] = { 2, 8,11, 5,11,19, 5};                                        // -+==-+-+
static const char vlist1803[] = { 3, 4, 9, 1, 1,11, 5,11,19, 5};                               // -+==-+=-
static const char vlist1804[] = { 2, 1,11, 5, 5,11,19};                                        // -+==-+==
static const char vlist1805[] = { 2, 1,11, 5, 5,11,19};                                        // -+==-+=+
static const char vlist1806[] = { 3, 4, 9, 8,10,19, 5,11,19,10};                               // -+==-++-
static const char vlist1807[] = { 2,10,11,19, 5,10,19};                                        // -+==-++=
static const char vlist1808[] = { 2,10,11,19, 5,10,19};                                        // -+==-+++
static const char vlist1809[] = { 3, 5, 4, 3, 3, 4,14, 3, 5,19};                               // -+===---
static const char vlist1810[] = { 3,14, 3, 0, 0, 3, 5,19, 3, 5};                               // -+===--=
static const char vlist1811[] = { 3,14, 3, 9, 8, 3, 5, 3,19, 5};                               // -+===--+
static const char vlist1812[] = { 3,14, 3, 1,19, 3, 5, 4,14, 1};                               // -+===-=-
static const char vlist1813[] = { 2, 3, 0,14, 5, 3,19};                                        // -+===-==
static const char vlist1814[] = { 2, 3, 9,14, 5, 3,19};                                        // -+===-=+
static const char vlist1815[] = { 3, 3, 8,14, 3,19, 5, 8, 4,14};                               // -+===-+-
static const char vlist1816[] = { 2, 3, 0,14, 5, 3,19};                                        // -+===-+=
static const char vlist1817[] = { 2, 3, 9,14, 5, 3,19};                                        // -+===-++
static const char vlist1818[] = { 3, 3, 4, 2, 4, 3, 5, 5, 3,19};                               // -+====--
static const char vlist1819[] = { 2, 0, 3, 5, 5, 3,19};                                        // -+====-=
static const char vlist1820[] = { 2, 8, 3, 5, 5, 3,19};                                        // -+====-+
static const char vlist1821[] = { 2, 2, 1, 4, 5, 3,19};                                        // -+=====-
static const char vlist1822[] = { 1, 5, 3,19};                                                 // -+======
static const char vlist1823[] = { 1, 5, 3,19};                                                 // -+=====+
static const char vlist1824[] = { 2, 2, 8, 4, 5, 3,19};                                        // -+====+-
static const char vlist1825[] = { 1, 5, 3,19};                                                 // -+====+=
static const char vlist1826[] = { 1, 5, 3,19};                                                 // -+====++
static const char vlist1827[] = { 3, 9, 3, 5, 5, 3,19, 4, 9, 5};                               // -+===+--
static const char vlist1828[] = { 2, 0, 3, 5, 5, 3,19};                                        // -+===+-=
static const char vlist1829[] = { 2, 8, 3, 5, 5, 3,19};                                        // -+===+-+
static const char vlist1830[] = { 2, 9, 1, 4, 5, 3,19};                                        // -+===+=-
static const char vlist1831[] = { 1, 5, 3,19};                                                 // -+===+==
static const char vlist1832[] = { 1, 5, 3,19};                                                 // -+===+=+
static const char vlist1833[] = { 2, 9, 8, 4, 5, 3,19};                                        // -+===++-
static const char vlist1834[] = { 1, 5, 3,19};                                                 // -+===++=
static const char vlist1835[] = { 1, 5, 3,19};                                                 // -+===+++
static const char vlist1836[] = { 4, 5, 4,10,11, 4,14,10, 4,11, 5,15,19};                      // -+==+---
static const char vlist1837[] = { 3,15,19, 5,14,11, 0, 0,10, 5};                               // -+==+--=
static const char vlist1838[] = { 3,15,19, 5,14,11, 9, 8,10, 5};                               // -+==+--+
static const char vlist1839[] = { 3,15,19, 5,11, 4,14, 4,11, 1};                               // -+==+-=-
static const char vlist1840[] = { 2,11, 0,14, 5,15,19};                                        // -+==+-==
static const char vlist1841[] = { 2,11, 9,14, 5,15,19};                                        // -+==+-=+
static const char vlist1842[] = { 3,11, 8,14, 8, 4,14,15,19, 5};                               // -+==+-+-
static const char vlist1843[] = { 2,11, 0,14, 5,15,19};                                        // -+==+-+=
static const char vlist1844[] = { 2,11, 9,14, 5,15,19};                                        // -+==+-++
static const char vlist1845[] = { 3, 5,15,19, 4,10, 5,10, 4, 2};                               // -+==+=--
static const char vlist1846[] = { 2, 0,10, 5, 5,15,19};                                        // -+==+=-=
static const char vlist1847[] = { 2, 8,10, 5, 5,15,19};                                        // -+==+=-+
static const char vlist1848[] = { 2, 2, 1, 4, 5,15,19};                                        // -+==+==-
static const char vlist1849[] = { 1, 5,15,19};                                                 // -+==+===
static const char vlist1850[] = { 1, 5,15,19};                                                 // -+==+==+
static const char vlist1851[] = { 2, 2, 8, 4, 5,15,19};                                        // -+==+=+-
static const char vlist1852[] = { 1, 5,15,19};                                                 // -+==+=+=
static const char vlist1853[] = { 1, 5,15,19};                                                 // -+==+=++
static const char vlist1854[] = { 3, 9,10, 5, 4, 9, 5, 5,15,19};                               // -+==++--
static const char vlist1855[] = { 2, 0,10, 5, 5,15,19};                                        // -+==++-=
static const char vlist1856[] = { 2, 8,10, 5, 5,15,19};                                        // -+==++-+
static const char vlist1857[] = { 2, 9, 1, 4, 5,15,19};                                        // -+==++=-
static const char vlist1858[] = { 1, 5,15,19};                                                 // -+==++==
static const char vlist1859[] = { 1, 5,15,19};                                                 // -+==++=+
static const char vlist1860[] = { 2, 9, 8, 4, 5,15,19};                                        // -+==+++-
static const char vlist1861[] = { 1, 5,15,19};                                                 // -+==+++=
static const char vlist1862[] = { 1, 5,15,19};                                                 // -+==++++
static const char vlist1863[] = { 2,19, 5,14,14, 5,12};                                        // -+=+----
static const char vlist1864[] = { 2, 5, 0,14, 5,14,19};                                        // -+=+---=
static const char vlist1865[] = { 3, 5, 8,19,19, 8,14, 8, 9,14};                               // -+=+---+
static const char vlist1866[] = { 3, 1,12,14, 1,14,19, 5, 1,19};                               // -+=+--=-
static const char vlist1867[] = { 3, 5, 1,19, 1,14,19, 1, 0,14};                               // -+=+--==
static const char vlist1868[] = { 3, 5, 1,19, 1,14,19, 1, 9,14};                               // -+=+--=+
static const char vlist1869[] = { 4,10, 8,14, 8,12,14,10,14,19, 5,10,19};                      // -+=+--+-
static const char vlist1870[] = { 3,10, 0,14,10,14,19, 5,10,19};                               // -+=+--+=
static const char vlist1871[] = { 3,10, 9,14,10,14,19, 5,10,19};                               // -+=+--++
static const char vlist1872[] = { 2, 5,12, 2, 5, 2,19};                                        // -+=+-=--
static const char vlist1873[] = { 2, 2,19, 5, 0, 2, 5};                                        // -+=+-=-=
static const char vlist1874[] = { 2, 2,19, 5, 8, 2, 5};                                        // -+=+-=-+
static const char vlist1875[] = { 3, 1, 2,19,12, 2, 1, 1,19, 5};                               // -+=+-==-
static const char vlist1876[] = { 2, 1, 2,19, 1,19, 5};                                        // -+=+-===
static const char vlist1877[] = { 2, 1, 2,19, 1,19, 5};                                        // -+=+-==+
static const char vlist1878[] = { 3, 2,19,10,12, 2, 8,10,19, 5};                               // -+=+-=+-
static const char vlist1879[] = { 2,10, 2,19, 5,10,19};                                        // -+=+-=+=
static const char vlist1880[] = { 2,10, 2,19, 5,10,19};                                        // -+=+-=++
static const char vlist1881[] = { 3, 5,12, 9, 5, 9,11, 5,11,19};                               // -+=+-+--
static const char vlist1882[] = { 2, 0,11, 5,11,19, 5};                                        // -+=+-+-=
static const char vlist1883[] = { 2, 8,11, 5,11,19, 5};                                        // -+=+-+-+
static const char vlist1884[] = { 3,12, 9, 1, 1,11, 5,11,19, 5};                               // -+=+-+=-
static const char vlist1885[] = { 2, 1,11, 5, 5,11,19};                                        // -+=+-+==
static const char vlist1886[] = { 2, 1,11, 5, 5,11,19};                                        // -+=+-+=+
static const char vlist1887[] = { 3,12, 9, 8,10,19, 5,11,19,10};                               // -+=+-++-
static const char vlist1888[] = { 2,10,11,19, 5,10,19};                                        // -+=+-++=
static const char vlist1889[] = { 2,10,11,19, 5,10,19};                                        // -+=+-+++
static const char vlist1890[] = { 3, 3,12,14, 5,12, 3, 5, 3,19};                               // -+=+=---
static const char vlist1891[] = { 3,14, 3, 0, 0, 3, 5, 3,19, 5};                               // -+=+=--=
static const char vlist1892[] = { 3,14, 3, 9, 8, 3, 5, 3,19, 5};                               // -+=+=--+
static const char vlist1893[] = { 3, 3,12,14,12, 3, 1, 3,19, 5};                               // -+=+=-=-
static const char vlist1894[] = { 2, 3, 0,14, 5, 3,19};                                        // -+=+=-==
static const char vlist1895[] = { 2, 3, 9,14, 5, 3,19};                                        // -+=+=-=+
static const char vlist1896[] = { 3, 3, 8,14, 3,19, 5, 8,12,14};                               // -+=+=-+-
static const char vlist1897[] = { 2, 3, 0,14, 5, 3,19};                                        // -+=+=-+=
static const char vlist1898[] = { 2, 3, 9,14, 5, 3,19};                                        // -+=+=-++
static const char vlist1899[] = { 3,12, 3, 5, 3,12, 2, 5, 3,19};                               // -+=+==--
static const char vlist1900[] = { 2, 0, 3, 5, 5, 3,19};                                        // -+=+==-=
static const char vlist1901[] = { 2, 8, 3, 5, 5, 3,19};                                        // -+=+==-+
static const char vlist1902[] = { 2, 2, 1,12, 5, 3,19};                                        // -+=+===-
static const char vlist1903[] = { 1, 5, 3,19};                                                 // -+=+====
static const char vlist1904[] = { 1, 5, 3,19};                                                 // -+=+===+
static const char vlist1905[] = { 2, 2, 8,12, 5, 3,19};                                        // -+=+==+-
static const char vlist1906[] = { 1, 5, 3,19};                                                 // -+=+==+=
static const char vlist1907[] = { 1, 5, 3,19};                                                 // -+=+==++
static const char vlist1908[] = { 3, 9, 3, 5, 5, 3,19,12, 9, 5};                               // -+=+=+--
static const char vlist1909[] = { 2, 0, 3, 5, 5, 3,19};                                        // -+=+=+-=
static const char vlist1910[] = { 2, 8, 3, 5, 5, 3,19};                                        // -+=+=+-+
static const char vlist1911[] = { 2, 9, 1,12, 5, 3,19};                                        // -+=+=+=-
static const char vlist1912[] = { 1, 5, 3,19};                                                 // -+=+=+==
static const char vlist1913[] = { 1, 5, 3,19};                                                 // -+=+=+=+
static const char vlist1914[] = { 2, 9, 8,12, 5, 3,19};                                        // -+=+=++-
static const char vlist1915[] = { 1, 5, 3,19};                                                 // -+=+=++=
static const char vlist1916[] = { 1, 5, 3,19};                                                 // -+=+=+++
static const char vlist1917[] = { 4,11,12,14,10,12,11, 5,12,10, 5,15,19};                      // -+=++---
static const char vlist1918[] = { 3,15,19, 5,14,11, 0, 0,10, 5};                               // -+=++--=
static const char vlist1919[] = { 3,15,19, 5,14,11, 9, 8,10, 5};                               // -+=++--+
static const char vlist1920[] = { 3,15,19, 5,12,11, 1,11,12,14};                               // -+=++-=-
static const char vlist1921[] = { 2,11, 0,14, 5,15,19};                                        // -+=++-==
static const char vlist1922[] = { 2,11, 9,14, 5,15,19};                                        // -+=++-=+
static const char vlist1923[] = { 3,11,12,14,11, 8,12,15,19, 5};                               // -+=++-+-
static const char vlist1924[] = { 2,11, 0,14, 5,15,19};                                        // -+=++-+=
static const char vlist1925[] = { 2,11, 9,14, 5,15,19};                                        // -+=++-++
static const char vlist1926[] = { 3, 5,15,19,10,12, 2,12,10, 5};                               // -+=++=--
static const char vlist1927[] = { 2, 0,10, 5, 5,15,19};                                        // -+=++=-=
static const char vlist1928[] = { 2, 8,10, 5, 5,15,19};                                        // -+=++=-+
static const char vlist1929[] = { 2, 2, 1,12, 5,15,19};                                        // -+=++==-
static const char vlist1930[] = { 1, 5,15,19};                                                 // -+=++===
static const char vlist1931[] = { 1, 5,15,19};                                                 // -+=++==+
static const char vlist1932[] = { 2, 2, 8,12, 5,15,19};                                        // -+=++=+-
static const char vlist1933[] = { 1, 5,15,19};                                                 // -+=++=+=
static const char vlist1934[] = { 1, 5,15,19};                                                 // -+=++=++
static const char vlist1935[] = { 3,12,10, 5, 9,10,12, 5,15,19};                               // -+=+++--
static const char vlist1936[] = { 2, 0,10, 5, 5,15,19};                                        // -+=+++-=
static const char vlist1937[] = { 2, 8,10, 5, 5,15,19};                                        // -+=+++-+
static const char vlist1938[] = { 2, 9, 1,12, 5,15,19};                                        // -+=+++=-
static const char vlist1939[] = { 1, 5,15,19};                                                 // -+=+++==
static const char vlist1940[] = { 1, 5,15,19};                                                 // -+=+++=+
static const char vlist1941[] = { 2, 9, 8,12, 5,15,19};                                        // -+=++++-
static const char vlist1942[] = { 1, 5,15,19};                                                 // -+=++++=
static const char vlist1943[] = { 1, 5,15,19};                                                 // -+=+++++
static const char vlist1944[] = { 4,19,18,14,18,13,14,14,13,16,17,14,16};                      // -++-----
static const char vlist1945[] = { 4,17,16, 0, 0,14,13,14,18,13,14,19,18};                      // -++----=
static const char vlist1946[] = { 5,17,16,12, 9,13, 8, 9,14,13,14,19,18,14,18,13};             // -++----+
static const char vlist1947[] = { 4,18, 1,19,19, 1,14,14, 1,17,17, 1,16};                      // -++---=-
static const char vlist1948[] = { 4, 1,19,18, 1,14,19, 1, 0,14,16, 0,17};                      // -++---==
static const char vlist1949[] = { 4, 1,19,18, 1,14,19, 1, 9,14,12,16,17};                      // -++---=+
static const char vlist1950[] = { 5,19,18,10,19,10,14,14,10, 8,17,14, 8,16,17, 8};             // -++---+-
static const char vlist1951[] = { 4,10, 0,14,10,14,19,18,10,19, 0,16,17};                      // -++---+=
static const char vlist1952[] = { 4,10, 9,14,10,14,19,18,10,19,12,16,17};                      // -++---++
static const char vlist1953[] = { 4, 2,18,13,19,18, 2,17, 2,16, 2,13,16};                      // -++--=--
static const char vlist1954[] = { 4,18, 2,19, 2,18,13, 2,13, 0,16, 0,17};                      // -++--=-=
static const char vlist1955[] = { 4,18, 2,19, 2,18,13, 2,13, 8,12,16,17};                      // -++--=-+
static const char vlist1956[] = { 4,18, 2,19, 2,18, 1, 2, 1,16,17, 2,16};                      // -++--==-
static const char vlist1957[] = { 3, 1, 2,18,17, 0,16, 2,19,18};                               // -++--===
static const char vlist1958[] = { 3, 1, 2,18,17,12,16, 2,19,18};                               // -++--==+
static const char vlist1959[] = { 4,10, 2,19,17, 2, 8,18,10,19,16,17, 8};                      // -++--=+-
static const char vlist1960[] = { 3,17, 0,16,10,19,18, 2,19,10};                               // -++--=+=
static const char vlist1961[] = { 3,17,12,16,10,19,18, 2,19,10};                               // -++--=++
static const char vlist1962[] = { 5,11,18,13,19,18,11,11,13, 9,13,16, 9,16,17, 9};             // -++--+--
static const char vlist1963[] = { 4, 0,16,17,18,11,19,11,18,13,11,13, 0};                      // -++--+-=
static const char vlist1964[] = { 4,18,11,19,11,18,13,11,13, 8,12,16,17};                      // -++--+-+
static const char vlist1965[] = { 4,11,18, 1,18,11,19, 1,16, 9,16,17, 9};                      // -++--+=-
static const char vlist1966[] = { 3, 1,11,18,17, 0,16,11,19,18};                               // -++--+==
static const char vlist1967[] = { 3, 1,11,18,17,12,16,11,19,18};                               // -++--+=+
static const char vlist1968[] = { 4,18,11,19,10,11,18,17, 9,16,16, 9, 8};                      // -++--++-
static const char vlist1969[] = { 3,17, 0,16,11,18,10,11,19,18};                               // -++--++=
static const char vlist1970[] = { 3,17,12,16,11,18,10,11,19,18};                               // -++--+++
static const char vlist1971[] = { 4,18,19, 3,14, 3,13,16,14,13,17,14,16};                      // -++-=---
static const char vlist1972[] = { 4,18, 3,19, 0, 3,14,13, 3, 0,16, 0,17};                      // -++-=--=
static const char vlist1973[] = { 4,18, 3,19, 3, 9,14, 3,13, 8,12,16,17};                      // -++-=--+
static const char vlist1974[] = { 4,18, 3,19, 3, 1,14,14, 1,17,17, 1,16};                      // -++-=-=-
static const char vlist1975[] = { 3,17, 0,16, 3, 0,14, 3,19,18};                               // -++-=-==
static const char vlist1976[] = { 3, 3, 9,14,17,12,16, 3,19,18};                               // -++-=-=+
static const char vlist1977[] = { 4,18, 3,19, 3, 8,14, 8,16,17,14, 8,17};                      // -++-=-+-
static const char vlist1978[] = { 3,17, 0,16, 3, 0,14, 3,19,18};                               // -++-=-+=
static const char vlist1979[] = { 3, 3, 9,14,17,12,16, 3,19,18};                               // -++-=-++
static const char vlist1980[] = { 4,18, 3,19, 3,13, 2,13,16, 2, 2,16,17};                      // -++-==--
static const char vlist1981[] = { 3, 3,19,18, 0, 3,13,17, 0,16};                               // -++-==-=
static const char vlist1982[] = { 3, 3,19,18, 8, 3,13,17,12,16};                               // -++-==-+
static const char vlist1983[] = { 3,17, 2,16, 2, 1,16, 3,19,18};                               // -++-===-
static const char vlist1984[] = { 2,17, 0,16, 3,19,18};                                        // -++-====
static const char vlist1985[] = { 2, 3,19,18,17,12,16};                                        // -++-===+
static const char vlist1986[] = { 3,17, 2, 8,17, 8,16, 3,19,18};                               // -++-==+-
static const char vlist1987[] = { 2,17, 0,16, 3,19,18};                                        // -++-==+=
static const char vlist1988[] = { 2, 3,19,18,17,12,16};                                        // -++-==++
static const char vlist1989[] = { 4,18, 3,19, 3,13, 9,13,16, 9, 9,16,17};                      // -++-=+--
static const char vlist1990[] = { 3, 3,19,18, 0, 3,13,17, 0,16};                               // -++-=+-=
static const char vlist1991[] = { 3, 3,19,18, 8, 3,13,17,12,16};                               // -++-=+-+
static const char vlist1992[] = { 3, 9, 1,16,17, 9,16, 3,19,18};                               // -++-=+=-
static const char vlist1993[] = { 2,17, 0,16, 3,19,18};                                        // -++-=+==
static const char vlist1994[] = { 2, 3,19,18,17,12,16};                                        // -++-=+=+
static const char vlist1995[] = { 3,16, 9, 8,17, 9,16, 3,19,18};                               // -++-=++-
static const char vlist1996[] = { 2,17, 0,16, 3,19,18};                                        // -++-=++=
static const char vlist1997[] = { 2, 3,19,18,17,12,16};                                        // -++-=+++
static const char vlist1998[] = { 5,19,18,15,13,11,10,14,11,13,17,14,16,16,14,13};             // -++-+---
static const char vlist1999[] = { 4,10,13, 0, 0,16,17,11, 0,14,18,15,19};                      // -++-+--=
static const char vlist2000[] = { 4,10,13, 8,11, 9,14,12,16,17,18,15,19};                      // -++-+--+
static const char vlist2001[] = { 4,11, 1,14,14, 1,17,17, 1,16,18,15,19};                      // -++-+-=-
static const char vlist2002[] = { 3,17, 0,16,11, 0,14,15,19,18};                               // -++-+-==
static const char vlist2003[] = { 3,11, 9,14,17,12,16,15,19,18};                               // -++-+-=+
static const char vlist2004[] = { 4, 8,16,17,14, 8,17,11, 8,14,18,15,19};                      // -++-+-+-
static const char vlist2005[] = { 3,17, 0,16,11, 0,14,15,19,18};                               // -++-+-+=
static const char vlist2006[] = { 3,11, 9,14,17,12,16,15,19,18};                               // -++-+-++
static const char vlist2007[] = { 4,10,13, 2,13,16, 2, 2,16,17,18,15,19};                      // -++-+=--
static const char vlist2008[] = { 3,15,19,18, 0,10,13,17, 0,16};                               // -++-+=-=
static const char vlist2009[] = { 3,15,19,18,17,12,16, 8,10,13};                               // -++-+=-+
static const char vlist2010[] = { 3,17, 2,16, 2, 1,16,15,19,18};                               // -++-+==-
static const char vlist2011[] = { 2,17, 0,16,15,19,18};                                        // -++-+===
static const char vlist2012[] = { 2,15,19,18,17,12,16};                                        // -++-+==+
static const char vlist2013[] = { 3,17, 2, 8,17, 8,16,15,19,18};                               // -++-+=+-
static const char vlist2014[] = { 2,17, 0,16,15,19,18};                                        // -++-+=+=
static const char vlist2015[] = { 2,15,19,18,17,12,16};                                        // -++-+=++
static const char vlist2016[] = { 4,10,13, 9,13,16, 9, 9,16,17,18,15,19};                      // -++-++--
static const char vlist2017[] = { 3,15,19,18, 0,10,13,17, 0,16};                               // -++-++-=
static const char vlist2018[] = { 3,15,19,18,17,12,16, 8,10,13};                               // -++-++-+
static const char vlist2019[] = { 3, 9, 1,16,17, 9,16,15,19,18};                               // -++-++=-
static const char vlist2020[] = { 2,17, 0,16,15,19,18};                                        // -++-++==
static const char vlist2021[] = { 2,15,19,18,17,12,16};                                        // -++-++=+
static const char vlist2022[] = { 3,16, 9, 8,17, 9,16,15,19,18};                               // -++-+++-
static const char vlist2023[] = { 2,17, 0,16,15,19,18};                                        // -++-+++=
static const char vlist2024[] = { 2,15,19,18,17,12,16};                                        // -++-++++
static const char vlist2025[] = { 3, 4,14,13,14,19,18,14,18,13};                               // -++=----
static const char vlist2026[] = { 3,13, 0,14,13,14,18,18,14,19};                               // -++=---=
static const char vlist2027[] = { 4, 8, 9,13,13, 9,14,18,14,19,13,14,18};                      // -++=---+
static const char vlist2028[] = { 3, 1, 4,14, 1,14,19, 1,19,18};                               // -++=--=-
static const char vlist2029[] = { 3, 1,19,18, 1,14,19, 1, 0,14};                               // -++=--==
static const char vlist2030[] = { 3, 1,19,18, 1,14,19, 1, 9,14};                               // -++=--=+
static const char vlist2031[] = { 4, 8, 4,14,10, 8,14,10,14,19,18,10,19};                      // -++=--+-
static const char vlist2032[] = { 3,10, 0,14,10,14,19,18,10,19};                               // -++=--+=
static const char vlist2033[] = { 3,10, 9,14,10,14,19,18,10,19};                               // -++=--++
static const char vlist2034[] = { 3,13, 4, 2,18,13, 2,18, 2,19};                               // -++=-=--
static const char vlist2035[] = { 3, 2,19,18,13, 2,18, 0, 2,13};                               // -++=-=-=
static const char vlist2036[] = { 3, 2,19,18,13, 2,18, 8, 2,13};                               // -++=-=-+
static const char vlist2037[] = { 3, 4, 2, 1, 2,19,18, 1, 2,18};                               // -++=-==-
static const char vlist2038[] = { 2, 1, 2,18,18, 2,19};                                        // -++=-===
static const char vlist2039[] = { 2, 1, 2,18,18, 2,19};                                        // -++=-==+
static const char vlist2040[] = { 3, 4, 2, 8, 2,19,10,10,19,18};                               // -++=-=+-
static const char vlist2041[] = { 2,10, 2,19,18,10,19};                                        // -++=-=+=
static const char vlist2042[] = { 2,10, 2,19,18,10,19};                                        // -++=-=++
static const char vlist2043[] = { 4,13, 4, 9,13, 9,11,18,11,19,18,13,11};                      // -++=-+--
static const char vlist2044[] = { 3, 0,11,13,13,11,18,11,19,18};                               // -++=-+-=
static const char vlist2045[] = { 3, 8,11,13,13,11,18,11,19,18};                               // -++=-+-+
static const char vlist2046[] = { 3, 4, 9, 1, 1,11,18,11,19,18};                               // -++=-+=-
static const char vlist2047[] = { 2, 1,11,18,18,11,19};                                        // -++=-+==
static const char vlist2048[] = { 2, 1,11,18,18,11,19};                                        // -++=-+=+
static const char vlist2049[] = { 3, 4, 9, 8,11,18,10,11,19,18};                               // -++=-++-
static const char vlist2050[] = { 2,10,11,18,18,11,19};                                        // -++=-++=
static const char vlist2051[] = { 2,10,11,18,18,11,19};                                        // -++=-+++
static const char vlist2052[] = { 3, 3, 4,14,13, 4, 3,18, 3,19};                               // -++==---
static const char vlist2053[] = { 3,14, 3, 0, 0, 3,13, 3,19,18};                               // -++==--=
static const char vlist2054[] = { 3,14, 3, 9, 8, 3,13, 3,19,18};                               // -++==--+
static const char vlist2055[] = { 3,14, 3, 1, 3,19,18, 4,14, 1};                               // -++==-=-
static const char vlist2056[] = { 2, 3, 0,14,18, 3,19};                                        // -++==-==
static const char vlist2057[] = { 2, 3, 9,14,18, 3,19};                                        // -++==-=+
static const char vlist2058[] = { 3, 3, 8,14, 3,19,18, 8, 4,14};                               // -++==-+-
static const char vlist2059[] = { 2, 3, 0,14,18, 3,19};                                        // -++==-+=
static const char vlist2060[] = { 2, 3, 9,14,18, 3,19};                                        // -++==-++
static const char vlist2061[] = { 3, 3,13, 2,18, 3,19,13, 4, 2};                               // -++===--
static const char vlist2062[] = { 2, 0, 3,13,18, 3,19};                                        // -++===-=
static const char vlist2063[] = { 2, 8, 3,13,18, 3,19};                                        // -++===-+
static const char vlist2064[] = { 2, 2, 1, 4,18, 3,19};                                        // -++====-
static const char vlist2065[] = { 1,18, 3,19};                                                 // -++=====
static const char vlist2066[] = { 1,18, 3,19};                                                 // -++====+
static const char vlist2067[] = { 2, 2, 8, 4,18, 3,19};                                        // -++===+-
static const char vlist2068[] = { 1,18, 3,19};                                                 // -++===+=
static const char vlist2069[] = { 1,18, 3,19};                                                 // -++===++
static const char vlist2070[] = { 3, 9, 3,13,18, 3,19, 4, 9,13};                               // -++==+--
static const char vlist2071[] = { 2, 0, 3,13,18, 3,19};                                        // -++==+-=
static const char vlist2072[] = { 2, 8, 3,13,18, 3,19};                                        // -++==+-+
static const char vlist2073[] = { 2, 9, 1, 4,18, 3,19};                                        // -++==+=-
static const char vlist2074[] = { 1,18, 3,19};                                                 // -++==+==
static const char vlist2075[] = { 1,18, 3,19};                                                 // -++==+=+
static const char vlist2076[] = { 2, 9, 8, 4,18, 3,19};                                        // -++==++-
static const char vlist2077[] = { 1,18, 3,19};                                                 // -++==++=
static const char vlist2078[] = { 1,18, 3,19};                                                 // -++==+++
static const char vlist2079[] = { 4,11, 4,14,10, 4,11,13, 4,10,18,15,19};                      // -++=+---
static const char vlist2080[] = { 3,15,19,18,14,11, 0, 0,10,13};                               // -++=+--=
static const char vlist2081[] = { 3,15,19,18,14,11, 9, 8,10,13};                               // -++=+--+
static const char vlist2082[] = { 3,15,19,18,11, 4,14, 4,11, 1};                               // -++=+-=-
static const char vlist2083[] = { 2,11, 0,14,18,15,19};                                        // -++=+-==
static const char vlist2084[] = { 2,11, 9,14,18,15,19};                                        // -++=+-=+
static const char vlist2085[] = { 3,11, 8,14, 8, 4,14,15,19,18};                               // -++=+-+-
static const char vlist2086[] = { 2,11, 0,14,18,15,19};                                        // -++=+-+=
static const char vlist2087[] = { 2,11, 9,14,18,15,19};                                        // -++=+-++
static const char vlist2088[] = { 3,18,15,19, 4,10,13,10, 4, 2};                               // -++=+=--
static const char vlist2089[] = { 2, 0,10,13,18,15,19};                                        // -++=+=-=
static const char vlist2090[] = { 2, 8,10,13,18,15,19};                                        // -++=+=-+
static const char vlist2091[] = { 2, 2, 1, 4,18,15,19};                                        // -++=+==-
static const char vlist2092[] = { 1,18,15,19};                                                 // -++=+===
static const char vlist2093[] = { 1,18,15,19};                                                 // -++=+==+
static const char vlist2094[] = { 2, 2, 8, 4,18,15,19};                                        // -++=+=+-
static const char vlist2095[] = { 1,18,15,19};                                                 // -++=+=+=
static const char vlist2096[] = { 1,18,15,19};                                                 // -++=+=++
static const char vlist2097[] = { 3, 9,10,13, 4, 9,13,18,15,19};                               // -++=++--
static const char vlist2098[] = { 2, 0,10,13,18,15,19};                                        // -++=++-=
static const char vlist2099[] = { 2, 8,10,13,18,15,19};                                        // -++=++-+
static const char vlist2100[] = { 2, 9, 1, 4,18,15,19};                                        // -++=++=-
static const char vlist2101[] = { 1,18,15,19};                                                 // -++=++==
static const char vlist2102[] = { 1,18,15,19};                                                 // -++=++=+
static const char vlist2103[] = { 2, 9, 8, 4,18,15,19};                                        // -++=+++-
static const char vlist2104[] = { 1,18,15,19};                                                 // -++=+++=
static const char vlist2105[] = { 1,18,15,19};                                                 // -++=++++
static const char vlist2106[] = { 3,19,18,14,18,13,14,14,13,12};                               // -+++----
static const char vlist2107[] = { 3,13, 0,14,13,14,18,18,14,19};                               // -+++---=
static const char vlist2108[] = { 4, 8, 9,13,13, 9,14,18,14,19,13,14,18};                      // -+++---+
static const char vlist2109[] = { 3, 1,12,14, 1,14,19, 1,19,18};                               // -+++--=-
static const char vlist2110[] = { 3, 1,19,18, 1,14,19, 1, 0,14};                               // -+++--==
static const char vlist2111[] = { 3, 1,19,18, 1,14,19, 1, 9,14};                               // -+++--=+
static const char vlist2112[] = { 4,10, 8,14, 8,12,14,10,14,19,18,10,19};                      // -+++--+-
static const char vlist2113[] = { 3,10, 0,14,10,14,19,18,10,19};                               // -+++--+=
static const char vlist2114[] = { 3,10, 9,14,10,14,19,18,10,19};                               // -+++--++
static const char vlist2115[] = { 3,13,12, 2,18,13, 2,18, 2,19};                               // -+++-=--
static const char vlist2116[] = { 3, 2,19,18,13, 2,18, 0, 2,13};                               // -+++-=-=
static const char vlist2117[] = { 3, 2,19,18,13, 2,18, 8, 2,13};                               // -+++-=-+
static const char vlist2118[] = { 3, 2,19,18, 1, 2,18,12, 2, 1};                               // -+++-==-
static const char vlist2119[] = { 2, 1, 2,18,18, 2,19};                                        // -+++-===
static const char vlist2120[] = { 2, 1, 2,18,18, 2,19};                                        // -+++-==+
static const char vlist2121[] = { 3, 2,19,10,12, 2, 8,10,19,18};                               // -+++-=+-
static const char vlist2122[] = { 2,10, 2,19,18,10,19};                                        // -+++-=+=
static const char vlist2123[] = { 2,10, 2,19,18,10,19};                                        // -+++-=++
static const char vlist2124[] = { 4,13,12, 9,18,13,11,13, 9,11,18,11,19};                      // -+++-+--
static const char vlist2125[] = { 3, 0,11,13,13,11,18,11,19,18};                               // -+++-+-=
static const char vlist2126[] = { 3, 8,11,13,13,11,18,11,19,18};                               // -+++-+-+
static const char vlist2127[] = { 3,12, 9, 1, 1,11,18,11,19,18};                               // -+++-+=-
static const char vlist2128[] = { 2, 1,11,18,18,11,19};                                        // -+++-+==
static const char vlist2129[] = { 2, 1,11,18,18,11,19};                                        // -+++-+=+
static const char vlist2130[] = { 3,12, 9, 8,11,18,10,11,19,18};                               // -+++-++-
static const char vlist2131[] = { 2,10,11,18,18,11,19};                                        // -+++-++=
static const char vlist2132[] = { 2,10,11,18,18,11,19};                                        // -+++-+++
static const char vlist2133[] = { 3, 3,12,14,13,12, 3,18, 3,19};                               // -+++=---
static const char vlist2134[] = { 3,14, 3, 0, 0, 3,13, 3,19,18};                               // -+++=--=
static const char vlist2135[] = { 3,14, 3, 9, 8, 3,13, 3,19,18};                               // -+++=--+
static const char vlist2136[] = { 3, 3,12,14,12, 3, 1, 3,19,18};                               // -+++=-=-
static const char vlist2137[] = { 2, 3, 0,14,18, 3,19};                                        // -+++=-==
static const char vlist2138[] = { 2, 3, 9,14,18, 3,19};                                        // -+++=-=+
static const char vlist2139[] = { 3, 3, 8,14, 3,19,18, 8,12,14};                               // -+++=-+-
static const char vlist2140[] = { 2, 3, 0,14,18, 3,19};                                        // -+++=-+=
static const char vlist2141[] = { 2, 3, 9,14,18, 3,19};                                        // -+++=-++
static const char vlist2142[] = { 3,12, 3,13, 3,12, 2,18, 3,19};                               // -+++==--
static const char vlist2143[] = { 2, 0, 3,13,18, 3,19};                                        // -+++==-=
static const char vlist2144[] = { 2, 8, 3,13,18, 3,19};                                        // -+++==-+
static const char vlist2145[] = { 2, 2, 1,12,18, 3,19};                                        // -+++===-
static const char vlist2146[] = { 1,18, 3,19};                                                 // -+++====
static const char vlist2147[] = { 1,18, 3,19};                                                 // -+++===+
static const char vlist2148[] = { 2, 2, 8,12,18, 3,19};                                        // -+++==+-
static const char vlist2149[] = { 1,18, 3,19};                                                 // -+++==+=
static const char vlist2150[] = { 1,18, 3,19};                                                 // -+++==++
static const char vlist2151[] = { 3, 9, 3,13,18, 3,19,12, 9,13};                               // -+++=+--
static const char vlist2152[] = { 2, 0, 3,13,18, 3,19};                                        // -+++=+-=
static const char vlist2153[] = { 2, 8, 3,13,18, 3,19};                                        // -+++=+-+
static const char vlist2154[] = { 2, 9, 1,12,18, 3,19};                                        // -+++=+=-
static const char vlist2155[] = { 1,18, 3,19};                                                 // -+++=+==
static const char vlist2156[] = { 1,18, 3,19};                                                 // -+++=+=+
static const char vlist2157[] = { 2, 9, 8,12,18, 3,19};                                        // -+++=++-
static const char vlist2158[] = { 1,18, 3,19};                                                 // -+++=++=
static const char vlist2159[] = { 1,18, 3,19};                                                 // -+++=+++
static const char vlist2160[] = { 4,11,12,14,10,12,11,13,12,10,18,15,19};                      // -++++---
static const char vlist2161[] = { 3,15,19,18,14,11, 0, 0,10,13};                               // -++++--=
static const char vlist2162[] = { 3,15,19,18,14,11, 9, 8,10,13};                               // -++++--+
static const char vlist2163[] = { 3,15,19,18,12,11, 1,11,12,14};                               // -++++-=-
static const char vlist2164[] = { 2,11, 0,14,18,15,19};                                        // -++++-==
static const char vlist2165[] = { 2,11, 9,14,18,15,19};                                        // -++++-=+
static const char vlist2166[] = { 3,11,12,14,11, 8,12,15,19,18};                               // -++++-+-
static const char vlist2167[] = { 2,11, 0,14,18,15,19};                                        // -++++-+=
static const char vlist2168[] = { 2,11, 9,14,18,15,19};                                        // -++++-++
static const char vlist2169[] = { 3,18,15,19,10,12, 2,12,10,13};                               // -++++=--
static const char vlist2170[] = { 2, 0,10,13,18,15,19};                                        // -++++=-=
static const char vlist2171[] = { 2, 8,10,13,18,15,19};                                        // -++++=-+
static const char vlist2172[] = { 2, 2, 1,12,18,15,19};                                        // -++++==-
static const char vlist2173[] = { 1,18,15,19};                                                 // -++++===
static const char vlist2174[] = { 1,18,15,19};                                                 // -++++==+
static const char vlist2175[] = { 2, 2, 8,12,18,15,19};                                        // -++++=+-
static const char vlist2176[] = { 1,18,15,19};                                                 // -++++=+=
static const char vlist2177[] = { 1,18,15,19};                                                 // -++++=++
static const char vlist2178[] = { 3,12,10,13, 9,10,12,18,15,19};                               // -+++++--
static const char vlist2179[] = { 2, 0,10,13,18,15,19};                                        // -+++++-=
static const char vlist2180[] = { 2, 8,10,13,18,15,19};                                        // -+++++-+
static const char vlist2181[] = { 2, 9, 1,12,18,15,19};                                        // -+++++=-
static const char vlist2182[] = { 1,18,15,19};                                                 // -+++++==
static const char vlist2183[] = { 1,18,15,19};                                                 // -+++++=+
static const char vlist2184[] = { 2, 9, 8,12,18,15,19};                                        // -++++++-
static const char vlist2185[] = { 1,18,15,19};                                                 // -++++++=
static const char vlist2186[] = { 1,18,15,19};                                                 // -+++++++
                                                                                               // =-------
                                                                                               // =------=
static const char vlist2189[] = { 3, 8, 7,12, 7, 8, 9,12, 7, 9};                               // =------+
                                                                                               // =-----=-
                                                                                               // =-----==
static const char vlist2192[] = { 3, 7, 1,12, 7, 1, 9,12, 7, 9};                               // =-----=+
static const char vlist2193[] = { 2, 7, 8,13, 8, 7,10};                                        // =-----+-
static const char vlist2194[] = { 2,13, 7, 0, 0, 7,10};                                        // =-----+=
static const char vlist2195[] = { 3,13, 7,12,12, 7, 9, 7,10, 9};                               // =-----++
                                                                                               // =----=--
                                                                                               // =----=-=
static const char vlist2198[] = { 3, 7, 2,12, 7, 2, 8, 7,12, 8};                               // =----=-+
                                                                                               // =----==-
static const char vlist2200[] = { 3, 2, 1, 7, 0, 1, 7, 2, 0, 7};                               // =----===
static const char vlist2201[] = { 3, 2, 1, 7,12, 1, 7, 2,12, 7};                               // =----==+
static const char vlist2202[] = { 3, 7, 2,10, 7, 2, 8,13, 7, 8};                               // =----=+-
static const char vlist2203[] = { 3, 0,13, 7, 2, 0, 7, 2,10, 7};                               // =----=+=
static const char vlist2204[] = { 3, 2,12, 7,12,13, 7, 2,10, 7};                               // =----=++
static const char vlist2205[] = { 2, 9, 7,14, 7, 9,11};                                        // =----+--
static const char vlist2206[] = { 2, 7,14, 0, 7, 0,11};                                        // =----+-=
static const char vlist2207[] = { 3, 7,14,12, 7,12, 8,11, 7, 8};                               // =----+-+
static const char vlist2208[] = { 3, 7,11, 1, 1, 7, 9, 7,14, 9};                               // =----+=-
static const char vlist2209[] = { 3,11, 1, 7, 0, 1, 7,14, 0, 7};                               // =----+==
static const char vlist2210[] = { 3,11, 1, 7,12, 1, 7,14,12, 7};                               // =----+=+
static const char vlist2211[] = { 4, 7,11,10,13, 9, 8,14, 9,13, 7,14,13};                      // =----++-
static const char vlist2212[] = { 3, 0,13, 7,14, 0, 7,11,10, 7};                               // =----++=
static const char vlist2213[] = { 3,14,12, 7,12,13, 7,10,11, 7};                               // =----+++
                                                                                               // =---=---
                                                                                               // =---=--=
static const char vlist2216[] = { 4, 7, 3, 9, 7, 3, 8, 7, 9,12, 7,12, 8};                      // =---=--+
static const char vlist2217[] = { 1, 7, 3, 1};                                                 // =---=-=-
static const char vlist2218[] = { 2, 0, 1, 7, 3, 0, 7};                                        // =---=-==
static const char vlist2219[] = { 3,12, 1, 7, 3, 9, 7, 9,12, 7};                               // =---=-=+
static const char vlist2220[] = { 2, 7, 3, 8,13, 7, 8};                                        // =---=-+-
static const char vlist2221[] = { 2, 0,13, 7, 3, 0, 7};                                        // =---=-+=
static const char vlist2222[] = { 3, 3, 9, 7, 9,12, 7,12,13, 7};                               // =---=-++
static const char vlist2223[] = { 1, 3, 7, 2};                                                 // =---==--
static const char vlist2224[] = { 2, 2, 0, 7, 0, 3, 7};                                        // =---==-=
static const char vlist2225[] = { 3, 2,12, 7, 8, 3, 7,12, 8, 7};                               // =---==-+
static const char vlist2226[] = { 1, 2, 1, 7};                                                 // =---===-
static const char vlist2227[] = { 2, 0, 1, 7, 2, 0, 7};                                        // =---====
static const char vlist2228[] = { 2,12, 1, 7, 2,12, 7};                                        // =---===+
static const char vlist2229[] = { 2, 2, 8, 7, 8,13, 7};                                        // =---==+-
static const char vlist2230[] = { 2, 0,13, 7, 2, 0, 7};                                        // =---==+=
static const char vlist2231[] = { 2, 2,12, 7,12,13, 7};                                        // =---==++
static const char vlist2232[] = { 2, 3, 7, 9, 7,14, 9};                                        // =---=+--
static const char vlist2233[] = { 2,14, 0, 7, 0, 3, 7};                                        // =---=+-=
static const char vlist2234[] = { 3, 8, 3, 7,12, 8, 7,14,12, 7};                               // =---=+-+
static const char vlist2235[] = { 2, 9, 1, 7,14, 9, 7};                                        // =---=+=-
static const char vlist2236[] = { 2, 0, 1, 7,14, 0, 7};                                        // =---=+==
static const char vlist2237[] = { 2,12, 1, 7,14,12, 7};                                        // =---=+=+
static const char vlist2238[] = { 3, 9, 8,13, 9,13,14,14,13, 7};                               // =---=++-
static const char vlist2239[] = { 2, 0,13, 7,14, 0, 7};                                        // =---=++=
static const char vlist2240[] = { 2,14,12, 7,12,13, 7};                                        // =---=+++
static const char vlist2241[] = { 1,10, 7,11};                                                 // =---+---
static const char vlist2242[] = { 2, 7,11, 0, 7, 0,10};                                        // =---+--=
static const char vlist2243[] = { 4, 7,11, 9, 7, 9,12, 7,12, 8,10, 7, 8};                      // =---+--+
static const char vlist2244[] = { 1, 1, 7,11};                                                 // =---+-=-
static const char vlist2245[] = { 2, 0, 1, 7,11, 0, 7};                                        // =---+-==
static const char vlist2246[] = { 3,12, 1, 7,11, 9, 7, 9,12, 7};                               // =---+-=+
static const char vlist2247[] = { 2,13, 7, 8, 7,11, 8};                                        // =---+-+-
static const char vlist2248[] = { 2, 0,13, 7,11, 0, 7};                                        // =---+-+=
static const char vlist2249[] = { 3,11, 9, 7, 9,12, 7,12,13, 7};                               // =---+-++
static const char vlist2250[] = { 1, 7, 2,10};                                                 // =---+=--
static const char vlist2251[] = { 2, 2, 0, 7, 0,10, 7};                                        // =---+=-=
static const char vlist2252[] = { 3, 2,12, 7, 8,10, 7,12, 8, 7};                               // =---+=-+
static const char vlist2253[] = { 1, 2, 1, 7};                                                 // =---+==-
static const char vlist2254[] = { 2, 0, 1, 7, 2, 0, 7};                                        // =---+===
static const char vlist2255[] = { 2,12, 1, 7, 2,12, 7};                                        // =---+==+
static const char vlist2256[] = { 2, 2, 8, 7, 8,13, 7};                                        // =---+=+-
static const char vlist2257[] = { 2, 0,13, 7, 2, 0, 7};                                        // =---+=+=
static const char vlist2258[] = { 2, 2,12, 7,12,13, 7};                                        // =---+=++
static const char vlist2259[] = { 2, 7,14, 9,10, 7, 9};                                        // =---++--
static const char vlist2260[] = { 2,14, 0, 7, 0,10, 7};                                        // =---++-=
static const char vlist2261[] = { 3, 8,10, 7,12, 8, 7,14,12, 7};                               // =---++-+
static const char vlist2262[] = { 2, 9, 1, 7,14, 9, 7};                                        // =---++=-
static const char vlist2263[] = { 2, 0, 1, 7,14, 0, 7};                                        // =---++==
static const char vlist2264[] = { 2,12, 1, 7,14,12, 7};                                        // =---++=+
static const char vlist2265[] = { 3, 9, 8,13, 9,13,14,14,13, 7};                               // =---+++-
static const char vlist2266[] = { 2, 0,13, 7,14, 0, 7};                                        // =---+++=
static const char vlist2267[] = { 2,14,12, 7,12,13, 7};                                        // =---++++
                                                                                               // =--=----
                                                                                               // =--=---=
static const char vlist2270[] = { 3, 7, 4, 9, 7, 4, 8, 9, 7, 8};                               // =--=---+
                                                                                               // =--=--=-
static const char vlist2272[] = { 3, 1, 4, 7, 1, 0, 7, 0, 4, 7};                               // =--=--==
static const char vlist2273[] = { 3, 1, 4, 7, 1, 9, 7, 9, 4, 7};                               // =--=--=+
static const char vlist2274[] = { 3, 4, 7,13, 4, 7, 8, 7,10, 8};                               // =--=--+-
static const char vlist2275[] = { 3,10, 0, 7, 0, 4, 7,13, 4, 7};                               // =--=--+=
static const char vlist2276[] = { 3, 9, 4, 7,10, 9, 7, 4,13, 7};                               // =--=--++
                                                                                               // =--=-=--
static const char vlist2278[] = { 3, 4, 2, 7, 0, 2, 7, 4, 0, 7};                               // =--=-=-=
static const char vlist2279[] = { 3, 4, 2, 7, 8, 2, 7, 4, 8, 7};                               // =--=-=-+
static const char vlist2280[] = { 4, 2, 4, 1, 4, 2, 7, 2, 1, 7, 1, 4, 7};                      // =--=-==-
static const char vlist2281[] = { 3, 2, 1, 7, 7, 1, 4, 2, 7, 4};                               // =--=-===
static const char vlist2282[] = { 3, 2, 1, 7, 7, 1, 4, 2, 7, 4};                               // =--=-==+
static const char vlist2283[] = { 4, 2, 4, 8, 4, 2, 7, 2,10, 7,13, 4, 7};                      // =--=-=+-
static const char vlist2284[] = { 3, 2, 7, 4, 2,10, 7, 7,13, 4};                               // =--=-=+=
static const char vlist2285[] = { 3, 7, 2, 4,10, 2, 7,13, 7, 4};                               // =--=-=++
static const char vlist2286[] = { 3, 7, 4,14, 7, 4, 9,11, 7, 9};                               // =--=-+--
static const char vlist2287[] = { 3, 0,11, 7, 4, 0, 7, 4,14, 7};                               // =--=-+-=
static const char vlist2288[] = { 3, 4, 8, 7, 8,11, 7, 4,14, 7};                               // =--=-+-+
static const char vlist2289[] = { 4, 9, 4, 1, 1, 4, 7,11, 1, 7, 4,14, 7};                      // =--=-+=-
static const char vlist2290[] = { 3,11, 1, 7, 7, 1, 4,14, 7, 4};                               // =--=-+==
static const char vlist2291[] = { 3, 1,11, 7, 1, 7, 4, 7,14, 4};                               // =--=-+=+
static const char vlist2292[] = { 4, 4, 9, 8,10,11, 7, 4,13, 7, 4,14, 7};                      // =--=-++-
static const char vlist2293[] = { 3,10,11, 7, 7,14, 4,13, 7, 4};                               // =--=-++=
static const char vlist2294[] = { 3,10,11, 7, 7,14, 4,13, 7, 4};                               // =--=-+++
                                                                                               // =--==---
                                                                                               // =--==--=
static const char vlist2297[] = { 4, 3, 9, 7, 3, 7, 8, 7, 4, 8, 9, 4, 7};                      // =--==--+
static const char vlist2298[] = { 3, 4, 3, 1, 3, 4, 7, 1, 4, 7};                               // =--==-=-
static const char vlist2299[] = { 3, 7, 1, 4, 3, 0, 4, 7, 3, 4};                               // =--==-==
static const char vlist2300[] = { 3, 7, 1, 4, 7, 3, 9, 7, 9, 4};                               // =--==-=+
static const char vlist2301[] = { 3, 4, 3, 8, 3, 4, 7,13, 4, 7};                               // =--==-+-
static const char vlist2302[] = { 3, 3, 0, 4, 7, 3, 4, 7,13, 4};                               // =--==-+=
static const char vlist2303[] = { 3, 7, 3, 9, 7, 9, 4,13, 7, 4};                               // =--==-++
static const char vlist2304[] = { 3, 3, 4, 2, 4, 3, 7, 4, 2, 7};                               // =--===--
static const char vlist2305[] = { 3, 2, 7, 4, 0, 3, 4, 3, 7, 4};                               // =--===-=
static const char vlist2306[] = { 3, 7, 2, 4, 3, 7, 8, 8, 7, 4};                               // =--===-+
static const char vlist2307[] = { 3, 2, 7, 4, 1, 2, 4, 7, 1, 4};                               // =--====-
static const char vlist2308[] = { 2, 7, 1, 4, 2, 7, 4};                                        // =--=====
static const char vlist2309[] = { 2, 1, 7, 4, 7, 2, 4};                                        // =--====+
static const char vlist2310[] = { 3, 7, 2, 4, 8, 2, 4, 7,13, 4};                               // =--===+-
static const char vlist2311[] = { 2, 7, 2, 4,13, 7, 4};                                        // =--===+=
static const char vlist2312[] = { 2, 7, 2, 4,13, 7, 4};                                        // =--===++
static const char vlist2313[] = { 3, 3, 4, 9, 4, 3, 7, 4,14, 7};                               // =--==+--
static const char vlist2314[] = { 3, 0, 3, 4, 3, 7, 4, 7,14, 4};                               // =--==+-=
static const char vlist2315[] = { 3, 3, 7, 8, 8, 7, 4, 7,14, 4};                               // =--==+-+
static const char vlist2316[] = { 3, 1, 9, 4, 7, 1, 4, 7,14, 4};                               // =--==+=-
static const char vlist2317[] = { 2, 1, 7, 4, 7,14, 4};                                        // =--==+==
static const char vlist2318[] = { 2, 1, 7, 4, 7,14, 4};                                        // =--==+=+
static const char vlist2319[] = { 3, 9, 8, 4,13, 7, 4, 7,14, 4};                               // =--==++-
static const char vlist2320[] = { 2, 7,14, 4,13, 7, 4};                                        // =--==++=
static const char vlist2321[] = { 2, 7,14, 4,13, 7, 4};                                        // =--==+++
static const char vlist2322[] = { 3, 7, 4,11, 7, 4,10, 4,11,10};                               // =--=+---
static const char vlist2323[] = { 4, 7, 4,10,11, 4, 7, 4,11, 0, 4, 0,10};                      // =--=+--=
static const char vlist2324[] = { 4,11, 9, 7, 9, 4, 7, 7, 4, 8,10, 7, 8};                      // =--=+--+
static const char vlist2325[] = { 3, 1, 4, 7, 4,11, 1,11, 4, 7};                               // =--=+-=-
static const char vlist2326[] = { 3, 7, 1, 4,11, 0, 4, 7,11, 4};                               // =--=+-==
static const char vlist2327[] = { 3, 1, 7, 4, 7,11, 9, 7, 9, 4};                               // =--=+-=+
static const char vlist2328[] = { 3, 4,11, 8,11, 4, 7, 4,13, 7};                               // =--=+-+-
static const char vlist2329[] = { 3,11, 0, 4, 7,11, 4,13, 7, 4};                               // =--=+-+=
static const char vlist2330[] = { 3, 7,11, 9, 7, 9, 4,13, 7, 4};                               // =--=+-++
static const char vlist2331[] = { 3, 4, 2, 7,10, 4, 2, 4,10, 7};                               // =--=+=--
static const char vlist2332[] = { 3, 7, 2, 4, 0,10, 4,10, 7, 4};                               // =--=+=-=
static const char vlist2333[] = { 3, 7, 2, 4,10, 7, 8, 8, 7, 4};                               // =--=+=-+
static const char vlist2334[] = { 3, 7, 2, 4, 1, 2, 4, 7, 1, 4};                               // =--=+==-
static const char vlist2335[] = { 2, 1, 7, 4, 7, 2, 4};                                        // =--=+===
static const char vlist2336[] = { 2, 1, 7, 4, 7, 2, 4};                                        // =--=+==+
static const char vlist2337[] = { 3, 7, 2, 4, 2, 8, 4,13, 7, 4};                               // =--=+=+-
static const char vlist2338[] = { 2, 7, 2, 4,13, 7, 4};                                        // =--=+=+=
static const char vlist2339[] = { 2, 7, 2, 4,13, 7, 4};                                        // =--=+=++
static const char vlist2340[] = { 3,10, 4, 9, 4,10, 7, 4,14, 7};                               // =--=++--
static const char vlist2341[] = { 3, 0,10, 4,10, 7, 4, 7,14, 4};                               // =--=++-=
static const char vlist2342[] = { 3,10, 7, 8, 8, 7, 4, 7,14, 4};                               // =--=++-+
static const char vlist2343[] = { 3, 9, 1, 4, 1, 7, 4, 7,14, 4};                               // =--=++=-
static const char vlist2344[] = { 2, 1, 7, 4, 7,14, 4};                                        // =--=++==
static const char vlist2345[] = { 2, 1, 7, 4, 7,14, 4};                                        // =--=++=+
static const char vlist2346[] = { 3, 9, 8, 4,13, 7, 4, 7,14, 4};                               // =--=+++-
static const char vlist2347[] = { 2, 7,14, 4,13, 7, 4};                                        // =--=+++=
static const char vlist2348[] = { 2, 7,14, 4,13, 7, 4};                                        // =--=++++
static const char vlist2349[] = { 2, 7,12,17,12, 7,16};                                        // =--+----
static const char vlist2350[] = { 2,17, 7, 0, 0, 7,16};                                        // =--+---=
static const char vlist2351[] = { 3,17, 7, 9, 9, 7, 8, 7,16, 8};                               // =--+---+
static const char vlist2352[] = { 3,16, 7, 1, 7, 1,12,17, 7,12};                               // =--+--=-
static const char vlist2353[] = { 3, 1,16, 7, 1, 0, 7, 0,17, 7};                               // =--+--==
static const char vlist2354[] = { 3,16, 1, 7, 1, 9, 7, 9,17, 7};                               // =--+--=+
static const char vlist2355[] = { 4,16, 7,13,12,10, 8,17,10,12, 7,10,17};                      // =--+--+-
static const char vlist2356[] = { 3,10, 0, 7, 0,17, 7,16,13, 7};                               // =--+--+=
static const char vlist2357[] = { 3, 9,17, 7,10, 9, 7,16,13, 7};                               // =--+--++
static const char vlist2358[] = { 3, 7,17, 2, 2, 7,12, 7,16,12};                               // =--+-=--
static const char vlist2359[] = { 3,17, 2, 7, 0, 2, 7,16, 0, 7};                               // =--+-=-=
static const char vlist2360[] = { 3,17, 2, 7, 8, 2, 7,16, 8, 7};                               // =--+-=-+
static const char vlist2361[] = { 4,17, 2, 7, 2, 1, 7, 2,12, 1, 1,16, 7};                      // =--+-==-
static const char vlist2362[] = { 3, 2, 1, 7, 7, 1,16, 2, 7,17};                               // =--+-===
static const char vlist2363[] = { 3, 1, 2, 7, 1, 7,16, 7, 2,17};                               // =--+-==+
static const char vlist2364[] = { 4,17, 2, 7,10, 2, 7,12, 2, 8,16,13, 7};                      // =--+-=+-
static const char vlist2365[] = { 3, 7, 2,17,10, 2, 7,13, 7,16};                               // =--+-=+=
static const char vlist2366[] = { 3, 7, 2,17,10, 2, 7,13, 7,16};                               // =--+-=++
static const char vlist2367[] = { 4, 7,17,14,11,12, 9,11,16,12,11, 7,16};                      // =--+-+--
static const char vlist2368[] = { 3, 0,11, 7,16, 0, 7,17,14, 7};                               // =--+-+-=
static const char vlist2369[] = { 3,16, 8, 7, 8,11, 7,14,17, 7};                               // =--+-+-+
static const char vlist2370[] = { 4,12, 9, 1,16, 1, 7, 1,11, 7,17,14, 7};                      // =--+-+=-
static const char vlist2371[] = { 3, 1,11, 7, 1, 7,16, 7,14,17};                               // =--+-+==
static const char vlist2372[] = { 3, 1,11, 7, 1, 7,16, 7,14,17};                               // =--+-+=+
static const char vlist2373[] = { 4,12, 9, 8,10,11, 7,16,13, 7,14,17, 7};                      // =--+-++-
static const char vlist2374[] = { 3,10,11, 7, 7,14,17,13, 7,16};                               // =--+-++=
static const char vlist2375[] = { 3,10,11, 7, 7,14,17,13, 7,16};                               // =--+-+++
static const char vlist2376[] = { 4,17, 3,12, 7,17, 3, 7, 3,16,12, 3,16};                      // =--+=---
static const char vlist2377[] = { 4, 3,17, 7,17, 3, 0, 0, 3,16, 3, 7,16};                      // =--+=--=
static const char vlist2378[] = { 4, 3, 9, 7, 3, 7, 8, 7,16, 8, 9,17, 7};                      // =--+=--+
static const char vlist2379[] = { 4, 3,17, 7,17, 3,12,12, 3, 1, 1,16, 7};                      // =--+=-=-
static const char vlist2380[] = { 3, 7, 1,16, 3, 0,17, 7, 3,17};                               // =--+=-==
static const char vlist2381[] = { 3, 1, 7,16, 7, 3, 9, 7, 9,17};                               // =--+=-=+
static const char vlist2382[] = { 4, 3,17, 7,17, 3,12,12, 3, 8,16,13, 7};                      // =--+=-+-
static const char vlist2383[] = { 3, 3, 0,17, 7, 3,17,13, 7,16};                               // =--+=-+=
static const char vlist2384[] = { 3, 7, 3, 9, 7, 9,17,13, 7,16};                               // =--+=-++
static const char vlist2385[] = { 4,16, 3, 7, 3,16,12, 3,12, 2,17, 2, 7};                      // =--+==--
static const char vlist2386[] = { 3, 7, 2,17, 0, 3,16, 3, 7,16};                               // =--+==-=
static const char vlist2387[] = { 3, 7, 2,17, 3, 7, 8, 8, 7,16};                               // =--+==-+
static const char vlist2388[] = { 3, 7, 2,17, 1, 2,12, 7, 1,16};                               // =--+===-
static const char vlist2389[] = { 2, 1, 7,16, 7, 2,17};                                        // =--+====
static const char vlist2390[] = { 2, 1, 7,16, 7, 2,17};                                        // =--+===+
static const char vlist2391[] = { 3, 7, 2,17, 2, 8,12,13, 7,16};                               // =--+==+-
static const char vlist2392[] = { 2, 7, 2,17,13, 7,16};                                        // =--+==+=
static const char vlist2393[] = { 2, 7, 2,17,13, 7,16};                                        // =--+==++
static const char vlist2394[] = { 4,16, 3, 7, 3,16,12, 3,12, 9,17,14, 7};                      // =--+=+--
static const char vlist2395[] = { 3, 0, 3,16, 3, 7,16, 7,14,17};                               // =--+=+-=
static const char vlist2396[] = { 3, 3, 7, 8, 8, 7,16, 7,14,17};                               // =--+=+-+
static const char vlist2397[] = { 3, 9, 1,12, 1, 7,16, 7,14,17};                               // =--+=+=-
static const char vlist2398[] = { 2, 1, 7,16, 7,14,17};                                        // =--+=+==
static const char vlist2399[] = { 2, 1, 7,16, 7,14,17};                                        // =--+=+=+
static const char vlist2400[] = { 3, 9, 8,12,13, 7,16, 7,14,17};                               // =--+=++-
static const char vlist2401[] = { 2, 7,14,17,13, 7,16};                                        // =--+=++=
static const char vlist2402[] = { 2, 7,14,17,13, 7,16};                                        // =--+=+++
static const char vlist2403[] = { 5,17,11,12,17, 7,11,12,11,10,16,12,10, 7,16,10};             // =--++---
static const char vlist2404[] = { 4, 7,16,10,17,11, 0,11,17, 7,16, 0,10};                      // =--++--=
static const char vlist2405[] = { 4,11,17, 7,11, 9,17,10, 7,16,10,16, 8};                      // =--++--+
static const char vlist2406[] = { 4,16, 1, 7,11,17, 7,17,11,12,12,11, 1};                      // =--++-=-
static const char vlist2407[] = { 3, 1, 7,16,11, 0,17, 7,11,17};                               // =--++-==
static const char vlist2408[] = { 3, 1, 7,16, 7,11,17,17,11, 9};                               // =--++-=+
static const char vlist2409[] = { 4,11,17, 7,17,11,12,12,11, 8,16,13, 7};                      // =--++-+-
static const char vlist2410[] = { 3,11, 0,17, 7,11,17,13, 7,16};                               // =--++-+=
static const char vlist2411[] = { 3, 7,11,17,13, 7,16,17,11, 9};                               // =--++-++
static const char vlist2412[] = { 4,17, 2, 7,16,10, 7,10,16,12,10,12, 2};                      // =--++=--
static const char vlist2413[] = { 3, 7, 2,17, 0,10,16,10, 7,16};                               // =--++=-=
static const char vlist2414[] = { 3, 7, 2,17,10, 7,16,10,16, 8};                               // =--++=-+
static const char vlist2415[] = { 3, 7, 2,17, 2, 1,12, 1, 7,16};                               // =--++==-
static const char vlist2416[] = { 2, 1, 7,16, 7, 2,17};                                        // =--++===
static const char vlist2417[] = { 2, 1, 7,16, 7, 2,17};                                        // =--++==+
static const char vlist2418[] = { 3, 7, 2,17, 2, 8,12,13, 7,16};                               // =--++=+-
static const char vlist2419[] = { 2, 7, 2,17,13, 7,16};                                        // =--++=+=
static const char vlist2420[] = { 2, 7, 2,17,13, 7,16};                                        // =--++=++
static const char vlist2421[] = { 4,16,10, 7,10,16,12,10,12, 9,14,17, 7};                      // =--+++--
static const char vlist2422[] = { 3, 0,10,16,10, 7,16, 7,14,17};                               // =--+++-=
static const char vlist2423[] = { 3,10, 7,16, 7,14,17,10,16, 8};                               // =--+++-+
static const char vlist2424[] = { 3, 9, 1,12, 1, 7,16, 7,14,17};                               // =--+++=-
static const char vlist2425[] = { 2, 1, 7,16, 7,14,17};                                        // =--+++==
static const char vlist2426[] = { 2, 1, 7,16, 7,14,17};                                        // =--+++=+
static const char vlist2427[] = { 3, 9, 8,12,13, 7,16, 7,14,17};                               // =--++++-
static const char vlist2428[] = { 2, 7,14,17,13, 7,16};                                        // =--++++=
static const char vlist2429[] = { 2, 7,14,17,13, 7,16};                                        // =--+++++
                                                                                               // =-=-----
                                                                                               // =-=----=
static const char vlist2432[] = { 4, 7, 5,12, 7, 5, 8, 9, 7, 8,12, 7, 9};                      // =-=----+
static const char vlist2433[] = { 1, 5, 7, 1};                                                 // =-=---=-
static const char vlist2434[] = { 2, 1, 0, 7, 0, 5, 7};                                        // =-=---==
static const char vlist2435[] = { 3, 1, 9, 7, 9,12, 7,12, 5, 7};                               // =-=---=+
static const char vlist2436[] = { 2, 5, 7, 8, 7,10, 8};                                        // =-=---+-
static const char vlist2437[] = { 2,10, 0, 7, 0, 5, 7};                                        // =-=---+=
static const char vlist2438[] = { 3, 9,12, 7,10, 9, 7,12, 5, 7};                               // =-=---++
                                                                                               // =-=--=--
                                                                                               // =-=--=-=
static const char vlist2441[] = { 4, 2,12, 7, 2, 7, 8, 7, 5, 8,12, 5, 7};                      // =-=--=-+
static const char vlist2442[] = { 3, 5, 2, 1, 2, 5, 7, 2, 1, 7};                               // =-=--==-
static const char vlist2443[] = { 3, 2, 1, 7, 2, 0, 5, 7, 2, 5};                               // =-=--===
static const char vlist2444[] = { 3, 2, 1, 7, 7, 2,12, 7,12, 5};                               // =-=--==+
static const char vlist2445[] = { 3, 5, 2, 8, 2, 5, 7, 2,10, 7};                               // =-=--=+-
static const char vlist2446[] = { 3, 2, 0, 5, 7, 2, 5, 2,10, 7};                               // =-=--=+=
static const char vlist2447[] = { 3, 7, 2,12,10, 2, 7, 7,12, 5};                               // =-=--=++
static const char vlist2448[] = { 4, 7, 5,14, 7, 5,11, 5, 9,11, 5,14, 9};                      // =-=--+--
static const char vlist2449[] = { 4, 7, 5,11,14, 5, 7, 5,14, 0, 5, 0,11};                      // =-=--+-=
static const char vlist2450[] = { 4,14,12, 7,12, 5, 7, 7, 5, 8,11, 7, 8};                      // =-=--+-+
static const char vlist2451[] = { 4,11, 1, 7, 5, 9, 1, 5,14, 9,14, 5, 7};                      // =-=--+=-
static const char vlist2452[] = { 3,11, 1, 7,14, 0, 5, 7,14, 5};                               // =-=--+==
static const char vlist2453[] = { 3, 1,11, 7, 7,14,12, 7,12, 5};                               // =-=--+=+
static const char vlist2454[] = { 4,10,11, 7, 5, 9, 8, 5,14, 9,14, 5, 7};                      // =-=--++-
static const char vlist2455[] = { 3,14, 0, 5,10,11, 7, 7,14, 5};                               // =-=--++=
static const char vlist2456[] = { 3,10,11, 7, 7,14,12, 7,12, 5};                               // =-=--+++
static const char vlist2457[] = { 1, 5, 7, 3};                                                 // =-=-=---
static const char vlist2458[] = { 3, 5, 3, 0, 3, 0, 7, 0, 5, 7};                               // =-=-=--=
static const char vlist2459[] = { 4, 5, 3, 8, 3, 9, 7, 9,12, 7,12, 5, 7};                      // =-=-=--+
static const char vlist2460[] = { 2, 5, 3, 1, 3, 5, 7};                                        // =-=-=-=-
static const char vlist2461[] = { 2, 7, 0, 5, 3, 0, 7};                                        // =-=-=-==
static const char vlist2462[] = { 3, 3, 9, 7, 9,12, 7, 7,12, 5};                               // =-=-=-=+
static const char vlist2463[] = { 2, 7, 3, 8, 5, 7, 8};                                        // =-=-=-+-
static const char vlist2464[] = { 2, 7, 0, 5, 3, 0, 7};                                        // =-=-=-+=
static const char vlist2465[] = { 3, 3, 9, 7, 9,12, 7, 7,12, 5};                               // =-=-=-++
static const char vlist2466[] = { 2, 3, 5, 2, 2, 5, 7};                                        // =-=-==--
static const char vlist2467[] = { 3, 7, 2, 5, 2, 0, 5, 3, 0, 5};                               // =-=-==-=
static const char vlist2468[] = { 3, 7, 2,12, 7,12, 5, 3, 8, 5};                               // =-=-==-+
static const char vlist2469[] = { 2, 7, 2, 5, 2, 1, 5};                                        // =-=-===-
static const char vlist2470[] = { 2, 2, 0, 5, 7, 2, 5};                                        // =-=-====
static const char vlist2471[] = { 2, 7, 2,12, 7,12, 5};                                        // =-=-===+
static const char vlist2472[] = { 2, 7, 2, 5, 2, 8, 5};                                        // =-=-==+-
static const char vlist2473[] = { 2, 2, 0, 5, 7, 2, 5};                                        // =-=-==+=
static const char vlist2474[] = { 2, 7, 2,12, 7,12, 5};                                        // =-=-==++
static const char vlist2475[] = { 3, 3, 5, 9, 9, 5,14,14, 5, 7};                               // =-=-=+--
static const char vlist2476[] = { 3,14, 0, 5, 7,14, 5, 3, 0, 5};                               // =-=-=+-=
static const char vlist2477[] = { 3, 7,12, 5, 7,14,12, 8, 3, 5};                               // =-=-=+-+
static const char vlist2478[] = { 3, 9, 1, 5,14, 9, 5, 7,14, 5};                               // =-=-=+=-
static const char vlist2479[] = { 2,14, 0, 5, 7,14, 5};                                        // =-=-=+==
static const char vlist2480[] = { 2, 7,12, 5, 7,14,12};                                        // =-=-=+=+
static const char vlist2481[] = { 3, 9, 8, 5,14, 9, 5, 7,14, 5};                               // =-=-=++-
static const char vlist2482[] = { 2,14, 0, 5, 7,14, 5};                                        // =-=-=++=
static const char vlist2483[] = { 2, 7,12, 5, 7,14,12};                                        // =-=-=+++
static const char vlist2484[] = { 2, 5, 7,11,10, 5,11};                                        // =-=-+---
static const char vlist2485[] = { 3, 5,10, 0, 0, 5, 7,11, 0, 7};                               // =-=-+--=
static const char vlist2486[] = { 4, 5,10, 8, 9,12, 7,11, 9, 7,12, 5, 7};                      // =-=-+--+
static const char vlist2487[] = { 2, 1, 5,11, 5, 7,11};                                        // =-=-+-=-
static const char vlist2488[] = { 2, 7, 0, 5,11, 0, 7};                                        // =-=-+-==
static const char vlist2489[] = { 3,11, 9, 7, 9,12, 7, 7,12, 5};                               // =-=-+-=+
static const char vlist2490[] = { 2, 5, 7, 8, 7,11, 8};                                        // =-=-+-+-
static const char vlist2491[] = { 2, 7, 0, 5,11, 0, 7};                                        // =-=-+-+=
static const char vlist2492[] = { 3,11, 9, 7, 9,12, 7, 7,12, 5};                               // =-=-+-++
static const char vlist2493[] = { 2,10, 5, 2, 2, 5, 7};                                        // =-=-+=--
static const char vlist2494[] = { 3, 7, 2, 5, 2, 0, 5,10, 0, 5};                               // =-=-+=-=
static const char vlist2495[] = { 3, 7, 2,12, 7,12, 5, 8,10, 5};                               // =-=-+=-+
static const char vlist2496[] = { 2, 7, 2, 5, 2, 1, 5};                                        // =-=-+==-
static const char vlist2497[] = { 2, 2, 0, 5, 7, 2, 5};                                        // =-=-+===
static const char vlist2498[] = { 2, 7, 2,12, 7,12, 5};                                        // =-=-+==+
static const char vlist2499[] = { 2, 7, 2, 5, 2, 8, 5};                                        // =-=-+=+-
static const char vlist2500[] = { 2, 2, 0, 5, 7, 2, 5};                                        // =-=-+=+=
static const char vlist2501[] = { 2, 7, 2,12, 7,12, 5};                                        // =-=-+=++
static const char vlist2502[] = { 3,10, 5, 9, 9, 5,14,14, 5, 7};                               // =-=-++--
static const char vlist2503[] = { 3,14, 0, 5, 7,14, 5, 0,10, 5};                               // =-=-++-=
static const char vlist2504[] = { 3, 7,12, 5, 7,14,12, 8,10, 5};                               // =-=-++-+
static const char vlist2505[] = { 3, 9, 1, 5,14, 9, 5, 7,14, 5};                               // =-=-++=-
static const char vlist2506[] = { 2,14, 0, 5, 7,14, 5};                                        // =-=-++==
static const char vlist2507[] = { 2, 7,12, 5, 7,14,12};                                        // =-=-++=+
static const char vlist2508[] = { 3, 9, 8, 5,14, 9, 5, 7,14, 5};                               // =-=-+++-
static const char vlist2509[] = { 2,14, 0, 5, 7,14, 5};                                        // =-=-+++=
static const char vlist2510[] = { 2, 7,12, 5, 7,14,12};                                        // =-=-++++
static const char vlist2511[] = { 1, 7, 5, 4};                                                 // =-==----
static const char vlist2512[] = { 2, 0, 4, 7, 5, 0, 7};                                        // =-==---=
static const char vlist2513[] = { 3, 9, 4, 7, 5, 8, 7, 8, 9, 7};                               // =-==---+
static const char vlist2514[] = { 1, 1, 4, 7};                                                 // =-==--=-
static const char vlist2515[] = { 2, 1, 0, 7, 0, 4, 7};                                        // =-==--==
static const char vlist2516[] = { 2, 1, 9, 7, 9, 4, 7};                                        // =-==--=+
static const char vlist2517[] = { 2, 8, 4, 7,10, 8, 7};                                        // =-==--+-
static const char vlist2518[] = { 2,10, 0, 7, 0, 4, 7};                                        // =-==--+=
static const char vlist2519[] = { 2, 9, 4, 7,10, 9, 7};                                        // =-==--++
static const char vlist2520[] = { 3, 5, 4, 2, 4, 2, 7, 5, 2, 7};                               // =-==-=--
static const char vlist2521[] = { 3, 4, 2, 7, 2, 7, 5, 0, 2, 5};                               // =-==-=-=
static const char vlist2522[] = { 3, 4, 2, 7, 2, 7, 8, 8, 7, 5};                               // =-==-=-+
static const char vlist2523[] = { 3, 2, 4, 1, 4, 2, 7, 2, 1, 7};                               // =-==-==-
static const char vlist2524[] = { 2, 2, 1, 7, 2, 7, 4};                                        // =-==-===
static const char vlist2525[] = { 2, 1, 2, 7, 7, 2, 4};                                        // =-==-==+
static const char vlist2526[] = { 3, 4, 2, 8, 4, 2, 7,10, 2, 7};                               // =-==-=+-
static const char vlist2527[] = { 2, 7, 2, 4,10, 2, 7};                                        // =-==-=+=
static const char vlist2528[] = { 2, 7, 2, 4,10, 2, 7};                                        // =-==-=++
static const char vlist2529[] = { 4, 5, 4, 9, 4,14, 7, 5, 9,11, 5,11, 7};                      // =-==-+--
static const char vlist2530[] = { 3, 0,11, 5,11, 7, 5, 4,14, 7};                               // =-==-+-=
static const char vlist2531[] = { 3, 8, 7, 5,11, 7, 8,14, 4, 7};                               // =-==-+-+
static const char vlist2532[] = { 3, 4, 9, 1, 1,11, 7, 4,14, 7};                               // =-==-+=-
static const char vlist2533[] = { 2, 1,11, 7, 7,14, 4};                                        // =-==-+==
static const char vlist2534[] = { 2, 1,11, 7, 7,14, 4};                                        // =-==-+=+
static const char vlist2535[] = { 3, 4, 9, 8,10,11, 7,14, 4, 7};                               // =-==-++-
static const char vlist2536[] = { 2,10,11, 7, 7,14, 4};                                        // =-==-++=
static const char vlist2537[] = { 2,10,11, 7, 7,14, 4};                                        // =-==-+++
static const char vlist2538[] = { 2, 5, 4, 3, 3, 4, 7};                                        // =-===---
static const char vlist2539[] = { 3, 4, 3, 0, 3, 4, 7, 3, 0, 5};                               // =-===--=
static const char vlist2540[] = { 3, 3, 9, 7, 8, 3, 5, 9, 4, 7};                               // =-===--+
static const char vlist2541[] = { 2, 4, 3, 1, 3, 4, 7};                                        // =-===-=-
static const char vlist2542[] = { 2, 3, 0, 4, 7, 3, 4};                                        // =-===-==
static const char vlist2543[] = { 2, 7, 3, 9, 7, 9, 4};                                        // =-===-=+
static const char vlist2544[] = { 2, 4, 3, 8, 3, 4, 7};                                        // =-===-+-
static const char vlist2545[] = { 2, 3, 0, 4, 7, 3, 4};                                        // =-===-+=
static const char vlist2546[] = { 2, 7, 3, 9, 7, 9, 4};                                        // =-===-++
static const char vlist2547[] = { 3, 3, 4, 2, 4, 3, 5, 4, 2, 7};                               // =-====--
static const char vlist2548[] = { 2, 7, 2, 4, 3, 0, 5};                                        // =-====-=
static const char vlist2549[] = { 2, 7, 2, 4, 8, 3, 5};                                        // =-====-+
static const char vlist2550[] = { 2, 7, 2, 4, 1, 2, 4};                                        // =-=====-
static const char vlist2551[] = { 1, 7, 2, 4};                                                 // =-======
static const char vlist2552[] = { 1, 7, 2, 4};                                                 // =-=====+
static const char vlist2553[] = { 2, 7, 2, 4, 2, 8, 4};                                        // =-====+-
static const char vlist2554[] = { 1, 7, 2, 4};                                                 // =-====+=
static const char vlist2555[] = { 1, 7, 2, 4};                                                 // =-====++
static const char vlist2556[] = { 3, 9, 3, 5, 4, 9, 5, 4,14, 7};                               // =-===+--
static const char vlist2557[] = { 2, 0, 3, 5, 7,14, 4};                                        // =-===+-=
static const char vlist2558[] = { 2, 8, 3, 5, 7,14, 4};                                        // =-===+-+
static const char vlist2559[] = { 2, 9, 1, 4, 7,14, 4};                                        // =-===+=-
static const char vlist2560[] = { 1, 7,14, 4};                                                 // =-===+==
static const char vlist2561[] = { 1, 7,14, 4};                                                 // =-===+=+
static const char vlist2562[] = { 2, 9, 8, 4, 7,14, 4};                                        // =-===++-
static const char vlist2563[] = { 1, 7,14, 4};                                                 // =-===++=
static const char vlist2564[] = { 1, 7,14, 4};                                                 // =-===+++
static const char vlist2565[] = { 3, 5, 4,10,11, 4, 7,10, 4,11};                               // =-==+---
static const char vlist2566[] = { 3,11, 4, 7, 4,11, 0, 0,10, 5};                               // =-==+--=
static const char vlist2567[] = { 3,11, 9, 7, 9, 4, 7, 8,10, 5};                               // =-==+--+
static const char vlist2568[] = { 2,11, 4, 7, 4,11, 1};                                        // =-==+-=-
static const char vlist2569[] = { 2,11, 0, 4, 7,11, 4};                                        // =-==+-==
static const char vlist2570[] = { 2, 7, 9, 4, 7,11, 9};                                        // =-==+-=+
static const char vlist2571[] = { 2,11, 4, 7, 4,11, 8};                                        // =-==+-+-
static const char vlist2572[] = { 2,11, 0, 4, 7,11, 4};                                        // =-==+-+=
static const char vlist2573[] = { 2, 7, 9, 4, 7,11, 9};                                        // =-==+-++
static const char vlist2574[] = { 3, 4,10, 5,10, 4, 2, 4, 2, 7};                               // =-==+=--
static const char vlist2575[] = { 2, 7, 2, 4, 0,10, 5};                                        // =-==+=-=
static const char vlist2576[] = { 2, 7, 2, 4, 8,10, 5};                                        // =-==+=-+
static const char vlist2577[] = { 2, 7, 2, 4, 2, 1, 4};                                        // =-==+==-
static const char vlist2578[] = { 1, 7, 2, 4};                                                 // =-==+===
static const char vlist2579[] = { 1, 7, 2, 4};                                                 // =-==+==+
static const char vlist2580[] = { 2, 7, 2, 4, 2, 8, 4};                                        // =-==+=+-
static const char vlist2581[] = { 1, 7, 2, 4};                                                 // =-==+=+=
static const char vlist2582[] = { 1, 7, 2, 4};                                                 // =-==+=++
static const char vlist2583[] = { 3, 9,10, 5, 4, 9, 5,14, 4, 7};                               // =-==++--
static const char vlist2584[] = { 2, 0,10, 5, 7,14, 4};                                        // =-==++-=
static const char vlist2585[] = { 2, 8,10, 5, 7,14, 4};                                        // =-==++-+
static const char vlist2586[] = { 2, 9, 1, 4, 7,14, 4};                                        // =-==++=-
static const char vlist2587[] = { 1, 7,14, 4};                                                 // =-==++==
static const char vlist2588[] = { 1, 7,14, 4};                                                 // =-==++=+
static const char vlist2589[] = { 2, 9, 8, 4, 7,14, 4};                                        // =-==+++-
static const char vlist2590[] = { 1, 7,14, 4};                                                 // =-==+++=
static const char vlist2591[] = { 1, 7,14, 4};                                                 // =-==++++
static const char vlist2592[] = { 2, 7, 5,12,17, 7,12};                                        // =-=+----
static const char vlist2593[] = { 2, 0,17, 7, 5, 0, 7};                                        // =-=+---=
static const char vlist2594[] = { 3, 5, 8, 7, 8, 9, 7, 9,17, 7};                               // =-=+---+
static const char vlist2595[] = { 2, 1,12, 7,12,17, 7};                                        // =-=+--=-
static const char vlist2596[] = { 2, 1, 0, 7, 0,17, 7};                                        // =-=+--==
static const char vlist2597[] = { 2, 1, 9, 7, 9,17, 7};                                        // =-=+--=+
static const char vlist2598[] = { 3,10, 8,12,10,12,17,10,17, 7};                               // =-=+--+-
static const char vlist2599[] = { 2,10, 0, 7, 0,17, 7};                                        // =-=+--+=
static const char vlist2600[] = { 2, 9,17, 7,10, 9, 7};                                        // =-=+--++
static const char vlist2601[] = { 3, 5,12, 2, 5, 2, 7,17, 2, 7};                               // =-=+-=--
static const char vlist2602[] = { 3,17, 2, 7, 2, 7, 5, 0, 2, 5};                               // =-=+-=-=
static const char vlist2603[] = { 3, 2,17, 7, 2, 7, 8, 8, 7, 5};                               // =-=+-=-+
static const char vlist2604[] = { 3,17, 2, 7, 1, 2, 7,12, 2, 1};                               // =-=+-==-
static const char vlist2605[] = { 2, 1, 2, 7, 7, 2,17};                                        // =-=+-===
static const char vlist2606[] = { 2, 1, 2, 7, 7, 2,17};                                        // =-=+-==+
static const char vlist2607[] = { 3, 2,17, 7,10, 2, 7,12, 2, 8};                               // =-=+-=+-
static const char vlist2608[] = { 2, 7, 2,17,10, 2, 7};                                        // =-=+-=+=
static const char vlist2609[] = { 2, 7, 2,17,10, 2, 7};                                        // =-=+-=++
static const char vlist2610[] = { 4, 5,12, 9, 5, 9,11,14,17, 7, 5,11, 7};                      // =-=+-+--
static const char vlist2611[] = { 3, 0,11, 5,11, 7, 5,14,17, 7};                               // =-=+-+-=
static const char vlist2612[] = { 3, 8, 7, 5,11, 7, 8,14,17, 7};                               // =-=+-+-+
static const char vlist2613[] = { 3,12, 9, 1, 1,11, 7,14,17, 7};                               // =-=+-+=-
static const char vlist2614[] = { 2, 1,11, 7, 7,14,17};                                        // =-=+-+==
static const char vlist2615[] = { 2, 1,11, 7, 7,14,17};                                        // =-=+-+=+
static const char vlist2616[] = { 3,12, 9, 8,10,11, 7,14,17, 7};                               // =-=+-++-
static const char vlist2617[] = { 2,10,11, 7, 7,14,17};                                        // =-=+-++=
static const char vlist2618[] = { 2,10,11, 7, 7,14,17};                                        // =-=+-+++
static const char vlist2619[] = { 3,12,17, 3, 3,17, 7, 5,12, 3};                               // =-=+=---
static const char vlist2620[] = { 3, 3,17, 7,17, 3, 0, 0, 3, 5};                               // =-=+=--=
static const char vlist2621[] = { 3, 3, 9, 7, 8, 3, 5, 9,17, 7};                               // =-=+=--+
static const char vlist2622[] = { 3, 3,17, 7,17, 3,12,12, 3, 1};                               // =-=+=-=-
static const char vlist2623[] = { 2, 3, 0,17, 7, 3,17};                                        // =-=+=-==
static const char vlist2624[] = { 2, 7, 3, 9, 7, 9,17};                                        // =-=+=-=+
static const char vlist2625[] = { 3, 3,17, 7,17, 3,12,12, 3, 8};                               // =-=+=-+-
static const char vlist2626[] = { 2, 3, 0,17, 7, 3,17};                                        // =-=+=-+=
static const char vlist2627[] = { 2, 7, 3, 9, 7, 9,17};                                        // =-=+=-++
static const char vlist2628[] = { 3,12, 3, 5, 3,12, 2,17, 2, 7};                               // =-=+==--
static const char vlist2629[] = { 2, 7, 2,17, 0, 3, 5};                                        // =-=+==-=
static const char vlist2630[] = { 2, 7, 2,17, 8, 3, 5};                                        // =-=+==-+
static const char vlist2631[] = { 2, 7, 2,17, 2, 1,12};                                        // =-=+===-
static const char vlist2632[] = { 1, 7, 2,17};                                                 // =-=+====
static const char vlist2633[] = { 1, 7, 2,17};                                                 // =-=+===+
static const char vlist2634[] = { 2, 7, 2,17, 2, 8,12};                                        // =-=+==+-
static const char vlist2635[] = { 1, 7, 2,17};                                                 // =-=+==+=
static const char vlist2636[] = { 1, 7, 2,17};                                                 // =-=+==++
static const char vlist2637[] = { 3, 9, 3, 5,14,17, 7,12, 9, 5};                               // =-=+=+--
static const char vlist2638[] = { 2, 0, 3, 5, 7,14,17};                                        // =-=+=+-=
static const char vlist2639[] = { 2, 8, 3, 5, 7,14,17};                                        // =-=+=+-+
static const char vlist2640[] = { 2, 9, 1,12, 7,14,17};                                        // =-=+=+=-
static const char vlist2641[] = { 1, 7,14,17};                                                 // =-=+=+==
static const char vlist2642[] = { 1, 7,14,17};                                                 // =-=+=+=+
static const char vlist2643[] = { 2, 9, 8,12, 7,14,17};                                        // =-=+=++-
static const char vlist2644[] = { 1, 7,14,17};                                                 // =-=+=++=
static const char vlist2645[] = { 1, 7,14,17};                                                 // =-=+=+++
static const char vlist2646[] = { 4,12,17,11,11,17, 7,10,12,11, 5,12,10};                      // =-=++---
static const char vlist2647[] = { 3,17,11, 0,11,17, 7, 0,10, 5};                               // =-=++--=
static const char vlist2648[] = { 3,11,17, 7,11, 9,17, 8,10, 5};                               // =-=++--+
static const char vlist2649[] = { 3,17,11,12,11,17, 7,12,11, 1};                               // =-=++-=-
static const char vlist2650[] = { 2,11, 0,17, 7,11,17};                                        // =-=++-==
static const char vlist2651[] = { 2, 7,11,17,17,11, 9};                                        // =-=++-=+
static const char vlist2652[] = { 3,17,11,12,11,17, 7,12,11, 8};                               // =-=++-+-
static const char vlist2653[] = { 2,11, 0,17, 7,11,17};                                        // =-=++-+=
static const char vlist2654[] = { 2, 7,11,17,17,11, 9};                                        // =-=++-++
static const char vlist2655[] = { 3,10,12, 2,12,10, 5, 2,17, 7};                               // =-=++=--
static const char vlist2656[] = { 2, 7, 2,17, 0,10, 5};                                        // =-=++=-=
static const char vlist2657[] = { 2, 7, 2,17, 8,10, 5};                                        // =-=++=-+
static const char vlist2658[] = { 2, 7, 2,17, 2, 1,12};                                        // =-=++==-
static const char vlist2659[] = { 1, 7, 2,17};                                                 // =-=++===
static const char vlist2660[] = { 1, 7, 2,17};                                                 // =-=++==+
static const char vlist2661[] = { 2, 7, 2,17, 2, 8,12};                                        // =-=++=+-
static const char vlist2662[] = { 1, 7, 2,17};                                                 // =-=++=+=
static const char vlist2663[] = { 1, 7, 2,17};                                                 // =-=++=++
static const char vlist2664[] = { 3,12,10, 5, 9,10,12,14,17, 7};                               // =-=+++--
static const char vlist2665[] = { 2, 0,10, 5, 7,14,17};                                        // =-=+++-=
static const char vlist2666[] = { 2, 8,10, 5, 7,14,17};                                        // =-=+++-+
static const char vlist2667[] = { 2, 9, 1,12, 7,14,17};                                        // =-=+++=-
static const char vlist2668[] = { 1, 7,14,17};                                                 // =-=+++==
static const char vlist2669[] = { 1, 7,14,17};                                                 // =-=+++=+
static const char vlist2670[] = { 2, 9, 8,12, 7,14,17};                                        // =-=++++-
static const char vlist2671[] = { 1, 7,14,17};                                                 // =-=++++=
static const char vlist2672[] = { 1, 7,14,17};                                                 // =-=+++++
static const char vlist2673[] = { 1, 7,13,16};                                                 // =-+-----
static const char vlist2674[] = { 2,16, 7, 0, 0, 7,13};                                        // =-+----=
static const char vlist2675[] = { 4,16, 7,12,12, 7, 9, 9, 7, 8, 7,13, 8};                      // =-+----+
static const char vlist2676[] = { 1, 7, 1,16};                                                 // =-+---=-
static const char vlist2677[] = { 2, 1, 0, 7, 0,16, 7};                                        // =-+---==
static const char vlist2678[] = { 3, 1, 9, 7, 9,12, 7,12,16, 7};                               // =-+---=+
static const char vlist2679[] = { 2, 7,10, 8,16, 7, 8};                                        // =-+---+-
static const char vlist2680[] = { 2,10, 0, 7, 0,16, 7};                                        // =-+---+=
static const char vlist2681[] = { 3, 9,12, 7,10, 9, 7,12,16, 7};                               // =-+---++
static const char vlist2682[] = { 3,16, 2,13, 7,16, 2, 7, 2,13};                               // =-+--=--
static const char vlist2683[] = { 4, 2,16, 7,16, 2, 0, 0, 2,13, 2, 7,13};                      // =-+--=-=
static const char vlist2684[] = { 4, 2,12, 7, 2, 7, 8, 7,13, 8,12,16, 7};                      // =-+--=-+
static const char vlist2685[] = { 3, 2,16, 7,16, 2, 1, 2, 1, 7};                               // =-+--==-
static const char vlist2686[] = { 3, 2, 1, 7, 2, 0,16, 7, 2,16};                               // =-+--===
static const char vlist2687[] = { 3, 1, 2, 7, 7, 2,12, 7,12,16};                               // =-+--==+
static const char vlist2688[] = { 3, 2,16, 7,16, 2, 8,10, 2, 7};                               // =-+--=+-
static const char vlist2689[] = { 3, 2, 0,16, 7, 2,16,10, 2, 7};                               // =-+--=+=
static const char vlist2690[] = { 3, 7, 2,12,10, 2, 7, 7,12,16};                               // =-+--=++
static const char vlist2691[] = { 5,16, 9,13,16,14, 9,16, 7,14,13, 9,11, 7,13,11};             // =-+--+--
static const char vlist2692[] = { 4, 7,13,11,16,14, 0,14,16, 7,13, 0,11};                      // =-+--+-=
static const char vlist2693[] = { 4,14,16, 7,14,12,16,11, 7,13,11,13, 8};                      // =-+--+-+
static const char vlist2694[] = { 4, 1,11, 7,14,16, 7,16,14, 9,16, 9, 1};                      // =-+--+=-
static const char vlist2695[] = { 3, 1,11, 7,14, 0,16, 7,14,16};                               // =-+--+==
static const char vlist2696[] = { 3, 1,11, 7, 7,14,16,16,14,12};                               // =-+--+=+
static const char vlist2697[] = { 4,10,11, 7,14,16, 7,16,14, 9,16, 9, 8};                      // =-+--++-
static const char vlist2698[] = { 3,14, 0,16,10,11, 7, 7,14,16};                               // =-+--++=
static const char vlist2699[] = { 3,10,11, 7, 7,14,16,16,14,12};                               // =-+--+++
static const char vlist2700[] = { 2, 3,13,16, 7, 3,16};                                        // =-+-=---
static const char vlist2701[] = { 3, 3, 0, 7,13, 3, 0, 0,16, 7};                               // =-+-=--=
static const char vlist2702[] = { 4, 3, 9, 7,13, 3, 8, 9,12, 7,12,16, 7};                      // =-+-=--+
static const char vlist2703[] = { 2, 7, 3,16, 3, 1,16};                                        // =-+-=-=-
static const char vlist2704[] = { 2, 7, 0,16, 3, 0, 7};                                        // =-+-=-==
static const char vlist2705[] = { 3, 3, 9, 7, 9,12, 7, 7,12,16};                               // =-+-=-=+
static const char vlist2706[] = { 2, 7, 3, 8,16, 7, 8};                                        // =-+-=-+-
static const char vlist2707[] = { 2, 7, 0,16, 3, 0, 7};                                        // =-+-=-+=
static const char vlist2708[] = { 3, 3, 9, 7, 9,12, 7, 7,12,16};                               // =-+-=-++
static const char vlist2709[] = { 3, 3,13, 2,13,16, 2, 2,16, 7};                               // =-+-==--
static const char vlist2710[] = { 3, 7, 2,16, 2, 0,16, 3, 0,13};                               // =-+-==-=
static const char vlist2711[] = { 3, 7, 2,12, 7,12,16, 8, 3,13};                               // =-+-==-+
static const char vlist2712[] = { 2, 7, 2,16, 2, 1,16};                                        // =-+-===-
static const char vlist2713[] = { 2, 2, 0,16, 7, 2,16};                                        // =-+-====
static const char vlist2714[] = { 2, 7, 2,12, 7,12,16};                                        // =-+-===+
static const char vlist2715[] = { 2, 7, 2,16, 2, 8,16};                                        // =-+-==+-
static const char vlist2716[] = { 2, 2, 0,16, 7, 2,16};                                        // =-+-==+=
static const char vlist2717[] = { 2, 7, 2,12, 7,12,16};                                        // =-+-==++
static const char vlist2718[] = { 4, 3,13, 9,13,16, 9,14,16, 7, 9,16,14};                      // =-+-=+--
static const char vlist2719[] = { 3,14, 0,16, 7,14,16, 0, 3,13};                               // =-+-=+-=
static const char vlist2720[] = { 3,16,14,12, 7,14,16, 8, 3,13};                               // =-+-=+-+
static const char vlist2721[] = { 3, 9, 1,16,14, 9,16, 7,14,16};                               // =-+-=+=-
static const char vlist2722[] = { 2,14, 0,16, 7,14,16};                                        // =-+-=+==
static const char vlist2723[] = { 2, 7,14,16,16,14,12};                                        // =-+-=+=+
static const char vlist2724[] = { 3, 9, 8,16,14, 9,16, 7,14,16};                               // =-+-=++-
static const char vlist2725[] = { 2,14, 0,16, 7,14,16};                                        // =-+-=++=
static const char vlist2726[] = { 2, 7,14,16,16,14,12};                                        // =-+-=+++
static const char vlist2727[] = { 3,10,13,11,13,16,11,16, 7,11};                               // =-+-+---
static const char vlist2728[] = { 3,13,10, 0, 0,16, 7,11, 0, 7};                               // =-+-+--=
static const char vlist2729[] = { 4,10,13, 8, 9,12, 7,11, 9, 7,12,16, 7};                      // =-+-+--+
static const char vlist2730[] = { 2, 1,16,11,16, 7,11};                                        // =-+-+-=-
static const char vlist2731[] = { 2, 7, 0,16,11, 0, 7};                                        // =-+-+-==
static const char vlist2732[] = { 3,11, 9, 7, 9,12, 7, 7,12,16};                               // =-+-+-=+
static const char vlist2733[] = { 2, 7,11,16,16,11, 8};                                        // =-+-+-+-
static const char vlist2734[] = { 2, 7, 0,16,11, 0, 7};                                        // =-+-+-+=
static const char vlist2735[] = { 3,11, 9, 7, 9,12, 7, 7,12,16};                               // =-+-+-++
static const char vlist2736[] = { 3,10,13, 2,13,16, 2, 2,16, 7};                               // =-+-+=--
static const char vlist2737[] = { 3, 7, 2,16, 2, 0,16, 0,10,13};                               // =-+-+=-=
static const char vlist2738[] = { 3, 7, 2,12, 7,12,16, 8,10,13};                               // =-+-+=-+
static const char vlist2739[] = { 2, 7, 2,16, 2, 1,16};                                        // =-+-+==-
static const char vlist2740[] = { 2, 2, 0,16, 7, 2,16};                                        // =-+-+===
static const char vlist2741[] = { 2, 7, 2,12, 7,12,16};                                        // =-+-+==+
static const char vlist2742[] = { 2, 7, 2,16, 2, 8,16};                                        // =-+-+=+-
static const char vlist2743[] = { 2, 2, 0,16, 7, 2,16};                                        // =-+-+=+=
static const char vlist2744[] = { 2, 7, 2,12, 7,12,16};                                        // =-+-+=++
static const char vlist2745[] = { 4,10,13, 9,13,16, 9, 9,16,14,14,16, 7};                      // =-+-++--
static const char vlist2746[] = { 3,14, 0,16, 7,14,16, 0,10,13};                               // =-+-++-=
static const char vlist2747[] = { 3,16,14,12, 7,14,16, 8,10,13};                               // =-+-++-+
static const char vlist2748[] = { 3, 9, 1,16,14, 9,16, 7,14,16};                               // =-+-++=-
static const char vlist2749[] = { 2,14, 0,16, 7,14,16};                                        // =-+-++==
static const char vlist2750[] = { 2, 7,14,16,16,14,12};                                        // =-+-++=+
static const char vlist2751[] = { 3, 9, 8,16,14, 9,16, 7,14,16};                               // =-+-+++-
static const char vlist2752[] = { 2,14, 0,16, 7,14,16};                                        // =-+-+++=
static const char vlist2753[] = { 2, 7,14,16,16,14,12};                                        // =-+-++++
static const char vlist2754[] = { 1, 4, 7,13};                                                 // =-+=----
static const char vlist2755[] = { 2, 0, 4, 7,13, 0, 7};                                        // =-+=---=
static const char vlist2756[] = { 3, 9, 4, 7,13, 8, 7, 8, 9, 7};                               // =-+=---+
static const char vlist2757[] = { 1, 1, 4, 7};                                                 // =-+=--=-
static const char vlist2758[] = { 2, 1, 0, 7, 0, 4, 7};                                        // =-+=--==
static const char vlist2759[] = { 2, 1, 9, 7, 9, 4, 7};                                        // =-+=--=+
static const char vlist2760[] = { 2, 8, 4, 7,10, 8, 7};                                        // =-+=--+-
static const char vlist2761[] = { 2,10, 0, 7, 0, 4, 7};                                        // =-+=--+=
static const char vlist2762[] = { 2, 9, 4, 7,10, 9, 7};                                        // =-+=--++
static const char vlist2763[] = { 3, 4, 2, 7,13, 4, 2,13, 2, 7};                               // =-+=-=--
static const char vlist2764[] = { 3, 4, 2, 7, 2, 7,13, 0, 2,13};                               // =-+=-=-=
static const char vlist2765[] = { 3, 2, 4, 7, 2, 7, 8, 8, 7,13};                               // =-+=-=-+
static const char vlist2766[] = { 3, 4, 2, 1, 4, 2, 7, 1, 2, 7};                               // =-+=-==-
static const char vlist2767[] = { 2, 1, 2, 7, 7, 2, 4};                                        // =-+=-===
static const char vlist2768[] = { 2, 1, 2, 7, 7, 2, 4};                                        // =-+=-==+
static const char vlist2769[] = { 3, 4, 2, 8, 2, 4, 7,10, 2, 7};                               // =-+=-=+-
static const char vlist2770[] = { 2, 7, 2, 4,10, 2, 7};                                        // =-+=-=+=
static const char vlist2771[] = { 2, 7, 2, 4,10, 2, 7};                                        // =-+=-=++
static const char vlist2772[] = { 4,14, 4, 7,13, 4, 9,13, 9,11,13,11, 7};                      // =-+=-+--
static const char vlist2773[] = { 3, 0,11,13,11, 7,13,14, 4, 7};                               // =-+=-+-=
static const char vlist2774[] = { 3,11,13, 8,11, 7,13,14, 4, 7};                               // =-+=-+-+
static const char vlist2775[] = { 3, 4, 9, 1, 1,11, 7,14, 4, 7};                               // =-+=-+=-
static const char vlist2776[] = { 2, 1,11, 7, 7,14, 4};                                        // =-+=-+==
static const char vlist2777[] = { 2, 1,11, 7, 7,14, 4};                                        // =-+=-+=+
static const char vlist2778[] = { 3, 4, 9, 8,10,11, 7,14, 4, 7};                               // =-+=-++-
static const char vlist2779[] = { 2,10,11, 7, 7,14, 4};                                        // =-+=-++=
static const char vlist2780[] = { 2,10,11, 7, 7,14, 4};                                        // =-+=-+++
static const char vlist2781[] = { 2, 3, 4, 7,13, 4, 3};                                        // =-+==---
static const char vlist2782[] = { 3, 4, 3, 0, 3, 4, 7, 0, 3,13};                               // =-+==--=
static const char vlist2783[] = { 3, 3, 9, 7, 8, 3,13, 9, 4, 7};                               // =-+==--+
static const char vlist2784[] = { 2, 4, 3, 1, 3, 4, 7};                                        // =-+==-=-
static const char vlist2785[] = { 2, 3, 0, 4, 7, 3, 4};                                        // =-+==-==
static const char vlist2786[] = { 2, 7, 3, 9, 7, 9, 4};                                        // =-+==-=+
static const char vlist2787[] = { 2, 4, 3, 8, 3, 4, 7};                                        // =-+==-+-
static const char vlist2788[] = { 2, 3, 0, 4, 7, 3, 4};                                        // =-+==-+=
static const char vlist2789[] = { 2, 7, 3, 9, 7, 9, 4};                                        // =-+==-++
static const char vlist2790[] = { 3, 3,13, 2,13, 4, 2, 4, 2, 7};                               // =-+===--
static const char vlist2791[] = { 2, 7, 2, 4, 0, 3,13};                                        // =-+===-=
static const char vlist2792[] = { 2, 7, 2, 4, 8, 3,13};                                        // =-+===-+
static const char vlist2793[] = { 2, 7, 2, 4, 2, 1, 4};                                        // =-+====-
static const char vlist2794[] = { 1, 7, 2, 4};                                                 // =-+=====
static const char vlist2795[] = { 1, 7, 2, 4};                                                 // =-+====+
static const char vlist2796[] = { 2, 7, 2, 4, 2, 8, 4};                                        // =-+===+-
static const char vlist2797[] = { 1, 7, 2, 4};                                                 // =-+===+=
static const char vlist2798[] = { 1, 7, 2, 4};                                                 // =-+===++
static const char vlist2799[] = { 3, 9, 3,13, 4, 9,13,14, 4, 7};                               // =-+==+--
static const char vlist2800[] = { 2, 0, 3,13, 7,14, 4};                                        // =-+==+-=
static const char vlist2801[] = { 2, 8, 3,13, 7,14, 4};                                        // =-+==+-+
static const char vlist2802[] = { 2, 9, 1, 4, 7,14, 4};                                        // =-+==+=-
static const char vlist2803[] = { 1, 7,14, 4};                                                 // =-+==+==
static const char vlist2804[] = { 1, 7,14, 4};                                                 // =-+==+=+
static const char vlist2805[] = { 2, 9, 8, 4, 7,14, 4};                                        // =-+==++-
static const char vlist2806[] = { 1, 7,14, 4};                                                 // =-+==++=
static const char vlist2807[] = { 1, 7,14, 4};                                                 // =-+==+++
static const char vlist2808[] = { 3,11, 4, 7,10, 4,11,13, 4,10};                               // =-+=+---
static const char vlist2809[] = { 3,11, 4, 7, 4,11, 0, 0,10,13};                               // =-+=+--=
static const char vlist2810[] = { 3,11, 9, 7, 9, 4, 7, 8,10,13};                               // =-+=+--+
static const char vlist2811[] = { 2,11, 4, 7, 4,11, 1};                                        // =-+=+-=-
static const char vlist2812[] = { 2,11, 0, 4, 7,11, 4};                                        // =-+=+-==
static const char vlist2813[] = { 2, 7, 9, 4, 7,11, 9};                                        // =-+=+-=+
static const char vlist2814[] = { 2,11, 4, 7, 4,11, 8};                                        // =-+=+-+-
static const char vlist2815[] = { 2,11, 0, 4, 7,11, 4};                                        // =-+=+-+=
static const char vlist2816[] = { 2, 7, 9, 4, 7,11, 9};                                        // =-+=+-++
static const char vlist2817[] = { 3, 4,10,13,10, 4, 2, 2, 4, 7};                               // =-+=+=--
static const char vlist2818[] = { 2, 7, 2, 4, 0,10,13};                                        // =-+=+=-=
static const char vlist2819[] = { 2, 7, 2, 4, 8,10,13};                                        // =-+=+=-+
static const char vlist2820[] = { 2, 7, 2, 4, 2, 1, 4};                                        // =-+=+==-
static const char vlist2821[] = { 1, 7, 2, 4};                                                 // =-+=+===
static const char vlist2822[] = { 1, 7, 2, 4};                                                 // =-+=+==+
static const char vlist2823[] = { 2, 7, 2, 4, 2, 8, 4};                                        // =-+=+=+-
static const char vlist2824[] = { 1, 7, 2, 4};                                                 // =-+=+=+=
static const char vlist2825[] = { 1, 7, 2, 4};                                                 // =-+=+=++
static const char vlist2826[] = { 3, 9,10,13, 4, 9,13,14, 4, 7};                               // =-+=++--
static const char vlist2827[] = { 2, 0,10,13, 7,14, 4};                                        // =-+=++-=
static const char vlist2828[] = { 2, 8,10,13, 7,14, 4};                                        // =-+=++-+
static const char vlist2829[] = { 2, 9, 1, 4, 7,14, 4};                                        // =-+=++=-
static const char vlist2830[] = { 1, 7,14, 4};                                                 // =-+=++==
static const char vlist2831[] = { 1, 7,14, 4};                                                 // =-+=++=+
static const char vlist2832[] = { 2, 9, 8, 4, 7,14, 4};                                        // =-+=+++-
static const char vlist2833[] = { 1, 7,14, 4};                                                 // =-+=+++=
static const char vlist2834[] = { 1, 7,14, 4};                                                 // =-+=++++
static const char vlist2835[] = { 2,17, 7,12, 7,13,12};                                        // =-++----
static const char vlist2836[] = { 2, 0,17, 7,13, 0, 7};                                        // =-++---=
static const char vlist2837[] = { 3,13, 8, 7, 8, 9, 7, 9,17, 7};                               // =-++---+
static const char vlist2838[] = { 2, 1,12, 7,12,17, 7};                                        // =-++--=-
static const char vlist2839[] = { 2, 1, 0, 7, 0,17, 7};                                        // =-++--==
static const char vlist2840[] = { 2, 1, 9, 7, 9,17, 7};                                        // =-++--=+
static const char vlist2841[] = { 3,10, 8,12,10,12,17,10,17, 7};                               // =-++--+-
static const char vlist2842[] = { 2,10, 0, 7, 0,17, 7};                                        // =-++--+=
static const char vlist2843[] = { 2, 9,17, 7,10, 9, 7};                                        // =-++--++
static const char vlist2844[] = { 3,13,12, 2,13, 2, 7, 2,17, 7};                               // =-++-=--
static const char vlist2845[] = { 3, 2,17, 7, 2, 7,13, 0, 2,13};                               // =-++-=-=
static const char vlist2846[] = { 3, 2,17, 7, 2, 7, 8, 8, 7,13};                               // =-++-=-+
static const char vlist2847[] = { 3, 2,17, 7, 1, 2, 7,12, 2, 1};                               // =-++-==-
static const char vlist2848[] = { 2, 1, 2, 7, 7, 2,17};                                        // =-++-===
static const char vlist2849[] = { 2, 1, 2, 7, 7, 2,17};                                        // =-++-==+
static const char vlist2850[] = { 3, 2,17, 7,10, 2, 7,12, 2, 8};                               // =-++-=+-
static const char vlist2851[] = { 2, 7, 2,17,10, 2, 7};                                        // =-++-=+=
static const char vlist2852[] = { 2, 7, 2,17,10, 2, 7};                                        // =-++-=++
static const char vlist2853[] = { 4,13,12, 9,13,11, 7,13, 9,11,14,17, 7};                      // =-++-+--
static const char vlist2854[] = { 3, 0,11,13,11, 7,13,14,17, 7};                               // =-++-+-=
static const char vlist2855[] = { 3,11,13, 8,11, 7,13,14,17, 7};                               // =-++-+-+
static const char vlist2856[] = { 3,12, 9, 1, 1,11, 7,14,17, 7};                               // =-++-+=-
static const char vlist2857[] = { 2, 1,11, 7, 7,14,17};                                        // =-++-+==
static const char vlist2858[] = { 2, 1,11, 7, 7,14,17};                                        // =-++-+=+
static const char vlist2859[] = { 3,12, 9, 8,10,11, 7,14,17, 7};                               // =-++-++-
static const char vlist2860[] = { 2,10,11, 7, 7,14,17};                                        // =-++-++=
static const char vlist2861[] = { 2,10,11, 7, 7,14,17};                                        // =-++-+++
static const char vlist2862[] = { 3,12,17, 3, 3,17, 7,13,12, 3};                               // =-++=---
static const char vlist2863[] = { 3, 3,17, 7,17, 3, 0, 0, 3,13};                               // =-++=--=
static const char vlist2864[] = { 3, 3, 9, 7, 8, 3,13, 9,17, 7};                               // =-++=--+
static const char vlist2865[] = { 3, 3,17, 7,17, 3,12,12, 3, 1};                               // =-++=-=-
static const char vlist2866[] = { 2, 3, 0,17, 7, 3,17};                                        // =-++=-==
static const char vlist2867[] = { 2, 7, 3, 9, 7, 9,17};                                        // =-++=-=+
static const char vlist2868[] = { 3, 3,17, 7,17, 3,12,12, 3, 8};                               // =-++=-+-
static const char vlist2869[] = { 2, 3, 0,17, 7, 3,17};                                        // =-++=-+=
static const char vlist2870[] = { 2, 7, 3, 9, 7, 9,17};                                        // =-++=-++
static const char vlist2871[] = { 3,12, 3,13, 3,12, 2, 2,17, 7};                               // =-++==--
static const char vlist2872[] = { 2, 7, 2,17, 0, 3,13};                                        // =-++==-=
static const char vlist2873[] = { 2, 7, 2,17, 8, 3,13};                                        // =-++==-+
static const char vlist2874[] = { 2, 7, 2,17, 2, 1,12};                                        // =-++===-
static const char vlist2875[] = { 1, 7, 2,17};                                                 // =-++====
static const char vlist2876[] = { 1, 7, 2,17};                                                 // =-++===+
static const char vlist2877[] = { 2, 7, 2,17, 2, 8,12};                                        // =-++==+-
static const char vlist2878[] = { 1, 7, 2,17};                                                 // =-++==+=
static const char vlist2879[] = { 1, 7, 2,17};                                                 // =-++==++
static const char vlist2880[] = { 3, 9, 3,13,12, 9,13,14,17, 7};                               // =-++=+--
static const char vlist2881[] = { 2, 0, 3,13, 7,14,17};                                        // =-++=+-=
static const char vlist2882[] = { 2, 8, 3,13, 7,14,17};                                        // =-++=+-+
static const char vlist2883[] = { 2, 9, 1,12, 7,14,17};                                        // =-++=+=-
static const char vlist2884[] = { 1, 7,14,17};                                                 // =-++=+==
static const char vlist2885[] = { 1, 7,14,17};                                                 // =-++=+=+
static const char vlist2886[] = { 2, 9, 8,12, 7,14,17};                                        // =-++=++-
static const char vlist2887[] = { 1, 7,14,17};                                                 // =-++=++=
static const char vlist2888[] = { 1, 7,14,17};                                                 // =-++=+++
static const char vlist2889[] = { 4,12,17,11,11,17, 7,10,12,11,13,12,10};                      // =-+++---
static const char vlist2890[] = { 3,17,11, 0,11,17, 7, 0,10,13};                               // =-+++--=
static const char vlist2891[] = { 3,11,17, 7,11, 9,17, 8,10,13};                               // =-+++--+
static const char vlist2892[] = { 3,17,11,12,11,17, 7,12,11, 1};                               // =-+++-=-
static const char vlist2893[] = { 2,11, 0,17, 7,11,17};                                        // =-+++-==
static const char vlist2894[] = { 2, 7,11,17,17,11, 9};                                        // =-+++-=+
static const char vlist2895[] = { 3,17,11,12,11,17, 7,12,11, 8};                               // =-+++-+-
static const char vlist2896[] = { 2,11, 0,17, 7,11,17};                                        // =-+++-+=
static const char vlist2897[] = { 2, 7,11,17,17,11, 9};                                        // =-+++-++
static const char vlist2898[] = { 3,10,12, 2,12,10,13, 2,17, 7};                               // =-+++=--
static const char vlist2899[] = { 2, 7, 2,17, 0,10,13};                                        // =-+++=-=
static const char vlist2900[] = { 2, 7, 2,17, 8,10,13};                                        // =-+++=-+
static const char vlist2901[] = { 2, 7, 2,17, 2, 1,12};                                        // =-+++==-
static const char vlist2902[] = { 1, 7, 2,17};                                                 // =-+++===
static const char vlist2903[] = { 1, 7, 2,17};                                                 // =-+++==+
static const char vlist2904[] = { 2, 7, 2,17, 2, 8,12};                                        // =-+++=+-
static const char vlist2905[] = { 1, 7, 2,17};                                                 // =-+++=+=
static const char vlist2906[] = { 1, 7, 2,17};                                                 // =-+++=++
static const char vlist2907[] = { 3,12,10,13, 9,10,12,14,17, 7};                               // =-++++--
static const char vlist2908[] = { 2, 0,10,13, 7,14,17};                                        // =-++++-=
static const char vlist2909[] = { 2, 8,10,13, 7,14,17};                                        // =-++++-+
static const char vlist2910[] = { 2, 9, 1,12, 7,14,17};                                        // =-++++=-
static const char vlist2911[] = { 1, 7,14,17};                                                 // =-++++==
static const char vlist2912[] = { 1, 7,14,17};                                                 // =-++++=+
static const char vlist2913[] = { 2, 9, 8,12, 7,14,17};                                        // =-+++++-
static const char vlist2914[] = { 1, 7,14,17};                                                 // =-+++++=
static const char vlist2915[] = { 1, 7,14,17};                                                 // =-++++++
                                                                                               // ==------
                                                                                               // ==-----=
static const char vlist2918[] = { 4, 7, 6,12, 7, 6, 9, 7, 8, 9, 7,12, 8};                      // ==-----+
                                                                                               // ==----=-
                                                                                               // ==----==
static const char vlist2921[] = { 4,12, 1, 7, 7, 1, 9, 6, 7, 9, 6,12, 7};                      // ==----=+
static const char vlist2922[] = { 4, 7, 6,13, 7, 6,10, 8, 6,10,13, 6, 8};                      // ==----+-
static const char vlist2923[] = { 4, 6, 7,10, 6,13, 7,13, 6, 0, 0, 6,10};                      // ==----+=
static const char vlist2924[] = { 4,12,13, 7, 6,12, 7, 6, 7, 9, 7,10, 9};                      // ==----++
static const char vlist2925[] = { 1, 7, 6, 2};                                                 // ==---=--
static const char vlist2926[] = { 2, 0, 2, 7, 6, 0, 7};                                        // ==---=-=
static const char vlist2927[] = { 3, 8, 2, 7,12, 8, 7, 6,12, 7};                               // ==---=-+
static const char vlist2928[] = { 3, 6, 2, 1, 2, 1, 7, 6, 1, 7};                               // ==---==-
static const char vlist2929[] = { 3, 2, 1, 7, 1, 7, 6, 0, 1, 6};                               // ==---===
static const char vlist2930[] = { 3, 2, 1, 7, 1, 7,12,12, 7, 6};                               // ==---==+
static const char vlist2931[] = { 4, 6, 2, 8, 2,10, 7, 6, 8,13, 6,13, 7};                      // ==---=+-
static const char vlist2932[] = { 3, 0,13, 6,13, 7, 6, 2,10, 7};                               // ==---=+=
static const char vlist2933[] = { 3,12, 7, 6,13, 7,12,10, 2, 7};                               // ==---=++
static const char vlist2934[] = { 2, 7, 6, 9,11, 7, 9};                                        // ==---+--
static const char vlist2935[] = { 2, 0,11, 7, 6, 0, 7};                                        // ==---+-=
static const char vlist2936[] = { 3,12, 8, 7, 8,11, 7, 6,12, 7};                               // ==---+-+
static const char vlist2937[] = { 3, 6, 9, 1, 6, 1, 7,11, 1, 7};                               // ==---+=-
static const char vlist2938[] = { 3,11, 1, 7, 1, 7, 6, 0, 1, 6};                               // ==---+==
static const char vlist2939[] = { 3, 1,11, 7, 1, 7,12,12, 7, 6};                               // ==---+=+
static const char vlist2940[] = { 4, 6, 9, 8, 6, 8,13,10,11, 7, 6,13, 7};                      // ==---++-
static const char vlist2941[] = { 3, 0,13, 6,13, 7, 6,10,11, 7};                               // ==---++=
static const char vlist2942[] = { 3,12, 7, 6,13, 7,12,10,11, 7};                               // ==---+++
static const char vlist2943[] = { 1, 7, 6, 3};                                                 // ==--=---
static const char vlist2944[] = { 3, 3, 6, 0, 0, 3, 7, 6, 0, 7};                               // ==--=--=
static const char vlist2945[] = { 4, 3, 6, 9, 8, 3, 7,12, 8, 7, 6,12, 7};                      // ==--=--+
static const char vlist2946[] = { 2, 6, 3, 1, 6, 1, 7};                                        // ==--=-=-
static const char vlist2947[] = { 3, 1, 7, 6, 0, 1, 6, 0, 3, 6};                               // ==--=-==
static const char vlist2948[] = { 3, 1, 7,12,12, 7, 6, 3, 9, 6};                               // ==--=-=+
static const char vlist2949[] = { 3, 6, 3, 8, 6, 8,13, 6,13, 7};                               // ==--=-+-
static const char vlist2950[] = { 3, 0,13, 6,13, 7, 6, 3, 0, 6};                               // ==--=-+=
static const char vlist2951[] = { 3,12, 7, 6,13, 7,12, 3, 9, 6};                               // ==--=-++
static const char vlist2952[] = { 2, 3, 6, 2, 6, 3, 7};                                        // ==--==--
static const char vlist2953[] = { 2, 0, 7, 6, 0, 3, 7};                                        // ==--==-=
static const char vlist2954[] = { 3, 8, 3, 7,12, 8, 7,12, 7, 6};                               // ==--==-+
static const char vlist2955[] = { 2, 2, 1, 6, 1, 7, 6};                                        // ==--===-
static const char vlist2956[] = { 2, 1, 7, 6, 0, 1, 6};                                        // ==--====
static const char vlist2957[] = { 2, 1, 7,12,12, 7, 6};                                        // ==--===+
static const char vlist2958[] = { 3, 2, 8, 6, 8,13, 6,13, 7, 6};                               // ==--==+-
static const char vlist2959[] = { 2, 0,13, 6,13, 7, 6};                                        // ==--==+=
static const char vlist2960[] = { 2,12, 7, 6,13, 7,12};                                        // ==--==++
static const char vlist2961[] = { 2, 3, 7, 9, 7, 6, 9};                                        // ==--=+--
static const char vlist2962[] = { 2, 0, 7, 6, 0, 3, 7};                                        // ==--=+-=
static const char vlist2963[] = { 3, 8, 3, 7,12, 8, 7,12, 7, 6};                               // ==--=+-+
static const char vlist2964[] = { 2, 9, 1, 6, 1, 7, 6};                                        // ==--=+=-
static const char vlist2965[] = { 2, 1, 7, 6, 0, 1, 6};                                        // ==--=+==
static const char vlist2966[] = { 2, 1, 7,12,12, 7, 6};                                        // ==--=+=+
static const char vlist2967[] = { 3, 8,13, 6, 9, 8, 6,13, 7, 6};                               // ==--=++-
static const char vlist2968[] = { 2, 0,13, 6,13, 7, 6};                                        // ==--=++=
static const char vlist2969[] = { 2,12, 7, 6,13, 7,12};                                        // ==--=+++
static const char vlist2970[] = { 2, 7, 6,10, 6,11,10};                                        // ==--+---
static const char vlist2971[] = { 3,11, 6, 0, 6, 0, 7, 0,10, 7};                               // ==--+--=
static const char vlist2972[] = { 4, 6,11, 9,12, 8, 7, 8,10, 7, 6,12, 7};                      // ==--+--+
static const char vlist2973[] = { 2, 6,11, 1, 6, 1, 7};                                        // ==--+-=-
static const char vlist2974[] = { 3, 1, 7, 6, 0, 1, 6,11, 0, 6};                               // ==--+-==
static const char vlist2975[] = { 3, 1, 7,12,12, 7, 6,11, 9, 6};                               // ==--+-=+
static const char vlist2976[] = { 3, 6,11, 8, 6, 8,13, 6,13, 7};                               // ==--+-+-
static const char vlist2977[] = { 3, 0,13, 6,13, 7, 6,11, 0, 6};                               // ==--+-+=
static const char vlist2978[] = { 3,12, 7, 6,13, 7,12,11, 9, 6};                               // ==--+-++
static const char vlist2979[] = { 2, 6, 2,10, 7, 6,10};                                        // ==--+=--
static const char vlist2980[] = { 2, 0, 7, 6, 0,10, 7};                                        // ==--+=-=
static const char vlist2981[] = { 3, 8,10, 7,12, 8, 7,12, 7, 6};                               // ==--+=-+
static const char vlist2982[] = { 2, 2, 1, 6, 1, 7, 6};                                        // ==--+==-
static const char vlist2983[] = { 2, 1, 7, 6, 0, 1, 6};                                        // ==--+===
static const char vlist2984[] = { 2, 1, 7,12,12, 7, 6};                                        // ==--+==+
static const char vlist2985[] = { 3, 2, 8, 6, 8,13, 6,13, 7, 6};                               // ==--+=+-
static const char vlist2986[] = { 2, 0,13, 6,13, 7, 6};                                        // ==--+=+=
static const char vlist2987[] = { 2,12, 7, 6,13, 7,12};                                        // ==--+=++
static const char vlist2988[] = { 2, 7, 6, 9,10, 7, 9};                                        // ==--++--
static const char vlist2989[] = { 2, 0, 7, 6, 0,10, 7};                                        // ==--++-=
static const char vlist2990[] = { 3, 8,10, 7,12, 8, 7,12, 7, 6};                               // ==--++-+
static const char vlist2991[] = { 2, 9, 1, 6, 1, 7, 6};                                        // ==--++=-
static const char vlist2992[] = { 2, 1, 7, 6, 0, 1, 6};                                        // ==--++==
static const char vlist2993[] = { 2, 1, 7,12,12, 7, 6};                                        // ==--++=+
static const char vlist2994[] = { 3, 8,13, 6, 9, 8, 6,13, 7, 6};                               // ==--+++-
static const char vlist2995[] = { 2, 0,13, 6,13, 7, 6};                                        // ==--+++=
static const char vlist2996[] = { 2,12, 7, 6,13, 7,12};                                        // ==--++++
static const char vlist2997[] = { 1, 6, 7, 4};                                                 // ==-=----
static const char vlist2998[] = { 2, 4, 0, 7, 0, 6, 7};                                        // ==-=---=
static const char vlist2999[] = { 3, 4, 8, 7, 8, 9, 7, 9, 6, 7};                               // ==-=---+
static const char vlist3000[] = { 3, 4, 6, 1, 1, 4, 7, 1, 6, 7};                               // ==-=--=-
static const char vlist3001[] = { 3, 1, 4, 7, 7, 1, 6, 1, 0, 6};                               // ==-=--==
static const char vlist3002[] = { 3, 4, 1, 7, 7, 1, 9, 7, 9, 6};                               // ==-=--=+
static const char vlist3003[] = { 4, 4, 6, 8,13, 4, 7, 8, 6,10,10, 6, 7};                      // ==-=--+-
static const char vlist3004[] = { 3,10, 0, 6, 7,10, 6, 4,13, 7};                               // ==-=--+=
static const char vlist3005[] = { 3, 7, 9, 6, 7,10, 9, 4,13, 7};                               // ==-=--++
static const char vlist3006[] = { 1, 4, 2, 7};                                                 // ==-=-=--
static const char vlist3007[] = { 2, 0, 2, 7, 4, 0, 7};                                        // ==-=-=-=
static const char vlist3008[] = { 2, 8, 2, 7, 4, 8, 7};                                        // ==-=-=-+
static const char vlist3009[] = { 3, 2, 4, 1, 2, 1, 7, 1, 4, 7};                               // ==-=-==-
static const char vlist3010[] = { 2, 2, 1, 7, 7, 1, 4};                                        // ==-=-===
static const char vlist3011[] = { 2, 1, 2, 7, 1, 7, 4};                                        // ==-=-==+
static const char vlist3012[] = { 3, 4, 2, 8,10, 2, 7, 4,13, 7};                               // ==-=-=+-
static const char vlist3013[] = { 2,10, 2, 7,13, 7, 4};                                        // ==-=-=+=
static const char vlist3014[] = { 2,10, 2, 7,13, 7, 4};                                        // ==-=-=++
static const char vlist3015[] = { 2, 4, 9, 7, 9,11, 7};                                        // ==-=-+--
static const char vlist3016[] = { 2, 0,11, 7, 4, 0, 7};                                        // ==-=-+-=
static const char vlist3017[] = { 2, 4, 8, 7, 8,11, 7};                                        // ==-=-+-+
static const char vlist3018[] = { 3, 4, 9, 1, 4, 1, 7, 1,11, 7};                               // ==-=-+=-
static const char vlist3019[] = { 2, 1,11, 7, 1, 7, 4};                                        // ==-=-+==
static const char vlist3020[] = { 2, 1,11, 7, 1, 7, 4};                                        // ==-=-+=+
static const char vlist3021[] = { 3, 4, 9, 8,10,11, 7, 4,13, 7};                               // ==-=-++-
static const char vlist3022[] = { 2,10,11, 7,13, 7, 4};                                        // ==-=-++=
static const char vlist3023[] = { 2,10,11, 7,13, 7, 4};                                        // ==-=-+++
static const char vlist3024[] = { 2, 4, 6, 3, 4, 3, 7};                                        // ==-==---
static const char vlist3025[] = { 3, 3, 4, 0, 4, 3, 7, 0, 3, 6};                               // ==-==--=
static const char vlist3026[] = { 3, 8, 3, 7, 9, 3, 6, 4, 8, 7};                               // ==-==--+
static const char vlist3027[] = { 3, 4, 3, 1, 3, 4, 6, 1, 4, 7};                               // ==-==-=-
static const char vlist3028[] = { 2, 7, 1, 4, 3, 0, 6};                                        // ==-==-==
static const char vlist3029[] = { 2, 1, 7, 4, 3, 9, 6};                                        // ==-==-=+
static const char vlist3030[] = { 3, 3, 8, 6, 8, 4, 6, 4,13, 7};                               // ==-==-+-
static const char vlist3031[] = { 2, 3, 0, 6,13, 7, 4};                                        // ==-==-+=
static const char vlist3032[] = { 2, 3, 9, 6,13, 7, 4};                                        // ==-==-++
static const char vlist3033[] = { 2, 3, 4, 2, 4, 3, 7};                                        // ==-===--
static const char vlist3034[] = { 2, 0, 3, 4, 3, 7, 4};                                        // ==-===-=
static const char vlist3035[] = { 2, 3, 7, 8, 8, 7, 4};                                        // ==-===-+
static const char vlist3036[] = { 2, 1, 2, 4, 7, 1, 4};                                        // ==-====-
static const char vlist3037[] = { 1, 1, 7, 4};                                                 // ==-=====
static const char vlist3038[] = { 1, 1, 7, 4};                                                 // ==-====+
static const char vlist3039[] = { 2, 2, 8, 4,13, 7, 4};                                        // ==-===+-
static const char vlist3040[] = { 1,13, 7, 4};                                                 // ==-===+=
static const char vlist3041[] = { 1,13, 7, 4};                                                 // ==-===++
static const char vlist3042[] = { 2, 3, 4, 9, 4, 3, 7};                                        // ==-==+--
static const char vlist3043[] = { 2, 0, 3, 4, 3, 7, 4};                                        // ==-==+-=
static const char vlist3044[] = { 2, 3, 7, 8, 8, 7, 4};                                        // ==-==+-+
static const char vlist3045[] = { 2, 9, 1, 4, 1, 7, 4};                                        // ==-==+=-
static const char vlist3046[] = { 1, 1, 7, 4};                                                 // ==-==+==
static const char vlist3047[] = { 1, 1, 7, 4};                                                 // ==-==+=+
static const char vlist3048[] = { 2, 9, 8, 4,13, 7, 4};                                        // ==-==++-
static const char vlist3049[] = { 1,13, 7, 4};                                                 // ==-==++=
static const char vlist3050[] = { 1,13, 7, 4};                                                 // ==-==+++
static const char vlist3051[] = { 3, 4, 6,11, 4,10, 7, 4,11,10};                               // ==-=+---
static const char vlist3052[] = { 3, 4,10, 7,10, 4, 0, 0,11, 6};                               // ==-=+--=
static const char vlist3053[] = { 3, 8,10, 7, 4, 8, 7,11, 9, 6};                               // ==-=+--+
static const char vlist3054[] = { 3,11, 4, 6, 4,11, 1, 4, 1, 7};                               // ==-=+-=-
static const char vlist3055[] = { 2, 1, 7, 4,11, 0, 6};                                        // ==-=+-==
static const char vlist3056[] = { 2, 1, 7, 4,11, 9, 6};                                        // ==-=+-=+
static const char vlist3057[] = { 3,11, 8, 6, 8, 4, 6, 4,13, 7};                               // ==-=+-+-
static const char vlist3058[] = { 2,11, 0, 6,13, 7, 4};                                        // ==-=+-+=
static const char vlist3059[] = { 2,11, 9, 6,13, 7, 4};                                        // ==-=+-++
static const char vlist3060[] = { 2, 4,10, 7,10, 4, 2};                                        // ==-=+=--
static const char vlist3061[] = { 2, 0,10, 4,10, 7, 4};                                        // ==-=+=-=
static const char vlist3062[] = { 2, 8, 7, 4,10, 7, 8};                                        // ==-=+=-+
static const char vlist3063[] = { 2, 2, 1, 4, 1, 7, 4};                                        // ==-=+==-
static const char vlist3064[] = { 1, 1, 7, 4};                                                 // ==-=+===
static const char vlist3065[] = { 1, 1, 7, 4};                                                 // ==-=+==+
static const char vlist3066[] = { 2, 2, 8, 4,13, 7, 4};                                        // ==-=+=+-
static const char vlist3067[] = { 1,13, 7, 4};                                                 // ==-=+=+=
static const char vlist3068[] = { 1,13, 7, 4};                                                 // ==-=+=++
static const char vlist3069[] = { 2, 4,10, 7,10, 4, 9};                                        // ==-=++--
static const char vlist3070[] = { 2, 0,10, 4,10, 7, 4};                                        // ==-=++-=
static const char vlist3071[] = { 2, 8, 7, 4,10, 7, 8};                                        // ==-=++-+
static const char vlist3072[] = { 2, 9, 1, 4, 1, 7, 4};                                        // ==-=++=-
static const char vlist3073[] = { 1, 1, 7, 4};                                                 // ==-=++==
static const char vlist3074[] = { 1, 1, 7, 4};                                                 // ==-=++=+
static const char vlist3075[] = { 2, 9, 8, 4,13, 7, 4};                                        // ==-=+++-
static const char vlist3076[] = { 1,13, 7, 4};                                                 // ==-=+++=
static const char vlist3077[] = { 1,13, 7, 4};                                                 // ==-=++++
static const char vlist3078[] = { 2, 6, 7,12, 7,16,12};                                        // ==-+----
static const char vlist3079[] = { 2,16, 0, 7, 0, 6, 7};                                        // ==-+---=
static const char vlist3080[] = { 3, 8, 9, 7,16, 8, 7, 9, 6, 7};                               // ==-+---+
static const char vlist3081[] = { 3,12, 6, 1, 1, 6, 7, 1,16, 7};                               // ==-+--=-
static const char vlist3082[] = { 3,16, 1, 7, 7, 1, 6, 1, 0, 6};                               // ==-+--==
static const char vlist3083[] = { 3,16, 1, 7, 7, 1, 9, 7, 9, 6};                               // ==-+--=+
static const char vlist3084[] = { 4,12, 6, 8, 8, 6,10,13,16, 7,10, 6, 7};                      // ==-+--+-
static const char vlist3085[] = { 3,10, 0, 6, 7,10, 6,16,13, 7};                               // ==-+--+=
static const char vlist3086[] = { 3, 7, 9, 6, 7,10, 9,16,13, 7};                               // ==-+--++
static const char vlist3087[] = { 2,12, 2, 7,16,12, 7};                                        // ==-+-=--
static const char vlist3088[] = { 2, 0, 2, 7,16, 0, 7};                                        // ==-+-=-=
static const char vlist3089[] = { 2, 8, 2, 7,16, 8, 7};                                        // ==-+-=-+
static const char vlist3090[] = { 3, 1, 2, 7,12, 2, 1,16, 1, 7};                               // ==-+-==-
static const char vlist3091[] = { 2, 1, 2, 7, 1, 7,16};                                        // ==-+-===
static const char vlist3092[] = { 2, 1, 2, 7, 1, 7,16};                                        // ==-+-==+
static const char vlist3093[] = { 3,10, 2, 7,12, 2, 8,16,13, 7};                               // ==-+-=+-
static const char vlist3094[] = { 2,10, 2, 7,13, 7,16};                                        // ==-+-=+=
static const char vlist3095[] = { 2,10, 2, 7,13, 7,16};                                        // ==-+-=++
static const char vlist3096[] = { 3, 9,11,12,12,11,16,16,11, 7};                               // ==-+-+--
static const char vlist3097[] = { 2, 0,11, 7,16, 0, 7};                                        // ==-+-+-=
static const char vlist3098[] = { 2,16, 8, 7, 8,11, 7};                                        // ==-+-+-+
static const char vlist3099[] = { 3,12, 9, 1,16, 1, 7, 1,11, 7};                               // ==-+-+=-
static const char vlist3100[] = { 2, 1,11, 7, 1, 7,16};                                        // ==-+-+==
static const char vlist3101[] = { 2, 1,11, 7, 1, 7,16};                                        // ==-+-+=+
static const char vlist3102[] = { 3,12, 9, 8,10,11, 7,16,13, 7};                               // ==-+-++-
static const char vlist3103[] = { 2,10,11, 7,13, 7,16};                                        // ==-+-++=
static const char vlist3104[] = { 2,10,11, 7,13, 7,16};                                        // ==-+-+++
static const char vlist3105[] = { 3,16,12, 3,16, 3, 7,12, 6, 3};                               // ==-+=---
static const char vlist3106[] = { 3,16, 3, 7, 3,16, 0, 0, 3, 6};                               // ==-+=--=
static const char vlist3107[] = { 3, 8, 3, 7, 3, 9, 6,16, 8, 7};                               // ==-+=--+
static const char vlist3108[] = { 3, 3,12, 6,12, 3, 1,16, 1, 7};                               // ==-+=-=-
static const char vlist3109[] = { 2, 1, 7,16, 3, 0, 6};                                        // ==-+=-==
static const char vlist3110[] = { 2, 1, 7,16, 3, 9, 6};                                        // ==-+=-=+
static const char vlist3111[] = { 3, 3, 8, 6,16,13, 7, 8,12, 6};                               // ==-+=-+-
static const char vlist3112[] = { 2, 3, 0, 6,13, 7,16};                                        // ==-+=-+=
static const char vlist3113[] = { 2, 3, 9, 6,13, 7,16};                                        // ==-+=-++
static const char vlist3114[] = { 3,16, 3, 7, 3,16,12, 3,12, 2};                               // ==-+==--
static const char vlist3115[] = { 2, 0, 3,16, 3, 7,16};                                        // ==-+==-=
static const char vlist3116[] = { 2, 3, 7, 8, 8, 7,16};                                        // ==-+==-+
static const char vlist3117[] = { 2, 2, 1,12, 1, 7,16};                                        // ==-+===-
static const char vlist3118[] = { 1, 1, 7,16};                                                 // ==-+====
static const char vlist3119[] = { 1, 1, 7,16};                                                 // ==-+===+
static const char vlist3120[] = { 2, 2, 8,12,13, 7,16};                                        // ==-+==+-
static const char vlist3121[] = { 1,13, 7,16};                                                 // ==-+==+=
static const char vlist3122[] = { 1,13, 7,16};                                                 // ==-+==++
static const char vlist3123[] = { 3,16, 3, 7, 3,16,12, 3,12, 9};                               // ==-+=+--
static const char vlist3124[] = { 2, 0, 3,16, 3, 7,16};                                        // ==-+=+-=
static const char vlist3125[] = { 2, 3, 7, 8, 8, 7,16};                                        // ==-+=+-+
static const char vlist3126[] = { 2, 9, 1,12, 1, 7,16};                                        // ==-+=+=-
static const char vlist3127[] = { 1, 1, 7,16};                                                 // ==-+=+==
static const char vlist3128[] = { 1, 1, 7,16};                                                 // ==-+=+=+
static const char vlist3129[] = { 2, 9, 8,12,13, 7,16};                                        // ==-+=++-
static const char vlist3130[] = { 1,13, 7,16};                                                 // ==-+=++=
static const char vlist3131[] = { 1,13, 7,16};                                                 // ==-+=+++
static const char vlist3132[] = { 4,16,12,10,16,10, 7,12, 6,11,12,11,10};                      // ==-++---
static const char vlist3133[] = { 3,10,16, 0,16,10, 7,11, 0, 6};                               // ==-++--=
static const char vlist3134[] = { 3,16,10, 7, 8,10,16,11, 9, 6};                               // ==-++--+
static const char vlist3135[] = { 3,12,11, 1,11,12, 6,16, 1, 7};                               // ==-++-=-
static const char vlist3136[] = { 2, 1, 7,16,11, 0, 6};                                        // ==-++-==
static const char vlist3137[] = { 2, 1, 7,16,11, 9, 6};                                        // ==-++-=+
static const char vlist3138[] = { 3,11,12, 6,11, 8,12,16,13, 7};                               // ==-++-+-
static const char vlist3139[] = { 2,11, 0, 6,13, 7,16};                                        // ==-++-+=
static const char vlist3140[] = { 2,11, 9, 6,13, 7,16};                                        // ==-++-++
static const char vlist3141[] = { 3,10,16,12,16,10, 7,10,12, 2};                               // ==-++=--
static const char vlist3142[] = { 2, 0,10,16,10, 7,16};                                        // ==-++=-=
static const char vlist3143[] = { 2,10, 7,16,10,16, 8};                                        // ==-++=-+
static const char vlist3144[] = { 2, 2, 1,12, 1, 7,16};                                        // ==-++==-
static const char vlist3145[] = { 1, 1, 7,16};                                                 // ==-++===
static const char vlist3146[] = { 1, 1, 7,16};                                                 // ==-++==+
static const char vlist3147[] = { 2, 2, 8,12,13, 7,16};                                        // ==-++=+-
static const char vlist3148[] = { 1,13, 7,16};                                                 // ==-++=+=
static const char vlist3149[] = { 1,13, 7,16};                                                 // ==-++=++
static const char vlist3150[] = { 3,10,16,12,16,10, 7,10,12, 9};                               // ==-+++--
static const char vlist3151[] = { 2, 0,10,16,10, 7,16};                                        // ==-+++-=
static const char vlist3152[] = { 2,10, 7,16,10,16, 8};                                        // ==-+++-+
static const char vlist3153[] = { 2, 9, 1,12, 1, 7,16};                                        // ==-+++=-
static const char vlist3154[] = { 1, 1, 7,16};                                                 // ==-+++==
static const char vlist3155[] = { 1, 1, 7,16};                                                 // ==-+++=+
static const char vlist3156[] = { 2, 9, 8,12,13, 7,16};                                        // ==-++++-
static const char vlist3157[] = { 1,13, 7,16};                                                 // ==-++++=
static const char vlist3158[] = { 1,13, 7,16};                                                 // ==-+++++
static const char vlist3159[] = { 1, 6, 7, 5};                                                 // ===-----
static const char vlist3160[] = { 3, 6, 5, 0, 5, 0, 7, 0, 6, 7};                               // ===----=
static const char vlist3161[] = { 4, 6, 5,12, 5, 8, 7, 8, 9, 7, 9, 6, 7};                      // ===----+
static const char vlist3162[] = { 2, 5, 6, 1, 1, 6, 7};                                        // ===---=-
static const char vlist3163[] = { 3, 7, 1, 6, 1, 0, 6, 5, 0, 6};                               // ===---==
static const char vlist3164[] = { 3, 7, 1, 9, 7, 9, 6, 5,12, 6};                               // ===---=+
static const char vlist3165[] = { 3, 5, 6, 8, 8, 6,10,10, 6, 7};                               // ===---+-
static const char vlist3166[] = { 3,10, 0, 6, 7,10, 6, 5, 0, 6};                               // ===---+=
static const char vlist3167[] = { 3, 7, 9, 6, 7,10, 9,12, 5, 6};                               // ===---++
static const char vlist3168[] = { 2, 5, 6, 2, 5, 2, 7};                                        // ===--=--
static const char vlist3169[] = { 3, 2, 5, 0, 5, 2, 7, 5, 0, 6};                               // ===--=-=
static const char vlist3170[] = { 3, 8, 2, 7, 5, 8, 7, 5,12, 6};                               // ===--=-+
static const char vlist3171[] = { 3, 5, 2, 1, 2, 5, 6, 2, 1, 7};                               // ===--==-
static const char vlist3172[] = { 2, 2, 1, 7, 6, 0, 5};                                        // ===--===
static const char vlist3173[] = { 2, 1, 2, 7, 6,12, 5};                                        // ===--==+
static const char vlist3174[] = { 3, 2, 8, 6,10, 2, 7, 8, 5, 6};                               // ===--=+-
static const char vlist3175[] = { 2, 6, 0, 5,10, 2, 7};                                        // ===--=+=
static const char vlist3176[] = { 2, 6,12, 5,10, 2, 7};                                        // ===--=++
static const char vlist3177[] = { 3, 5, 6, 9, 5,11, 7, 5, 9,11};                               // ===--+--
static const char vlist3178[] = { 3, 5,11, 7,11, 5, 0, 5, 0, 6};                               // ===--+-=
static const char vlist3179[] = { 3, 8,11, 7, 5, 8, 7,12, 5, 6};                               // ===--+-+
static const char vlist3180[] = { 3, 9, 5, 6, 5, 9, 1, 1,11, 7};                               // ===--+=-
static const char vlist3181[] = { 2, 1,11, 7, 6, 0, 5};                                        // ===--+==
static const char vlist3182[] = { 2, 1,11, 7, 6,12, 5};                                        // ===--+=+
static const char vlist3183[] = { 3, 9, 8, 6, 8, 5, 6,10,11, 7};                               // ===--++-
static const char vlist3184[] = { 2, 6, 0, 5,10,11, 7};                                        // ===--++=
static const char vlist3185[] = { 2, 6,12, 5,10,11, 7};                                        // ===--+++
static const char vlist3186[] = { 1, 5, 6, 3};                                                 // ===-=---
static const char vlist3187[] = { 3, 5, 3, 0, 0, 3, 6, 5, 0, 6};                               // ===-=--=
static const char vlist3188[] = { 3, 5, 3, 8, 9, 3, 6, 5,12, 6};                               // ===-=--+
static const char vlist3189[] = { 2, 3, 1, 6, 1, 5, 6};                                        // ===-=-=-
static const char vlist3190[] = { 2, 6, 0, 5, 3, 0, 6};                                        // ===-=-==
static const char vlist3191[] = { 2, 3, 9, 6, 6,12, 5};                                        // ===-=-=+
static const char vlist3192[] = { 2, 3, 8, 6, 8, 5, 6};                                        // ===-=-+-
static const char vlist3193[] = { 2, 6, 0, 5, 3, 0, 6};                                        // ===-=-+=
static const char vlist3194[] = { 2, 3, 9, 6, 6,12, 5};                                        // ===-=-++
static const char vlist3195[] = { 2, 3, 5, 2, 2, 5, 6};                                        // ===-==--
static const char vlist3196[] = { 2, 0, 6, 5, 3, 0, 5};                                        // ===-==-=
static const char vlist3197[] = { 2, 6,12, 5, 8, 3, 5};                                        // ===-==-+
static const char vlist3198[] = { 2, 6, 2, 5, 2, 1, 5};                                        // ===-===-
static const char vlist3199[] = { 1, 6, 0, 5};                                                 // ===-====
static const char vlist3200[] = { 1, 6,12, 5};                                                 // ===-===+
static const char vlist3201[] = { 2, 6, 2, 8, 6, 8, 5};                                        // ===-==+-
static const char vlist3202[] = { 1, 6, 0, 5};                                                 // ===-==+=
static const char vlist3203[] = { 1, 6,12, 5};                                                 // ===-==++
static const char vlist3204[] = { 2, 3, 5, 9, 9, 5, 6};                                        // ===-=+--
static const char vlist3205[] = { 2, 6, 0, 5, 0, 3, 5};                                        // ===-=+-=
static const char vlist3206[] = { 2, 6,12, 5, 8, 3, 5};                                        // ===-=+-+
static const char vlist3207[] = { 2, 9, 1, 5, 6, 9, 5};                                        // ===-=+=-
static const char vlist3208[] = { 1, 6, 0, 5};                                                 // ===-=+==
static const char vlist3209[] = { 1, 6,12, 5};                                                 // ===-=+=+
static const char vlist3210[] = { 2, 6, 8, 5, 6, 9, 8};                                        // ===-=++-
static const char vlist3211[] = { 1, 6, 0, 5};                                                 // ===-=++=
static const char vlist3212[] = { 1, 6,12, 5};                                                 // ===-=+++
static const char vlist3213[] = { 2, 5, 6,10,10, 6,11};                                        // ===-+---
static const char vlist3214[] = { 3, 5,10, 0, 5, 0, 6, 0,11, 6};                               // ===-+--=
static const char vlist3215[] = { 3,10, 5, 8,11, 9, 6,12, 5, 6};                               // ===-+--+
static const char vlist3216[] = { 2,11, 1, 6, 1, 5, 6};                                        // ===-+-=-
static const char vlist3217[] = { 2, 6, 0, 5,11, 0, 6};                                        // ===-+-==
static const char vlist3218[] = { 2,11, 9, 6, 6,12, 5};                                        // ===-+-=+
static const char vlist3219[] = { 2, 8, 5, 6,11, 8, 6};                                        // ===-+-+-
static const char vlist3220[] = { 2, 6, 0, 5,11, 0, 6};                                        // ===-+-+=
static const char vlist3221[] = { 2,11, 9, 6, 6,12, 5};                                        // ===-+-++
static const char vlist3222[] = { 2,10, 5, 2, 2, 5, 6};                                        // ===-+=--
static const char vlist3223[] = { 2, 6, 0, 5, 0,10, 5};                                        // ===-+=-=
static const char vlist3224[] = { 2, 6,12, 5, 8,10, 5};                                        // ===-+=-+
static const char vlist3225[] = { 2, 6, 2, 5, 2, 1, 5};                                        // ===-+==-
static const char vlist3226[] = { 1, 6, 0, 5};                                                 // ===-+===
static const char vlist3227[] = { 1, 6,12, 5};                                                 // ===-+==+
static const char vlist3228[] = { 2, 6, 2, 8, 6, 8, 5};                                        // ===-+=+-
static const char vlist3229[] = { 1, 6, 0, 5};                                                 // ===-+=+=
static const char vlist3230[] = { 1, 6,12, 5};                                                 // ===-+=++
static const char vlist3231[] = { 2,10, 5, 9, 9, 5, 6};                                        // ===-++--
static const char vlist3232[] = { 2, 6, 0, 5, 0,10, 5};                                        // ===-++-=
static const char vlist3233[] = { 2, 6,12, 5, 8,10, 5};                                        // ===-++-+
static const char vlist3234[] = { 2, 9, 1, 5, 6, 9, 5};                                        // ===-++=-
static const char vlist3235[] = { 1, 6, 0, 5};                                                 // ===-++==
static const char vlist3236[] = { 1, 6,12, 5};                                                 // ===-++=+
static const char vlist3237[] = { 2, 6, 8, 5, 6, 9, 8};                                        // ===-+++-
static const char vlist3238[] = { 1, 6, 0, 5};                                                 // ===-+++=
static const char vlist3239[] = { 1, 6,12, 5};                                                 // ===-++++
static const char vlist3240[] = { 2, 6, 5, 4, 5, 6, 7};                                        // ====----
static const char vlist3241[] = { 2, 7, 0, 6, 5, 0, 7};                                        // ====---=
static const char vlist3242[] = { 3, 5, 8, 7, 8, 9, 7, 7, 9, 6};                               // ====---+
static const char vlist3243[] = { 2, 1, 4, 6, 7, 1, 6};                                        // ====--=-
static const char vlist3244[] = { 2, 7, 1, 6, 1, 0, 6};                                        // ====--==
static const char vlist3245[] = { 2, 7, 1, 9, 7, 9, 6};                                        // ====--=+
static const char vlist3246[] = { 3, 8, 4, 6,10, 8, 6, 7,10, 6};                               // ====--+-
static const char vlist3247[] = { 2,10, 0, 6, 7,10, 6};                                        // ====--+=
static const char vlist3248[] = { 2, 7, 9, 6, 7,10, 9};                                        // ====--++
static const char vlist3249[] = { 2, 5, 4, 2, 5, 2, 7};                                        // ====-=--
static const char vlist3250[] = { 2, 2, 7, 5, 0, 2, 5};                                        // ====-=-=
static const char vlist3251[] = { 2, 2, 7, 8, 8, 7, 5};                                        // ====-=-+
static const char vlist3252[] = { 2, 4, 2, 1, 1, 2, 7};                                        // ====-==-
static const char vlist3253[] = { 1, 1, 2, 7};                                                 // ====-===
static const char vlist3254[] = { 1, 1, 2, 7};                                                 // ====-==+
static const char vlist3255[] = { 2, 4, 2, 8,10, 2, 7};                                        // ====-=+-
static const char vlist3256[] = { 1,10, 2, 7};                                                 // ====-=+=
static const char vlist3257[] = { 1,10, 2, 7};                                                 // ====-=++
static const char vlist3258[] = { 3, 5, 4, 9, 5, 9,11, 5,11, 7};                               // ====-+--
static const char vlist3259[] = { 2, 0,11, 5,11, 7, 5};                                        // ====-+-=
static const char vlist3260[] = { 2, 8, 7, 5,11, 7, 8};                                        // ====-+-+
static const char vlist3261[] = { 2, 4, 9, 1, 1,11, 7};                                        // ====-+=-
static const char vlist3262[] = { 1, 1,11, 7};                                                 // ====-+==
static const char vlist3263[] = { 1, 1,11, 7};                                                 // ====-+=+
static const char vlist3264[] = { 2, 4, 9, 8,10,11, 7};                                        // ====-++-
static const char vlist3265[] = { 1,10,11, 7};                                                 // ====-++=
static const char vlist3266[] = { 1,10,11, 7};                                                 // ====-+++
static const char vlist3267[] = { 2, 5, 4, 3, 3, 4, 6};                                        // =====---
static const char vlist3268[] = { 2, 6, 3, 0, 0, 3, 5};                                        // =====--=
static const char vlist3269[] = { 2, 6, 3, 9, 8, 3, 5};                                        // =====--+
static const char vlist3270[] = { 2, 4, 3, 1, 3, 4, 6};                                        // =====-=-
static const char vlist3271[] = { 1, 3, 0, 6};                                                 // =====-==
static const char vlist3272[] = { 1, 3, 9, 6};                                                 // =====-=+
static const char vlist3273[] = { 2, 3, 8, 6, 8, 4, 6};                                        // =====-+-
static const char vlist3274[] = { 1, 3, 0, 6};                                                 // =====-+=
static const char vlist3275[] = { 1, 3, 9, 6};                                                 // =====-++
static const char vlist3276[] = { 2, 3, 4, 2, 4, 3, 5};                                        // ======--
static const char vlist3277[] = { 1, 0, 3, 5};                                                 // ======-=
static const char vlist3278[] = { 1, 8, 3, 5};                                                 // ======-+
static const char vlist3279[] = { 1, 2, 1, 4};                                                 // =======-
                                                                                               // ========
                                                                                               // =======+
static const char vlist3282[] = { 1, 2, 8, 4};                                                 // ======+-
                                                                                               // ======+=
                                                                                               // ======++
static const char vlist3285[] = { 2, 9, 3, 5, 4, 9, 5};                                        // =====+--
static const char vlist3286[] = { 1, 0, 3, 5};                                                 // =====+-=
static const char vlist3287[] = { 1, 8, 3, 5};                                                 // =====+-+
static const char vlist3288[] = { 1, 9, 1, 4};                                                 // =====+=-
                                                                                               // =====+==
                                                                                               // =====+=+
static const char vlist3291[] = { 1, 9, 8, 4};                                                 // =====++-
                                                                                               // =====++=
                                                                                               // =====+++
static const char vlist3294[] = { 3, 5, 4,10,11, 4, 6,10, 4,11};                               // ====+---
static const char vlist3295[] = { 2, 6,11, 0, 0,10, 5};                                        // ====+--=
static const char vlist3296[] = { 2, 6,11, 9, 8,10, 5};                                        // ====+--+
static const char vlist3297[] = { 2,11, 4, 6, 4,11, 1};                                        // ====+-=-
static const char vlist3298[] = { 1,11, 0, 6};                                                 // ====+-==
static const char vlist3299[] = { 1,11, 9, 6};                                                 // ====+-=+
static const char vlist3300[] = { 2,11, 8, 6, 8, 4, 6};                                        // ====+-+-
static const char vlist3301[] = { 1,11, 0, 6};                                                 // ====+-+=
static const char vlist3302[] = { 1,11, 9, 6};                                                 // ====+-++
static const char vlist3303[] = { 2, 4,10, 5,10, 4, 2};                                        // ====+=--
static const char vlist3304[] = { 1, 0,10, 5};                                                 // ====+=-=
static const char vlist3305[] = { 1, 8,10, 5};                                                 // ====+=-+
static const char vlist3306[] = { 1, 2, 1, 4};                                                 // ====+==-
                                                                                               // ====+===
                                                                                               // ====+==+
static const char vlist3309[] = { 1, 2, 8, 4};                                                 // ====+=+-
                                                                                               // ====+=+=
                                                                                               // ====+=++
static const char vlist3312[] = { 2, 9,10, 5, 4, 9, 5};                                        // ====++--
static const char vlist3313[] = { 1, 0,10, 5};                                                 // ====++-=
static const char vlist3314[] = { 1, 8,10, 5};                                                 // ====++-+
static const char vlist3315[] = { 1, 9, 1, 4};                                                 // ====++=-
                                                                                               // ====++==
                                                                                               // ====++=+
static const char vlist3318[] = { 1, 9, 8, 4};                                                 // ====+++-
                                                                                               // ====+++=
                                                                                               // ====++++
static const char vlist3321[] = { 2, 7, 5,12, 6, 7,12};                                        // ===+----
static const char vlist3322[] = { 2, 7, 0, 6, 5, 0, 7};                                        // ===+---=
static const char vlist3323[] = { 3, 5, 8, 7, 8, 9, 7, 7, 9, 6};                               // ===+---+
static const char vlist3324[] = { 2, 1,12, 6, 7, 1, 6};                                        // ===+--=-
static const char vlist3325[] = { 2, 7, 1, 6, 1, 0, 6};                                        // ===+--==
static const char vlist3326[] = { 2, 7, 1, 9, 7, 9, 6};                                        // ===+--=+
static const char vlist3327[] = { 3,10, 8, 6, 8,12, 6, 7,10, 6};                               // ===+--+-
static const char vlist3328[] = { 2,10, 0, 6, 7,10, 6};                                        // ===+--+=
static const char vlist3329[] = { 2, 7, 9, 6, 7,10, 9};                                        // ===+--++
static const char vlist3330[] = { 2, 5,12, 2, 5, 2, 7};                                        // ===+-=--
static const char vlist3331[] = { 2, 2, 7, 5, 0, 2, 5};                                        // ===+-=-=
static const char vlist3332[] = { 2, 2, 7, 8, 8, 7, 5};                                        // ===+-=-+
static const char vlist3333[] = { 2, 1, 2, 7,12, 2, 1};                                        // ===+-==-
static const char vlist3334[] = { 1, 1, 2, 7};                                                 // ===+-===
static const char vlist3335[] = { 1, 1, 2, 7};                                                 // ===+-==+
static const char vlist3336[] = { 2,10, 2, 7,12, 2, 8};                                        // ===+-=+-
static const char vlist3337[] = { 1,10, 2, 7};                                                 // ===+-=+=
static const char vlist3338[] = { 1,10, 2, 7};                                                 // ===+-=++
static const char vlist3339[] = { 3, 5,12, 9, 5, 9,11, 5,11, 7};                               // ===+-+--
static const char vlist3340[] = { 2, 0,11, 5,11, 7, 5};                                        // ===+-+-=
static const char vlist3341[] = { 2, 8, 7, 5,11, 7, 8};                                        // ===+-+-+
static const char vlist3342[] = { 2,12, 9, 1, 1,11, 7};                                        // ===+-+=-
static const char vlist3343[] = { 1, 1,11, 7};                                                 // ===+-+==
static const char vlist3344[] = { 1, 1,11, 7};                                                 // ===+-+=+
static const char vlist3345[] = { 2,12, 9, 8,10,11, 7};                                        // ===+-++-
static const char vlist3346[] = { 1,10,11, 7};                                                 // ===+-++=
static const char vlist3347[] = { 1,10,11, 7};                                                 // ===+-+++
static const char vlist3348[] = { 2, 3,12, 6, 5,12, 3};                                        // ===+=---
static const char vlist3349[] = { 2, 6, 3, 0, 0, 3, 5};                                        // ===+=--=
static const char vlist3350[] = { 2, 6, 3, 9, 8, 3, 5};                                        // ===+=--+
static const char vlist3351[] = { 2, 3,12, 6,12, 3, 1};                                        // ===+=-=-
static const char vlist3352[] = { 1, 3, 0, 6};                                                 // ===+=-==
static const char vlist3353[] = { 1, 3, 9, 6};                                                 // ===+=-=+
static const char vlist3354[] = { 2, 3, 8, 6, 8,12, 6};                                        // ===+=-+-
static const char vlist3355[] = { 1, 3, 0, 6};                                                 // ===+=-+=
static const char vlist3356[] = { 1, 3, 9, 6};                                                 // ===+=-++
static const char vlist3357[] = { 2,12, 3, 5, 3,12, 2};                                        // ===+==--
static const char vlist3358[] = { 1, 0, 3, 5};                                                 // ===+==-=
static const char vlist3359[] = { 1, 8, 3, 5};                                                 // ===+==-+
static const char vlist3360[] = { 1, 2, 1,12};                                                 // ===+===-
                                                                                               // ===+====
                                                                                               // ===+===+
static const char vlist3363[] = { 1, 2, 8,12};                                                 // ===+==+-
                                                                                               // ===+==+=
                                                                                               // ===+==++
static const char vlist3366[] = { 2, 9, 3, 5,12, 9, 5};                                        // ===+=+--
static const char vlist3367[] = { 1, 0, 3, 5};                                                 // ===+=+-=
static const char vlist3368[] = { 1, 8, 3, 5};                                                 // ===+=+-+
static const char vlist3369[] = { 1, 9, 1,12};                                                 // ===+=+=-
                                                                                               // ===+=+==
                                                                                               // ===+=+=+
static const char vlist3372[] = { 1, 9, 8,12};                                                 // ===+=++-
                                                                                               // ===+=++=
                                                                                               // ===+=+++
static const char vlist3375[] = { 3,11,12, 6,10,12,11, 5,12,10};                               // ===++---
static const char vlist3376[] = { 2, 6,11, 0, 0,10, 5};                                        // ===++--=
static const char vlist3377[] = { 2, 6,11, 9, 8,10, 5};                                        // ===++--+
static const char vlist3378[] = { 2,12,11, 1,11,12, 6};                                        // ===++-=-
static const char vlist3379[] = { 1,11, 0, 6};                                                 // ===++-==
static const char vlist3380[] = { 1,11, 9, 6};                                                 // ===++-=+
static const char vlist3381[] = { 2,11,12, 6,11, 8,12};                                        // ===++-+-
static const char vlist3382[] = { 1,11, 0, 6};                                                 // ===++-+=
static const char vlist3383[] = { 1,11, 9, 6};                                                 // ===++-++
static const char vlist3384[] = { 2,10,12, 2,12,10, 5};                                        // ===++=--
static const char vlist3385[] = { 1, 0,10, 5};                                                 // ===++=-=
static const char vlist3386[] = { 1, 8,10, 5};                                                 // ===++=-+
static const char vlist3387[] = { 1, 2, 1,12};                                                 // ===++==-
                                                                                               // ===++===
                                                                                               // ===++==+
static const char vlist3390[] = { 1, 2, 8,12};                                                 // ===++=+-
                                                                                               // ===++=+=
                                                                                               // ===++=++
static const char vlist3393[] = { 2,12,10, 5, 9,10,12};                                        // ===+++--
static const char vlist3394[] = { 1, 0,10, 5};                                                 // ===+++-=
static const char vlist3395[] = { 1, 8,10, 5};                                                 // ===+++-+
static const char vlist3396[] = { 1, 9, 1,12};                                                 // ===+++=-
                                                                                               // ===+++==
                                                                                               // ===+++=+
static const char vlist3399[] = { 1, 9, 8,12};                                                 // ===++++-
                                                                                               // ===++++=
                                                                                               // ===+++++
static const char vlist3402[] = { 2, 6, 7,13,16, 6,13};                                        // ==+-----
static const char vlist3403[] = { 3, 6,16, 0, 0, 6, 7,13, 0, 7};                               // ==+----=
static const char vlist3404[] = { 4, 6,16,12, 8, 9, 7,13, 8, 7, 9, 6, 7};                      // ==+----+
static const char vlist3405[] = { 2,16, 6, 1, 1, 6, 7};                                        // ==+---=-
static const char vlist3406[] = { 3, 7, 1, 6, 1, 0, 6,16, 0, 6};                               // ==+---==
static const char vlist3407[] = { 3, 7, 1, 9, 7, 9, 6,12,16, 6};                               // ==+---=+
static const char vlist3408[] = { 3,16, 6, 8, 8, 6,10,10, 6, 7};                               // ==+---+-
static const char vlist3409[] = { 3,10, 0, 6, 7,10, 6, 0,16, 6};                               // ==+---+=
static const char vlist3410[] = { 3, 7, 9, 6, 7,10, 9,12,16, 6};                               // ==+---++
static const char vlist3411[] = { 3,16, 2,13,16, 6, 2,13, 2, 7};                               // ==+--=--
static const char vlist3412[] = { 3,13, 2, 7, 2,13, 0,16, 0, 6};                               // ==+--=-=
static const char vlist3413[] = { 3, 8, 2, 7,12,16, 6,13, 8, 7};                               // ==+--=-+
static const char vlist3414[] = { 3, 2,16, 6,16, 2, 1, 1, 2, 7};                               // ==+--==-
static const char vlist3415[] = { 2, 1, 2, 7, 6, 0,16};                                        // ==+--===
static const char vlist3416[] = { 2, 1, 2, 7, 6,12,16};                                        // ==+--==+
static const char vlist3417[] = { 3, 2, 8, 6,10, 2, 7, 8,16, 6};                               // ==+--=+-
static const char vlist3418[] = { 2, 6, 0,16,10, 2, 7};                                        // ==+--=+=
static const char vlist3419[] = { 2, 6,12,16,10, 2, 7};                                        // ==+--=++
static const char vlist3420[] = { 4,16, 9,13,16, 6, 9,13, 9,11,13,11, 7};                      // ==+--+--
static const char vlist3421[] = { 3,11,13, 0,13,11, 7, 0,16, 6};                               // ==+--+-=
static const char vlist3422[] = { 3,13,11, 7, 8,11,13,12,16, 6};                               // ==+--+-+
static const char vlist3423[] = { 3,16, 9, 1, 9,16, 6, 1,11, 7};                               // ==+--+=-
static const char vlist3424[] = { 2, 1,11, 7, 6, 0,16};                                        // ==+--+==
static const char vlist3425[] = { 2, 1,11, 7, 6,12,16};                                        // ==+--+=+
static const char vlist3426[] = { 3, 9,16, 6, 9, 8,16,10,11, 7};                               // ==+--++-
static const char vlist3427[] = { 2, 6, 0,16,10,11, 7};                                        // ==+--++=
static const char vlist3428[] = { 2, 6,12,16,10,11, 7};                                        // ==+--+++
static const char vlist3429[] = { 2,16, 6,13,13, 6, 3};                                        // ==+-=---
static const char vlist3430[] = { 3, 0, 3, 6,13, 3, 0,16, 0, 6};                               // ==+-=--=
static const char vlist3431[] = { 3, 3, 9, 6, 3,13, 8,12,16, 6};                               // ==+-=--+
static const char vlist3432[] = { 2, 3, 1, 6, 1,16, 6};                                        // ==+-=-=-
static const char vlist3433[] = { 2, 6, 0,16, 3, 0, 6};                                        // ==+-=-==
static const char vlist3434[] = { 2, 3, 9, 6, 6,12,16};                                        // ==+-=-=+
static const char vlist3435[] = { 2, 3, 8, 6, 8,16, 6};                                        // ==+-=-+-
static const char vlist3436[] = { 2, 6, 0,16, 3, 0, 6};                                        // ==+-=-+=
static const char vlist3437[] = { 2, 3, 9, 6, 6,12,16};                                        // ==+-=-++
static const char vlist3438[] = { 3, 3,13, 2,13,16, 2, 2,16, 6};                               // ==+-==--
static const char vlist3439[] = { 2, 6, 0,16, 0, 3,13};                                        // ==+-==-=
static const char vlist3440[] = { 2, 6,12,16, 8, 3,13};                                        // ==+-==-+
static const char vlist3441[] = { 2, 6, 2,16, 2, 1,16};                                        // ==+-===-
static const char vlist3442[] = { 1, 6, 0,16};                                                 // ==+-====
static const char vlist3443[] = { 1, 6,12,16};                                                 // ==+-===+
static const char vlist3444[] = { 2, 6, 2, 8, 6, 8,16};                                        // ==+-==+-
static const char vlist3445[] = { 1, 6, 0,16};                                                 // ==+-==+=
static const char vlist3446[] = { 1, 6,12,16};                                                 // ==+-==++
static const char vlist3447[] = { 3, 3,13, 9,13,16, 9, 9,16, 6};                               // ==+-=+--
static const char vlist3448[] = { 2, 6, 0,16, 0, 3,13};                                        // ==+-=+-=
static const char vlist3449[] = { 2, 6,12,16, 8, 3,13};                                        // ==+-=+-+
static const char vlist3450[] = { 2, 9, 1,16, 6, 9,16};                                        // ==+-=+=-
static const char vlist3451[] = { 1, 6, 0,16};                                                 // ==+-=+==
static const char vlist3452[] = { 1, 6,12,16};                                                 // ==+-=+=+
static const char vlist3453[] = { 2,16, 9, 8, 6, 9,16};                                        // ==+-=++-
static const char vlist3454[] = { 1, 6, 0,16};                                                 // ==+-=++=
static const char vlist3455[] = { 1, 6,12,16};                                                 // ==+-=+++
static const char vlist3456[] = { 3,13,16,11,10,13,11,16, 6,11};                               // ==+-+---
static const char vlist3457[] = { 3,10,13, 0, 0,16, 6,11, 0, 6};                               // ==+-+--=
static const char vlist3458[] = { 3,10,13, 8,11, 9, 6,12,16, 6};                               // ==+-+--+
static const char vlist3459[] = { 2,11, 1, 6, 1,16, 6};                                        // ==+-+-=-
static const char vlist3460[] = { 2, 6, 0,16,11, 0, 6};                                        // ==+-+-==
static const char vlist3461[] = { 2,11, 9, 6, 6,12,16};                                        // ==+-+-=+
static const char vlist3462[] = { 2, 8,16, 6,11, 8, 6};                                        // ==+-+-+-
static const char vlist3463[] = { 2, 6, 0,16,11, 0, 6};                                        // ==+-+-+=
static const char vlist3464[] = { 2,11, 9, 6, 6,12,16};                                        // ==+-+-++
static const char vlist3465[] = { 3,10,13, 2,13,16, 2, 2,16, 6};                               // ==+-+=--
static const char vlist3466[] = { 2, 6, 0,16, 0,10,13};                                        // ==+-+=-=
static const char vlist3467[] = { 2, 6,12,16, 8,10,13};                                        // ==+-+=-+
static const char vlist3468[] = { 2, 6, 2,16, 2, 1,16};                                        // ==+-+==-
static const char vlist3469[] = { 1, 6, 0,16};                                                 // ==+-+===
static const char vlist3470[] = { 1, 6,12,16};                                                 // ==+-+==+
static const char vlist3471[] = { 2, 6, 2, 8, 6, 8,16};                                        // ==+-+=+-
static const char vlist3472[] = { 1, 6, 0,16};                                                 // ==+-+=+=
static const char vlist3473[] = { 1, 6,12,16};                                                 // ==+-+=++
static const char vlist3474[] = { 3,10,13, 9,13,16, 9, 9,16, 6};                               // ==+-++--
static const char vlist3475[] = { 2, 6, 0,16, 0,10,13};                                        // ==+-++-=
static const char vlist3476[] = { 2, 6,12,16, 8,10,13};                                        // ==+-++-+
static const char vlist3477[] = { 2, 9, 1,16, 6, 9,16};                                        // ==+-++=-
static const char vlist3478[] = { 1, 6, 0,16};                                                 // ==+-++==
static const char vlist3479[] = { 1, 6,12,16};                                                 // ==+-++=+
static const char vlist3480[] = { 2,16, 9, 8, 6, 9,16};                                        // ==+-+++-
static const char vlist3481[] = { 1, 6, 0,16};                                                 // ==+-+++=
static const char vlist3482[] = { 1, 6,12,16};                                                 // ==+-++++
static const char vlist3483[] = { 2, 4, 6,13, 6, 7,13};                                        // ==+=----
static const char vlist3484[] = { 2, 7, 0, 6,13, 0, 7};                                        // ==+=---=
static const char vlist3485[] = { 3,13, 8, 7, 8, 9, 7, 7, 9, 6};                               // ==+=---+
static const char vlist3486[] = { 2, 1, 4, 6, 7, 1, 6};                                        // ==+=--=-
static const char vlist3487[] = { 2, 7, 1, 6, 1, 0, 6};                                        // ==+=--==
static const char vlist3488[] = { 2, 7, 1, 9, 7, 9, 6};                                        // ==+=--=+
static const char vlist3489[] = { 3, 8, 4, 6,10, 8, 6, 7,10, 6};                               // ==+=--+-
static const char vlist3490[] = { 2,10, 0, 6, 7,10, 6};                                        // ==+=--+=
static const char vlist3491[] = { 2, 7, 9, 6, 7,10, 9};                                        // ==+=--++
static const char vlist3492[] = { 2,13, 4, 2,13, 2, 7};                                        // ==+=-=--
static const char vlist3493[] = { 2, 2, 7,13, 0, 2,13};                                        // ==+=-=-=
static const char vlist3494[] = { 2, 2, 7, 8, 8, 7,13};                                        // ==+=-=-+
static const char vlist3495[] = { 2, 4, 2, 1, 1, 2, 7};                                        // ==+=-==-
static const char vlist3496[] = { 1, 1, 2, 7};                                                 // ==+=-===
static const char vlist3497[] = { 1, 1, 2, 7};                                                 // ==+=-==+
static const char vlist3498[] = { 2, 4, 2, 8,10, 2, 7};                                        // ==+=-=+-
static const char vlist3499[] = { 1,10, 2, 7};                                                 // ==+=-=+=
static const char vlist3500[] = { 1,10, 2, 7};                                                 // ==+=-=++
static const char vlist3501[] = { 3,13, 4, 9,13, 9,11,13,11, 7};                               // ==+=-+--
static const char vlist3502[] = { 2, 0,11,13,11, 7,13};                                        // ==+=-+-=
static const char vlist3503[] = { 2,11,13, 8,11, 7,13};                                        // ==+=-+-+
static const char vlist3504[] = { 2, 4, 9, 1, 1,11, 7};                                        // ==+=-+=-
static const char vlist3505[] = { 1, 1,11, 7};                                                 // ==+=-+==
static const char vlist3506[] = { 1, 1,11, 7};                                                 // ==+=-+=+
static const char vlist3507[] = { 2, 4, 9, 8,10,11, 7};                                        // ==+=-++-
static const char vlist3508[] = { 1,10,11, 7};                                                 // ==+=-++=
static const char vlist3509[] = { 1,10,11, 7};                                                 // ==+=-+++
static const char vlist3510[] = { 2, 3, 4, 6,13, 4, 3};                                        // ==+==---
static const char vlist3511[] = { 2, 6, 3, 0, 0, 3,13};                                        // ==+==--=
static const char vlist3512[] = { 2, 6, 3, 9, 8, 3,13};                                        // ==+==--+
static const char vlist3513[] = { 2, 4, 3, 1, 3, 4, 6};                                        // ==+==-=-
static const char vlist3514[] = { 1, 3, 0, 6};                                                 // ==+==-==
static const char vlist3515[] = { 1, 3, 9, 6};                                                 // ==+==-=+
static const char vlist3516[] = { 2, 3, 8, 6, 8, 4, 6};                                        // ==+==-+-
static const char vlist3517[] = { 1, 3, 0, 6};                                                 // ==+==-+=
static const char vlist3518[] = { 1, 3, 9, 6};                                                 // ==+==-++
static const char vlist3519[] = { 2, 3,13, 2,13, 4, 2};                                        // ==+===--
static const char vlist3520[] = { 1, 0, 3,13};                                                 // ==+===-=
static const char vlist3521[] = { 1, 8, 3,13};                                                 // ==+===-+
static const char vlist3522[] = { 1, 2, 1, 4};                                                 // ==+====-
                                                                                               // ==+=====
                                                                                               // ==+====+
static const char vlist3525[] = { 1, 2, 8, 4};                                                 // ==+===+-
                                                                                               // ==+===+=
                                                                                               // ==+===++
static const char vlist3528[] = { 2, 9, 3,13, 4, 9,13};                                        // ==+==+--
static const char vlist3529[] = { 1, 0, 3,13};                                                 // ==+==+-=
static const char vlist3530[] = { 1, 8, 3,13};                                                 // ==+==+-+
static const char vlist3531[] = { 1, 9, 1, 4};                                                 // ==+==+=-
                                                                                               // ==+==+==
                                                                                               // ==+==+=+
static const char vlist3534[] = { 1, 9, 8, 4};                                                 // ==+==++-
                                                                                               // ==+==++=
                                                                                               // ==+==+++
static const char vlist3537[] = { 3,11, 4, 6,10, 4,11,13, 4,10};                               // ==+=+---
static const char vlist3538[] = { 2, 6,11, 0, 0,10,13};                                        // ==+=+--=
static const char vlist3539[] = { 2, 6,11, 9, 8,10,13};                                        // ==+=+--+
static const char vlist3540[] = { 2,11, 4, 6, 4,11, 1};                                        // ==+=+-=-
static const char vlist3541[] = { 1,11, 0, 6};                                                 // ==+=+-==
static const char vlist3542[] = { 1,11, 9, 6};                                                 // ==+=+-=+
static const char vlist3543[] = { 2,11, 8, 6, 8, 4, 6};                                        // ==+=+-+-
static const char vlist3544[] = { 1,11, 0, 6};                                                 // ==+=+-+=
static const char vlist3545[] = { 1,11, 9, 6};                                                 // ==+=+-++
static const char vlist3546[] = { 2, 4,10,13,10, 4, 2};                                        // ==+=+=--
static const char vlist3547[] = { 1, 0,10,13};                                                 // ==+=+=-=
static const char vlist3548[] = { 1, 8,10,13};                                                 // ==+=+=-+
static const char vlist3549[] = { 1, 2, 1, 4};                                                 // ==+=+==-
                                                                                               // ==+=+===
                                                                                               // ==+=+==+
static const char vlist3552[] = { 1, 2, 8, 4};                                                 // ==+=+=+-
                                                                                               // ==+=+=+=
                                                                                               // ==+=+=++
static const char vlist3555[] = { 2, 9,10,13, 4, 9,13};                                        // ==+=++--
static const char vlist3556[] = { 1, 0,10,13};                                                 // ==+=++-=
static const char vlist3557[] = { 1, 8,10,13};                                                 // ==+=++-+
static const char vlist3558[] = { 1, 9, 1, 4};                                                 // ==+=++=-
                                                                                               // ==+=++==
                                                                                               // ==+=++=+
static const char vlist3561[] = { 1, 9, 8, 4};                                                 // ==+=+++-
                                                                                               // ==+=+++=
                                                                                               // ==+=++++
static const char vlist3564[] = { 2, 6, 7,12, 7,13,12};                                        // ==++----
static const char vlist3565[] = { 2, 7, 0, 6,13, 0, 7};                                        // ==++---=
static const char vlist3566[] = { 3,13, 8, 7, 8, 9, 7, 7, 9, 6};                               // ==++---+
static const char vlist3567[] = { 2, 1,12, 6, 7, 1, 6};                                        // ==++--=-
static const char vlist3568[] = { 2, 7, 1, 6, 1, 0, 6};                                        // ==++--==
static const char vlist3569[] = { 2, 7, 1, 9, 7, 9, 6};                                        // ==++--=+
static const char vlist3570[] = { 3,10, 8, 6, 8,12, 6, 7,10, 6};                               // ==++--+-
static const char vlist3571[] = { 2,10, 0, 6, 7,10, 6};                                        // ==++--+=
static const char vlist3572[] = { 2, 7, 9, 6, 7,10, 9};                                        // ==++--++
static const char vlist3573[] = { 2,13,12, 2,13, 2, 7};                                        // ==++-=--
static const char vlist3574[] = { 2, 2, 7,13, 0, 2,13};                                        // ==++-=-=
static const char vlist3575[] = { 2, 2, 7, 8, 8, 7,13};                                        // ==++-=-+
static const char vlist3576[] = { 2, 1, 2, 7,12, 2, 1};                                        // ==++-==-
static const char vlist3577[] = { 1, 1, 2, 7};                                                 // ==++-===
static const char vlist3578[] = { 1, 1, 2, 7};                                                 // ==++-==+
static const char vlist3579[] = { 2,10, 2, 7,12, 2, 8};                                        // ==++-=+-
static const char vlist3580[] = { 1,10, 2, 7};                                                 // ==++-=+=
static const char vlist3581[] = { 1,10, 2, 7};                                                 // ==++-=++
static const char vlist3582[] = { 3,13,12, 9,13,11, 7,13, 9,11};                               // ==++-+--
static const char vlist3583[] = { 2, 0,11,13,11, 7,13};                                        // ==++-+-=
static const char vlist3584[] = { 2,11,13, 8,11, 7,13};                                        // ==++-+-+
static const char vlist3585[] = { 2,12, 9, 1, 1,11, 7};                                        // ==++-+=-
static const char vlist3586[] = { 1, 1,11, 7};                                                 // ==++-+==
static const char vlist3587[] = { 1, 1,11, 7};                                                 // ==++-+=+
static const char vlist3588[] = { 2,12, 9, 8,10,11, 7};                                        // ==++-++-
static const char vlist3589[] = { 1,10,11, 7};                                                 // ==++-++=
static const char vlist3590[] = { 1,10,11, 7};                                                 // ==++-+++
static const char vlist3591[] = { 2, 3,12, 6,13,12, 3};                                        // ==++=---
static const char vlist3592[] = { 2, 6, 3, 0, 0, 3,13};                                        // ==++=--=
static const char vlist3593[] = { 2, 6, 3, 9, 8, 3,13};                                        // ==++=--+
static const char vlist3594[] = { 2, 3,12, 6,12, 3, 1};                                        // ==++=-=-
static const char vlist3595[] = { 1, 3, 0, 6};                                                 // ==++=-==
static const char vlist3596[] = { 1, 3, 9, 6};                                                 // ==++=-=+
static const char vlist3597[] = { 2, 3, 8, 6, 8,12, 6};                                        // ==++=-+-
static const char vlist3598[] = { 1, 3, 0, 6};                                                 // ==++=-+=
static const char vlist3599[] = { 1, 3, 9, 6};                                                 // ==++=-++
static const char vlist3600[] = { 2,12, 3,13, 3,12, 2};                                        // ==++==--
static const char vlist3601[] = { 1, 0, 3,13};                                                 // ==++==-=
static const char vlist3602[] = { 1, 8, 3,13};                                                 // ==++==-+
static const char vlist3603[] = { 1, 2, 1,12};                                                 // ==++===-
                                                                                               // ==++====
                                                                                               // ==++===+
static const char vlist3606[] = { 1, 2, 8,12};                                                 // ==++==+-
                                                                                               // ==++==+=
                                                                                               // ==++==++
static const char vlist3609[] = { 2, 9, 3,13,12, 9,13};                                        // ==++=+--
static const char vlist3610[] = { 1, 0, 3,13};                                                 // ==++=+-=
static const char vlist3611[] = { 1, 8, 3,13};                                                 // ==++=+-+
static const char vlist3612[] = { 1, 9, 1,12};                                                 // ==++=+=-
                                                                                               // ==++=+==
                                                                                               // ==++=+=+
static const char vlist3615[] = { 1, 9, 8,12};                                                 // ==++=++-
                                                                                               // ==++=++=
                                                                                               // ==++=+++
static const char vlist3618[] = { 3,11,12, 6,10,12,11,13,12,10};                               // ==+++---
static const char vlist3619[] = { 2, 6,11, 0, 0,10,13};                                        // ==+++--=
static const char vlist3620[] = { 2, 6,11, 9, 8,10,13};                                        // ==+++--+
static const char vlist3621[] = { 2,12,11, 1,11,12, 6};                                        // ==+++-=-
static const char vlist3622[] = { 1,11, 0, 6};                                                 // ==+++-==
static const char vlist3623[] = { 1,11, 9, 6};                                                 // ==+++-=+
static const char vlist3624[] = { 2,11,12, 6,11, 8,12};                                        // ==+++-+-
static const char vlist3625[] = { 1,11, 0, 6};                                                 // ==+++-+=
static const char vlist3626[] = { 1,11, 9, 6};                                                 // ==+++-++
static const char vlist3627[] = { 2,10,12, 2,12,10,13};                                        // ==+++=--
static const char vlist3628[] = { 1, 0,10,13};                                                 // ==+++=-=
static const char vlist3629[] = { 1, 8,10,13};                                                 // ==+++=-+
static const char vlist3630[] = { 1, 2, 1,12};                                                 // ==+++==-
                                                                                               // ==+++===
                                                                                               // ==+++==+
static const char vlist3633[] = { 1, 2, 8,12};                                                 // ==+++=+-
                                                                                               // ==+++=+=
                                                                                               // ==+++=++
static const char vlist3636[] = { 2,12,10,13, 9,10,12};                                        // ==++++--
static const char vlist3637[] = { 1, 0,10,13};                                                 // ==++++-=
static const char vlist3638[] = { 1, 8,10,13};                                                 // ==++++-+
static const char vlist3639[] = { 1, 9, 1,12};                                                 // ==++++=-
                                                                                               // ==++++==
                                                                                               // ==++++=+
static const char vlist3642[] = { 1, 9, 8,12};                                                 // ==+++++-
                                                                                               // ==+++++=
                                                                                               // ==++++++
static const char vlist3645[] = { 1,14, 7,17};                                                 // =+------
static const char vlist3646[] = { 2, 7,17, 0, 7, 0,14};                                        // =+-----=
static const char vlist3647[] = { 4, 7,17,12, 7,12, 8, 7, 8, 9,14, 7, 9};                      // =+-----+
static const char vlist3648[] = { 3, 1,17,14, 7,17, 1, 7, 1,14};                               // =+----=-
static const char vlist3649[] = { 4,17, 1, 7, 1,17, 0, 1, 0,14, 7, 1,14};                      // =+----==
static const char vlist3650[] = { 4,12, 1, 7, 7, 1, 9,14, 7, 9,17,12, 7};                      // =+----=+
static const char vlist3651[] = { 5, 8,17,14,13,17, 8, 7,17,13, 8,14,10,14, 7,10};             // =+----+-
static const char vlist3652[] = { 4,14, 7,10,13,17, 0,17,13, 7, 0,14,10};                      // =+----+=
static const char vlist3653[] = { 4,17,13, 7,12,13,17, 7,10,14,14,10, 9};                      // =+----++
static const char vlist3654[] = { 1, 2, 7,17};                                                 // =+---=--
static const char vlist3655[] = { 2, 0, 2, 7,17, 0, 7};                                        // =+---=-=
static const char vlist3656[] = { 3, 8, 2, 7,12, 8, 7,17,12, 7};                               // =+---=-+
static const char vlist3657[] = { 3, 2, 1, 7,17, 2, 1,17, 1, 7};                               // =+---==-
static const char vlist3658[] = { 3, 2, 1, 7, 1, 7,17, 0, 1,17};                               // =+---===
static const char vlist3659[] = { 3, 1, 2, 7, 1, 7,12,12, 7,17};                               // =+---==+
static const char vlist3660[] = { 4,10, 2, 7,17, 2, 8,17, 8,13,17,13, 7};                      // =+---=+-
static const char vlist3661[] = { 3, 0,13,17,13, 7,17,10, 2, 7};                               // =+---=+=
static const char vlist3662[] = { 3,13,17,12,13, 7,17,10, 2, 7};                               // =+---=++
static const char vlist3663[] = { 2,11, 7, 9, 7,17, 9};                                        // =+---+--
static const char vlist3664[] = { 2, 0,11, 7,17, 0, 7};                                        // =+---+-=
static const char vlist3665[] = { 3,12, 8, 7, 8,11, 7,17,12, 7};                               // =+---+-+
static const char vlist3666[] = { 3,17, 9, 1,17, 1, 7, 1,11, 7};                               // =+---+=-
static const char vlist3667[] = { 3, 1,11, 7, 1, 7,17, 0, 1,17};                               // =+---+==
static const char vlist3668[] = { 3, 1,11, 7, 1, 7,12,12, 7,17};                               // =+---+=+
static const char vlist3669[] = { 4,17, 9, 8,17,13, 7,17, 8,13,10,11, 7};                      // =+---++-
static const char vlist3670[] = { 3, 0,13,17,13, 7,17,10,11, 7};                               // =+---++=
static const char vlist3671[] = { 3,13,17,12,13, 7,17,10,11, 7};                               // =+---+++
static const char vlist3672[] = { 2,14, 3,17, 3, 7,17};                                        // =+--=---
static const char vlist3673[] = { 3, 0, 3, 7, 3,14, 0,17, 0, 7};                               // =+--=--=
static const char vlist3674[] = { 4, 8, 3, 7,14, 3, 9,12, 8, 7,17,12, 7};                      // =+--=--+
static const char vlist3675[] = { 3,14, 3, 1,17,14, 1,17, 1, 7};                               // =+--=-=-
static const char vlist3676[] = { 3, 1, 7,17, 0, 1,17, 3, 0,14};                               // =+--=-==
static const char vlist3677[] = { 3, 1, 7,12,12, 7,17, 3, 9,14};                               // =+--=-=+
static const char vlist3678[] = { 4,14, 3, 8,17,14, 8,17,13, 7,17, 8,13};                      // =+--=-+-
static const char vlist3679[] = { 3, 0,13,17,13, 7,17, 3, 0,14};                               // =+--=-+=
static const char vlist3680[] = { 3,13,17,12,13, 7,17, 3, 9,14};                               // =+--=-++
static const char vlist3681[] = { 2, 3, 7,17, 2, 3,17};                                        // =+--==--
static const char vlist3682[] = { 2, 0, 7,17, 0, 3, 7};                                        // =+--==-=
static const char vlist3683[] = { 3, 8, 3, 7,12, 8, 7,12, 7,17};                               // =+--==-+
static const char vlist3684[] = { 2, 2, 1,17, 1, 7,17};                                        // =+--===-
static const char vlist3685[] = { 2, 1, 7,17, 0, 1,17};                                        // =+--====
static const char vlist3686[] = { 2, 1, 7,12,12, 7,17};                                        // =+--===+
static const char vlist3687[] = { 3, 2, 8,17, 8,13,17,13, 7,17};                               // =+--==+-
static const char vlist3688[] = { 2, 0,13,17,13, 7,17};                                        // =+--==+=
static const char vlist3689[] = { 2,13,17,12,13, 7,17};                                        // =+--==++
static const char vlist3690[] = { 2, 3, 7, 9, 7,17, 9};                                        // =+--=+--
static const char vlist3691[] = { 2, 0, 7,17, 0, 3, 7};                                        // =+--=+-=
static const char vlist3692[] = { 3, 8, 3, 7,12, 8, 7,12, 7,17};                               // =+--=+-+
static const char vlist3693[] = { 2, 9, 1,17, 1, 7,17};                                        // =+--=+=-
static const char vlist3694[] = { 2, 1, 7,17, 0, 1,17};                                        // =+--=+==
static const char vlist3695[] = { 2, 1, 7,12,12, 7,17};                                        // =+--=+=+
static const char vlist3696[] = { 3, 8,13,17, 9, 8,17,13, 7,17};                               // =+--=++-
static const char vlist3697[] = { 2, 0,13,17,13, 7,17};                                        // =+--=++=
static const char vlist3698[] = { 2,13,17,12,13, 7,17};                                        // =+--=+++
static const char vlist3699[] = { 3,14,11,10,17,14,10, 7,17,10};                               // =+--+---
static const char vlist3700[] = { 3,14,11, 0,17, 0, 7, 0,10, 7};                               // =+--+--=
static const char vlist3701[] = { 4,14,11, 9,12, 8, 7, 8,10, 7,17,12, 7};                      // =+--+--+
static const char vlist3702[] = { 3,14,11, 1,17,14, 1,17, 1, 7};                               // =+--+-=-
static const char vlist3703[] = { 3, 1, 7,17, 0, 1,17,11, 0,14};                               // =+--+-==
static const char vlist3704[] = { 3, 1, 7,12,12, 7,17,11, 9,14};                               // =+--+-=+
static const char vlist3705[] = { 4,14,11, 8,17,14, 8,17, 8,13,17,13, 7};                      // =+--+-+-
static const char vlist3706[] = { 3, 0,13,17,13, 7,17,11, 0,14};                               // =+--+-+=
static const char vlist3707[] = { 3,13,17,12,13, 7,17,11, 9,14};                               // =+--+-++
static const char vlist3708[] = { 2,17, 2,10, 7,17,10};                                        // =+--+=--
static const char vlist3709[] = { 2, 0, 7,17, 0,10, 7};                                        // =+--+=-=
static const char vlist3710[] = { 3, 8,10, 7,12, 8, 7,12, 7,17};                               // =+--+=-+
static const char vlist3711[] = { 2, 2, 1,17, 1, 7,17};                                        // =+--+==-
static const char vlist3712[] = { 2, 1, 7,17, 0, 1,17};                                        // =+--+===
static const char vlist3713[] = { 2, 1, 7,12,12, 7,17};                                        // =+--+==+
static const char vlist3714[] = { 3, 2, 8,17, 8,13,17,13, 7,17};                               // =+--+=+-
static const char vlist3715[] = { 2, 0,13,17,13, 7,17};                                        // =+--+=+=
static const char vlist3716[] = { 2,13,17,12,13, 7,17};                                        // =+--+=++
static const char vlist3717[] = { 2,10, 7,17,10,17, 9};                                        // =+--++--
static const char vlist3718[] = { 2, 0, 7,17, 0,10, 7};                                        // =+--++-=
static const char vlist3719[] = { 3, 8,10, 7,12, 8, 7,12, 7,17};                               // =+--++-+
static const char vlist3720[] = { 2, 9, 1,17, 1, 7,17};                                        // =+--++=-
static const char vlist3721[] = { 2, 1, 7,17, 0, 1,17};                                        // =+--++==
static const char vlist3722[] = { 2, 1, 7,12,12, 7,17};                                        // =+--++=+
static const char vlist3723[] = { 3, 8,13,17, 9, 8,17,13, 7,17};                               // =+--+++-
static const char vlist3724[] = { 2, 0,13,17,13, 7,17};                                        // =+--+++=
static const char vlist3725[] = { 2,13,17,12,13, 7,17};                                        // =+--++++
static const char vlist3726[] = { 1, 7, 4,14};                                                 // =+-=----
static const char vlist3727[] = { 2, 4, 0, 7, 0,14, 7};                                        // =+-=---=
static const char vlist3728[] = { 3, 4, 8, 7, 8, 9, 7, 9,14, 7};                               // =+-=---+
static const char vlist3729[] = { 3, 1, 4, 7, 4,14, 1, 1,14, 7};                               // =+-=--=-
static const char vlist3730[] = { 3, 4, 1, 7, 7, 1,14, 1, 0,14};                               // =+-=--==
static const char vlist3731[] = { 3, 4, 1, 7, 7, 1, 9, 7, 9,14};                               // =+-=--=+
static const char vlist3732[] = { 4,13, 4, 7, 4,14, 8, 8,14,10,10,14, 7};                      // =+-=--+-
static const char vlist3733[] = { 3,10, 0,14, 7,10,14, 4,13, 7};                               // =+-=--+=
static const char vlist3734[] = { 3,14,10, 9, 7,10,14, 4,13, 7};                               // =+-=--++
static const char vlist3735[] = { 1, 4, 2, 7};                                                 // =+-=-=--
static const char vlist3736[] = { 2, 0, 2, 7, 4, 0, 7};                                        // =+-=-=-=
static const char vlist3737[] = { 2, 8, 2, 7, 4, 8, 7};                                        // =+-=-=-+
static const char vlist3738[] = { 3, 4, 2, 1, 1, 2, 7, 4, 1, 7};                               // =+-=-==-
static const char vlist3739[] = { 2, 1, 2, 7, 1, 7, 4};                                        // =+-=-===
static const char vlist3740[] = { 2, 1, 2, 7, 1, 7, 4};                                        // =+-=-==+
static const char vlist3741[] = { 3, 4, 2, 8,10, 2, 7, 4,13, 7};                               // =+-=-=+-
static const char vlist3742[] = { 2,10, 2, 7,13, 7, 4};                                        // =+-=-=+=
static const char vlist3743[] = { 2,10, 2, 7,13, 7, 4};                                        // =+-=-=++
static const char vlist3744[] = { 2, 4, 9, 7, 9,11, 7};                                        // =+-=-+--
static const char vlist3745[] = { 2, 0,11, 7, 4, 0, 7};                                        // =+-=-+-=
static const char vlist3746[] = { 2, 4, 8, 7, 8,11, 7};                                        // =+-=-+-+
static const char vlist3747[] = { 3, 4, 9, 1, 4, 1, 7, 1,11, 7};                               // =+-=-+=-
static const char vlist3748[] = { 2, 1,11, 7, 1, 7, 4};                                        // =+-=-+==
static const char vlist3749[] = { 2, 1,11, 7, 1, 7, 4};                                        // =+-=-+=+
static const char vlist3750[] = { 3, 4, 9, 8,10,11, 7, 4,13, 7};                               // =+-=-++-
static const char vlist3751[] = { 2,10,11, 7,13, 7, 4};                                        // =+-=-++=
static const char vlist3752[] = { 2,10,11, 7,13, 7, 4};                                        // =+-=-+++
static const char vlist3753[] = { 2, 4, 3, 7, 4,14, 3};                                        // =+-==---
static const char vlist3754[] = { 3, 3, 4, 0, 4, 3, 7, 0, 3,14};                               // =+-==--=
static const char vlist3755[] = { 3, 8, 3, 7, 3, 9,14, 4, 8, 7};                               // =+-==--+
static const char vlist3756[] = { 3,14, 3, 1, 4,14, 1, 4, 1, 7};                               // =+-==-=-
static const char vlist3757[] = { 2, 1, 7, 4, 3, 0,14};                                        // =+-==-==
static const char vlist3758[] = { 2, 1, 7, 4, 3, 9,14};                                        // =+-==-=+
static const char vlist3759[] = { 3, 3, 8,14, 8, 4,14, 4,13, 7};                               // =+-==-+-
static const char vlist3760[] = { 2, 3, 0,14,13, 7, 4};                                        // =+-==-+=
static const char vlist3761[] = { 2, 3, 9,14,13, 7, 4};                                        // =+-==-++
static const char vlist3762[] = { 2, 3, 4, 2, 4, 3, 7};                                        // =+-===--
static const char vlist3763[] = { 2, 0, 3, 4, 3, 7, 4};                                        // =+-===-=
static const char vlist3764[] = { 2, 3, 7, 8, 8, 7, 4};                                        // =+-===-+
static const char vlist3765[] = { 2, 2, 1, 4, 1, 7, 4};                                        // =+-====-
static const char vlist3766[] = { 1, 1, 7, 4};                                                 // =+-=====
static const char vlist3767[] = { 1, 1, 7, 4};                                                 // =+-====+
static const char vlist3768[] = { 2, 2, 8, 4,13, 7, 4};                                        // =+-===+-
static const char vlist3769[] = { 1,13, 7, 4};                                                 // =+-===+=
static const char vlist3770[] = { 1,13, 7, 4};                                                 // =+-===++
static const char vlist3771[] = { 2, 3, 4, 9, 4, 3, 7};                                        // =+-==+--
static const char vlist3772[] = { 2, 0, 3, 4, 3, 7, 4};                                        // =+-==+-=
static const char vlist3773[] = { 2, 3, 7, 8, 8, 7, 4};                                        // =+-==+-+
static const char vlist3774[] = { 2, 9, 1, 4, 1, 7, 4};                                        // =+-==+=-
static const char vlist3775[] = { 1, 1, 7, 4};                                                 // =+-==+==
static const char vlist3776[] = { 1, 1, 7, 4};                                                 // =+-==+=+
static const char vlist3777[] = { 2, 9, 8, 4,13, 7, 4};                                        // =+-==++-
static const char vlist3778[] = { 1,13, 7, 4};                                                 // =+-==++=
static const char vlist3779[] = { 1,13, 7, 4};                                                 // =+-==+++
static const char vlist3780[] = { 3, 4,10, 7, 4,11,10, 4,14,11};                               // =+-=+---
static const char vlist3781[] = { 3, 4,10, 7,10, 4, 0,11, 0,14};                               // =+-=+--=
static const char vlist3782[] = { 3, 8,10, 7, 4, 8, 7,11, 9,14};                               // =+-=+--+
static const char vlist3783[] = { 3,11, 4,14, 4,11, 1, 4, 1, 7};                               // =+-=+-=-
static const char vlist3784[] = { 2, 1, 7, 4,11, 0,14};                                        // =+-=+-==
static const char vlist3785[] = { 2, 1, 7, 4,11, 9,14};                                        // =+-=+-=+
static const char vlist3786[] = { 3,11, 8,14, 8, 4,14, 4,13, 7};                               // =+-=+-+-
static const char vlist3787[] = { 2,11, 0,14,13, 7, 4};                                        // =+-=+-+=
static const char vlist3788[] = { 2,11, 9,14,13, 7, 4};                                        // =+-=+-++
static const char vlist3789[] = { 2, 4,10, 7,10, 4, 2};                                        // =+-=+=--
static const char vlist3790[] = { 2, 0,10, 4,10, 7, 4};                                        // =+-=+=-=
static const char vlist3791[] = { 2, 8, 7, 4,10, 7, 8};                                        // =+-=+=-+
static const char vlist3792[] = { 2, 2, 1, 4, 1, 7, 4};                                        // =+-=+==-
static const char vlist3793[] = { 1, 1, 7, 4};                                                 // =+-=+===
static const char vlist3794[] = { 1, 1, 7, 4};                                                 // =+-=+==+
static const char vlist3795[] = { 2, 2, 8, 4,13, 7, 4};                                        // =+-=+=+-
static const char vlist3796[] = { 1,13, 7, 4};                                                 // =+-=+=+=
static const char vlist3797[] = { 1,13, 7, 4};                                                 // =+-=+=++
static const char vlist3798[] = { 2, 4,10, 7,10, 4, 9};                                        // =+-=++--
static const char vlist3799[] = { 2, 0,10, 4,10, 7, 4};                                        // =+-=++-=
static const char vlist3800[] = { 2, 8, 7, 4,10, 7, 8};                                        // =+-=++-+
static const char vlist3801[] = { 2, 9, 1, 4, 1, 7, 4};                                        // =+-=++=-
static const char vlist3802[] = { 1, 1, 7, 4};                                                 // =+-=++==
static const char vlist3803[] = { 1, 1, 7, 4};                                                 // =+-=++=+
static const char vlist3804[] = { 2, 9, 8, 4,13, 7, 4};                                        // =+-=+++-
static const char vlist3805[] = { 1,13, 7, 4};                                                 // =+-=+++=
static const char vlist3806[] = { 1,13, 7, 4};                                                 // =+-=++++
static const char vlist3807[] = { 2, 7,16,12,14, 7,12};                                        // =+-+----
static const char vlist3808[] = { 2,16, 0, 7, 0,14, 7};                                        // =+-+---=
static const char vlist3809[] = { 3, 8, 9, 7,16, 8, 7, 9,14, 7};                               // =+-+---+
static const char vlist3810[] = { 3,12,14, 1, 1,14, 7, 1,16, 7};                               // =+-+--=-
static const char vlist3811[] = { 3,16, 1, 7, 7, 1,14, 1, 0,14};                               // =+-+--==
static const char vlist3812[] = { 3,16, 1, 7, 7, 1, 9, 7, 9,14};                               // =+-+--=+
static const char vlist3813[] = { 4,12,14, 8,10,14, 7, 8,14,10,16,13, 7};                      // =+-+--+-
static const char vlist3814[] = { 3,10, 0,14, 7,10,14,16,13, 7};                               // =+-+--+=
static const char vlist3815[] = { 3,14,10, 9, 7,10,14,16,13, 7};                               // =+-+--++
static const char vlist3816[] = { 2,12, 2, 7,16,12, 7};                                        // =+-+-=--
static const char vlist3817[] = { 2, 0, 2, 7,16, 0, 7};                                        // =+-+-=-=
static const char vlist3818[] = { 2, 8, 2, 7,16, 8, 7};                                        // =+-+-=-+
static const char vlist3819[] = { 3, 1, 2, 7,12, 2, 1,16, 1, 7};                               // =+-+-==-
static const char vlist3820[] = { 2, 1, 2, 7, 1, 7,16};                                        // =+-+-===
static const char vlist3821[] = { 2, 1, 2, 7, 1, 7,16};                                        // =+-+-==+
static const char vlist3822[] = { 3,10, 2, 7,12, 2, 8,16,13, 7};                               // =+-+-=+-
static const char vlist3823[] = { 2,10, 2, 7,13, 7,16};                                        // =+-+-=+=
static const char vlist3824[] = { 2,10, 2, 7,13, 7,16};                                        // =+-+-=++
static const char vlist3825[] = { 3, 9,11,12,12,11,16,16,11, 7};                               // =+-+-+--
static const char vlist3826[] = { 2, 0,11, 7,16, 0, 7};                                        // =+-+-+-=
static const char vlist3827[] = { 2,16, 8, 7, 8,11, 7};                                        // =+-+-+-+
static const char vlist3828[] = { 3,12, 9, 1,16, 1, 7, 1,11, 7};                               // =+-+-+=-
static const char vlist3829[] = { 2, 1,11, 7, 1, 7,16};                                        // =+-+-+==
static const char vlist3830[] = { 2, 1,11, 7, 1, 7,16};                                        // =+-+-+=+
static const char vlist3831[] = { 3,12, 9, 8,10,11, 7,16,13, 7};                               // =+-+-++-
static const char vlist3832[] = { 2,10,11, 7,13, 7,16};                                        // =+-+-++=
static const char vlist3833[] = { 2,10,11, 7,13, 7,16};                                        // =+-+-+++
static const char vlist3834[] = { 3,16,12, 3,16, 3, 7,12,14, 3};                               // =+-+=---
static const char vlist3835[] = { 3,16, 3, 7, 3,16, 0, 3, 0,14};                               // =+-+=--=
static const char vlist3836[] = { 3, 8, 3, 7, 3, 9,14,16, 8, 7};                               // =+-+=--+
static const char vlist3837[] = { 3, 3,12,14,12, 3, 1,16, 1, 7};                               // =+-+=-=-
static const char vlist3838[] = { 2, 1, 7,16, 3, 0,14};                                        // =+-+=-==
static const char vlist3839[] = { 2, 1, 7,16, 3, 9,14};                                        // =+-+=-=+
static const char vlist3840[] = { 3, 3, 8,14, 8,12,14,16,13, 7};                               // =+-+=-+-
static const char vlist3841[] = { 2, 3, 0,14,13, 7,16};                                        // =+-+=-+=
static const char vlist3842[] = { 2, 3, 9,14,13, 7,16};                                        // =+-+=-++
static const char vlist3843[] = { 3,16, 3, 7, 3,16,12, 3,12, 2};                               // =+-+==--
static const char vlist3844[] = { 2, 0, 3,16, 3, 7,16};                                        // =+-+==-=
static const char vlist3845[] = { 2, 3, 7, 8, 8, 7,16};                                        // =+-+==-+
static const char vlist3846[] = { 2, 2, 1,12, 1, 7,16};                                        // =+-+===-
static const char vlist3847[] = { 1, 1, 7,16};                                                 // =+-+====
static const char vlist3848[] = { 1, 1, 7,16};                                                 // =+-+===+
static const char vlist3849[] = { 2, 2, 8,12,13, 7,16};                                        // =+-+==+-
static const char vlist3850[] = { 1,13, 7,16};                                                 // =+-+==+=
static const char vlist3851[] = { 1,13, 7,16};                                                 // =+-+==++
static const char vlist3852[] = { 3,16, 3, 7, 3,16,12, 3,12, 9};                               // =+-+=+--
static const char vlist3853[] = { 2, 0, 3,16, 3, 7,16};                                        // =+-+=+-=
static const char vlist3854[] = { 2, 3, 7, 8, 8, 7,16};                                        // =+-+=+-+
static const char vlist3855[] = { 2, 9, 1,12, 1, 7,16};                                        // =+-+=+=-
static const char vlist3856[] = { 1, 1, 7,16};                                                 // =+-+=+==
static const char vlist3857[] = { 1, 1, 7,16};                                                 // =+-+=+=+
static const char vlist3858[] = { 2, 9, 8,12,13, 7,16};                                        // =+-+=++-
static const char vlist3859[] = { 1,13, 7,16};                                                 // =+-+=++=
static const char vlist3860[] = { 1,13, 7,16};                                                 // =+-+=+++
static const char vlist3861[] = { 4,16,12,10,16,10, 7,12,14,11,12,11,10};                      // =+-++---
static const char vlist3862[] = { 3,10,16, 0,16,10, 7,11, 0,14};                               // =+-++--=
static const char vlist3863[] = { 3,16,10, 7, 8,10,16,11, 9,14};                               // =+-++--+
static const char vlist3864[] = { 3,12,11, 1,11,12,14,16, 1, 7};                               // =+-++-=-
static const char vlist3865[] = { 2, 1, 7,16,11, 0,14};                                        // =+-++-==
static const char vlist3866[] = { 2, 1, 7,16,11, 9,14};                                        // =+-++-=+
static const char vlist3867[] = { 3,11,12,14,11, 8,12,16,13, 7};                               // =+-++-+-
static const char vlist3868[] = { 2,11, 0,14,13, 7,16};                                        // =+-++-+=
static const char vlist3869[] = { 2,11, 9,14,13, 7,16};                                        // =+-++-++
static const char vlist3870[] = { 3,10,16,12,16,10, 7,10,12, 2};                               // =+-++=--
static const char vlist3871[] = { 2, 0,10,16,10, 7,16};                                        // =+-++=-=
static const char vlist3872[] = { 2,10, 7,16,10,16, 8};                                        // =+-++=-+
static const char vlist3873[] = { 2, 2, 1,12, 1, 7,16};                                        // =+-++==-
static const char vlist3874[] = { 1, 1, 7,16};                                                 // =+-++===
static const char vlist3875[] = { 1, 1, 7,16};                                                 // =+-++==+
static const char vlist3876[] = { 2, 2, 8,12,13, 7,16};                                        // =+-++=+-
static const char vlist3877[] = { 1,13, 7,16};                                                 // =+-++=+=
static const char vlist3878[] = { 1,13, 7,16};                                                 // =+-++=++
static const char vlist3879[] = { 3,10,16,12,16,10, 7,10,12, 9};                               // =+-+++--
static const char vlist3880[] = { 2, 0,10,16,10, 7,16};                                        // =+-+++-=
static const char vlist3881[] = { 2,10, 7,16,10,16, 8};                                        // =+-+++-+
static const char vlist3882[] = { 2, 9, 1,12, 1, 7,16};                                        // =+-+++=-
static const char vlist3883[] = { 1, 1, 7,16};                                                 // =+-+++==
static const char vlist3884[] = { 1, 1, 7,16};                                                 // =+-+++=+
static const char vlist3885[] = { 2, 9, 8,12,13, 7,16};                                        // =+-++++-
static const char vlist3886[] = { 1,13, 7,16};                                                 // =+-++++=
static const char vlist3887[] = { 1,13, 7,16};                                                 // =+-+++++
static const char vlist3888[] = { 2, 5,17,14, 7, 5,14};                                        // =+=-----
static const char vlist3889[] = { 3, 5, 0, 7,17, 5, 0, 0,14, 7};                               // =+=----=
static const char vlist3890[] = { 4, 5, 8, 7,17, 5,12, 8, 9, 7, 9,14, 7};                      // =+=----+
static const char vlist3891[] = { 3, 5,17, 1, 1,17,14, 1,14, 7};                               // =+=---=-
static const char vlist3892[] = { 3, 7, 1,14, 1, 0,14, 5, 0,17};                               // =+=---==
static const char vlist3893[] = { 3, 7, 1, 9, 7, 9,14,12, 5,17};                               // =+=---=+
static const char vlist3894[] = { 4, 5,17, 8, 8,17,14, 8,14,10,10,14, 7};                      // =+=---+-
static const char vlist3895[] = { 3,10, 0,14, 7,10,14, 0, 5,17};                               // =+=---+=
static const char vlist3896[] = { 3,14,10, 9, 7,10,14,12, 5,17};                               // =+=---++
static const char vlist3897[] = { 2, 5, 2, 7, 5,17, 2};                                        // =+=--=--
static const char vlist3898[] = { 3, 2, 5, 0, 5, 2, 7, 5, 0,17};                               // =+=--=-=
static const char vlist3899[] = { 3, 8, 2, 7, 5, 8, 7,12, 5,17};                               // =+=--=-+
static const char vlist3900[] = { 3,17, 2, 1, 1, 2, 7, 5,17, 1};                               // =+=--==-
static const char vlist3901[] = { 2, 1, 2, 7,17, 0, 5};                                        // =+=--===
static const char vlist3902[] = { 2, 1, 2, 7,17,12, 5};                                        // =+=--==+
static const char vlist3903[] = { 3, 2, 8,17,10, 2, 7, 8, 5,17};                               // =+=--=+-
static const char vlist3904[] = { 2,17, 0, 5,10, 2, 7};                                        // =+=--=+=
static const char vlist3905[] = { 2,17,12, 5,10, 2, 7};                                        // =+=--=++
static const char vlist3906[] = { 3, 5,11, 7, 5, 9,11, 5,17, 9};                               // =+=--+--
static const char vlist3907[] = { 3, 5,11, 7,11, 5, 0, 0, 5,17};                               // =+=--+-=
static const char vlist3908[] = { 3, 8,11, 7, 5, 8, 7,12, 5,17};                               // =+=--+-+
static const char vlist3909[] = { 3, 9, 5,17, 5, 9, 1, 1,11, 7};                               // =+=--+=-
static const char vlist3910[] = { 2, 1,11, 7,17, 0, 5};                                        // =+=--+==
static const char vlist3911[] = { 2, 1,11, 7,17,12, 5};                                        // =+=--+=+
static const char vlist3912[] = { 3, 9, 8,17, 8, 5,17,10,11, 7};                               // =+=--++-
static const char vlist3913[] = { 2,17, 0, 5,10,11, 7};                                        // =+=--++=
static const char vlist3914[] = { 2,17,12, 5,10,11, 7};                                        // =+=--+++
static const char vlist3915[] = { 2, 5,14, 3, 5,17,14};                                        // =+=-=---
static const char vlist3916[] = { 3, 5, 3, 0, 0, 3,14, 5, 0,17};                               // =+=-=--=
static const char vlist3917[] = { 3, 3, 5, 8, 3, 9,14,12, 5,17};                               // =+=-=--+
static const char vlist3918[] = { 3, 3, 1,14,14, 1,17, 1, 5,17};                               // =+=-=-=-
static const char vlist3919[] = { 2,17, 0, 5, 3, 0,14};                                        // =+=-=-==
static const char vlist3920[] = { 2, 3, 9,14,17,12, 5};                                        // =+=-=-=+
static const char vlist3921[] = { 3, 3, 8,14, 8, 5,17,14, 8,17};                               // =+=-=-+-
static const char vlist3922[] = { 2,17, 0, 5, 3, 0,14};                                        // =+=-=-+=
static const char vlist3923[] = { 2, 3, 9,14,17,12, 5};                                        // =+=-=-++
static const char vlist3924[] = { 2, 3, 5, 2, 2, 5,17};                                        // =+=-==--
static const char vlist3925[] = { 2, 0, 3, 5,17, 0, 5};                                        // =+=-==-=
static const char vlist3926[] = { 2, 8, 3, 5,17,12, 5};                                        // =+=-==-+
static const char vlist3927[] = { 2, 2, 1,17,17, 1, 5};                                        // =+=-===-
static const char vlist3928[] = { 1,17, 0, 5};                                                 // =+=-====
static const char vlist3929[] = { 1,17,12, 5};                                                 // =+=-===+
static const char vlist3930[] = { 2,17, 2, 8,17, 8, 5};                                        // =+=-==+-
static const char vlist3931[] = { 1,17, 0, 5};                                                 // =+=-==+=
static const char vlist3932[] = { 1,17,12, 5};                                                 // =+=-==++
static const char vlist3933[] = { 2, 3, 5, 9, 9, 5,17};                                        // =+=-=+--
static const char vlist3934[] = { 2, 0, 3, 5,17, 0, 5};                                        // =+=-=+-=
static const char vlist3935[] = { 2, 8, 3, 5,17,12, 5};                                        // =+=-=+-+
static const char vlist3936[] = { 2, 9, 1, 5,17, 9, 5};                                        // =+=-=+=-
static const char vlist3937[] = { 1,17, 0, 5};                                                 // =+=-=+==
static const char vlist3938[] = { 1,17,12, 5};                                                 // =+=-=+=+
static const char vlist3939[] = { 2,17, 8, 5,17, 9, 8};                                        // =+=-=++-
static const char vlist3940[] = { 1,17, 0, 5};                                                 // =+=-=++=
static const char vlist3941[] = { 1,17,12, 5};                                                 // =+=-=+++
static const char vlist3942[] = { 3, 5,17,10,17,14,10,10,14,11};                               // =+=-+---
static const char vlist3943[] = { 3,10, 5, 0, 0, 5,17,11, 0,14};                               // =+=-+--=
static const char vlist3944[] = { 3,10, 5, 8,11, 9,14,12, 5,17};                               // =+=-+--+
static const char vlist3945[] = { 3,11, 1,14,14, 1,17, 1, 5,17};                               // =+=-+-=-
static const char vlist3946[] = { 2,17, 0, 5,11, 0,14};                                        // =+=-+-==
static const char vlist3947[] = { 2,11, 9,14,17,12, 5};                                        // =+=-+-=+
static const char vlist3948[] = { 3, 8, 5,17,14, 8,17,11, 8,14};                               // =+=-+-+-
static const char vlist3949[] = { 2,17, 0, 5,11, 0,14};                                        // =+=-+-+=
static const char vlist3950[] = { 2,11, 9,14,17,12, 5};                                        // =+=-+-++
static const char vlist3951[] = { 2,10, 5, 2, 2, 5,17};                                        // =+=-+=--
static const char vlist3952[] = { 2, 0,10, 5,17, 0, 5};                                        // =+=-+=-=
static const char vlist3953[] = { 2,17,12, 5, 8,10, 5};                                        // =+=-+=-+
static const char vlist3954[] = { 2, 2, 1,17,17, 1, 5};                                        // =+=-+==-
static const char vlist3955[] = { 1,17, 0, 5};                                                 // =+=-+===
static const char vlist3956[] = { 1,17,12, 5};                                                 // =+=-+==+
static const char vlist3957[] = { 2,17, 2, 8,17, 8, 5};                                        // =+=-+=+-
static const char vlist3958[] = { 1,17, 0, 5};                                                 // =+=-+=+=
static const char vlist3959[] = { 1,17,12, 5};                                                 // =+=-+=++
static const char vlist3960[] = { 2,10, 5, 9, 9, 5,17};                                        // =+=-++--
static const char vlist3961[] = { 2, 0,10, 5,17, 0, 5};                                        // =+=-++-=
static const char vlist3962[] = { 2,17,12, 5, 8,10, 5};                                        // =+=-++-+
static const char vlist3963[] = { 2, 9, 1, 5,17, 9, 5};                                        // =+=-++=-
static const char vlist3964[] = { 1,17, 0, 5};                                                 // =+=-++==
static const char vlist3965[] = { 1,17,12, 5};                                                 // =+=-++=+
static const char vlist3966[] = { 2,17, 8, 5,17, 9, 8};                                        // =+=-+++-
static const char vlist3967[] = { 1,17, 0, 5};                                                 // =+=-+++=
static const char vlist3968[] = { 1,17,12, 5};                                                 // =+=-++++
static const char vlist3969[] = { 2, 7, 5,14, 5, 4,14};                                        // =+==----
static const char vlist3970[] = { 2, 7, 0,14, 5, 0, 7};                                        // =+==---=
static const char vlist3971[] = { 3, 5, 8, 7, 8, 9, 7, 7, 9,14};                               // =+==---+
static const char vlist3972[] = { 2, 1, 4,14, 7, 1,14};                                        // =+==--=-
static const char vlist3973[] = { 2, 7, 1,14, 1, 0,14};                                        // =+==--==
static const char vlist3974[] = { 2, 7, 1, 9, 7, 9,14};                                        // =+==--=+
static const char vlist3975[] = { 3, 8, 4,14,10, 8,14, 7,10,14};                               // =+==--+-
static const char vlist3976[] = { 2,10, 0,14, 7,10,14};                                        // =+==--+=
static const char vlist3977[] = { 2,14,10, 9, 7,10,14};                                        // =+==--++
static const char vlist3978[] = { 2, 5, 4, 2, 5, 2, 7};                                        // =+==-=--
static const char vlist3979[] = { 2, 2, 7, 5, 0, 2, 5};                                        // =+==-=-=
static const char vlist3980[] = { 2, 2, 7, 8, 8, 7, 5};                                        // =+==-=-+
static const char vlist3981[] = { 2, 4, 2, 1, 1, 2, 7};                                        // =+==-==-
static const char vlist3982[] = { 1, 1, 2, 7};                                                 // =+==-===
static const char vlist3983[] = { 1, 1, 2, 7};                                                 // =+==-==+
static const char vlist3984[] = { 2, 4, 2, 8,10, 2, 7};                                        // =+==-=+-
static const char vlist3985[] = { 1,10, 2, 7};                                                 // =+==-=+=
static const char vlist3986[] = { 1,10, 2, 7};                                                 // =+==-=++
static const char vlist3987[] = { 3, 5, 4, 9, 5, 9,11, 5,11, 7};                               // =+==-+--
static const char vlist3988[] = { 2, 0,11, 5,11, 7, 5};                                        // =+==-+-=
static const char vlist3989[] = { 2, 8, 7, 5,11, 7, 8};                                        // =+==-+-+
static const char vlist3990[] = { 2, 4, 9, 1, 1,11, 7};                                        // =+==-+=-
static const char vlist3991[] = { 1, 1,11, 7};                                                 // =+==-+==
static const char vlist3992[] = { 1, 1,11, 7};                                                 // =+==-+=+
static const char vlist3993[] = { 2, 4, 9, 8,10,11, 7};                                        // =+==-++-
static const char vlist3994[] = { 1,10,11, 7};                                                 // =+==-++=
static const char vlist3995[] = { 1,10,11, 7};                                                 // =+==-+++
static const char vlist3996[] = { 2, 5, 4, 3, 3, 4,14};                                        // =+===---
static const char vlist3997[] = { 2,14, 3, 0, 0, 3, 5};                                        // =+===--=
static const char vlist3998[] = { 2,14, 3, 9, 8, 3, 5};                                        // =+===--+
static const char vlist3999[] = { 2,14, 3, 1, 4,14, 1};                                        // =+===-=-
static const char vlist4000[] = { 1, 3, 0,14};                                                 // =+===-==
static const char vlist4001[] = { 1, 3, 9,14};                                                 // =+===-=+
static const char vlist4002[] = { 2, 3, 8,14, 8, 4,14};                                        // =+===-+-
static const char vlist4003[] = { 1, 3, 0,14};                                                 // =+===-+=
static const char vlist4004[] = { 1, 3, 9,14};                                                 // =+===-++
static const char vlist4005[] = { 2, 3, 4, 2, 4, 3, 5};                                        // =+====--
static const char vlist4006[] = { 1, 0, 3, 5};                                                 // =+====-=
static const char vlist4007[] = { 1, 8, 3, 5};                                                 // =+====-+
static const char vlist4008[] = { 1, 2, 1, 4};                                                 // =+=====-
                                                                                               // =+======
                                                                                               // =+=====+
static const char vlist4011[] = { 1, 2, 8, 4};                                                 // =+====+-
                                                                                               // =+====+=
                                                                                               // =+====++
static const char vlist4014[] = { 2, 9, 3, 5, 4, 9, 5};                                        // =+===+--
static const char vlist4015[] = { 1, 0, 3, 5};                                                 // =+===+-=
static const char vlist4016[] = { 1, 8, 3, 5};                                                 // =+===+-+
static const char vlist4017[] = { 1, 9, 1, 4};                                                 // =+===+=-
                                                                                               // =+===+==
                                                                                               // =+===+=+
static const char vlist4020[] = { 1, 9, 8, 4};                                                 // =+===++-
                                                                                               // =+===++=
                                                                                               // =+===+++
static const char vlist4023[] = { 3, 5, 4,10,11, 4,14,10, 4,11};                               // =+==+---
static const char vlist4024[] = { 2,14,11, 0, 0,10, 5};                                        // =+==+--=
static const char vlist4025[] = { 2,14,11, 9, 8,10, 5};                                        // =+==+--+
static const char vlist4026[] = { 2,11, 4,14, 4,11, 1};                                        // =+==+-=-
static const char vlist4027[] = { 1,11, 0,14};                                                 // =+==+-==
static const char vlist4028[] = { 1,11, 9,14};                                                 // =+==+-=+
static const char vlist4029[] = { 2,11, 8,14, 8, 4,14};                                        // =+==+-+-
static const char vlist4030[] = { 1,11, 0,14};                                                 // =+==+-+=
static const char vlist4031[] = { 1,11, 9,14};                                                 // =+==+-++
static const char vlist4032[] = { 2, 4,10, 5,10, 4, 2};                                        // =+==+=--
static const char vlist4033[] = { 1, 0,10, 5};                                                 // =+==+=-=
static const char vlist4034[] = { 1, 8,10, 5};                                                 // =+==+=-+
static const char vlist4035[] = { 1, 2, 1, 4};                                                 // =+==+==-
                                                                                               // =+==+===
                                                                                               // =+==+==+
static const char vlist4038[] = { 1, 2, 8, 4};                                                 // =+==+=+-
                                                                                               // =+==+=+=
                                                                                               // =+==+=++
static const char vlist4041[] = { 2, 9,10, 5, 4, 9, 5};                                        // =+==++--
static const char vlist4042[] = { 1, 0,10, 5};                                                 // =+==++-=
static const char vlist4043[] = { 1, 8,10, 5};                                                 // =+==++-+
static const char vlist4044[] = { 1, 9, 1, 4};                                                 // =+==++=-
                                                                                               // =+==++==
                                                                                               // =+==++=+
static const char vlist4047[] = { 1, 9, 8, 4};                                                 // =+==+++-
                                                                                               // =+==+++=
                                                                                               // =+==++++
static const char vlist4050[] = { 2, 7, 5,12,14, 7,12};                                        // =+=+----
static const char vlist4051[] = { 2, 7, 0,14, 5, 0, 7};                                        // =+=+---=
static const char vlist4052[] = { 3, 5, 8, 7, 8, 9, 7, 7, 9,14};                               // =+=+---+
static const char vlist4053[] = { 2, 1,12,14, 7, 1,14};                                        // =+=+--=-
static const char vlist4054[] = { 2, 7, 1,14, 1, 0,14};                                        // =+=+--==
static const char vlist4055[] = { 2, 7, 1, 9, 7, 9,14};                                        // =+=+--=+
static const char vlist4056[] = { 3,10, 8,14, 8,12,14, 7,10,14};                               // =+=+--+-
static const char vlist4057[] = { 2,10, 0,14, 7,10,14};                                        // =+=+--+=
static const char vlist4058[] = { 2,14,10, 9, 7,10,14};                                        // =+=+--++
static const char vlist4059[] = { 2, 5,12, 2, 5, 2, 7};                                        // =+=+-=--
static const char vlist4060[] = { 2, 2, 7, 5, 0, 2, 5};                                        // =+=+-=-=
static const char vlist4061[] = { 2, 2, 7, 8, 8, 7, 5};                                        // =+=+-=-+
static const char vlist4062[] = { 2, 1, 2, 7,12, 2, 1};                                        // =+=+-==-
static const char vlist4063[] = { 1, 1, 2, 7};                                                 // =+=+-===
static const char vlist4064[] = { 1, 1, 2, 7};                                                 // =+=+-==+
static const char vlist4065[] = { 2,10, 2, 7,12, 2, 8};                                        // =+=+-=+-
static const char vlist4066[] = { 1,10, 2, 7};                                                 // =+=+-=+=
static const char vlist4067[] = { 1,10, 2, 7};                                                 // =+=+-=++
static const char vlist4068[] = { 3, 5,12, 9, 5, 9,11, 5,11, 7};                               // =+=+-+--
static const char vlist4069[] = { 2, 0,11, 5,11, 7, 5};                                        // =+=+-+-=
static const char vlist4070[] = { 2, 8, 7, 5,11, 7, 8};                                        // =+=+-+-+
static const char vlist4071[] = { 2,12, 9, 1, 1,11, 7};                                        // =+=+-+=-
static const char vlist4072[] = { 1, 1,11, 7};                                                 // =+=+-+==
static const char vlist4073[] = { 1, 1,11, 7};                                                 // =+=+-+=+
static const char vlist4074[] = { 2,12, 9, 8,10,11, 7};                                        // =+=+-++-
static const char vlist4075[] = { 1,10,11, 7};                                                 // =+=+-++=
static const char vlist4076[] = { 1,10,11, 7};                                                 // =+=+-+++
static const char vlist4077[] = { 2, 3,12,14, 5,12, 3};                                        // =+=+=---
static const char vlist4078[] = { 2,14, 3, 0, 0, 3, 5};                                        // =+=+=--=
static const char vlist4079[] = { 2,14, 3, 9, 8, 3, 5};                                        // =+=+=--+
static const char vlist4080[] = { 2, 3,12,14,12, 3, 1};                                        // =+=+=-=-
static const char vlist4081[] = { 1, 3, 0,14};                                                 // =+=+=-==
static const char vlist4082[] = { 1, 3, 9,14};                                                 // =+=+=-=+
static const char vlist4083[] = { 2, 3, 8,14, 8,12,14};                                        // =+=+=-+-
static const char vlist4084[] = { 1, 3, 0,14};                                                 // =+=+=-+=
static const char vlist4085[] = { 1, 3, 9,14};                                                 // =+=+=-++
static const char vlist4086[] = { 2,12, 3, 5, 3,12, 2};                                        // =+=+==--
static const char vlist4087[] = { 1, 0, 3, 5};                                                 // =+=+==-=
static const char vlist4088[] = { 1, 8, 3, 5};                                                 // =+=+==-+
static const char vlist4089[] = { 1, 2, 1,12};                                                 // =+=+===-
                                                                                               // =+=+====
                                                                                               // =+=+===+
static const char vlist4092[] = { 1, 2, 8,12};                                                 // =+=+==+-
                                                                                               // =+=+==+=
                                                                                               // =+=+==++
static const char vlist4095[] = { 2, 9, 3, 5,12, 9, 5};                                        // =+=+=+--
static const char vlist4096[] = { 1, 0, 3, 5};                                                 // =+=+=+-=
static const char vlist4097[] = { 1, 8, 3, 5};                                                 // =+=+=+-+
static const char vlist4098[] = { 1, 9, 1,12};                                                 // =+=+=+=-
                                                                                               // =+=+=+==
                                                                                               // =+=+=+=+
static const char vlist4101[] = { 1, 9, 8,12};                                                 // =+=+=++-
                                                                                               // =+=+=++=
                                                                                               // =+=+=+++
static const char vlist4104[] = { 3,11,12,14,10,12,11, 5,12,10};                               // =+=++---
static const char vlist4105[] = { 2,14,11, 0, 0,10, 5};                                        // =+=++--=
static const char vlist4106[] = { 2,14,11, 9, 8,10, 5};                                        // =+=++--+
static const char vlist4107[] = { 2,12,11, 1,11,12,14};                                        // =+=++-=-
static const char vlist4108[] = { 1,11, 0,14};                                                 // =+=++-==
static const char vlist4109[] = { 1,11, 9,14};                                                 // =+=++-=+
static const char vlist4110[] = { 2,11,12,14,11, 8,12};                                        // =+=++-+-
static const char vlist4111[] = { 1,11, 0,14};                                                 // =+=++-+=
static const char vlist4112[] = { 1,11, 9,14};                                                 // =+=++-++
static const char vlist4113[] = { 2,10,12, 2,12,10, 5};                                        // =+=++=--
static const char vlist4114[] = { 1, 0,10, 5};                                                 // =+=++=-=
static const char vlist4115[] = { 1, 8,10, 5};                                                 // =+=++=-+
static const char vlist4116[] = { 1, 2, 1,12};                                                 // =+=++==-
                                                                                               // =+=++===
                                                                                               // =+=++==+
static const char vlist4119[] = { 1, 2, 8,12};                                                 // =+=++=+-
                                                                                               // =+=++=+=
                                                                                               // =+=++=++
static const char vlist4122[] = { 2,12,10, 5, 9,10,12};                                        // =+=+++--
static const char vlist4123[] = { 1, 0,10, 5};                                                 // =+=+++-=
static const char vlist4124[] = { 1, 8,10, 5};                                                 // =+=+++-+
static const char vlist4125[] = { 1, 9, 1,12};                                                 // =+=+++=-
                                                                                               // =+=+++==
                                                                                               // =+=+++=+
static const char vlist4128[] = { 1, 9, 8,12};                                                 // =+=++++-
                                                                                               // =+=++++=
                                                                                               // =+=+++++
static const char vlist4131[] = { 3,16,14,13,17,14,16,14, 7,13};                               // =++-----
static const char vlist4132[] = { 3,17,16, 0, 0,14, 7,13, 0, 7};                               // =++----=
static const char vlist4133[] = { 4,16,17,12, 8, 9, 7,13, 8, 7, 9,14, 7};                      // =++----+
static const char vlist4134[] = { 3, 1,16,17, 1,17,14, 1,14, 7};                               // =++---=-
static const char vlist4135[] = { 3, 7, 1,14, 1, 0,14, 0,16,17};                               // =++---==
static const char vlist4136[] = { 3, 7, 1, 9, 7, 9,14,12,16,17};                               // =++---=+
static const char vlist4137[] = { 4,16,17, 8, 8,17,14, 8,14,10,10,14, 7};                      // =++---+-
static const char vlist4138[] = { 3,10, 0,14, 7,10,14, 0,16,17};                               // =++---+=
static const char vlist4139[] = { 3,14,10, 9, 7,10,14,12,16,17};                               // =++---++
static const char vlist4140[] = { 3,16, 2,13,16,17, 2,13, 2, 7};                               // =++--=--
static const char vlist4141[] = { 3,13, 2, 7, 2,13, 0, 0,16,17};                               // =++--=-=
static const char vlist4142[] = { 3, 8, 2, 7,12,16,17,13, 8, 7};                               // =++--=-+
static const char vlist4143[] = { 3, 2,16,17,16, 2, 1, 1, 2, 7};                               // =++--==-
static const char vlist4144[] = { 2, 1, 2, 7,17, 0,16};                                        // =++--===
static const char vlist4145[] = { 2, 1, 2, 7,17,12,16};                                        // =++--==+
static const char vlist4146[] = { 3, 2, 8,17,10, 2, 7, 8,16,17};                               // =++--=+-
static const char vlist4147[] = { 2,17, 0,16,10, 2, 7};                                        // =++--=+=
static const char vlist4148[] = { 2,17,12,16,10, 2, 7};                                        // =++--=++
static const char vlist4149[] = { 4,16, 9,13,16,17, 9,13, 9,11,13,11, 7};                      // =++--+--
static const char vlist4150[] = { 3,11,13, 0,13,11, 7, 0,16,17};                               // =++--+-=
static const char vlist4151[] = { 3,13,11, 7, 8,11,13,12,16,17};                               // =++--+-+
static const char vlist4152[] = { 3,16, 9, 1, 9,16,17, 1,11, 7};                               // =++--+=-
static const char vlist4153[] = { 2, 1,11, 7,17, 0,16};                                        // =++--+==
static const char vlist4154[] = { 2, 1,11, 7,17,12,16};                                        // =++--+=+
static const char vlist4155[] = { 3, 9,16,17, 9, 8,16,10,11, 7};                               // =++--++-
static const char vlist4156[] = { 2,17, 0,16,10,11, 7};                                        // =++--++=
static const char vlist4157[] = { 2,17,12,16,10,11, 7};                                        // =++--+++
static const char vlist4158[] = { 3,13,16,14,13,14, 3,16,17,14};                               // =++-=---
static const char vlist4159[] = { 3, 3, 0,14, 3,13, 0, 0,16,17};                               // =++-=--=
static const char vlist4160[] = { 3, 3, 9,14, 3,13, 8,12,16,17};                               // =++-=--+
static const char vlist4161[] = { 3, 3, 1,14,14, 1,17,17, 1,16};                               // =++-=-=-
static const char vlist4162[] = { 2,17, 0,16, 3, 0,14};                                        // =++-=-==
static const char vlist4163[] = { 2, 3, 9,14,17,12,16};                                        // =++-=-=+
static const char vlist4164[] = { 3, 3, 8,14, 8,16,17,14, 8,17};                               // =++-=-+-
static const char vlist4165[] = { 2,17, 0,16, 3, 0,14};                                        // =++-=-+=
static const char vlist4166[] = { 2, 3, 9,14,17,12,16};                                        // =++-=-++
static const char vlist4167[] = { 3, 3,13, 2,13,16, 2, 2,16,17};                               // =++-==--
static const char vlist4168[] = { 2, 0, 3,13,17, 0,16};                                        // =++-==-=
static const char vlist4169[] = { 2, 8, 3,13,17,12,16};                                        // =++-==-+
static const char vlist4170[] = { 2,17, 2,16, 2, 1,16};                                        // =++-===-
static const char vlist4171[] = { 1,17, 0,16};                                                 // =++-====
static const char vlist4172[] = { 1,17,12,16};                                                 // =++-===+
static const char vlist4173[] = { 2,17, 2, 8,17, 8,16};                                        // =++-==+-
static const char vlist4174[] = { 1,17, 0,16};                                                 // =++-==+=
static const char vlist4175[] = { 1,17,12,16};                                                 // =++-==++
static const char vlist4176[] = { 3, 3,13, 9,13,16, 9, 9,16,17};                               // =++-=+--
static const char vlist4177[] = { 2, 0, 3,13,17, 0,16};                                        // =++-=+-=
static const char vlist4178[] = { 2, 8, 3,13,17,12,16};                                        // =++-=+-+
static const char vlist4179[] = { 2, 9, 1,16,17, 9,16};                                        // =++-=+=-
static const char vlist4180[] = { 1,17, 0,16};                                                 // =++-=+==
static const char vlist4181[] = { 1,17,12,16};                                                 // =++-=+=+
static const char vlist4182[] = { 2,16, 9, 8,17, 9,16};                                        // =++-=++-
static const char vlist4183[] = { 1,17, 0,16};                                                 // =++-=++=
static const char vlist4184[] = { 1,17,12,16};                                                 // =++-=+++
static const char vlist4185[] = { 4,13,16,14,13,14,11,10,13,11,16,17,14};                      // =++-+---
static const char vlist4186[] = { 3,10,13, 0, 0,16,17,11, 0,14};                               // =++-+--=
static const char vlist4187[] = { 3,10,13, 8,11, 9,14,12,16,17};                               // =++-+--+
static const char vlist4188[] = { 3,11, 1,14,14, 1,17,17, 1,16};                               // =++-+-=-
static const char vlist4189[] = { 2,17, 0,16,11, 0,14};                                        // =++-+-==
static const char vlist4190[] = { 2,11, 9,14,17,12,16};                                        // =++-+-=+
static const char vlist4191[] = { 3, 8,16,17,14, 8,17,11, 8,14};                               // =++-+-+-
static const char vlist4192[] = { 2,17, 0,16,11, 0,14};                                        // =++-+-+=
static const char vlist4193[] = { 2,11, 9,14,17,12,16};                                        // =++-+-++
static const char vlist4194[] = { 3,10,13, 2,13,16, 2, 2,16,17};                               // =++-+=--
static const char vlist4195[] = { 2, 0,10,13,17, 0,16};                                        // =++-+=-=
static const char vlist4196[] = { 2,17,12,16, 8,10,13};                                        // =++-+=-+
static const char vlist4197[] = { 2,17, 2,16, 2, 1,16};                                        // =++-+==-
static const char vlist4198[] = { 1,17, 0,16};                                                 // =++-+===
static const char vlist4199[] = { 1,17,12,16};                                                 // =++-+==+
static const char vlist4200[] = { 2,17, 2, 8,17, 8,16};                                        // =++-+=+-
static const char vlist4201[] = { 1,17, 0,16};                                                 // =++-+=+=
static const char vlist4202[] = { 1,17,12,16};                                                 // =++-+=++
static const char vlist4203[] = { 3,10,13, 9,13,16, 9, 9,16,17};                               // =++-++--
static const char vlist4204[] = { 2, 0,10,13,17, 0,16};                                        // =++-++-=
static const char vlist4205[] = { 2,17,12,16, 8,10,13};                                        // =++-++-+
static const char vlist4206[] = { 2, 9, 1,16,17, 9,16};                                        // =++-++=-
static const char vlist4207[] = { 1,17, 0,16};                                                 // =++-++==
static const char vlist4208[] = { 1,17,12,16};                                                 // =++-++=+
static const char vlist4209[] = { 2,16, 9, 8,17, 9,16};                                        // =++-+++-
static const char vlist4210[] = { 1,17, 0,16};                                                 // =++-+++=
static const char vlist4211[] = { 1,17,12,16};                                                 // =++-++++
static const char vlist4212[] = { 2, 4,14,13,14, 7,13};                                        // =++=----
static const char vlist4213[] = { 2, 7, 0,14,13, 0, 7};                                        // =++=---=
static const char vlist4214[] = { 3,13, 8, 7, 8, 9, 7, 7, 9,14};                               // =++=---+
static const char vlist4215[] = { 2, 1, 4,14, 7, 1,14};                                        // =++=--=-
static const char vlist4216[] = { 2, 7, 1,14, 1, 0,14};                                        // =++=--==
static const char vlist4217[] = { 2, 7, 1, 9, 7, 9,14};                                        // =++=--=+
static const char vlist4218[] = { 3, 8, 4,14,10, 8,14, 7,10,14};                               // =++=--+-
static const char vlist4219[] = { 2,10, 0,14, 7,10,14};                                        // =++=--+=
static const char vlist4220[] = { 2,14,10, 9, 7,10,14};                                        // =++=--++
static const char vlist4221[] = { 2,13, 4, 2,13, 2, 7};                                        // =++=-=--
static const char vlist4222[] = { 2, 2, 7,13, 0, 2,13};                                        // =++=-=-=
static const char vlist4223[] = { 2, 2, 7, 8, 8, 7,13};                                        // =++=-=-+
static const char vlist4224[] = { 2, 4, 2, 1, 1, 2, 7};                                        // =++=-==-
static const char vlist4225[] = { 1, 1, 2, 7};                                                 // =++=-===
static const char vlist4226[] = { 1, 1, 2, 7};                                                 // =++=-==+
static const char vlist4227[] = { 2, 4, 2, 8,10, 2, 7};                                        // =++=-=+-
static const char vlist4228[] = { 1,10, 2, 7};                                                 // =++=-=+=
static const char vlist4229[] = { 1,10, 2, 7};                                                 // =++=-=++
static const char vlist4230[] = { 3,13, 4, 9,13, 9,11,13,11, 7};                               // =++=-+--
static const char vlist4231[] = { 2, 0,11,13,11, 7,13};                                        // =++=-+-=
static const char vlist4232[] = { 2,11,13, 8,11, 7,13};                                        // =++=-+-+
static const char vlist4233[] = { 2, 4, 9, 1, 1,11, 7};                                        // =++=-+=-
static const char vlist4234[] = { 1, 1,11, 7};                                                 // =++=-+==
static const char vlist4235[] = { 1, 1,11, 7};                                                 // =++=-+=+
static const char vlist4236[] = { 2, 4, 9, 8,10,11, 7};                                        // =++=-++-
static const char vlist4237[] = { 1,10,11, 7};                                                 // =++=-++=
static const char vlist4238[] = { 1,10,11, 7};                                                 // =++=-+++
static const char vlist4239[] = { 2, 3, 4,14,13, 4, 3};                                        // =++==---
static const char vlist4240[] = { 2,14, 3, 0, 0, 3,13};                                        // =++==--=
static const char vlist4241[] = { 2,14, 3, 9, 8, 3,13};                                        // =++==--+
static const char vlist4242[] = { 2,14, 3, 1, 4,14, 1};                                        // =++==-=-
static const char vlist4243[] = { 1, 3, 0,14};                                                 // =++==-==
static const char vlist4244[] = { 1, 3, 9,14};                                                 // =++==-=+
static const char vlist4245[] = { 2, 3, 8,14, 8, 4,14};                                        // =++==-+-
static const char vlist4246[] = { 1, 3, 0,14};                                                 // =++==-+=
static const char vlist4247[] = { 1, 3, 9,14};                                                 // =++==-++
static const char vlist4248[] = { 2, 3,13, 2,13, 4, 2};                                        // =++===--
static const char vlist4249[] = { 1, 0, 3,13};                                                 // =++===-=
static const char vlist4250[] = { 1, 8, 3,13};                                                 // =++===-+
static const char vlist4251[] = { 1, 2, 1, 4};                                                 // =++====-
                                                                                               // =++=====
                                                                                               // =++====+
static const char vlist4254[] = { 1, 2, 8, 4};                                                 // =++===+-
                                                                                               // =++===+=
                                                                                               // =++===++
static const char vlist4257[] = { 2, 9, 3,13, 4, 9,13};                                        // =++==+--
static const char vlist4258[] = { 1, 0, 3,13};                                                 // =++==+-=
static const char vlist4259[] = { 1, 8, 3,13};                                                 // =++==+-+
static const char vlist4260[] = { 1, 9, 1, 4};                                                 // =++==+=-
                                                                                               // =++==+==
                                                                                               // =++==+=+
static const char vlist4263[] = { 1, 9, 8, 4};                                                 // =++==++-
                                                                                               // =++==++=
                                                                                               // =++==+++
static const char vlist4266[] = { 3,11, 4,14,10, 4,11,13, 4,10};                               // =++=+---
static const char vlist4267[] = { 2,14,11, 0, 0,10,13};                                        // =++=+--=
static const char vlist4268[] = { 2,14,11, 9, 8,10,13};                                        // =++=+--+
static const char vlist4269[] = { 2,11, 4,14, 4,11, 1};                                        // =++=+-=-
static const char vlist4270[] = { 1,11, 0,14};                                                 // =++=+-==
static const char vlist4271[] = { 1,11, 9,14};                                                 // =++=+-=+
static const char vlist4272[] = { 2,11, 8,14, 8, 4,14};                                        // =++=+-+-
static const char vlist4273[] = { 1,11, 0,14};                                                 // =++=+-+=
static const char vlist4274[] = { 1,11, 9,14};                                                 // =++=+-++
static const char vlist4275[] = { 2, 4,10,13,10, 4, 2};                                        // =++=+=--
static const char vlist4276[] = { 1, 0,10,13};                                                 // =++=+=-=
static const char vlist4277[] = { 1, 8,10,13};                                                 // =++=+=-+
static const char vlist4278[] = { 1, 2, 1, 4};                                                 // =++=+==-
                                                                                               // =++=+===
                                                                                               // =++=+==+
static const char vlist4281[] = { 1, 2, 8, 4};                                                 // =++=+=+-
                                                                                               // =++=+=+=
                                                                                               // =++=+=++
static const char vlist4284[] = { 2, 9,10,13, 4, 9,13};                                        // =++=++--
static const char vlist4285[] = { 1, 0,10,13};                                                 // =++=++-=
static const char vlist4286[] = { 1, 8,10,13};                                                 // =++=++-+
static const char vlist4287[] = { 1, 9, 1, 4};                                                 // =++=++=-
                                                                                               // =++=++==
                                                                                               // =++=++=+
static const char vlist4290[] = { 1, 9, 8, 4};                                                 // =++=+++-
                                                                                               // =++=+++=
                                                                                               // =++=++++
static const char vlist4293[] = { 2, 7,13,14,14,13,12};                                        // =+++----
static const char vlist4294[] = { 2, 7, 0,14,13, 0, 7};                                        // =+++---=
static const char vlist4295[] = { 3,13, 8, 7, 8, 9, 7, 7, 9,14};                               // =+++---+
static const char vlist4296[] = { 2, 1,12,14, 7, 1,14};                                        // =+++--=-
static const char vlist4297[] = { 2, 7, 1,14, 1, 0,14};                                        // =+++--==
static const char vlist4298[] = { 2, 7, 1, 9, 7, 9,14};                                        // =+++--=+
static const char vlist4299[] = { 3,10, 8,14, 8,12,14, 7,10,14};                               // =+++--+-
static const char vlist4300[] = { 2,10, 0,14, 7,10,14};                                        // =+++--+=
static const char vlist4301[] = { 2,14,10, 9, 7,10,14};                                        // =+++--++
static const char vlist4302[] = { 2,13,12, 2,13, 2, 7};                                        // =+++-=--
static const char vlist4303[] = { 2, 2, 7,13, 0, 2,13};                                        // =+++-=-=
static const char vlist4304[] = { 2, 2, 7, 8, 8, 7,13};                                        // =+++-=-+
static const char vlist4305[] = { 2, 1, 2, 7,12, 2, 1};                                        // =+++-==-
static const char vlist4306[] = { 1, 1, 2, 7};                                                 // =+++-===
static const char vlist4307[] = { 1, 1, 2, 7};                                                 // =+++-==+
static const char vlist4308[] = { 2,10, 2, 7,12, 2, 8};                                        // =+++-=+-
static const char vlist4309[] = { 1,10, 2, 7};                                                 // =+++-=+=
static const char vlist4310[] = { 1,10, 2, 7};                                                 // =+++-=++
static const char vlist4311[] = { 3,13,12, 9,13,11, 7,13, 9,11};                               // =+++-+--
static const char vlist4312[] = { 2, 0,11,13,11, 7,13};                                        // =+++-+-=
static const char vlist4313[] = { 2,11,13, 8,11, 7,13};                                        // =+++-+-+
static const char vlist4314[] = { 2,12, 9, 1, 1,11, 7};                                        // =+++-+=-
static const char vlist4315[] = { 1, 1,11, 7};                                                 // =+++-+==
static const char vlist4316[] = { 1, 1,11, 7};                                                 // =+++-+=+
static const char vlist4317[] = { 2,12, 9, 8,10,11, 7};                                        // =+++-++-
static const char vlist4318[] = { 1,10,11, 7};                                                 // =+++-++=
static const char vlist4319[] = { 1,10,11, 7};                                                 // =+++-+++
static const char vlist4320[] = { 2, 3,12,14,13,12, 3};                                        // =+++=---
static const char vlist4321[] = { 2,14, 3, 0, 0, 3,13};                                        // =+++=--=
static const char vlist4322[] = { 2,14, 3, 9, 8, 3,13};                                        // =+++=--+
static const char vlist4323[] = { 2, 3,12,14,12, 3, 1};                                        // =+++=-=-
static const char vlist4324[] = { 1, 3, 0,14};                                                 // =+++=-==
static const char vlist4325[] = { 1, 3, 9,14};                                                 // =+++=-=+
static const char vlist4326[] = { 2, 3, 8,14, 8,12,14};                                        // =+++=-+-
static const char vlist4327[] = { 1, 3, 0,14};                                                 // =+++=-+=
static const char vlist4328[] = { 1, 3, 9,14};                                                 // =+++=-++
static const char vlist4329[] = { 2,12, 3,13, 3,12, 2};                                        // =+++==--
static const char vlist4330[] = { 1, 0, 3,13};                                                 // =+++==-=
static const char vlist4331[] = { 1, 8, 3,13};                                                 // =+++==-+
static const char vlist4332[] = { 1, 2, 1,12};                                                 // =+++===-
                                                                                               // =+++====
                                                                                               // =+++===+
static const char vlist4335[] = { 1, 2, 8,12};                                                 // =+++==+-
                                                                                               // =+++==+=
                                                                                               // =+++==++
static const char vlist4338[] = { 2, 9, 3,13,12, 9,13};                                        // =+++=+--
static const char vlist4339[] = { 1, 0, 3,13};                                                 // =+++=+-=
static const char vlist4340[] = { 1, 8, 3,13};                                                 // =+++=+-+
static const char vlist4341[] = { 1, 9, 1,12};                                                 // =+++=+=-
                                                                                               // =+++=+==
                                                                                               // =+++=+=+
static const char vlist4344[] = { 1, 9, 8,12};                                                 // =+++=++-
                                                                                               // =+++=++=
                                                                                               // =+++=+++
static const char vlist4347[] = { 3,11,12,14,10,12,11,13,12,10};                               // =++++---
static const char vlist4348[] = { 2,14,11, 0, 0,10,13};                                        // =++++--=
static const char vlist4349[] = { 2,14,11, 9, 8,10,13};                                        // =++++--+
static const char vlist4350[] = { 2,12,11, 1,11,12,14};                                        // =++++-=-
static const char vlist4351[] = { 1,11, 0,14};                                                 // =++++-==
static const char vlist4352[] = { 1,11, 9,14};                                                 // =++++-=+
static const char vlist4353[] = { 2,11,12,14,11, 8,12};                                        // =++++-+-
static const char vlist4354[] = { 1,11, 0,14};                                                 // =++++-+=
static const char vlist4355[] = { 1,11, 9,14};                                                 // =++++-++
static const char vlist4356[] = { 2,10,12, 2,12,10,13};                                        // =++++=--
static const char vlist4357[] = { 1, 0,10,13};                                                 // =++++=-=
static const char vlist4358[] = { 1, 8,10,13};                                                 // =++++=-+
static const char vlist4359[] = { 1, 2, 1,12};                                                 // =++++==-
                                                                                               // =++++===
                                                                                               // =++++==+
static const char vlist4362[] = { 1, 2, 8,12};                                                 // =++++=+-
                                                                                               // =++++=+=
                                                                                               // =++++=++
static const char vlist4365[] = { 2,12,10,13, 9,10,12};                                        // =+++++--
static const char vlist4366[] = { 1, 0,10,13};                                                 // =+++++-=
static const char vlist4367[] = { 1, 8,10,13};                                                 // =+++++-+
static const char vlist4368[] = { 1, 9, 1,12};                                                 // =+++++=-
                                                                                               // =+++++==
                                                                                               // =+++++=+
static const char vlist4371[] = { 1, 9, 8,12};                                                 // =++++++-
                                                                                               // =++++++=
                                                                                               // =+++++++
static const char vlist4374[] = { 1,15,18,19};                                                 // +-------
static const char vlist4375[] = { 3,19, 0,18,19,15, 0, 0,15,18};                               // +------=
static const char vlist4376[] = { 6,19,15, 9,12,19, 9,18,19,12, 8,18,12,18, 8,15,15, 8, 9};    // +------+
static const char vlist4377[] = { 2, 1,18,19,15, 1,19};                                        // +-----=-
static const char vlist4378[] = { 4, 0, 1,15,18, 1, 0,18,19, 0, 0,19,15};                      // +-----==
static const char vlist4379[] = { 5,15, 1, 9, 1,18,12,18,19,12,19,15, 9,12,19, 9};             // +-----=+
static const char vlist4380[] = { 4, 8,15,10, 8,19,15,19, 8,18,18, 8,13};                      // +-----+-
static const char vlist4381[] = { 4,13,18, 0,18,19, 0, 0,19,15, 0,15,10};                      // +-----+=
static const char vlist4382[] = { 5,13,18,12,18,19,12,12,19, 9,19,15, 9,15,10, 9};             // +-----++
static const char vlist4383[] = { 2,19, 2,18, 2,15,18};                                        // +----=--
static const char vlist4384[] = { 4, 0, 2,15,19, 2, 0,19, 0,18,18, 0,15};                      // +----=-=
static const char vlist4385[] = { 5, 2,15, 8,19, 2,12,19,12,18,18,12, 8,15,18, 8};             // +----=-+
static const char vlist4386[] = { 3, 2, 1,15,18, 2, 1,19, 2,18};                               // +----==-
static const char vlist4387[] = { 4, 2, 1,15, 0, 1,18, 0,18,19, 2, 0,19};                      // +----===
static const char vlist4388[] = { 4, 2, 1,15,12, 1,18, 2,12,19,12,18,19};                      // +----==+
static const char vlist4389[] = { 4,19, 2, 8,15, 2,10,13,18, 8,18,19, 8};                      // +----=+-
static const char vlist4390[] = { 4, 0,13,18, 0,18,19, 2, 0,19, 2,10,15};                      // +----=+=
static const char vlist4391[] = { 4, 2,12,19,12,18,19,12,13,18,10, 2,15};                      // +----=++
static const char vlist4392[] = { 4,15, 9,11,18, 9,15, 9,18,14,18,19,14};                      // +----+--
static const char vlist4393[] = { 4,19,14, 0,18,19, 0,18, 0,15,15, 0,11};                      // +----+-=
static const char vlist4394[] = { 5,19,14,12,19,12,18,18,12, 8,15,18, 8,11,15, 8};             // +----+-+
static const char vlist4395[] = { 4,15,11, 1, 1,18, 9,18,14, 9,19,14,18};                      // +----+=-
static const char vlist4396[] = { 4,11, 1,15, 0, 1,18,19, 0,18,14, 0,19};                      // +----+==
static const char vlist4397[] = { 4, 1,11,15,12, 1,18,14,12,19,12,18,19};                      // +----+=+
static const char vlist4398[] = { 5,15,11,10,13, 9, 8,14, 9,13,18,14,13,19,14,18};             // +----++-
static const char vlist4399[] = { 4, 0,13,18,19, 0,18,14, 0,19,10,11,15};                      // +----++=
static const char vlist4400[] = { 4,14,12,19,12,18,19,12,13,18,10,11,15};                      // +----+++
static const char vlist4401[] = { 1, 3,18,19};                                                 // +---=---
static const char vlist4402[] = { 3, 0, 3,18,19, 3, 0,19, 0,18};                               // +---=--=
static const char vlist4403[] = { 5, 3,18, 8,19, 3, 9,19, 9,12,18,12, 8,19,12,18};             // +---=--+
static const char vlist4404[] = { 2, 3, 1,19, 1,18,19};                                        // +---=-=-
static const char vlist4405[] = { 3, 0, 1,18, 0,18,19, 3, 0,19};                               // +---=-==
static const char vlist4406[] = { 4,12, 1,18, 3, 9,19, 9,12,19,12,18,19};                      // +---=-=+
static const char vlist4407[] = { 3,19, 3, 8,13,18, 8,18,19, 8};                               // +---=-+-
static const char vlist4408[] = { 3, 0,13,18, 0,18,19, 3, 0,19};                               // +---=-+=
static const char vlist4409[] = { 4, 3, 9,19, 9,12,19,12,18,19,12,13,18};                      // +---=-++
static const char vlist4410[] = { 2, 2, 3,18,19, 2,18};                                        // +---==--
static const char vlist4411[] = { 3, 2, 0,19, 0,18,19, 0, 3,18};                               // +---==-=
static const char vlist4412[] = { 4, 2,12,19, 8, 3,18,12, 8,18,12,18,19};                      // +---==-+
static const char vlist4413[] = { 2, 2,18,19, 2, 1,18};                                        // +---===-
static const char vlist4414[] = { 3, 0, 1,18, 0,18,19, 2, 0,19};                               // +---====
static const char vlist4415[] = { 3,12, 1,18, 2,12,19,12,18,19};                               // +---===+
static const char vlist4416[] = { 3, 2, 8,19, 8,13,18,19, 8,18};                               // +---==+-
static const char vlist4417[] = { 3, 0,13,18, 0,18,19, 2, 0,19};                               // +---==+=
static const char vlist4418[] = { 3, 2,12,19,12,18,19,12,13,18};                               // +---==++
static const char vlist4419[] = { 3, 3,18, 9,19,14,18,18,14, 9};                               // +---=+--
static const char vlist4420[] = { 3,14, 0,19,19, 0,18, 0, 3,18};                               // +---=+-=
static const char vlist4421[] = { 4, 8, 3,18,12, 8,18,12,18,19,14,12,19};                      // +---=+-+
static const char vlist4422[] = { 3, 9, 1,18,14, 9,18,14,18,19};                               // +---=+=-
static const char vlist4423[] = { 3, 0, 1,18,19, 0,18,14, 0,19};                               // +---=+==
static const char vlist4424[] = { 3,12, 1,18,14,12,19,12,18,19};                               // +---=+=+
static const char vlist4425[] = { 4, 9, 8,13, 9,13,14,14,13,18,14,18,19};                      // +---=++-
static const char vlist4426[] = { 3, 0,13,18,19, 0,18,14, 0,19};                               // +---=++=
static const char vlist4427[] = { 3,14,12,19,12,18,19,12,13,18};                               // +---=+++
static const char vlist4428[] = { 2,10,18,11,18,19,11};                                        // +---+---
static const char vlist4429[] = { 3,19,11, 0,19, 0,18,18, 0,10};                               // +---+--=
static const char vlist4430[] = { 5,19,11, 9,19,12,18,19, 9,12,18,12, 8,10,18, 8};             // +---+--+
static const char vlist4431[] = { 2, 1,19,11, 1,18,19};                                        // +---+-=-
static const char vlist4432[] = { 3, 0, 1,18, 0,18,19,11, 0,19};                               // +---+-==
static const char vlist4433[] = { 4,12, 1,18,11, 9,19, 9,12,19,12,18,19};                      // +---+-=+
static const char vlist4434[] = { 3,13,18, 8,18,19, 8,19,11, 8};                               // +---+-+-
static const char vlist4435[] = { 3, 0,13,18, 0,18,19,11, 0,19};                               // +---+-+=
static const char vlist4436[] = { 4,11, 9,19, 9,12,19,12,18,19,12,13,18};                      // +---+-++
static const char vlist4437[] = { 2,18, 2,10,19, 2,18};                                        // +---+=--
static const char vlist4438[] = { 3, 2, 0,19, 0,18,19, 0,10,18};                               // +---+=-=
static const char vlist4439[] = { 4, 2,12,19, 8,10,18,12, 8,18,12,18,19};                      // +---+=-+
static const char vlist4440[] = { 2, 2,18,19, 2, 1,18};                                        // +---+==-
static const char vlist4441[] = { 3, 0, 1,18, 0,18,19, 2, 0,19};                               // +---+===
static const char vlist4442[] = { 3,12, 1,18, 2,12,19,12,18,19};                               // +---+==+
static const char vlist4443[] = { 3, 2, 8,19, 8,13,18,19, 8,18};                               // +---+=+-
static const char vlist4444[] = { 3, 0,13,18, 0,18,19, 2, 0,19};                               // +---+=+=
static const char vlist4445[] = { 3, 2,12,19,12,18,19,12,13,18};                               // +---+=++
static const char vlist4446[] = { 3,19,14,18,18,14, 9,10,18, 9};                               // +---++--
static const char vlist4447[] = { 3,14, 0,19,19, 0,18, 0,10,18};                               // +---++-=
static const char vlist4448[] = { 4, 8,10,18,12, 8,18,12,18,19,14,12,19};                      // +---++-+
static const char vlist4449[] = { 3, 9, 1,18,14, 9,18,14,18,19};                               // +---++=-
static const char vlist4450[] = { 3, 0, 1,18,19, 0,18,14, 0,19};                               // +---++==
static const char vlist4451[] = { 3,12, 1,18,14,12,19,12,18,19};                               // +---++=+
static const char vlist4452[] = { 4, 9, 8,13, 9,13,14,14,13,18,14,18,19};                      // +---+++-
static const char vlist4453[] = { 3, 0,13,18,19, 0,18,14, 0,19};                               // +---+++=
static const char vlist4454[] = { 3,14,12,19,12,18,19,12,13,18};                               // +---++++
static const char vlist4455[] = { 2,19,15, 4, 4,15,18};                                        // +--=----
static const char vlist4456[] = { 4, 0, 4,18,19, 4, 0, 0,19,15, 0,15,18};                      // +--=---=
static const char vlist4457[] = { 5,18, 4, 8, 4,19, 9, 9,19,15, 9,15, 8,15,18, 8};             // +--=---+
static const char vlist4458[] = { 3, 4,19,15, 1, 4,15, 4,18, 1};                               // +--=--=-
static const char vlist4459[] = { 4, 1, 4,18, 1, 0,15,15, 0,19, 0, 4,19};                      // +--=--==
static const char vlist4460[] = { 4, 4, 1,18, 1, 9,15, 9, 4,19,15, 9,19};                      // +--=--=+
static const char vlist4461[] = { 4, 4,19, 8, 4,18,13,15,10, 8,19,15, 8};                      // +--=--+-
static const char vlist4462[] = { 4,10, 0,15,15, 0,19, 0, 4,19, 4,13,18};                      // +--=--+=
static const char vlist4463[] = { 4, 9, 4,19,15, 9,19,10, 9,15, 4,13,18};                      // +--=--++
static const char vlist4464[] = { 3,18, 4,15, 4, 2,15,19, 4, 2};                               // +--=-=--
static const char vlist4465[] = { 4, 4, 2,19, 0, 2,15, 0,15,18, 4, 0,18};                      // +--=-=-=
static const char vlist4466[] = { 4, 4, 2,19, 8, 2,15, 4, 8,18, 8,15,18};                      // +--=-=-+
static const char vlist4467[] = { 4, 2, 4, 1, 4, 2,19, 2, 1,15, 1, 4,18};                      // +--=-==-
static const char vlist4468[] = { 3, 2, 1,15,18, 1, 4, 2,19, 4};                               // +--=-===
static const char vlist4469[] = { 3, 1, 2,15, 1,18, 4,19, 2, 4};                               // +--=-==+
static const char vlist4470[] = { 4, 4, 2, 8, 4, 2,19,10, 2,15, 4,13,18};                      // +--=-=+-
static const char vlist4471[] = { 3,19, 2, 4,10, 2,15,13,18, 4};                               // +--=-=+=
static const char vlist4472[] = { 3,19, 2, 4,10, 2,15,13,18, 4};                               // +--=-=++
static const char vlist4473[] = { 4,18, 4, 9,19, 4,14,11,15, 9,15,18, 9};                      // +--=-+--
static const char vlist4474[] = { 4, 0,11,15, 0,15,18, 4, 0,18, 4,14,19};                      // +--=-+-=
static const char vlist4475[] = { 4, 4, 8,18, 8,15,18, 8,11,15,14, 4,19};                      // +--=-+-+
static const char vlist4476[] = { 4, 4, 9, 1, 4, 1,18, 1,11,15, 4,14,19};                      // +--=-+=-
static const char vlist4477[] = { 3, 1,11,15, 1,18, 4,19,14, 4};                               // +--=-+==
static const char vlist4478[] = { 3, 1,11,15, 1,18, 4,19,14, 4};                               // +--=-+=+
static const char vlist4479[] = { 4, 4, 9, 8,10,11,15, 4,13,18,14, 4,19};                      // +--=-++-
static const char vlist4480[] = { 3,10,11,15,13,18, 4,19,14, 4};                               // +--=-++=
static const char vlist4481[] = { 3,10,11,15,13,18, 4,19,14, 4};                               // +--=-+++
static const char vlist4482[] = { 2, 4, 3,18, 4,19, 3};                                        // +--==---
static const char vlist4483[] = { 4,19, 3, 0, 0, 3,18, 4,19, 0, 4, 0,18};                      // +--==--=
static const char vlist4484[] = { 4, 3, 9,19, 3,18, 8, 9, 4,19,18, 4, 8};                      // +--==--+
static const char vlist4485[] = { 3, 4, 3, 1, 3, 4,19, 1, 4,18};                               // +--==-=-
static const char vlist4486[] = { 3,18, 1, 4,19, 0, 4, 3, 0,19};                               // +--==-==
static const char vlist4487[] = { 3, 1,18, 4,19, 3, 9,19, 9, 4};                               // +--==-=+
static const char vlist4488[] = { 3, 4, 3, 8, 3, 4,19, 4,13,18};                               // +--==-+-
static const char vlist4489[] = { 3,19, 0, 4, 3, 0,19,13,18, 4};                               // +--==-+=
static const char vlist4490[] = { 3,19, 3, 9,13,18, 4,19, 9, 4};                               // +--==-++
static const char vlist4491[] = { 3, 3, 4, 2, 4, 3,18, 4, 2,19};                               // +--===--
static const char vlist4492[] = { 3,19, 2, 4, 0,18, 4, 0, 3,18};                               // +--===-=
static const char vlist4493[] = { 3,19, 2, 4, 3,18, 8, 8,18, 4};                               // +--===-+
static const char vlist4494[] = { 3,19, 2, 4, 1, 2, 4,18, 1, 4};                               // +--====-
static const char vlist4495[] = { 2, 1,18, 4,19, 2, 4};                                        // +--=====
static const char vlist4496[] = { 2, 1,18, 4,19, 2, 4};                                        // +--====+
static const char vlist4497[] = { 3,19, 2, 4, 2, 8, 4,13,18, 4};                               // +--===+-
static const char vlist4498[] = { 2,19, 2, 4,13,18, 4};                                        // +--===+=
static const char vlist4499[] = { 2,19, 2, 4,13,18, 4};                                        // +--===++
static const char vlist4500[] = { 3, 3, 4, 9, 4, 3,18, 4,14,19};                               // +--==+--
static const char vlist4501[] = { 3, 0,18, 4, 0, 3,18,19,14, 4};                               // +--==+-=
static const char vlist4502[] = { 3, 3,18, 8,19,14, 4, 8,18, 4};                               // +--==+-+
static const char vlist4503[] = { 3, 9, 1, 4, 1,18, 4,19,14, 4};                               // +--==+=-
static const char vlist4504[] = { 2, 1,18, 4,19,14, 4};                                        // +--==+==
static const char vlist4505[] = { 2, 1,18, 4,19,14, 4};                                        // +--==+=+
static const char vlist4506[] = { 3, 9, 8, 4,13,18, 4,19,14, 4};                               // +--==++-
static const char vlist4507[] = { 2,13,18, 4,19,14, 4};                                        // +--==++=
static const char vlist4508[] = { 2,13,18, 4,19,14, 4};                                        // +--==+++
static const char vlist4509[] = { 3,18, 4,10, 4,11,10, 4,19,11};                               // +--=+---
static const char vlist4510[] = { 4,11, 4,19, 4,11, 0, 4, 0,10,18, 4,10};                      // +--=+--=
static const char vlist4511[] = { 4,11, 9,19, 9, 4,19,18, 4, 8,10,18, 8};                      // +--=+--+
static const char vlist4512[] = { 3, 4, 1,18, 4,11, 1,11, 4,19};                               // +--=+-=-
static const char vlist4513[] = { 3, 1,18, 4,11, 0, 4,19,11, 4};                               // +--=+-==
static const char vlist4514[] = { 3, 1,18, 4,19, 9, 4,19,11, 9};                               // +--=+-=+
static const char vlist4515[] = { 3, 4,11, 8,11, 4,19, 4,13,18};                               // +--=+-+-
static const char vlist4516[] = { 3,11, 0, 4,13,18, 4,19,11, 4};                               // +--=+-+=
static const char vlist4517[] = { 3,13,18, 4,19, 9, 4,19,11, 9};                               // +--=+-++
static const char vlist4518[] = { 3, 4, 2,19,10, 4, 2, 4,10,18};                               // +--=+=--
static const char vlist4519[] = { 3,19, 2, 4, 0,10, 4,10,18, 4};                               // +--=+=-=
static const char vlist4520[] = { 3,19, 2, 4, 8,18, 4,10,18, 8};                               // +--=+=-+
static const char vlist4521[] = { 3,19, 2, 4, 2, 1, 4, 1,18, 4};                               // +--=+==-
static const char vlist4522[] = { 2, 1,18, 4,19, 2, 4};                                        // +--=+===
static const char vlist4523[] = { 2, 1,18, 4,19, 2, 4};                                        // +--=+==+
static const char vlist4524[] = { 3,19, 2, 4, 2, 8, 4,13,18, 4};                               // +--=+=+-
static const char vlist4525[] = { 2,19, 2, 4,13,18, 4};                                        // +--=+=+=
static const char vlist4526[] = { 2,19, 2, 4,13,18, 4};                                        // +--=+=++
static const char vlist4527[] = { 3,10, 4, 9, 4,10,18,14, 4,19};                               // +--=++--
static const char vlist4528[] = { 3, 0,10, 4,19,14, 4,10,18, 4};                               // +--=++-=
static const char vlist4529[] = { 3,19,14, 4, 8,18, 4,10,18, 8};                               // +--=++-+
static const char vlist4530[] = { 3, 9, 1, 4, 1,18, 4,19,14, 4};                               // +--=++=-
static const char vlist4531[] = { 2, 1,18, 4,19,14, 4};                                        // +--=++==
static const char vlist4532[] = { 2, 1,18, 4,19,14, 4};                                        // +--=++=+
static const char vlist4533[] = { 3, 9, 8, 4,13,18, 4,19,14, 4};                               // +--=+++-
static const char vlist4534[] = { 2,13,18, 4,19,14, 4};                                        // +--=+++=
static const char vlist4535[] = { 2,13,18, 4,19,14, 4};                                        // +--=++++
static const char vlist4536[] = { 4,17,19,15,12,17,15,12,15,16,15,18,16};                      // +--+----
static const char vlist4537[] = { 4, 0,17,19, 0,19,15, 0,15,18, 0,18,16};                      // +--+---=
static const char vlist4538[] = { 5,17,19, 9, 9,19,15, 9,15, 8,15,18, 8,18,16, 8};             // +--+---+
static const char vlist4539[] = { 4,16,18, 1,15, 1,12,17,15,12,19,15,17};                      // +--+--=-
static const char vlist4540[] = { 4,16, 1,18, 1, 0,15,15, 0,19,19, 0,17};                      // +--+--==
static const char vlist4541[] = { 4,16, 1,18, 1, 9,15, 9,17,19,15, 9,19};                      // +--+--=+
static const char vlist4542[] = { 5,18,16,13,12,10, 8,15,10,12,19,15,17,17,15,12};             // +--+--+-
static const char vlist4543[] = { 4,10, 0,15,15, 0,19,19, 0,17,16,13,18};                      // +--+--+=
static const char vlist4544[] = { 4, 9,17,19,15, 9,19,10, 9,15,16,13,18};                      // +--+--++
static const char vlist4545[] = { 4,19,17, 2, 2,15,12,15,16,12,15,18,16};                      // +--+-=--
static const char vlist4546[] = { 4,17, 2,19, 0, 2,15, 0,15,18, 0,18,16};                      // +--+-=-=
static const char vlist4547[] = { 4, 2,17,19, 8, 2,15,16, 8,18, 8,15,18};                      // +--+-=-+
static const char vlist4548[] = { 4,17, 2,19, 1, 2,15,12, 2, 1,16, 1,18};                      // +--+-==-
static const char vlist4549[] = { 3, 1, 2,15, 1,18,16,19, 2,17};                               // +--+-===
static const char vlist4550[] = { 3, 1, 2,15, 1,18,16,19, 2,17};                               // +--+-==+
static const char vlist4551[] = { 4, 2,17,19,10, 2,15,12, 2, 8,16,13,18};                      // +--+-=+-
static const char vlist4552[] = { 3,19, 2,17,10, 2,15,13,18,16};                               // +--+-=+=
static const char vlist4553[] = { 3,19, 2,17,10, 2,15,13,18,16};                               // +--+-=++
static const char vlist4554[] = { 5,19,17,14,11,12, 9,11,15,12,15,18,16,15,16,12};             // +--+-+--
static const char vlist4555[] = { 4, 0,11,15, 0,15,18, 0,18,16,14,17,19};                      // +--+-+-=
static const char vlist4556[] = { 4,16, 8,18, 8,15,18, 8,11,15,14,17,19};                      // +--+-+-+
static const char vlist4557[] = { 4,12, 9, 1,16, 1,18, 1,11,15,14,17,19};                      // +--+-+=-
static const char vlist4558[] = { 3, 1,11,15, 1,18,16,19,14,17};                               // +--+-+==
static const char vlist4559[] = { 3, 1,11,15, 1,18,16,19,14,17};                               // +--+-+=+
static const char vlist4560[] = { 4,12, 9, 8,10,11,15,16,13,18,14,17,19};                      // +--+-++-
static const char vlist4561[] = { 3,10,11,15,13,18,16,19,14,17};                               // +--+-++=
static const char vlist4562[] = { 3,10,11,15,13,18,16,19,14,17};                               // +--+-+++
static const char vlist4563[] = { 4,17, 3,12,17,19, 3, 3,18,16,12, 3,16};                      // +--+=---
static const char vlist4564[] = { 4, 3,17,19,17, 3, 0, 0, 3,16, 3,18,16};                      // +--+=--=
static const char vlist4565[] = { 4, 3, 9,19, 3,18, 8, 9,17,19,18,16, 8};                      // +--+=--+
static const char vlist4566[] = { 4, 3,17,19,17, 3,12,12, 3, 1,16, 1,18};                      // +--+=-=-
static const char vlist4567[] = { 3, 1,18,16, 3, 0,17,19, 3,17};                               // +--+=-==
static const char vlist4568[] = { 3, 1,18,16,19, 3, 9,19, 9,17};                               // +--+=-=+
static const char vlist4569[] = { 4, 3,17,19,17, 3,12,12, 3, 8,16,13,18};                      // +--+=-+-
static const char vlist4570[] = { 3, 3, 0,17,19, 3,17,13,18,16};                               // +--+=-+=
static const char vlist4571[] = { 3,19, 3, 9,13,18,16,19, 9,17};                               // +--+=-++
static const char vlist4572[] = { 4,16, 3,18, 3,16,12, 3,12, 2,17, 2,19};                      // +--+==--
static const char vlist4573[] = { 3,19, 2,17, 0, 3,16, 3,18,16};                               // +--+==-=
static const char vlist4574[] = { 3,19, 2,17, 3,18, 8, 8,18,16};                               // +--+==-+
static const char vlist4575[] = { 3,19, 2,17, 2, 1,12, 1,18,16};                               // +--+===-
static const char vlist4576[] = { 2, 1,18,16,19, 2,17};                                        // +--+====
static const char vlist4577[] = { 2, 1,18,16,19, 2,17};                                        // +--+===+
static const char vlist4578[] = { 3,19, 2,17, 2, 8,12,13,18,16};                               // +--+==+-
static const char vlist4579[] = { 2,19, 2,17,13,18,16};                                        // +--+==+=
static const char vlist4580[] = { 2,19, 2,17,13,18,16};                                        // +--+==++
static const char vlist4581[] = { 4,16, 3,18, 3,16,12, 3,12, 9,14,17,19};                      // +--+=+--
static const char vlist4582[] = { 3, 0, 3,16, 3,18,16,19,14,17};                               // +--+=+-=
static const char vlist4583[] = { 3, 3,18, 8,19,14,17, 8,18,16};                               // +--+=+-+
static const char vlist4584[] = { 3, 9, 1,12, 1,18,16,19,14,17};                               // +--+=+=-
static const char vlist4585[] = { 2, 1,18,16,19,14,17};                                        // +--+=+==
static const char vlist4586[] = { 2, 1,18,16,19,14,17};                                        // +--+=+=+
static const char vlist4587[] = { 3, 9, 8,12,13,18,16,19,14,17};                               // +--+=++-
static const char vlist4588[] = { 2,13,18,16,19,14,17};                                        // +--+=++=
static const char vlist4589[] = { 2,13,18,16,19,14,17};                                        // +--+=+++
static const char vlist4590[] = { 5,17,11,12,17,19,11,12,11,10,16,12,10,18,16,10};             // +--++---
static const char vlist4591[] = { 4,17,11, 0,11,17,19,16, 0,10,18,16,10};                      // +--++--=
static const char vlist4592[] = { 4,11,17,19,11, 9,17,10,18,16,10,16, 8};                      // +--++--+
static const char vlist4593[] = { 4,16, 1,18,11,17,19,17,11,12,12,11, 1};                      // +--++-=-
static const char vlist4594[] = { 3, 1,18,16,11, 0,17,19,11,17};                               // +--++-==
static const char vlist4595[] = { 3, 1,18,16,17,11, 9,19,11,17};                               // +--++-=+
static const char vlist4596[] = { 4,11,17,19,17,11,12,12,11, 8,16,13,18};                      // +--++-+-
static const char vlist4597[] = { 3,11, 0,17,13,18,16,19,11,17};                               // +--++-+=
static const char vlist4598[] = { 3,13,18,16,17,11, 9,19,11,17};                               // +--++-++
static const char vlist4599[] = { 4, 2,17,19,16,10,18,10,16,12,10,12, 2};                      // +--++=--
static const char vlist4600[] = { 3,19, 2,17, 0,10,16,10,18,16};                               // +--++=-=
static const char vlist4601[] = { 3,19, 2,17,10,16, 8,10,18,16};                               // +--++=-+
static const char vlist4602[] = { 3,19, 2,17, 2, 1,12, 1,18,16};                               // +--++==-
static const char vlist4603[] = { 2, 1,18,16,19, 2,17};                                        // +--++===
static const char vlist4604[] = { 2, 1,18,16,19, 2,17};                                        // +--++==+
static const char vlist4605[] = { 3,19, 2,17, 2, 8,12,13,18,16};                               // +--++=+-
static const char vlist4606[] = { 2,19, 2,17,13,18,16};                                        // +--++=+=
static const char vlist4607[] = { 2,19, 2,17,13,18,16};                                        // +--++=++
static const char vlist4608[] = { 4,16,10,18,10,16,12,10,12, 9,14,17,19};                      // +--+++--
static const char vlist4609[] = { 3, 0,10,16,19,14,17,10,18,16};                               // +--+++-=
static const char vlist4610[] = { 3,19,14,17,10,16, 8,10,18,16};                               // +--+++-+
static const char vlist4611[] = { 3, 9, 1,12, 1,18,16,19,14,17};                               // +--+++=-
static const char vlist4612[] = { 2, 1,18,16,19,14,17};                                        // +--+++==
static const char vlist4613[] = { 2, 1,18,16,19,14,17};                                        // +--+++=+
static const char vlist4614[] = { 3, 9, 8,12,13,18,16,19,14,17};                               // +--++++-
static const char vlist4615[] = { 2,13,18,16,19,14,17};                                        // +--++++=
static const char vlist4616[] = { 2,13,18,16,19,14,17};                                        // +--+++++
static const char vlist4617[] = { 1,15, 5,19};                                                 // +-=-----
static const char vlist4618[] = { 3, 0, 5,15,19, 5, 0, 0,19,15};                               // +-=----=
static const char vlist4619[] = { 5,15, 5, 8, 5,19,12,12,19, 9, 9,15, 8, 9,19,15};             // +-=----+
static const char vlist4620[] = { 2, 1, 5,19,15, 1,19};                                        // +-=---=-
static const char vlist4621[] = { 3, 1, 0,15,15, 0,19, 0, 5,19};                               // +-=---==
static const char vlist4622[] = { 4, 1, 9,15, 9,12,19,15, 9,19,12, 5,19};                      // +-=---=+
static const char vlist4623[] = { 3, 5,19, 8,15,10, 8,19,15, 8};                               // +-=---+-
static const char vlist4624[] = { 3,10, 0,15,15, 0,19, 0, 5,19};                               // +-=---+=
static const char vlist4625[] = { 4, 9,12,19,15, 9,19,10, 9,15,12, 5,19};                      // +-=---++
static const char vlist4626[] = { 2, 5, 2,15, 5,19, 2};                                        // +-=--=--
static const char vlist4627[] = { 4,19, 2, 0, 0, 2,15, 5,19, 0, 5, 0,15};                      // +-=--=-=
static const char vlist4628[] = { 4, 2,12,19, 2,15, 8,12, 5,19,15, 5, 8};                      // +-=--=-+
static const char vlist4629[] = { 3, 5, 2, 1, 2, 5,19, 2, 1,15};                               // +-=--==-
static const char vlist4630[] = { 3, 2, 1,15,19, 0, 5, 2, 0,19};                               // +-=--===
static const char vlist4631[] = { 3, 1, 2,15,19, 2,12,19,12, 5};                               // +-=--==+
static const char vlist4632[] = { 3, 5, 2, 8, 2, 5,19,10, 2,15};                               // +-=--=+-
static const char vlist4633[] = { 3,19, 0, 5, 2, 0,19,10, 2,15};                               // +-=--=+=
static const char vlist4634[] = { 3,19, 2,12,10, 2,15,19,12, 5};                               // +-=--=++
static const char vlist4635[] = { 4,15, 5,11, 5, 9,11, 5,14, 9, 5,19,14};                      // +-=--+--
static const char vlist4636[] = { 4,14, 5,19, 5,14, 0, 5, 0,11,15, 5,11};                      // +-=--+-=
static const char vlist4637[] = { 4,14,12,19,12, 5,19,15, 5, 8,11,15, 8};                      // +-=--+-+
static const char vlist4638[] = { 4, 1,11,15, 5, 9, 1, 5,14, 9,14, 5,19};                      // +-=--+=-
static const char vlist4639[] = { 3, 1,11,15,14, 0, 5,19,14, 5};                               // +-=--+==
static const char vlist4640[] = { 3, 1,11,15,19,12, 5,19,14,12};                               // +-=--+=+
static const char vlist4641[] = { 4,10,11,15, 5, 9, 8, 5,14, 9,14, 5,19};                      // +-=--++-
static const char vlist4642[] = { 3,14, 0, 5,10,11,15,19,14, 5};                               // +-=--++=
static const char vlist4643[] = { 3,10,11,15,19,12, 5,19,14,12};                               // +-=--+++
static const char vlist4644[] = { 1, 3, 5,19};                                                 // +-=-=---
static const char vlist4645[] = { 3, 5, 3, 0, 3, 0,19, 0, 5,19};                               // +-=-=--=
static const char vlist4646[] = { 4, 5, 3, 8, 3, 9,19, 9,12,19,12, 5,19};                      // +-=-=--+
static const char vlist4647[] = { 2, 3, 1,19, 1, 5,19};                                        // +-=-=-=-
static const char vlist4648[] = { 2,19, 0, 5, 3, 0,19};                                        // +-=-=-==
static const char vlist4649[] = { 3, 3, 9,19, 9,12,19,19,12, 5};                               // +-=-=-=+
static const char vlist4650[] = { 2,19, 3, 8, 5,19, 8};                                        // +-=-=-+-
static const char vlist4651[] = { 2,19, 0, 5, 3, 0,19};                                        // +-=-=-+=
static const char vlist4652[] = { 3, 3, 9,19, 9,12,19,19,12, 5};                               // +-=-=-++
static const char vlist4653[] = { 2, 3, 5, 2, 2, 5,19};                                        // +-=-==--
static const char vlist4654[] = { 3, 2, 0,19,19, 0, 5, 3, 0, 5};                               // +-=-==-=
static const char vlist4655[] = { 3,19, 2,12,19,12, 5, 8, 3, 5};                               // +-=-==-+
static const char vlist4656[] = { 2,19, 2, 5, 2, 1, 5};                                        // +-=-===-
static const char vlist4657[] = { 2,19, 0, 5, 2, 0,19};                                        // +-=-====
static const char vlist4658[] = { 2,19, 2,12,19,12, 5};                                        // +-=-===+
static const char vlist4659[] = { 2,19, 2, 5, 2, 8, 5};                                        // +-=-==+-
static const char vlist4660[] = { 2,19, 0, 5, 2, 0,19};                                        // +-=-==+=
static const char vlist4661[] = { 2,19, 2,12,19,12, 5};                                        // +-=-==++
static const char vlist4662[] = { 3, 3, 5, 9, 9, 5,14,14, 5,19};                               // +-=-=+--
static const char vlist4663[] = { 3,14, 0, 5,19,14, 5, 0, 3, 5};                               // +-=-=+-=
static const char vlist4664[] = { 3,19,12, 5,19,14,12, 8, 3, 5};                               // +-=-=+-+
static const char vlist4665[] = { 3, 9, 1, 5,14, 9, 5,19,14, 5};                               // +-=-=+=-
static const char vlist4666[] = { 2,14, 0, 5,19,14, 5};                                        // +-=-=+==
static const char vlist4667[] = { 2,19,14,12,19,12, 5};                                        // +-=-=+=+
static const char vlist4668[] = { 3, 9, 8, 5,14, 9, 5,19,14, 5};                               // +-=-=++-
static const char vlist4669[] = { 2,14, 0, 5,19,14, 5};                                        // +-=-=++=
static const char vlist4670[] = { 2,19,14,12,19,12, 5};                                        // +-=-=+++
static const char vlist4671[] = { 2, 5,19,11,10, 5,11};                                        // +-=-+---
static const char vlist4672[] = { 3, 5,10, 0, 0, 5,19,11, 0,19};                               // +-=-+--=
static const char vlist4673[] = { 4,10, 5, 8, 9,12,19,11, 9,19,12, 5,19};                      // +-=-+--+
static const char vlist4674[] = { 2, 1, 5,11, 5,19,11};                                        // +-=-+-=-
static const char vlist4675[] = { 2,19, 0, 5,11, 0,19};                                        // +-=-+-==
static const char vlist4676[] = { 3,11, 9,19, 9,12,19,19,12, 5};                               // +-=-+-=+
static const char vlist4677[] = { 2, 5,19, 8,19,11, 8};                                        // +-=-+-+-
static const char vlist4678[] = { 2,19, 0, 5,11, 0,19};                                        // +-=-+-+=
static const char vlist4679[] = { 3,11, 9,19, 9,12,19,19,12, 5};                               // +-=-+-++
static const char vlist4680[] = { 2,10, 5, 2, 2, 5,19};                                        // +-=-+=--
static const char vlist4681[] = { 3, 2, 0,19,19, 0, 5, 0,10, 5};                               // +-=-+=-=
static const char vlist4682[] = { 3,19, 2,12,19,12, 5, 8,10, 5};                               // +-=-+=-+
static const char vlist4683[] = { 2,19, 2, 5, 2, 1, 5};                                        // +-=-+==-
static const char vlist4684[] = { 2,19, 0, 5, 2, 0,19};                                        // +-=-+===
static const char vlist4685[] = { 2,19, 2,12,19,12, 5};                                        // +-=-+==+
static const char vlist4686[] = { 2,19, 2, 5, 2, 8, 5};                                        // +-=-+=+-
static const char vlist4687[] = { 2,19, 0, 5, 2, 0,19};                                        // +-=-+=+=
static const char vlist4688[] = { 2,19, 2,12,19,12, 5};                                        // +-=-+=++
static const char vlist4689[] = { 3,10, 5, 9, 9, 5,14,14, 5,19};                               // +-=-++--
static const char vlist4690[] = { 3,14, 0, 5,19,14, 5, 0,10, 5};                               // +-=-++-=
static const char vlist4691[] = { 3,19,12, 5,19,14,12, 8,10, 5};                               // +-=-++-+
static const char vlist4692[] = { 3, 9, 1, 5,14, 9, 5,19,14, 5};                               // +-=-++=-
static const char vlist4693[] = { 2,14, 0, 5,19,14, 5};                                        // +-=-++==
static const char vlist4694[] = { 2,19,14,12,19,12, 5};                                        // +-=-++=+
static const char vlist4695[] = { 3, 9, 8, 5,14, 9, 5,19,14, 5};                               // +-=-+++-
static const char vlist4696[] = { 2,14, 0, 5,19,14, 5};                                        // +-=-+++=
static const char vlist4697[] = { 2,19,14,12,19,12, 5};                                        // +-=-++++
static const char vlist4698[] = { 2, 5, 4,15, 4,19,15};                                        // +-==----
static const char vlist4699[] = { 3, 0, 4,19,15, 0,19, 5, 0,15};                               // +-==---=
static const char vlist4700[] = { 4, 9, 4,19, 5, 8,15, 8, 9,15,15, 9,19};                      // +-==---+
static const char vlist4701[] = { 2,15, 4,19, 1, 4,15};                                        // +-==--=-
static const char vlist4702[] = { 3, 1, 0,15,15, 0,19, 0, 4,19};                               // +-==--==
static const char vlist4703[] = { 3, 1, 9,15, 9, 4,19,15, 9,19};                               // +-==--=+
static const char vlist4704[] = { 3, 8, 4,19,10, 8,15, 8,19,15};                               // +-==--+-
static const char vlist4705[] = { 3,10, 0,15,15, 0,19, 0, 4,19};                               // +-==--+=
static const char vlist4706[] = { 3, 9, 4,19,15, 9,19,10, 9,15};                               // +-==--++
static const char vlist4707[] = { 3, 5, 4, 2, 4, 2,19, 5, 2,15};                               // +-==-=--
static const char vlist4708[] = { 3, 4, 2,19, 0, 2,15, 0,15, 5};                               // +-==-=-=
static const char vlist4709[] = { 3, 2, 4,19, 2,15, 8, 8,15, 5};                               // +-==-=-+
static const char vlist4710[] = { 3, 4, 2, 1, 4, 2,19, 1, 2,15};                               // +-==-==-
static const char vlist4711[] = { 2, 1, 2,15,19, 2, 4};                                        // +-==-===
static const char vlist4712[] = { 2, 1, 2,15,19, 2, 4};                                        // +-==-==+
static const char vlist4713[] = { 3, 4, 2, 8, 2, 4,19,10, 2,15};                               // +-==-=+-
static const char vlist4714[] = { 2,19, 2, 4,10, 2,15};                                        // +-==-=+=
static const char vlist4715[] = { 2,19, 2, 4,10, 2,15};                                        // +-==-=++
static const char vlist4716[] = { 4, 5, 4, 9,14, 4,19, 5, 9,11, 5,11,15};                      // +-==-+--
static const char vlist4717[] = { 3, 0,11, 5,11,15, 5,14, 4,19};                               // +-==-+-=
static const char vlist4718[] = { 3, 8,15, 5,11,15, 8,14, 4,19};                               // +-==-+-+
static const char vlist4719[] = { 3, 4, 9, 1, 1,11,15,14, 4,19};                               // +-==-+=-
static const char vlist4720[] = { 2, 1,11,15,19,14, 4};                                        // +-==-+==
static const char vlist4721[] = { 2, 1,11,15,19,14, 4};                                        // +-==-+=+
static const char vlist4722[] = { 3, 4, 9, 8,10,11,15,14, 4,19};                               // +-==-++-
static const char vlist4723[] = { 2,10,11,15,19,14, 4};                                        // +-==-++=
static const char vlist4724[] = { 2,10,11,15,19,14, 4};                                        // +-==-+++
static const char vlist4725[] = { 2, 5, 4, 3, 3, 4,19};                                        // +-===---
static const char vlist4726[] = { 3,19, 3, 0, 0, 3, 5, 4,19, 0};                               // +-===--=
static const char vlist4727[] = { 3, 3, 9,19, 8, 3, 5, 9, 4,19};                               // +-===--+
static const char vlist4728[] = { 2, 4, 3, 1, 3, 4,19};                                        // +-===-=-
static const char vlist4729[] = { 2,19, 0, 4, 3, 0,19};                                        // +-===-==
static const char vlist4730[] = { 2,19, 3, 9,19, 9, 4};                                        // +-===-=+
static const char vlist4731[] = { 2, 4, 3, 8, 3, 4,19};                                        // +-===-+-
static const char vlist4732[] = { 2,19, 0, 4, 3, 0,19};                                        // +-===-+=
static const char vlist4733[] = { 2,19, 3, 9,19, 9, 4};                                        // +-===-++
static const char vlist4734[] = { 3, 3, 4, 2, 4, 3, 5, 4, 2,19};                               // +-====--
static const char vlist4735[] = { 2,19, 2, 4, 0, 3, 5};                                        // +-====-=
static const char vlist4736[] = { 2,19, 2, 4, 8, 3, 5};                                        // +-====-+
static const char vlist4737[] = { 2,19, 2, 4, 2, 1, 4};                                        // +-=====-
static const char vlist4738[] = { 1,19, 2, 4};                                                 // +-======
static const char vlist4739[] = { 1,19, 2, 4};                                                 // +-=====+
static const char vlist4740[] = { 2,19, 2, 4, 2, 8, 4};                                        // +-====+-
static const char vlist4741[] = { 1,19, 2, 4};                                                 // +-====+=
static const char vlist4742[] = { 1,19, 2, 4};                                                 // +-====++
static const char vlist4743[] = { 3, 9, 3, 5, 4, 9, 5,14, 4,19};                               // +-===+--
static const char vlist4744[] = { 2, 0, 3, 5,19,14, 4};                                        // +-===+-=
static const char vlist4745[] = { 2, 8, 3, 5,19,14, 4};                                        // +-===+-+
static const char vlist4746[] = { 2, 9, 1, 4,19,14, 4};                                        // +-===+=-
static const char vlist4747[] = { 1,19,14, 4};                                                 // +-===+==
static const char vlist4748[] = { 1,19,14, 4};                                                 // +-===+=+
static const char vlist4749[] = { 2, 9, 8, 4,19,14, 4};                                        // +-===++-
static const char vlist4750[] = { 1,19,14, 4};                                                 // +-===++=
static const char vlist4751[] = { 1,19,14, 4};                                                 // +-===+++
static const char vlist4752[] = { 3, 5, 4,10,11, 4,19,10, 4,11};                               // +-==+---
static const char vlist4753[] = { 3,11, 4,19, 4,11, 0, 0,10, 5};                               // +-==+--=
static const char vlist4754[] = { 3,11, 9,19, 9, 4,19, 8,10, 5};                               // +-==+--+
static const char vlist4755[] = { 2,11, 4,19, 4,11, 1};                                        // +-==+-=-
static const char vlist4756[] = { 2,11, 0, 4,19,11, 4};                                        // +-==+-==
static const char vlist4757[] = { 2,19,11, 9,19, 9, 4};                                        // +-==+-=+
static const char vlist4758[] = { 2,11, 4,19, 4,11, 8};                                        // +-==+-+-
static const char vlist4759[] = { 2,11, 0, 4,19,11, 4};                                        // +-==+-+=
static const char vlist4760[] = { 2,19,11, 9,19, 9, 4};                                        // +-==+-++
static const char vlist4761[] = { 3, 4,10, 5,10, 4, 2, 2, 4,19};                               // +-==+=--
static const char vlist4762[] = { 2,19, 2, 4, 0,10, 5};                                        // +-==+=-=
static const char vlist4763[] = { 2,19, 2, 4, 8,10, 5};                                        // +-==+=-+
static const char vlist4764[] = { 2,19, 2, 4, 2, 1, 4};                                        // +-==+==-
static const char vlist4765[] = { 1,19, 2, 4};                                                 // +-==+===
static const char vlist4766[] = { 1,19, 2, 4};                                                 // +-==+==+
static const char vlist4767[] = { 2,19, 2, 4, 2, 8, 4};                                        // +-==+=+-
static const char vlist4768[] = { 1,19, 2, 4};                                                 // +-==+=+=
static const char vlist4769[] = { 1,19, 2, 4};                                                 // +-==+=++
static const char vlist4770[] = { 3, 9,10, 5, 4, 9, 5,14, 4,19};                               // +-==++--
static const char vlist4771[] = { 2, 0,10, 5,19,14, 4};                                        // +-==++-=
static const char vlist4772[] = { 2, 8,10, 5,19,14, 4};                                        // +-==++-+
static const char vlist4773[] = { 2, 9, 1, 4,19,14, 4};                                        // +-==++=-
static const char vlist4774[] = { 1,19,14, 4};                                                 // +-==++==
static const char vlist4775[] = { 1,19,14, 4};                                                 // +-==++=+
static const char vlist4776[] = { 2, 9, 8, 4,19,14, 4};                                        // +-==+++-
static const char vlist4777[] = { 1,19,14, 4};                                                 // +-==+++=
static const char vlist4778[] = { 1,19,14, 4};                                                 // +-==++++
static const char vlist4779[] = { 3,15, 5,12,19,15,17,17,15,12};                               // +-=+----
static const char vlist4780[] = { 3,19, 0,17,15, 0,19, 5, 0,15};                               // +-=+---=
static const char vlist4781[] = { 4, 5, 8,15, 8, 9,15,15, 9,19, 9,17,19};                      // +-=+---+
static const char vlist4782[] = { 3, 1,12,15,15,12,17,15,17,19};                               // +-=+--=-
static const char vlist4783[] = { 3, 1, 0,15,15, 0,19,19, 0,17};                               // +-=+--==
static const char vlist4784[] = { 3, 1, 9,15, 9,17,19,15, 9,19};                               // +-=+--=+
static const char vlist4785[] = { 4,10, 8,12,10,12,15,15,12,17,15,17,19};                      // +-=+--+-
static const char vlist4786[] = { 3,10, 0,15,15, 0,19,19, 0,17};                               // +-=+--+=
static const char vlist4787[] = { 3, 9,17,19,15, 9,19,10, 9,15};                               // +-=+--++
static const char vlist4788[] = { 3, 5,12, 2, 5, 2,15, 2,17,19};                               // +-=+-=--
static const char vlist4789[] = { 3, 2,17,19, 0, 2,15, 0,15, 5};                               // +-=+-=-=
static const char vlist4790[] = { 3, 2,17,19, 2,15, 8, 8,15, 5};                               // +-=+-=-+
static const char vlist4791[] = { 3, 2,17,19, 1, 2,15,12, 2, 1};                               // +-=+-==-
static const char vlist4792[] = { 2, 1, 2,15,19, 2,17};                                        // +-=+-===
static const char vlist4793[] = { 2, 1, 2,15,19, 2,17};                                        // +-=+-==+
static const char vlist4794[] = { 3, 2,17,19,10, 2,15,12, 2, 8};                               // +-=+-=+-
static const char vlist4795[] = { 2,19, 2,17,10, 2,15};                                        // +-=+-=+=
static const char vlist4796[] = { 2,19, 2,17,10, 2,15};                                        // +-=+-=++
static const char vlist4797[] = { 4, 5,12, 9, 5, 9,11,14,17,19, 5,11,15};                      // +-=+-+--
static const char vlist4798[] = { 3, 0,11, 5,11,15, 5,14,17,19};                               // +-=+-+-=
static const char vlist4799[] = { 3, 8,15, 5,11,15, 8,14,17,19};                               // +-=+-+-+
static const char vlist4800[] = { 3,12, 9, 1, 1,11,15,14,17,19};                               // +-=+-+=-
static const char vlist4801[] = { 2, 1,11,15,19,14,17};                                        // +-=+-+==
static const char vlist4802[] = { 2, 1,11,15,19,14,17};                                        // +-=+-+=+
static const char vlist4803[] = { 3,12, 9, 8,10,11,15,14,17,19};                               // +-=+-++-
static const char vlist4804[] = { 2,10,11,15,19,14,17};                                        // +-=+-++=
static const char vlist4805[] = { 2,10,11,15,19,14,17};                                        // +-=+-+++
static const char vlist4806[] = { 3,12,17, 3, 3,17,19, 5,12, 3};                               // +-=+=---
static const char vlist4807[] = { 3, 3,17,19,17, 3, 0, 0, 3, 5};                               // +-=+=--=
static const char vlist4808[] = { 3, 3, 9,19, 8, 3, 5, 9,17,19};                               // +-=+=--+
static const char vlist4809[] = { 3, 3,17,19,17, 3,12,12, 3, 1};                               // +-=+=-=-
static const char vlist4810[] = { 2, 3, 0,17,19, 3,17};                                        // +-=+=-==
static const char vlist4811[] = { 2,19, 3, 9,19, 9,17};                                        // +-=+=-=+
static const char vlist4812[] = { 3, 3,17,19,17, 3,12,12, 3, 8};                               // +-=+=-+-
static const char vlist4813[] = { 2, 3, 0,17,19, 3,17};                                        // +-=+=-+=
static const char vlist4814[] = { 2,19, 3, 9,19, 9,17};                                        // +-=+=-++
static const char vlist4815[] = { 3,12, 3, 5, 3,12, 2, 2,17,19};                               // +-=+==--
static const char vlist4816[] = { 2,19, 2,17, 0, 3, 5};                                        // +-=+==-=
static const char vlist4817[] = { 2,19, 2,17, 8, 3, 5};                                        // +-=+==-+
static const char vlist4818[] = { 2,19, 2,17, 2, 1,12};                                        // +-=+===-
static const char vlist4819[] = { 1,19, 2,17};                                                 // +-=+====
static const char vlist4820[] = { 1,19, 2,17};                                                 // +-=+===+
static const char vlist4821[] = { 2,19, 2,17, 2, 8,12};                                        // +-=+==+-
static const char vlist4822[] = { 1,19, 2,17};                                                 // +-=+==+=
static const char vlist4823[] = { 1,19, 2,17};                                                 // +-=+==++
static const char vlist4824[] = { 3, 9, 3, 5,14,17,19,12, 9, 5};                               // +-=+=+--
static const char vlist4825[] = { 2, 0, 3, 5,19,14,17};                                        // +-=+=+-=
static const char vlist4826[] = { 2, 8, 3, 5,19,14,17};                                        // +-=+=+-+
static const char vlist4827[] = { 2, 9, 1,12,19,14,17};                                        // +-=+=+=-
static const char vlist4828[] = { 1,19,14,17};                                                 // +-=+=+==
static const char vlist4829[] = { 1,19,14,17};                                                 // +-=+=+=+
static const char vlist4830[] = { 2, 9, 8,12,19,14,17};                                        // +-=+=++-
static const char vlist4831[] = { 1,19,14,17};                                                 // +-=+=++=
static const char vlist4832[] = { 1,19,14,17};                                                 // +-=+=+++
static const char vlist4833[] = { 4,12,17,11,11,17,19,10,12,11, 5,12,10};                      // +-=++---
static const char vlist4834[] = { 3,17,11, 0,11,17,19, 0,10, 5};                               // +-=++--=
static const char vlist4835[] = { 3,11,17,19,11, 9,17, 8,10, 5};                               // +-=++--+
static const char vlist4836[] = { 3,17,11,12,11,17,19,12,11, 1};                               // +-=++-=-
static const char vlist4837[] = { 2,11, 0,17,19,11,17};                                        // +-=++-==
static const char vlist4838[] = { 2,19,11,17,17,11, 9};                                        // +-=++-=+
static const char vlist4839[] = { 3,17,11,12,11,17,19,12,11, 8};                               // +-=++-+-
static const char vlist4840[] = { 2,11, 0,17,19,11,17};                                        // +-=++-+=
static const char vlist4841[] = { 2,19,11,17,17,11, 9};                                        // +-=++-++
static const char vlist4842[] = { 3,10,12, 2,12,10, 5, 2,17,19};                               // +-=++=--
static const char vlist4843[] = { 2,19, 2,17, 0,10, 5};                                        // +-=++=-=
static const char vlist4844[] = { 2,19, 2,17, 8,10, 5};                                        // +-=++=-+
static const char vlist4845[] = { 2,19, 2,17, 2, 1,12};                                        // +-=++==-
static const char vlist4846[] = { 1,19, 2,17};                                                 // +-=++===
static const char vlist4847[] = { 1,19, 2,17};                                                 // +-=++==+
static const char vlist4848[] = { 2,19, 2,17, 2, 8,12};                                        // +-=++=+-
static const char vlist4849[] = { 1,19, 2,17};                                                 // +-=++=+=
static const char vlist4850[] = { 1,19, 2,17};                                                 // +-=++=++
static const char vlist4851[] = { 3,12,10, 5, 9,10,12,14,17,19};                               // +-=+++--
static const char vlist4852[] = { 2, 0,10, 5,19,14,17};                                        // +-=+++-=
static const char vlist4853[] = { 2, 8,10, 5,19,14,17};                                        // +-=+++-+
static const char vlist4854[] = { 2, 9, 1,12,19,14,17};                                        // +-=+++=-
static const char vlist4855[] = { 1,19,14,17};                                                 // +-=+++==
static const char vlist4856[] = { 1,19,14,17};                                                 // +-=+++=+
static const char vlist4857[] = { 2, 9, 8,12,19,14,17};                                        // +-=++++-
static const char vlist4858[] = { 1,19,14,17};                                                 // +-=++++=
static const char vlist4859[] = { 1,19,14,17};                                                 // +-=+++++
static const char vlist4860[] = { 2,15,13,16,19,15,16};                                        // +-+-----
static const char vlist4861[] = { 3,16,19, 0, 0,19,15, 0,15,13};                               // +-+----=
static const char vlist4862[] = { 5,16,19,12, 9,19,15,12,19, 9, 9,15, 8,15,13, 8};             // +-+----+
static const char vlist4863[] = { 2,19, 1,16,15, 1,19};                                        // +-+---=-
static const char vlist4864[] = { 3, 1, 0,15,15, 0,19, 0,16,19};                               // +-+---==
static const char vlist4865[] = { 4, 1, 9,15, 9,12,19,15, 9,19,12,16,19};                      // +-+---=+
static const char vlist4866[] = { 3,15,10, 8,19,15, 8,16,19, 8};                               // +-+---+-
static const char vlist4867[] = { 3,10, 0,15,15, 0,19, 0,16,19};                               // +-+---+=
static const char vlist4868[] = { 4, 9,12,19,15, 9,19,10, 9,15,12,16,19};                      // +-+---++
static const char vlist4869[] = { 3,16, 2,13,16,19, 2, 2,15,13};                               // +-+--=--
static const char vlist4870[] = { 4, 2,16,19,16, 2, 0, 0, 2,13, 2,15,13};                      // +-+--=-=
static const char vlist4871[] = { 4, 2,12,19, 2,15, 8,12,16,19,15,13, 8};                      // +-+--=-+
static const char vlist4872[] = { 3, 2,16,19,16, 2, 1, 1, 2,15};                               // +-+--==-
static const char vlist4873[] = { 3, 1, 2,15, 2, 0,16,19, 2,16};                               // +-+--===
static const char vlist4874[] = { 3, 1, 2,15,19, 2,12,19,12,16};                               // +-+--==+
static const char vlist4875[] = { 3, 2,16,19,16, 2, 8,10, 2,15};                               // +-+--=+-
static const char vlist4876[] = { 3, 2, 0,16,19, 2,16,10, 2,15};                               // +-+--=+=
static const char vlist4877[] = { 3,19, 2,12,10, 2,15,19,12,16};                               // +-+--=++
static const char vlist4878[] = { 5,16, 9,13,16,14, 9,16,19,14,13, 9,11,15,13,11};             // +-+--+--
static const char vlist4879[] = { 4,16,14, 0,14,16,19,13, 0,11,15,13,11};                      // +-+--+-=
static const char vlist4880[] = { 4,14,16,19,14,12,16,11,15,13,11,13, 8};                      // +-+--+-+
static const char vlist4881[] = { 4, 1,11,15,14,16,19,16,14, 9,16, 9, 1};                      // +-+--+=-
static const char vlist4882[] = { 3, 1,11,15,14, 0,16,19,14,16};                               // +-+--+==
static const char vlist4883[] = { 3, 1,11,15,16,14,12,19,14,16};                               // +-+--+=+
static const char vlist4884[] = { 4,10,11,15,14,16,19,16,14, 9,16, 9, 8};                      // +-+--++-
static const char vlist4885[] = { 3,14, 0,16,10,11,15,19,14,16};                               // +-+--++=
static const char vlist4886[] = { 3,10,11,15,16,14,12,19,14,16};                               // +-+--+++
static const char vlist4887[] = { 2, 3,13,16,19, 3,16};                                        // +-+-=---
static const char vlist4888[] = { 3, 3, 0,19,13, 3, 0, 0,16,19};                               // +-+-=--=
static const char vlist4889[] = { 4, 3, 9,19, 3,13, 8, 9,12,19,12,16,19};                      // +-+-=--+
static const char vlist4890[] = { 2,19, 3,16, 3, 1,16};                                        // +-+-=-=-
static const char vlist4891[] = { 2,19, 0,16, 3, 0,19};                                        // +-+-=-==
static const char vlist4892[] = { 3, 3, 9,19, 9,12,19,19,12,16};                               // +-+-=-=+
static const char vlist4893[] = { 2,19, 3, 8,16,19, 8};                                        // +-+-=-+-
static const char vlist4894[] = { 2,19, 0,16, 3, 0,19};                                        // +-+-=-+=
static const char vlist4895[] = { 3, 3, 9,19, 9,12,19,19,12,16};                               // +-+-=-++
static const char vlist4896[] = { 3, 3,13, 2,13,16, 2, 2,16,19};                               // +-+-==--
static const char vlist4897[] = { 3,19, 2,16, 2, 0,16, 0, 3,13};                               // +-+-==-=
static const char vlist4898[] = { 3,19, 2,12,19,12,16, 8, 3,13};                               // +-+-==-+
static const char vlist4899[] = { 2,19, 2,16, 2, 1,16};                                        // +-+-===-
static const char vlist4900[] = { 2, 2, 0,16,19, 2,16};                                        // +-+-====
static const char vlist4901[] = { 2,19, 2,12,19,12,16};                                        // +-+-===+
static const char vlist4902[] = { 2,19, 2,16, 2, 8,16};                                        // +-+-==+-
static const char vlist4903[] = { 2, 2, 0,16,19, 2,16};                                        // +-+-==+=
static const char vlist4904[] = { 2,19, 2,12,19,12,16};                                        // +-+-==++
static const char vlist4905[] = { 4, 3,13, 9,13,16, 9,14,16,19, 9,16,14};                      // +-+-=+--
static const char vlist4906[] = { 3,14, 0,16,19,14,16, 0, 3,13};                               // +-+-=+-=
static const char vlist4907[] = { 3,16,14,12,19,14,16, 8, 3,13};                               // +-+-=+-+
static const char vlist4908[] = { 3, 9, 1,16,14, 9,16,19,14,16};                               // +-+-=+=-
static const char vlist4909[] = { 2,14, 0,16,19,14,16};                                        // +-+-=+==
static const char vlist4910[] = { 2,19,14,16,16,14,12};                                        // +-+-=+=+
static const char vlist4911[] = { 3, 9, 8,16,14, 9,16,19,14,16};                               // +-+-=++-
static const char vlist4912[] = { 2,14, 0,16,19,14,16};                                        // +-+-=++=
static const char vlist4913[] = { 2,19,14,16,16,14,12};                                        // +-+-=+++
static const char vlist4914[] = { 3,10,13,11,13,16,11,16,19,11};                               // +-+-+---
static const char vlist4915[] = { 3,10,13, 0, 0,16,19,11, 0,19};                               // +-+-+--=
static const char vlist4916[] = { 4,10,13, 8, 9,12,19,11, 9,19,12,16,19};                      // +-+-+--+
static const char vlist4917[] = { 2, 1,16,11,16,19,11};                                        // +-+-+-=-
static const char vlist4918[] = { 2,19, 0,16,11, 0,19};                                        // +-+-+-==
static const char vlist4919[] = { 3,11, 9,19, 9,12,19,19,12,16};                               // +-+-+-=+
static const char vlist4920[] = { 2,19,11,16,16,11, 8};                                        // +-+-+-+-
static const char vlist4921[] = { 2,19, 0,16,11, 0,19};                                        // +-+-+-+=
static const char vlist4922[] = { 3,11, 9,19, 9,12,19,19,12,16};                               // +-+-+-++
static const char vlist4923[] = { 3,10,13, 2,13,16, 2, 2,16,19};                               // +-+-+=--
static const char vlist4924[] = { 3,19, 2,16, 2, 0,16, 0,10,13};                               // +-+-+=-=
static const char vlist4925[] = { 3,19, 2,12,19,12,16, 8,10,13};                               // +-+-+=-+
static const char vlist4926[] = { 2,19, 2,16, 2, 1,16};                                        // +-+-+==-
static const char vlist4927[] = { 2, 2, 0,16,19, 2,16};                                        // +-+-+===
static const char vlist4928[] = { 2,19, 2,12,19,12,16};                                        // +-+-+==+
static const char vlist4929[] = { 2,19, 2,16, 2, 8,16};                                        // +-+-+=+-
static const char vlist4930[] = { 2, 2, 0,16,19, 2,16};                                        // +-+-+=+=
static const char vlist4931[] = { 2,19, 2,12,19,12,16};                                        // +-+-+=++
static const char vlist4932[] = { 4,10,13, 9,13,16, 9, 9,16,14,14,16,19};                      // +-+-++--
static const char vlist4933[] = { 3,14, 0,16,19,14,16, 0,10,13};                               // +-+-++-=
static const char vlist4934[] = { 3,16,14,12,19,14,16, 8,10,13};                               // +-+-++-+
static const char vlist4935[] = { 3, 9, 1,16,14, 9,16,19,14,16};                               // +-+-++=-
static const char vlist4936[] = { 2,14, 0,16,19,14,16};                                        // +-+-++==
static const char vlist4937[] = { 2,19,14,16,16,14,12};                                        // +-+-++=+
static const char vlist4938[] = { 3, 9, 8,16,14, 9,16,19,14,16};                               // +-+-+++-
static const char vlist4939[] = { 2,14, 0,16,19,14,16};                                        // +-+-+++=
static const char vlist4940[] = { 2,19,14,16,16,14,12};                                        // +-+-++++
static const char vlist4941[] = { 2, 4,15,13, 4,19,15};                                        // +-+=----
static const char vlist4942[] = { 3, 0, 4,19,15, 0,19,13, 0,15};                               // +-+=---=
static const char vlist4943[] = { 4, 9, 4,19,13, 8,15, 8, 9,15,15, 9,19};                      // +-+=---+
static const char vlist4944[] = { 2,15, 4,19, 1, 4,15};                                        // +-+=--=-
static const char vlist4945[] = { 3, 1, 0,15,15, 0,19, 0, 4,19};                               // +-+=--==
static const char vlist4946[] = { 3, 1, 9,15, 9, 4,19,15, 9,19};                               // +-+=--=+
static const char vlist4947[] = { 3, 8, 4,19,10, 8,15, 8,19,15};                               // +-+=--+-
static const char vlist4948[] = { 3,10, 0,15,15, 0,19, 0, 4,19};                               // +-+=--+=
static const char vlist4949[] = { 3, 9, 4,19,15, 9,19,10, 9,15};                               // +-+=--++
static const char vlist4950[] = { 3, 2, 4,19,13, 4, 2,13, 2,15};                               // +-+=-=--
static const char vlist4951[] = { 3, 2, 4,19, 2,15,13, 0, 2,13};                               // +-+=-=-=
static const char vlist4952[] = { 3, 2, 4,19, 2,15, 8, 8,15,13};                               // +-+=-=-+
static const char vlist4953[] = { 3, 4, 2, 1, 2, 4,19, 1, 2,15};                               // +-+=-==-
static const char vlist4954[] = { 2, 1, 2,15,19, 2, 4};                                        // +-+=-===
static const char vlist4955[] = { 2, 1, 2,15,19, 2, 4};                                        // +-+=-==+
static const char vlist4956[] = { 3, 4, 2, 8, 2, 4,19,10, 2,15};                               // +-+=-=+-
static const char vlist4957[] = { 2,19, 2, 4,10, 2,15};                                        // +-+=-=+=
static const char vlist4958[] = { 2,19, 2, 4,10, 2,15};                                        // +-+=-=++
static const char vlist4959[] = { 4,14, 4,19,13, 4, 9,13, 9,11,13,11,15};                      // +-+=-+--
static const char vlist4960[] = { 3, 0,11,13,11,15,13,14, 4,19};                               // +-+=-+-=
static const char vlist4961[] = { 3,11,13, 8,11,15,13,14, 4,19};                               // +-+=-+-+
static const char vlist4962[] = { 3, 4, 9, 1, 1,11,15,14, 4,19};                               // +-+=-+=-
static const char vlist4963[] = { 2, 1,11,15,19,14, 4};                                        // +-+=-+==
static const char vlist4964[] = { 2, 1,11,15,19,14, 4};                                        // +-+=-+=+
static const char vlist4965[] = { 3, 4, 9, 8,10,11,15,14, 4,19};                               // +-+=-++-
static const char vlist4966[] = { 2,10,11,15,19,14, 4};                                        // +-+=-++=
static const char vlist4967[] = { 2,10,11,15,19,14, 4};                                        // +-+=-+++
static const char vlist4968[] = { 2, 3, 4,19,13, 4, 3};                                        // +-+==---
static const char vlist4969[] = { 3,19, 3, 0, 0, 3,13, 4,19, 0};                               // +-+==--=
static const char vlist4970[] = { 3, 3, 9,19, 8, 3,13, 9, 4,19};                               // +-+==--+
static const char vlist4971[] = { 2, 4, 3, 1, 3, 4,19};                                        // +-+==-=-
static const char vlist4972[] = { 2,19, 0, 4, 3, 0,19};                                        // +-+==-==
static const char vlist4973[] = { 2,19, 3, 9,19, 9, 4};                                        // +-+==-=+
static const char vlist4974[] = { 2, 4, 3, 8, 3, 4,19};                                        // +-+==-+-
static const char vlist4975[] = { 2,19, 0, 4, 3, 0,19};                                        // +-+==-+=
static const char vlist4976[] = { 2,19, 3, 9,19, 9, 4};                                        // +-+==-++
static const char vlist4977[] = { 3, 3,13, 2,13, 4, 2, 2, 4,19};                               // +-+===--
static const char vlist4978[] = { 2,19, 2, 4, 0, 3,13};                                        // +-+===-=
static const char vlist4979[] = { 2,19, 2, 4, 8, 3,13};                                        // +-+===-+
static const char vlist4980[] = { 2,19, 2, 4, 2, 1, 4};                                        // +-+====-
static const char vlist4981[] = { 1,19, 2, 4};                                                 // +-+=====
static const char vlist4982[] = { 1,19, 2, 4};                                                 // +-+====+
static const char vlist4983[] = { 2,19, 2, 4, 2, 8, 4};                                        // +-+===+-
static const char vlist4984[] = { 1,19, 2, 4};                                                 // +-+===+=
static const char vlist4985[] = { 1,19, 2, 4};                                                 // +-+===++
static const char vlist4986[] = { 3, 9, 3,13, 4, 9,13,14, 4,19};                               // +-+==+--
static const char vlist4987[] = { 2, 0, 3,13,19,14, 4};                                        // +-+==+-=
static const char vlist4988[] = { 2, 8, 3,13,19,14, 4};                                        // +-+==+-+
static const char vlist4989[] = { 2, 9, 1, 4,19,14, 4};                                        // +-+==+=-
static const char vlist4990[] = { 1,19,14, 4};                                                 // +-+==+==
static const char vlist4991[] = { 1,19,14, 4};                                                 // +-+==+=+
static const char vlist4992[] = { 2, 9, 8, 4,19,14, 4};                                        // +-+==++-
static const char vlist4993[] = { 1,19,14, 4};                                                 // +-+==++=
static const char vlist4994[] = { 1,19,14, 4};                                                 // +-+==+++
static const char vlist4995[] = { 3,11, 4,19,10, 4,11,13, 4,10};                               // +-+=+---
static const char vlist4996[] = { 3,11, 4,19, 4,11, 0, 0,10,13};                               // +-+=+--=
static const char vlist4997[] = { 3,11, 9,19, 9, 4,19, 8,10,13};                               // +-+=+--+
static const char vlist4998[] = { 2,11, 4,19, 4,11, 1};                                        // +-+=+-=-
static const char vlist4999[] = { 2,11, 0, 4,19,11, 4};                                        // +-+=+-==
static const char vlist5000[] = { 2,19,11, 9,19, 9, 4};                                        // +-+=+-=+
static const char vlist5001[] = { 2,11, 4,19, 4,11, 8};                                        // +-+=+-+-
static const char vlist5002[] = { 2,11, 0, 4,19,11, 4};                                        // +-+=+-+=
static const char vlist5003[] = { 2,19,11, 9,19, 9, 4};                                        // +-+=+-++
static const char vlist5004[] = { 3, 4,10,13,10, 4, 2, 2, 4,19};                               // +-+=+=--
static const char vlist5005[] = { 2,19, 2, 4, 0,10,13};                                        // +-+=+=-=
static const char vlist5006[] = { 2,19, 2, 4, 8,10,13};                                        // +-+=+=-+
static const char vlist5007[] = { 2,19, 2, 4, 2, 1, 4};                                        // +-+=+==-
static const char vlist5008[] = { 1,19, 2, 4};                                                 // +-+=+===
static const char vlist5009[] = { 1,19, 2, 4};                                                 // +-+=+==+
static const char vlist5010[] = { 2,19, 2, 4, 2, 8, 4};                                        // +-+=+=+-
static const char vlist5011[] = { 1,19, 2, 4};                                                 // +-+=+=+=
static const char vlist5012[] = { 1,19, 2, 4};                                                 // +-+=+=++
static const char vlist5013[] = { 3, 9,10,13, 4, 9,13,14, 4,19};                               // +-+=++--
static const char vlist5014[] = { 2, 0,10,13,19,14, 4};                                        // +-+=++-=
static const char vlist5015[] = { 2, 8,10,13,19,14, 4};                                        // +-+=++-+
static const char vlist5016[] = { 2, 9, 1, 4,19,14, 4};                                        // +-+=++=-
static const char vlist5017[] = { 1,19,14, 4};                                                 // +-+=++==
static const char vlist5018[] = { 1,19,14, 4};                                                 // +-+=++=+
static const char vlist5019[] = { 2, 9, 8, 4,19,14, 4};                                        // +-+=+++-
static const char vlist5020[] = { 1,19,14, 4};                                                 // +-+=+++=
static const char vlist5021[] = { 1,19,14, 4};                                                 // +-+=++++
static const char vlist5022[] = { 3,15,13,12,17,15,12,19,15,17};                               // +-++----
static const char vlist5023[] = { 3,19, 0,17,15, 0,19,13, 0,15};                               // +-++---=
static const char vlist5024[] = { 4,13, 8,15, 8, 9,15,15, 9,19, 9,17,19};                      // +-++---+
static const char vlist5025[] = { 3, 1,12,15,15,12,17,15,17,19};                               // +-++--=-
static const char vlist5026[] = { 3, 1, 0,15,15, 0,19,19, 0,17};                               // +-++--==
static const char vlist5027[] = { 3, 1, 9,15, 9,17,19,15, 9,19};                               // +-++--=+
static const char vlist5028[] = { 4,10, 8,12,10,12,15,15,12,17,15,17,19};                      // +-++--+-
static const char vlist5029[] = { 3,10, 0,15,15, 0,19,19, 0,17};                               // +-++--+=
static const char vlist5030[] = { 3, 9,17,19,15, 9,19,10, 9,15};                               // +-++--++
static const char vlist5031[] = { 3,13,12, 2,13, 2,15, 2,17,19};                               // +-++-=--
static const char vlist5032[] = { 3, 2,17,19, 2,15,13, 0, 2,13};                               // +-++-=-=
static const char vlist5033[] = { 3, 2,17,19, 2,15, 8, 8,15,13};                               // +-++-=-+
static const char vlist5034[] = { 3, 2,17,19, 1, 2,15,12, 2, 1};                               // +-++-==-
static const char vlist5035[] = { 2, 1, 2,15,19, 2,17};                                        // +-++-===
static const char vlist5036[] = { 2, 1, 2,15,19, 2,17};                                        // +-++-==+
static const char vlist5037[] = { 3, 2,17,19,10, 2,15,12, 2, 8};                               // +-++-=+-
static const char vlist5038[] = { 2,19, 2,17,10, 2,15};                                        // +-++-=+=
static const char vlist5039[] = { 2,19, 2,17,10, 2,15};                                        // +-++-=++
static const char vlist5040[] = { 4,13,12, 9,13,11,15,13, 9,11,14,17,19};                      // +-++-+--
static const char vlist5041[] = { 3, 0,11,13,11,15,13,14,17,19};                               // +-++-+-=
static const char vlist5042[] = { 3,11,13, 8,11,15,13,14,17,19};                               // +-++-+-+
static const char vlist5043[] = { 3,12, 9, 1, 1,11,15,14,17,19};                               // +-++-+=-
static const char vlist5044[] = { 2, 1,11,15,19,14,17};                                        // +-++-+==
static const char vlist5045[] = { 2, 1,11,15,19,14,17};                                        // +-++-+=+
static const char vlist5046[] = { 3,12, 9, 8,10,11,15,14,17,19};                               // +-++-++-
static const char vlist5047[] = { 2,10,11,15,19,14,17};                                        // +-++-++=
static const char vlist5048[] = { 2,10,11,15,19,14,17};                                        // +-++-+++
static const char vlist5049[] = { 3,12,17, 3, 3,17,19,13,12, 3};                               // +-++=---
static const char vlist5050[] = { 3, 3,17,19,17, 3, 0, 0, 3,13};                               // +-++=--=
static const char vlist5051[] = { 3, 3, 9,19, 8, 3,13, 9,17,19};                               // +-++=--+
static const char vlist5052[] = { 3, 3,17,19,17, 3,12,12, 3, 1};                               // +-++=-=-
static const char vlist5053[] = { 2, 3, 0,17,19, 3,17};                                        // +-++=-==
static const char vlist5054[] = { 2,19, 3, 9,19, 9,17};                                        // +-++=-=+
static const char vlist5055[] = { 3, 3,17,19,17, 3,12,12, 3, 8};                               // +-++=-+-
static const char vlist5056[] = { 2, 3, 0,17,19, 3,17};                                        // +-++=-+=
static const char vlist5057[] = { 2,19, 3, 9,19, 9,17};                                        // +-++=-++
static const char vlist5058[] = { 3,12, 3,13, 3,12, 2, 2,17,19};                               // +-++==--
static const char vlist5059[] = { 2,19, 2,17, 0, 3,13};                                        // +-++==-=
static const char vlist5060[] = { 2,19, 2,17, 8, 3,13};                                        // +-++==-+
static const char vlist5061[] = { 2,19, 2,17, 2, 1,12};                                        // +-++===-
static const char vlist5062[] = { 1,19, 2,17};                                                 // +-++====
static const char vlist5063[] = { 1,19, 2,17};                                                 // +-++===+
static const char vlist5064[] = { 2,19, 2,17, 2, 8,12};                                        // +-++==+-
static const char vlist5065[] = { 1,19, 2,17};                                                 // +-++==+=
static const char vlist5066[] = { 1,19, 2,17};                                                 // +-++==++
static const char vlist5067[] = { 3, 9, 3,13,12, 9,13,14,17,19};                               // +-++=+--
static const char vlist5068[] = { 2, 0, 3,13,19,14,17};                                        // +-++=+-=
static const char vlist5069[] = { 2, 8, 3,13,19,14,17};                                        // +-++=+-+
static const char vlist5070[] = { 2, 9, 1,12,19,14,17};                                        // +-++=+=-
static const char vlist5071[] = { 1,19,14,17};                                                 // +-++=+==
static const char vlist5072[] = { 1,19,14,17};                                                 // +-++=+=+
static const char vlist5073[] = { 2, 9, 8,12,19,14,17};                                        // +-++=++-
static const char vlist5074[] = { 1,19,14,17};                                                 // +-++=++=
static const char vlist5075[] = { 1,19,14,17};                                                 // +-++=+++
static const char vlist5076[] = { 4,12,17,11,11,17,19,10,12,11,13,12,10};                      // +-+++---
static const char vlist5077[] = { 3,17,11, 0,11,17,19, 0,10,13};                               // +-+++--=
static const char vlist5078[] = { 3,11,17,19,11, 9,17, 8,10,13};                               // +-+++--+
static const char vlist5079[] = { 3,17,11,12,11,17,19,12,11, 1};                               // +-+++-=-
static const char vlist5080[] = { 2,11, 0,17,19,11,17};                                        // +-+++-==
static const char vlist5081[] = { 2,19,11,17,17,11, 9};                                        // +-+++-=+
static const char vlist5082[] = { 3,17,11,12,11,17,19,12,11, 8};                               // +-+++-+-
static const char vlist5083[] = { 2,11, 0,17,19,11,17};                                        // +-+++-+=
static const char vlist5084[] = { 2,19,11,17,17,11, 9};                                        // +-+++-++
static const char vlist5085[] = { 3,10,12, 2,12,10,13, 2,17,19};                               // +-+++=--
static const char vlist5086[] = { 2,19, 2,17, 0,10,13};                                        // +-+++=-=
static const char vlist5087[] = { 2,19, 2,17, 8,10,13};                                        // +-+++=-+
static const char vlist5088[] = { 2,19, 2,17, 2, 1,12};                                        // +-+++==-
static const char vlist5089[] = { 1,19, 2,17};                                                 // +-+++===
static const char vlist5090[] = { 1,19, 2,17};                                                 // +-+++==+
static const char vlist5091[] = { 2,19, 2,17, 2, 8,12};                                        // +-+++=+-
static const char vlist5092[] = { 1,19, 2,17};                                                 // +-+++=+=
static const char vlist5093[] = { 1,19, 2,17};                                                 // +-+++=++
static const char vlist5094[] = { 3,12,10,13, 9,10,12,14,17,19};                               // +-++++--
static const char vlist5095[] = { 2, 0,10,13,19,14,17};                                        // +-++++-=
static const char vlist5096[] = { 2, 8,10,13,19,14,17};                                        // +-++++-+
static const char vlist5097[] = { 2, 9, 1,12,19,14,17};                                        // +-++++=-
static const char vlist5098[] = { 1,19,14,17};                                                 // +-++++==
static const char vlist5099[] = { 1,19,14,17};                                                 // +-++++=+
static const char vlist5100[] = { 2, 9, 8,12,19,14,17};                                        // +-+++++-
static const char vlist5101[] = { 1,19,14,17};                                                 // +-+++++=
static const char vlist5102[] = { 1,19,14,17};                                                 // +-++++++
static const char vlist5103[] = { 1, 6,15,18};                                                 // +=------
static const char vlist5104[] = { 3, 0, 6,15,18, 6, 0,18, 0,15};                               // +=-----=
static const char vlist5105[] = { 5, 6,15, 9,18, 6,12,18,12, 8,15, 8, 9,18, 8,15};             // +=-----+
static const char vlist5106[] = { 2, 1, 6,15,18, 6, 1};                                        // +=----=-
static const char vlist5107[] = { 4, 1,18, 0, 1, 0,15,18, 6, 0, 0, 6,15};                      // +=----==
static const char vlist5108[] = { 4,12, 1,18,15, 1, 9, 6,12,18, 6,15, 9};                      // +=----=+
static const char vlist5109[] = { 4, 6,15,10, 8, 6,10,13, 6, 8,18, 6,13};                      // +=----+-
static const char vlist5110[] = { 4, 6,13,18,13, 6, 0, 0, 6,10, 6,15,10};                      // +=----+=
static const char vlist5111[] = { 4,12,13,18, 6,12,18, 6,15, 9,15,10, 9};                      // +=----++
static const char vlist5112[] = { 2, 6, 2,18, 2,15,18};                                        // +=---=--
static const char vlist5113[] = { 3, 0, 2,15, 0,15,18, 6, 0,18};                               // +=---=-=
static const char vlist5114[] = { 4, 8, 2,15,12, 8,18, 8,15,18, 6,12,18};                      // +=---=-+
static const char vlist5115[] = { 3, 6, 2, 1, 2, 1,15, 6, 1,18};                               // +=---==-
static const char vlist5116[] = { 3, 2, 1,15, 0, 1,18, 0,18, 6};                               // +=---===
static const char vlist5117[] = { 3, 1, 2,15, 1,18,12,12,18, 6};                               // +=---==+
static const char vlist5118[] = { 4, 6, 2, 8,10, 2,15, 6, 8,13, 6,13,18};                      // +=---=+-
static const char vlist5119[] = { 3, 0,13, 6,13,18, 6,10, 2,15};                               // +=---=+=
static const char vlist5120[] = { 3,12,18, 6,13,18,12,10, 2,15};                               // +=---=++
static const char vlist5121[] = { 3,18, 6, 9,11,15, 9,15,18, 9};                               // +=---+--
static const char vlist5122[] = { 3, 0,11,15, 0,15,18, 6, 0,18};                               // +=---+-=
static const char vlist5123[] = { 4,12, 8,18, 8,15,18, 8,11,15, 6,12,18};                      // +=---+-+
static const char vlist5124[] = { 3, 6, 9, 1, 6, 1,18, 1,11,15};                               // +=---+=-
static const char vlist5125[] = { 3, 1,11,15, 0, 1,18, 0,18, 6};                               // +=---+==
static const char vlist5126[] = { 3, 1,11,15, 1,18,12,12,18, 6};                               // +=---+=+
static const char vlist5127[] = { 4, 6, 9, 8, 6, 8,13,10,11,15, 6,13,18};                      // +=---++-
static const char vlist5128[] = { 3, 0,13, 6,13,18, 6,10,11,15};                               // +=---++=
static const char vlist5129[] = { 3,12,18, 6,13,18,12,10,11,15};                               // +=---+++
static const char vlist5130[] = { 1, 6, 3,18};                                                 // +=--=---
static const char vlist5131[] = { 3, 3, 6, 0, 0, 3,18, 6, 0,18};                               // +=--=--=
static const char vlist5132[] = { 4, 6, 3, 9, 8, 3,18,12, 8,18, 6,12,18};                      // +=--=--+
static const char vlist5133[] = { 2, 6, 3, 1, 6, 1,18};                                        // +=--=-=-
static const char vlist5134[] = { 3, 0, 1,18, 0,18, 6, 3, 0, 6};                               // +=--=-==
static const char vlist5135[] = { 3, 1,18,12,12,18, 6, 3, 9, 6};                               // +=--=-=+
static const char vlist5136[] = { 3, 6, 3, 8, 6, 8,13, 6,13,18};                               // +=--=-+-
static const char vlist5137[] = { 3, 0,13, 6,13,18, 6, 3, 0, 6};                               // +=--=-+=
static const char vlist5138[] = { 3,12,18, 6,13,18,12, 3, 9, 6};                               // +=--=-++
static const char vlist5139[] = { 2, 2, 3,18, 6, 2,18};                                        // +=--==--
static const char vlist5140[] = { 2, 0,18, 6, 0, 3,18};                                        // +=--==-=
static const char vlist5141[] = { 3, 8, 3,18,12, 8,18,12,18, 6};                               // +=--==-+
static const char vlist5142[] = { 2, 2, 1, 6, 1,18, 6};                                        // +=--===-
static const char vlist5143[] = { 2, 0, 1,18, 0,18, 6};                                        // +=--====
static const char vlist5144[] = { 2, 1,18,12,12,18, 6};                                        // +=--===+
static const char vlist5145[] = { 3, 2, 8, 6, 8,13, 6,13,18, 6};                               // +=--==+-
static const char vlist5146[] = { 2, 0,13, 6,13,18, 6};                                        // +=--==+=
static const char vlist5147[] = { 2,12,18, 6,13,18,12};                                        // +=--==++
static const char vlist5148[] = { 2, 3,18, 9,18, 6, 9};                                        // +=--=+--
static const char vlist5149[] = { 2, 0,18, 6, 0, 3,18};                                        // +=--=+-=
static const char vlist5150[] = { 3, 8, 3,18,12, 8,18,12,18, 6};                               // +=--=+-+
static const char vlist5151[] = { 2, 9, 1, 6, 1,18, 6};                                        // +=--=+=-
static const char vlist5152[] = { 2, 0, 1,18, 0,18, 6};                                        // +=--=+==
static const char vlist5153[] = { 2, 1,18,12,12,18, 6};                                        // +=--=+=+
static const char vlist5154[] = { 3, 8,13, 6, 9, 8, 6,13,18, 6};                               // +=--=++-
static const char vlist5155[] = { 2, 0,13, 6,13,18, 6};                                        // +=--=++=
static const char vlist5156[] = { 2,12,18, 6,13,18,12};                                        // +=--=+++
static const char vlist5157[] = { 2,18, 6,10, 6,11,10};                                        // +=--+---
static const char vlist5158[] = { 3, 6,11, 0, 6, 0,18, 0,10,18};                               // +=--+--=
static const char vlist5159[] = { 4, 6,11, 9,12, 8,18, 8,10,18, 6,12,18};                      // +=--+--+
static const char vlist5160[] = { 2, 6,11, 1, 6, 1,18};                                        // +=--+-=-
static const char vlist5161[] = { 3, 0, 1,18, 0,18, 6,11, 0, 6};                               // +=--+-==
static const char vlist5162[] = { 3, 1,18,12,12,18, 6,11, 9, 6};                               // +=--+-=+
static const char vlist5163[] = { 3, 6,11, 8, 6, 8,13, 6,13,18};                               // +=--+-+-
static const char vlist5164[] = { 3, 0,13, 6,13,18, 6,11, 0, 6};                               // +=--+-+=
static const char vlist5165[] = { 3,12,18, 6,13,18,12,11, 9, 6};                               // +=--+-++
static const char vlist5166[] = { 2, 6, 2,10,18, 6,10};                                        // +=--+=--
static const char vlist5167[] = { 2, 0,18, 6, 0,10,18};                                        // +=--+=-=
static const char vlist5168[] = { 3, 8,10,18,12, 8,18,12,18, 6};                               // +=--+=-+
static const char vlist5169[] = { 2, 2, 1, 6, 1,18, 6};                                        // +=--+==-
static const char vlist5170[] = { 2, 0, 1,18, 0,18, 6};                                        // +=--+===
static const char vlist5171[] = { 2, 1,18,12,12,18, 6};                                        // +=--+==+
static const char vlist5172[] = { 3, 2, 8, 6, 8,13, 6,13,18, 6};                               // +=--+=+-
static const char vlist5173[] = { 2, 0,13, 6,13,18, 6};                                        // +=--+=+=
static const char vlist5174[] = { 2,12,18, 6,13,18,12};                                        // +=--+=++
static const char vlist5175[] = { 2,18, 6, 9,10,18, 9};                                        // +=--++--
static const char vlist5176[] = { 2, 0,18, 6, 0,10,18};                                        // +=--++-=
static const char vlist5177[] = { 3, 8,10,18,12, 8,18,12,18, 6};                               // +=--++-+
static const char vlist5178[] = { 2, 9, 1, 6, 1,18, 6};                                        // +=--++=-
static const char vlist5179[] = { 2, 0, 1,18, 0,18, 6};                                        // +=--++==
static const char vlist5180[] = { 2, 1,18,12,12,18, 6};                                        // +=--++=+
static const char vlist5181[] = { 3, 8,13, 6, 9, 8, 6,13,18, 6};                               // +=--+++-
static const char vlist5182[] = { 2, 0,13, 6,13,18, 6};                                        // +=--+++=
static const char vlist5183[] = { 2,12,18, 6,13,18,12};                                        // +=--++++
static const char vlist5184[] = { 2, 4, 6,15,18, 4,15};                                        // +=-=----
static const char vlist5185[] = { 3, 4, 0,18, 0,15,18, 0, 6,15};                               // +=-=---=
static const char vlist5186[] = { 4, 4, 8,18, 8, 9,15, 8,15,18, 9, 6,15};                      // +=-=---+
static const char vlist5187[] = { 3, 4, 6, 1, 1, 4,18, 1, 6,15};                               // +=-=--=-
static const char vlist5188[] = { 3, 4, 1,18, 1, 0,15,15, 0, 6};                               // +=-=--==
static const char vlist5189[] = { 3, 4, 1,18,15, 1, 9,15, 9, 6};                               // +=-=--=+
static const char vlist5190[] = { 4, 4, 6, 8,13, 4,18, 8, 6,10,10, 6,15};                      // +=-=--+-
static const char vlist5191[] = { 3,10, 0, 6,15,10, 6, 4,13,18};                               // +=-=--+=
static const char vlist5192[] = { 3,15, 9, 6,15,10, 9, 4,13,18};                               // +=-=--++
static const char vlist5193[] = { 2, 4,15,18, 4, 2,15};                                        // +=-=-=--
static const char vlist5194[] = { 3, 0, 2,15, 0,15,18, 4, 0,18};                               // +=-=-=-=
static const char vlist5195[] = { 3, 8, 2,15, 4, 8,18, 8,15,18};                               // +=-=-=-+
static const char vlist5196[] = { 3, 4, 2, 1, 1, 2,15, 4, 1,18};                               // +=-=-==-
static const char vlist5197[] = { 2, 1, 2,15, 1,18, 4};                                        // +=-=-===
static const char vlist5198[] = { 2, 1, 2,15, 1,18, 4};                                        // +=-=-==+
static const char vlist5199[] = { 3, 4, 2, 8,10, 2,15, 4,13,18};                               // +=-=-=+-
static const char vlist5200[] = { 2,10, 2,15,13,18, 4};                                        // +=-=-=+=
static const char vlist5201[] = { 2,10, 2,15,13,18, 4};                                        // +=-=-=++
static const char vlist5202[] = { 3, 4, 9,18, 9,11,15,18, 9,15};                               // +=-=-+--
static const char vlist5203[] = { 3, 0,11,15, 0,15,18, 4, 0,18};                               // +=-=-+-=
static const char vlist5204[] = { 3, 4, 8,18, 8,15,18, 8,11,15};                               // +=-=-+-+
static const char vlist5205[] = { 3, 4, 9, 1, 4, 1,18, 1,11,15};                               // +=-=-+=-
static const char vlist5206[] = { 2, 1,11,15, 1,18, 4};                                        // +=-=-+==
static const char vlist5207[] = { 2, 1,11,15, 1,18, 4};                                        // +=-=-+=+
static const char vlist5208[] = { 3, 4, 9, 8,10,11,15, 4,13,18};                               // +=-=-++-
static const char vlist5209[] = { 2,10,11,15,13,18, 4};                                        // +=-=-++=
static const char vlist5210[] = { 2,10,11,15,13,18, 4};                                        // +=-=-+++
static const char vlist5211[] = { 2, 4, 6, 3, 4, 3,18};                                        // +=-==---
static const char vlist5212[] = { 3, 3,18, 0, 0, 3, 6,18, 4, 0};                               // +=-==--=
static const char vlist5213[] = { 3, 8, 3,18, 3, 9, 6, 4, 8,18};                               // +=-==--+
static const char vlist5214[] = { 3, 4, 3, 1, 3, 4, 6, 4, 1,18};                               // +=-==-=-
static const char vlist5215[] = { 2, 1,18, 4, 3, 0, 6};                                        // +=-==-==
static const char vlist5216[] = { 2, 1,18, 4, 3, 9, 6};                                        // +=-==-=+
static const char vlist5217[] = { 3, 3, 8, 6, 8, 4, 6, 4,13,18};                               // +=-==-+-
static const char vlist5218[] = { 2, 3, 0, 6,13,18, 4};                                        // +=-==-+=
static const char vlist5219[] = { 2, 3, 9, 6,13,18, 4};                                        // +=-==-++
static const char vlist5220[] = { 2, 3, 4, 2, 4, 3,18};                                        // +=-===--
static const char vlist5221[] = { 2, 0,18, 4, 0, 3,18};                                        // +=-===-=
static const char vlist5222[] = { 2, 3,18, 8, 8,18, 4};                                        // +=-===-+
static const char vlist5223[] = { 2, 2, 1, 4, 1,18, 4};                                        // +=-====-
static const char vlist5224[] = { 1, 1,18, 4};                                                 // +=-=====
static const char vlist5225[] = { 1, 1,18, 4};                                                 // +=-====+
static const char vlist5226[] = { 2, 2, 8, 4,13,18, 4};                                        // +=-===+-
static const char vlist5227[] = { 1,13,18, 4};                                                 // +=-===+=
static const char vlist5228[] = { 1,13,18, 4};                                                 // +=-===++
static const char vlist5229[] = { 2, 3, 4, 9, 4, 3,18};                                        // +=-==+--
static const char vlist5230[] = { 2, 0,18, 4, 0, 3,18};                                        // +=-==+-=
static const char vlist5231[] = { 2, 3,18, 8, 8,18, 4};                                        // +=-==+-+
static const char vlist5232[] = { 2, 9, 1, 4, 1,18, 4};                                        // +=-==+=-
static const char vlist5233[] = { 1, 1,18, 4};                                                 // +=-==+==
static const char vlist5234[] = { 1, 1,18, 4};                                                 // +=-==+=+
static const char vlist5235[] = { 2, 9, 8, 4,13,18, 4};                                        // +=-==++-
static const char vlist5236[] = { 1,13,18, 4};                                                 // +=-==++=
static const char vlist5237[] = { 1,13,18, 4};                                                 // +=-==+++
static const char vlist5238[] = { 3, 4, 6,11, 4,10,18, 4,11,10};                               // +=-=+---
static const char vlist5239[] = { 3, 4,10,18,10, 4, 0,11, 0, 6};                               // +=-=+--=
static const char vlist5240[] = { 3, 8,10,18, 4, 8,18,11, 9, 6};                               // +=-=+--+
static const char vlist5241[] = { 3,11, 4, 6, 4,11, 1, 4, 1,18};                               // +=-=+-=-
static const char vlist5242[] = { 2, 1,18, 4,11, 0, 6};                                        // +=-=+-==
static const char vlist5243[] = { 2, 1,18, 4,11, 9, 6};                                        // +=-=+-=+
static const char vlist5244[] = { 3,11, 8, 6, 8, 4, 6, 4,13,18};                               // +=-=+-+-
static const char vlist5245[] = { 2,11, 0, 6,13,18, 4};                                        // +=-=+-+=
static const char vlist5246[] = { 2,11, 9, 6,13,18, 4};                                        // +=-=+-++
static const char vlist5247[] = { 2, 4,10,18,10, 4, 2};                                        // +=-=+=--
static const char vlist5248[] = { 2, 0,10, 4,10,18, 4};                                        // +=-=+=-=
static const char vlist5249[] = { 2,10,18, 8, 8,18, 4};                                        // +=-=+=-+
static const char vlist5250[] = { 2, 2, 1, 4, 1,18, 4};                                        // +=-=+==-
static const char vlist5251[] = { 1, 1,18, 4};                                                 // +=-=+===
static const char vlist5252[] = { 1, 1,18, 4};                                                 // +=-=+==+
static const char vlist5253[] = { 2, 2, 8, 4,13,18, 4};                                        // +=-=+=+-
static const char vlist5254[] = { 1,13,18, 4};                                                 // +=-=+=+=
static const char vlist5255[] = { 1,13,18, 4};                                                 // +=-=+=++
static const char vlist5256[] = { 2, 4,10,18,10, 4, 9};                                        // +=-=++--
static const char vlist5257[] = { 2, 0,10, 4,10,18, 4};                                        // +=-=++-=
static const char vlist5258[] = { 2,10,18, 8, 8,18, 4};                                        // +=-=++-+
static const char vlist5259[] = { 2, 9, 1, 4, 1,18, 4};                                        // +=-=++=-
static const char vlist5260[] = { 1, 1,18, 4};                                                 // +=-=++==
static const char vlist5261[] = { 1, 1,18, 4};                                                 // +=-=++=+
static const char vlist5262[] = { 2, 9, 8, 4,13,18, 4};                                        // +=-=+++-
static const char vlist5263[] = { 1,13,18, 4};                                                 // +=-=+++=
static const char vlist5264[] = { 1,13,18, 4};                                                 // +=-=++++
static const char vlist5265[] = { 3, 6,15,12,15,18,16,15,16,12};                               // +=-+----
static const char vlist5266[] = { 3, 0,18,16, 0,15,18, 0, 6,15};                               // +=-+---=
static const char vlist5267[] = { 4, 8, 9,15, 8,15,18,16, 8,18, 9, 6,15};                      // +=-+---+
static const char vlist5268[] = { 3,12, 6, 1, 1, 6,15, 1,16,18};                               // +=-+--=-
static const char vlist5269[] = { 3,16, 1,18, 1, 0,15,15, 0, 6};                               // +=-+--==
static const char vlist5270[] = { 3,16, 1,18,15, 1, 9,15, 9, 6};                               // +=-+--=+
static const char vlist5271[] = { 4,12, 6, 8, 8, 6,10,16,13,18,10, 6,15};                      // +=-+--+-
static const char vlist5272[] = { 3,10, 0, 6,15,10, 6,16,13,18};                               // +=-+--+=
static const char vlist5273[] = { 3,15, 9, 6,15,10, 9,16,13,18};                               // +=-+--++
static const char vlist5274[] = { 3,12, 2,15,12,15,16,16,15,18};                               // +=-+-=--
static const char vlist5275[] = { 3, 0, 2,15, 0,15,18, 0,18,16};                               // +=-+-=-=
static const char vlist5276[] = { 3, 8, 2,15,16, 8,18, 8,15,18};                               // +=-+-=-+
static const char vlist5277[] = { 3, 1, 2,15,12, 2, 1,16, 1,18};                               // +=-+-==-
static const char vlist5278[] = { 2, 1, 2,15, 1,18,16};                                        // +=-+-===
static const char vlist5279[] = { 2, 1, 2,15, 1,18,16};                                        // +=-+-==+
static const char vlist5280[] = { 3,10, 2,15,12, 2, 8,16,13,18};                               // +=-+-=+-
static const char vlist5281[] = { 2,10, 2,15,13,18,16};                                        // +=-+-=+=
static const char vlist5282[] = { 2,10, 2,15,13,18,16};                                        // +=-+-=++
static const char vlist5283[] = { 4, 9,11,12,12,11,15,12,15,16,16,15,18};                      // +=-+-+--
static const char vlist5284[] = { 3, 0,11,15, 0,15,18, 0,18,16};                               // +=-+-+-=
static const char vlist5285[] = { 3,16, 8,18, 8,15,18, 8,11,15};                               // +=-+-+-+
static const char vlist5286[] = { 3,12, 9, 1,16, 1,18, 1,11,15};                               // +=-+-+=-
static const char vlist5287[] = { 2, 1,11,15, 1,18,16};                                        // +=-+-+==
static const char vlist5288[] = { 2, 1,11,15, 1,18,16};                                        // +=-+-+=+
static const char vlist5289[] = { 3,12, 9, 8,10,11,15,16,13,18};                               // +=-+-++-
static const char vlist5290[] = { 2,10,11,15,13,18,16};                                        // +=-+-++=
static const char vlist5291[] = { 2,10,11,15,13,18,16};                                        // +=-+-+++
static const char vlist5292[] = { 3,16,12, 3,16, 3,18,12, 6, 3};                               // +=-+=---
static const char vlist5293[] = { 3,16, 3,18, 3,16, 0, 3, 0, 6};                               // +=-+=--=
static const char vlist5294[] = { 3, 8, 3,18, 3, 9, 6,16, 8,18};                               // +=-+=--+
static const char vlist5295[] = { 3, 3,12, 6,12, 3, 1,16, 1,18};                               // +=-+=-=-
static const char vlist5296[] = { 2, 1,18,16, 3, 0, 6};                                        // +=-+=-==
static const char vlist5297[] = { 2, 1,18,16, 3, 9, 6};                                        // +=-+=-=+
static const char vlist5298[] = { 3, 3, 8, 6,16,13,18, 8,12, 6};                               // +=-+=-+-
static const char vlist5299[] = { 2, 3, 0, 6,13,18,16};                                        // +=-+=-+=
static const char vlist5300[] = { 2, 3, 9, 6,13,18,16};                                        // +=-+=-++
static const char vlist5301[] = { 3,16, 3,18, 3,16,12, 3,12, 2};                               // +=-+==--
static const char vlist5302[] = { 2, 0, 3,16, 3,18,16};                                        // +=-+==-=
static const char vlist5303[] = { 2, 3,18, 8, 8,18,16};                                        // +=-+==-+
static const char vlist5304[] = { 2, 2, 1,12, 1,18,16};                                        // +=-+===-
static const char vlist5305[] = { 1, 1,18,16};                                                 // +=-+====
static const char vlist5306[] = { 1, 1,18,16};                                                 // +=-+===+
static const char vlist5307[] = { 2, 2, 8,12,13,18,16};                                        // +=-+==+-
static const char vlist5308[] = { 1,13,18,16};                                                 // +=-+==+=
static const char vlist5309[] = { 1,13,18,16};                                                 // +=-+==++
static const char vlist5310[] = { 3,16, 3,18, 3,16,12, 3,12, 9};                               // +=-+=+--
static const char vlist5311[] = { 2, 0, 3,16, 3,18,16};                                        // +=-+=+-=
static const char vlist5312[] = { 2, 3,18, 8, 8,18,16};                                        // +=-+=+-+
static const char vlist5313[] = { 2, 9, 1,12, 1,18,16};                                        // +=-+=+=-
static const char vlist5314[] = { 1, 1,18,16};                                                 // +=-+=+==
static const char vlist5315[] = { 1, 1,18,16};                                                 // +=-+=+=+
static const char vlist5316[] = { 2, 9, 8,12,13,18,16};                                        // +=-+=++-
static const char vlist5317[] = { 1,13,18,16};                                                 // +=-+=++=
static const char vlist5318[] = { 1,13,18,16};                                                 // +=-+=+++
static const char vlist5319[] = { 4,16,12,10,16,10,18,12, 6,11,12,11,10};                      // +=-++---
static const char vlist5320[] = { 3,10,16, 0,16,10,18,11, 0, 6};                               // +=-++--=
static const char vlist5321[] = { 3,16,10,18, 8,10,16,11, 9, 6};                               // +=-++--+
static const char vlist5322[] = { 3,12,11, 1,11,12, 6,16, 1,18};                               // +=-++-=-
static const char vlist5323[] = { 2, 1,18,16,11, 0, 6};                                        // +=-++-==
static const char vlist5324[] = { 2, 1,18,16,11, 9, 6};                                        // +=-++-=+
static const char vlist5325[] = { 3,11,12, 6,11, 8,12,16,13,18};                               // +=-++-+-
static const char vlist5326[] = { 2,11, 0, 6,13,18,16};                                        // +=-++-+=
static const char vlist5327[] = { 2,11, 9, 6,13,18,16};                                        // +=-++-++
static const char vlist5328[] = { 3,10,16,12,16,10,18,10,12, 2};                               // +=-++=--
static const char vlist5329[] = { 2, 0,10,16,10,18,16};                                        // +=-++=-=
static const char vlist5330[] = { 2,10,18,16,10,16, 8};                                        // +=-++=-+
static const char vlist5331[] = { 2, 2, 1,12, 1,18,16};                                        // +=-++==-
static const char vlist5332[] = { 1, 1,18,16};                                                 // +=-++===
static const char vlist5333[] = { 1, 1,18,16};                                                 // +=-++==+
static const char vlist5334[] = { 2, 2, 8,12,13,18,16};                                        // +=-++=+-
static const char vlist5335[] = { 1,13,18,16};                                                 // +=-++=+=
static const char vlist5336[] = { 1,13,18,16};                                                 // +=-++=++
static const char vlist5337[] = { 3,10,16,12,16,10,18,10,12, 9};                               // +=-+++--
static const char vlist5338[] = { 2, 0,10,16,10,18,16};                                        // +=-+++-=
static const char vlist5339[] = { 2,10,18,16,10,16, 8};                                        // +=-+++-+
static const char vlist5340[] = { 2, 9, 1,12, 1,18,16};                                        // +=-+++=-
static const char vlist5341[] = { 1, 1,18,16};                                                 // +=-+++==
static const char vlist5342[] = { 1, 1,18,16};                                                 // +=-+++=+
static const char vlist5343[] = { 2, 9, 8,12,13,18,16};                                        // +=-++++-
static const char vlist5344[] = { 1,13,18,16};                                                 // +=-++++=
static const char vlist5345[] = { 1,13,18,16};                                                 // +=-+++++
static const char vlist5346[] = { 1, 5, 6,15};                                                 // +==-----
static const char vlist5347[] = { 3, 6, 5, 0, 5, 0,15, 0, 6,15};                               // +==----=
static const char vlist5348[] = { 4, 6, 5,12, 5, 8,15, 8, 9,15, 9, 6,15};                      // +==----+
static const char vlist5349[] = { 2, 5, 6, 1, 1, 6,15};                                        // +==---=-
static const char vlist5350[] = { 3, 1, 0,15,15, 0, 6, 5, 0, 6};                               // +==---==
static const char vlist5351[] = { 3,15, 1, 9,15, 9, 6,12, 5, 6};                               // +==---=+
static const char vlist5352[] = { 3, 5, 6, 8, 8, 6,10,10, 6,15};                               // +==---+-
static const char vlist5353[] = { 3,10, 0, 6,15,10, 6, 0, 5, 6};                               // +==---+=
static const char vlist5354[] = { 3,15, 9, 6,15,10, 9,12, 5, 6};                               // +==---++
static const char vlist5355[] = { 2, 5, 6, 2, 5, 2,15};                                        // +==--=--
static const char vlist5356[] = { 3, 2,15, 0,15, 5, 0, 5, 0, 6};                               // +==--=-=
static const char vlist5357[] = { 3, 8, 2,15, 5, 8,15,12, 5, 6};                               // +==--=-+
static const char vlist5358[] = { 3, 5, 2, 1, 2, 5, 6, 1, 2,15};                               // +==--==-
static const char vlist5359[] = { 2, 1, 2,15, 6, 0, 5};                                        // +==--===
static const char vlist5360[] = { 2, 1, 2,15, 6,12, 5};                                        // +==--==+
static const char vlist5361[] = { 3, 2, 8, 6,10, 2,15, 8, 5, 6};                               // +==--=+-
static const char vlist5362[] = { 2, 6, 0, 5,10, 2,15};                                        // +==--=+=
static const char vlist5363[] = { 2, 6,12, 5,10, 2,15};                                        // +==--=++
static const char vlist5364[] = { 3, 5, 6, 9, 5,11,15, 5, 9,11};                               // +==--+--
static const char vlist5365[] = { 3, 5,11,15,11, 5, 0, 0, 5, 6};                               // +==--+-=
static const char vlist5366[] = { 3, 8,11,15, 5, 8,15,12, 5, 6};                               // +==--+-+
static const char vlist5367[] = { 3, 9, 5, 6, 5, 9, 1, 1,11,15};                               // +==--+=-
static const char vlist5368[] = { 2, 1,11,15, 6, 0, 5};                                        // +==--+==
static const char vlist5369[] = { 2, 1,11,15, 6,12, 5};                                        // +==--+=+
static const char vlist5370[] = { 3, 9, 8, 6, 8, 5, 6,10,11,15};                               // +==--++-
static const char vlist5371[] = { 2, 6, 0, 5,10,11,15};                                        // +==--++=
static const char vlist5372[] = { 2, 6,12, 5,10,11,15};                                        // +==--+++
static const char vlist5373[] = { 1, 5, 6, 3};                                                 // +==-=---
static const char vlist5374[] = { 3, 5, 3, 0, 0, 3, 6, 5, 0, 6};                               // +==-=--=
static const char vlist5375[] = { 3, 3, 5, 8, 3, 9, 6,12, 5, 6};                               // +==-=--+
static const char vlist5376[] = { 2, 3, 1, 6, 1, 5, 6};                                        // +==-=-=-
static const char vlist5377[] = { 2, 6, 0, 5, 3, 0, 6};                                        // +==-=-==
static const char vlist5378[] = { 2, 3, 9, 6, 6,12, 5};                                        // +==-=-=+
static const char vlist5379[] = { 2, 3, 8, 6, 8, 5, 6};                                        // +==-=-+-
static const char vlist5380[] = { 2, 6, 0, 5, 3, 0, 6};                                        // +==-=-+=
static const char vlist5381[] = { 2, 3, 9, 6, 6,12, 5};                                        // +==-=-++
static const char vlist5382[] = { 2, 3, 5, 2, 2, 5, 6};                                        // +==-==--
static const char vlist5383[] = { 2, 6, 0, 5, 0, 3, 5};                                        // +==-==-=
static const char vlist5384[] = { 2, 6,12, 5, 8, 3, 5};                                        // +==-==-+
static const char vlist5385[] = { 2, 6, 2, 5, 2, 1, 5};                                        // +==-===-
static const char vlist5386[] = { 1, 6, 0, 5};                                                 // +==-====
static const char vlist5387[] = { 1, 6,12, 5};                                                 // +==-===+
static const char vlist5388[] = { 2, 6, 2, 8, 6, 8, 5};                                        // +==-==+-
static const char vlist5389[] = { 1, 6, 0, 5};                                                 // +==-==+=
static const char vlist5390[] = { 1, 6,12, 5};                                                 // +==-==++
static const char vlist5391[] = { 2, 3, 5, 9, 9, 5, 6};                                        // +==-=+--
static const char vlist5392[] = { 2, 6, 0, 5, 0, 3, 5};                                        // +==-=+-=
static const char vlist5393[] = { 2, 6,12, 5, 8, 3, 5};                                        // +==-=+-+
static const char vlist5394[] = { 2, 9, 1, 5, 6, 9, 5};                                        // +==-=+=-
static const char vlist5395[] = { 1, 6, 0, 5};                                                 // +==-=+==
static const char vlist5396[] = { 1, 6,12, 5};                                                 // +==-=+=+
static const char vlist5397[] = { 2, 6, 8, 5, 6, 9, 8};                                        // +==-=++-
static const char vlist5398[] = { 1, 6, 0, 5};                                                 // +==-=++=
static const char vlist5399[] = { 1, 6,12, 5};                                                 // +==-=+++
static const char vlist5400[] = { 2, 5, 6,10,10, 6,11};                                        // +==-+---
static const char vlist5401[] = { 3,10, 5, 0, 0, 5, 6,11, 0, 6};                               // +==-+--=
static const char vlist5402[] = { 3,10, 5, 8,11, 9, 6,12, 5, 6};                               // +==-+--+
static const char vlist5403[] = { 2,11, 1, 6, 1, 5, 6};                                        // +==-+-=-
static const char vlist5404[] = { 2, 6, 0, 5,11, 0, 6};                                        // +==-+-==
static const char vlist5405[] = { 2,11, 9, 6, 6,12, 5};                                        // +==-+-=+
static const char vlist5406[] = { 2, 8, 5, 6,11, 8, 6};                                        // +==-+-+-
static const char vlist5407[] = { 2, 6, 0, 5,11, 0, 6};                                        // +==-+-+=
static const char vlist5408[] = { 2,11, 9, 6, 6,12, 5};                                        // +==-+-++
static const char vlist5409[] = { 2,10, 5, 2, 2, 5, 6};                                        // +==-+=--
static const char vlist5410[] = { 2, 6, 0, 5, 0,10, 5};                                        // +==-+=-=
static const char vlist5411[] = { 2, 6,12, 5, 8,10, 5};                                        // +==-+=-+
static const char vlist5412[] = { 2, 6, 2, 5, 2, 1, 5};                                        // +==-+==-
static const char vlist5413[] = { 1, 6, 0, 5};                                                 // +==-+===
static const char vlist5414[] = { 1, 6,12, 5};                                                 // +==-+==+
static const char vlist5415[] = { 2, 6, 2, 8, 6, 8, 5};                                        // +==-+=+-
static const char vlist5416[] = { 1, 6, 0, 5};                                                 // +==-+=+=
static const char vlist5417[] = { 1, 6,12, 5};                                                 // +==-+=++
static const char vlist5418[] = { 2,10, 5, 9, 9, 5, 6};                                        // +==-++--
static const char vlist5419[] = { 2, 6, 0, 5, 0,10, 5};                                        // +==-++-=
static const char vlist5420[] = { 2, 6,12, 5, 8,10, 5};                                        // +==-++-+
static const char vlist5421[] = { 2, 9, 1, 5, 6, 9, 5};                                        // +==-++=-
static const char vlist5422[] = { 1, 6, 0, 5};                                                 // +==-++==
static const char vlist5423[] = { 1, 6,12, 5};                                                 // +==-++=+
static const char vlist5424[] = { 2, 6, 8, 5, 6, 9, 8};                                        // +==-+++-
static const char vlist5425[] = { 1, 6, 0, 5};                                                 // +==-+++=
static const char vlist5426[] = { 1, 6,12, 5};                                                 // +==-++++
static const char vlist5427[] = { 2, 5, 4,15, 4, 6,15};                                        // +===----
static const char vlist5428[] = { 2,15, 0, 6, 5, 0,15};                                        // +===---=
static const char vlist5429[] = { 3, 5, 8,15, 8, 9,15,15, 9, 6};                               // +===---+
static const char vlist5430[] = { 2, 1, 4, 6,15, 1, 6};                                        // +===--=-
static const char vlist5431[] = { 2, 1, 0,15,15, 0, 6};                                        // +===--==
static const char vlist5432[] = { 2,15, 1, 9,15, 9, 6};                                        // +===--=+
static const char vlist5433[] = { 3, 8, 4, 6,10, 8, 6,15,10, 6};                               // +===--+-
static const char vlist5434[] = { 2,10, 0, 6,15,10, 6};                                        // +===--+=
static const char vlist5435[] = { 2,15, 9, 6,15,10, 9};                                        // +===--++
static const char vlist5436[] = { 2, 5, 4, 2, 5, 2,15};                                        // +===-=--
static const char vlist5437[] = { 2, 0, 2,15, 0,15, 5};                                        // +===-=-=
static const char vlist5438[] = { 2, 2,15, 8, 8,15, 5};                                        // +===-=-+
static const char vlist5439[] = { 2, 4, 2, 1, 1, 2,15};                                        // +===-==-
static const char vlist5440[] = { 1, 1, 2,15};                                                 // +===-===
static const char vlist5441[] = { 1, 1, 2,15};                                                 // +===-==+
static const char vlist5442[] = { 2, 4, 2, 8,10, 2,15};                                        // +===-=+-
static const char vlist5443[] = { 1,10, 2,15};                                                 // +===-=+=
static const char vlist5444[] = { 1,10, 2,15};                                                 // +===-=++
static const char vlist5445[] = { 3, 5, 4, 9, 5, 9,11, 5,11,15};                               // +===-+--
static const char vlist5446[] = { 2, 0,11, 5,11,15, 5};                                        // +===-+-=
static const char vlist5447[] = { 2, 8,15, 5,11,15, 8};                                        // +===-+-+
static const char vlist5448[] = { 2, 4, 9, 1, 1,11,15};                                        // +===-+=-
static const char vlist5449[] = { 1, 1,11,15};                                                 // +===-+==
static const char vlist5450[] = { 1, 1,11,15};                                                 // +===-+=+
static const char vlist5451[] = { 2, 4, 9, 8,10,11,15};                                        // +===-++-
static const char vlist5452[] = { 1,10,11,15};                                                 // +===-++=
static const char vlist5453[] = { 1,10,11,15};                                                 // +===-+++
static const char vlist5454[] = { 2, 5, 4, 3, 3, 4, 6};                                        // +====---
static const char vlist5455[] = { 2, 6, 3, 0, 0, 3, 5};                                        // +====--=
static const char vlist5456[] = { 2, 6, 3, 9, 8, 3, 5};                                        // +====--+
static const char vlist5457[] = { 2, 4, 3, 1, 3, 4, 6};                                        // +====-=-
static const char vlist5458[] = { 1, 3, 0, 6};                                                 // +====-==
static const char vlist5459[] = { 1, 3, 9, 6};                                                 // +====-=+
static const char vlist5460[] = { 2, 3, 8, 6, 8, 4, 6};                                        // +====-+-
static const char vlist5461[] = { 1, 3, 0, 6};                                                 // +====-+=
static const char vlist5462[] = { 1, 3, 9, 6};                                                 // +====-++
static const char vlist5463[] = { 2, 3, 4, 2, 4, 3, 5};                                        // +=====--
static const char vlist5464[] = { 1, 0, 3, 5};                                                 // +=====-=
static const char vlist5465[] = { 1, 8, 3, 5};                                                 // +=====-+
static const char vlist5466[] = { 1, 2, 1, 4};                                                 // +======-
                                                                                               // +=======
                                                                                               // +======+
static const char vlist5469[] = { 1, 2, 8, 4};                                                 // +=====+-
                                                                                               // +=====+=
                                                                                               // +=====++
static const char vlist5472[] = { 2, 9, 3, 5, 4, 9, 5};                                        // +====+--
static const char vlist5473[] = { 1, 0, 3, 5};                                                 // +====+-=
static const char vlist5474[] = { 1, 8, 3, 5};                                                 // +====+-+
static const char vlist5475[] = { 1, 9, 1, 4};                                                 // +====+=-
                                                                                               // +====+==
                                                                                               // +====+=+
static const char vlist5478[] = { 1, 9, 8, 4};                                                 // +====++-
                                                                                               // +====++=
                                                                                               // +====+++
static const char vlist5481[] = { 3, 5, 4,10,11, 4, 6,10, 4,11};                               // +===+---
static const char vlist5482[] = { 2, 6,11, 0, 0,10, 5};                                        // +===+--=
static const char vlist5483[] = { 2, 6,11, 9, 8,10, 5};                                        // +===+--+
static const char vlist5484[] = { 2,11, 4, 6, 4,11, 1};                                        // +===+-=-
static const char vlist5485[] = { 1,11, 0, 6};                                                 // +===+-==
static const char vlist5486[] = { 1,11, 9, 6};                                                 // +===+-=+
static const char vlist5487[] = { 2,11, 8, 6, 8, 4, 6};                                        // +===+-+-
static const char vlist5488[] = { 1,11, 0, 6};                                                 // +===+-+=
static const char vlist5489[] = { 1,11, 9, 6};                                                 // +===+-++
static const char vlist5490[] = { 2, 4,10, 5,10, 4, 2};                                        // +===+=--
static const char vlist5491[] = { 1, 0,10, 5};                                                 // +===+=-=
static const char vlist5492[] = { 1, 8,10, 5};                                                 // +===+=-+
static const char vlist5493[] = { 1, 2, 1, 4};                                                 // +===+==-
                                                                                               // +===+===
                                                                                               // +===+==+
static const char vlist5496[] = { 1, 2, 8, 4};                                                 // +===+=+-
                                                                                               // +===+=+=
                                                                                               // +===+=++
static const char vlist5499[] = { 2, 9,10, 5, 4, 9, 5};                                        // +===++--
static const char vlist5500[] = { 1, 0,10, 5};                                                 // +===++-=
static const char vlist5501[] = { 1, 8,10, 5};                                                 // +===++-+
static const char vlist5502[] = { 1, 9, 1, 4};                                                 // +===++=-
                                                                                               // +===++==
                                                                                               // +===++=+
static const char vlist5505[] = { 1, 9, 8, 4};                                                 // +===+++-
                                                                                               // +===+++=
                                                                                               // +===++++
static const char vlist5508[] = { 2,15, 5,12, 6,15,12};                                        // +==+----
static const char vlist5509[] = { 2,15, 0, 6, 5, 0,15};                                        // +==+---=
static const char vlist5510[] = { 3, 5, 8,15, 8, 9,15,15, 9, 6};                               // +==+---+
static const char vlist5511[] = { 2, 1,12, 6,15, 1, 6};                                        // +==+--=-
static const char vlist5512[] = { 2, 1, 0,15,15, 0, 6};                                        // +==+--==
static const char vlist5513[] = { 2,15, 1, 9,15, 9, 6};                                        // +==+--=+
static const char vlist5514[] = { 3,10, 8, 6, 8,12, 6,15,10, 6};                               // +==+--+-
static const char vlist5515[] = { 2,10, 0, 6,15,10, 6};                                        // +==+--+=
static const char vlist5516[] = { 2,15, 9, 6,15,10, 9};                                        // +==+--++
static const char vlist5517[] = { 2, 5,12, 2, 5, 2,15};                                        // +==+-=--
static const char vlist5518[] = { 2, 0, 2,15, 0,15, 5};                                        // +==+-=-=
static const char vlist5519[] = { 2, 2,15, 8, 8,15, 5};                                        // +==+-=-+
static const char vlist5520[] = { 2, 1, 2,15,12, 2, 1};                                        // +==+-==-
static const char vlist5521[] = { 1, 1, 2,15};                                                 // +==+-===
static const char vlist5522[] = { 1, 1, 2,15};                                                 // +==+-==+
static const char vlist5523[] = { 2,10, 2,15,12, 2, 8};                                        // +==+-=+-
static const char vlist5524[] = { 1,10, 2,15};                                                 // +==+-=+=
static const char vlist5525[] = { 1,10, 2,15};                                                 // +==+-=++
static const char vlist5526[] = { 3, 5,12, 9, 5, 9,11, 5,11,15};                               // +==+-+--
static const char vlist5527[] = { 2, 0,11, 5,11,15, 5};                                        // +==+-+-=
static const char vlist5528[] = { 2, 8,15, 5,11,15, 8};                                        // +==+-+-+
static const char vlist5529[] = { 2,12, 9, 1, 1,11,15};                                        // +==+-+=-
static const char vlist5530[] = { 1, 1,11,15};                                                 // +==+-+==
static const char vlist5531[] = { 1, 1,11,15};                                                 // +==+-+=+
static const char vlist5532[] = { 2,12, 9, 8,10,11,15};                                        // +==+-++-
static const char vlist5533[] = { 1,10,11,15};                                                 // +==+-++=
static const char vlist5534[] = { 1,10,11,15};                                                 // +==+-+++
static const char vlist5535[] = { 2, 3,12, 6, 5,12, 3};                                        // +==+=---
static const char vlist5536[] = { 2, 6, 3, 0, 0, 3, 5};                                        // +==+=--=
static const char vlist5537[] = { 2, 6, 3, 9, 8, 3, 5};                                        // +==+=--+
static const char vlist5538[] = { 2, 3,12, 6,12, 3, 1};                                        // +==+=-=-
static const char vlist5539[] = { 1, 3, 0, 6};                                                 // +==+=-==
static const char vlist5540[] = { 1, 3, 9, 6};                                                 // +==+=-=+
static const char vlist5541[] = { 2, 3, 8, 6, 8,12, 6};                                        // +==+=-+-
static const char vlist5542[] = { 1, 3, 0, 6};                                                 // +==+=-+=
static const char vlist5543[] = { 1, 3, 9, 6};                                                 // +==+=-++
static const char vlist5544[] = { 2,12, 3, 5, 3,12, 2};                                        // +==+==--
static const char vlist5545[] = { 1, 0, 3, 5};                                                 // +==+==-=
static const char vlist5546[] = { 1, 8, 3, 5};                                                 // +==+==-+
static const char vlist5547[] = { 1, 2, 1,12};                                                 // +==+===-
                                                                                               // +==+====
                                                                                               // +==+===+
static const char vlist5550[] = { 1, 2, 8,12};                                                 // +==+==+-
                                                                                               // +==+==+=
                                                                                               // +==+==++
static const char vlist5553[] = { 2, 9, 3, 5,12, 9, 5};                                        // +==+=+--
static const char vlist5554[] = { 1, 0, 3, 5};                                                 // +==+=+-=
static const char vlist5555[] = { 1, 8, 3, 5};                                                 // +==+=+-+
static const char vlist5556[] = { 1, 9, 1,12};                                                 // +==+=+=-
                                                                                               // +==+=+==
                                                                                               // +==+=+=+
static const char vlist5559[] = { 1, 9, 8,12};                                                 // +==+=++-
                                                                                               // +==+=++=
                                                                                               // +==+=+++
static const char vlist5562[] = { 3,11,12, 6,10,12,11, 5,12,10};                               // +==++---
static const char vlist5563[] = { 2, 6,11, 0, 0,10, 5};                                        // +==++--=
static const char vlist5564[] = { 2, 6,11, 9, 8,10, 5};                                        // +==++--+
static const char vlist5565[] = { 2,12,11, 1,11,12, 6};                                        // +==++-=-
static const char vlist5566[] = { 1,11, 0, 6};                                                 // +==++-==
static const char vlist5567[] = { 1,11, 9, 6};                                                 // +==++-=+
static const char vlist5568[] = { 2,11,12, 6,11, 8,12};                                        // +==++-+-
static const char vlist5569[] = { 1,11, 0, 6};                                                 // +==++-+=
static const char vlist5570[] = { 1,11, 9, 6};                                                 // +==++-++
static const char vlist5571[] = { 2,10,12, 2,12,10, 5};                                        // +==++=--
static const char vlist5572[] = { 1, 0,10, 5};                                                 // +==++=-=
static const char vlist5573[] = { 1, 8,10, 5};                                                 // +==++=-+
static const char vlist5574[] = { 1, 2, 1,12};                                                 // +==++==-
                                                                                               // +==++===
                                                                                               // +==++==+
static const char vlist5577[] = { 1, 2, 8,12};                                                 // +==++=+-
                                                                                               // +==++=+=
                                                                                               // +==++=++
static const char vlist5580[] = { 2,12,10, 5, 9,10,12};                                        // +==+++--
static const char vlist5581[] = { 1, 0,10, 5};                                                 // +==+++-=
static const char vlist5582[] = { 1, 8,10, 5};                                                 // +==+++-+
static const char vlist5583[] = { 1, 9, 1,12};                                                 // +==+++=-
                                                                                               // +==+++==
                                                                                               // +==+++=+
static const char vlist5586[] = { 1, 9, 8,12};                                                 // +==++++-
                                                                                               // +==++++=
                                                                                               // +==+++++
static const char vlist5589[] = { 2, 6,15,13,16, 6,13};                                        // +=+-----
static const char vlist5590[] = { 3, 6,16, 0, 0, 6,15,13, 0,15};                               // +=+----=
static const char vlist5591[] = { 4,16, 6,12, 8, 9,15,13, 8,15, 9, 6,15};                      // +=+----+
static const char vlist5592[] = { 2,16, 6, 1, 1, 6,15};                                        // +=+---=-
static const char vlist5593[] = { 3, 1, 0,15,15, 0, 6, 0,16, 6};                               // +=+---==
static const char vlist5594[] = { 3,15, 1, 9,15, 9, 6,12,16, 6};                               // +=+---=+
static const char vlist5595[] = { 3,16, 6, 8, 8, 6,10,10, 6,15};                               // +=+---+-
static const char vlist5596[] = { 3,10, 0, 6,15,10, 6, 0,16, 6};                               // +=+---+=
static const char vlist5597[] = { 3,15, 9, 6,15,10, 9,12,16, 6};                               // +=+---++
static const char vlist5598[] = { 3,16, 2,13,16, 6, 2,13, 2,15};                               // +=+--=--
static const char vlist5599[] = { 3,13, 2,15, 2,13, 0, 0,16, 6};                               // +=+--=-=
static const char vlist5600[] = { 3, 8, 2,15,13, 8,15,12,16, 6};                               // +=+--=-+
static const char vlist5601[] = { 3, 2,16, 6,16, 2, 1, 1, 2,15};                               // +=+--==-
static const char vlist5602[] = { 2, 1, 2,15, 6, 0,16};                                        // +=+--===
static const char vlist5603[] = { 2, 1, 2,15, 6,12,16};                                        // +=+--==+
static const char vlist5604[] = { 3, 2, 8, 6,10, 2,15, 8,16, 6};                               // +=+--=+-
static const char vlist5605[] = { 2, 6, 0,16,10, 2,15};                                        // +=+--=+=
static const char vlist5606[] = { 2, 6,12,16,10, 2,15};                                        // +=+--=++
static const char vlist5607[] = { 4,16, 9,13,16, 6, 9,13, 9,11,13,11,15};                      // +=+--+--
static const char vlist5608[] = { 3,11,13, 0,13,11,15, 0,16, 6};                               // +=+--+-=
static const char vlist5609[] = { 3,13,11,15, 8,11,13,12,16, 6};                               // +=+--+-+
static const char vlist5610[] = { 3,16, 9, 1, 9,16, 6, 1,11,15};                               // +=+--+=-
static const char vlist5611[] = { 2, 1,11,15, 6, 0,16};                                        // +=+--+==
static const char vlist5612[] = { 2, 1,11,15, 6,12,16};                                        // +=+--+=+
static const char vlist5613[] = { 3, 9,16, 6, 9, 8,16,10,11,15};                               // +=+--++-
static const char vlist5614[] = { 2, 6, 0,16,10,11,15};                                        // +=+--++=
static const char vlist5615[] = { 2, 6,12,16,10,11,15};                                        // +=+--+++
static const char vlist5616[] = { 2,16, 6,13,13, 6, 3};                                        // +=+-=---
static const char vlist5617[] = { 3, 3, 0, 6, 3,13, 0, 0,16, 6};                               // +=+-=--=
static const char vlist5618[] = { 3, 3, 9, 6, 3,13, 8,12,16, 6};                               // +=+-=--+
static const char vlist5619[] = { 2, 3, 1, 6, 1,16, 6};                                        // +=+-=-=-
static const char vlist5620[] = { 2, 6, 0,16, 3, 0, 6};                                        // +=+-=-==
static const char vlist5621[] = { 2, 3, 9, 6, 6,12,16};                                        // +=+-=-=+
static const char vlist5622[] = { 2, 3, 8, 6, 8,16, 6};                                        // +=+-=-+-
static const char vlist5623[] = { 2, 6, 0,16, 3, 0, 6};                                        // +=+-=-+=
static const char vlist5624[] = { 2, 3, 9, 6, 6,12,16};                                        // +=+-=-++
static const char vlist5625[] = { 3, 3,13, 2,13,16, 2, 2,16, 6};                               // +=+-==--
static const char vlist5626[] = { 2, 6, 0,16, 0, 3,13};                                        // +=+-==-=
static const char vlist5627[] = { 2, 6,12,16, 8, 3,13};                                        // +=+-==-+
static const char vlist5628[] = { 2, 6, 2,16, 2, 1,16};                                        // +=+-===-
static const char vlist5629[] = { 1, 6, 0,16};                                                 // +=+-====
static const char vlist5630[] = { 1, 6,12,16};                                                 // +=+-===+
static const char vlist5631[] = { 2, 6, 2, 8, 6, 8,16};                                        // +=+-==+-
static const char vlist5632[] = { 1, 6, 0,16};                                                 // +=+-==+=
static const char vlist5633[] = { 1, 6,12,16};                                                 // +=+-==++
static const char vlist5634[] = { 3, 3,13, 9,13,16, 9, 9,16, 6};                               // +=+-=+--
static const char vlist5635[] = { 2, 6, 0,16, 0, 3,13};                                        // +=+-=+-=
static const char vlist5636[] = { 2, 6,12,16, 8, 3,13};                                        // +=+-=+-+
static const char vlist5637[] = { 2, 9, 1,16, 6, 9,16};                                        // +=+-=+=-
static const char vlist5638[] = { 1, 6, 0,16};                                                 // +=+-=+==
static const char vlist5639[] = { 1, 6,12,16};                                                 // +=+-=+=+
static const char vlist5640[] = { 2,16, 9, 8, 6, 9,16};                                        // +=+-=++-
static const char vlist5641[] = { 1, 6, 0,16};                                                 // +=+-=++=
static const char vlist5642[] = { 1, 6,12,16};                                                 // +=+-=+++
static const char vlist5643[] = { 3,13,16,11,10,13,11,16, 6,11};                               // +=+-+---
static const char vlist5644[] = { 3,10,13, 0, 0,16, 6,11, 0, 6};                               // +=+-+--=
static const char vlist5645[] = { 3,10,13, 8,11, 9, 6,12,16, 6};                               // +=+-+--+
static const char vlist5646[] = { 2,11, 1, 6, 1,16, 6};                                        // +=+-+-=-
static const char vlist5647[] = { 2, 6, 0,16,11, 0, 6};                                        // +=+-+-==
static const char vlist5648[] = { 2,11, 9, 6, 6,12,16};                                        // +=+-+-=+
static const char vlist5649[] = { 2, 8,16, 6,11, 8, 6};                                        // +=+-+-+-
static const char vlist5650[] = { 2, 6, 0,16,11, 0, 6};                                        // +=+-+-+=
static const char vlist5651[] = { 2,11, 9, 6, 6,12,16};                                        // +=+-+-++
static const char vlist5652[] = { 3,10,13, 2,13,16, 2, 2,16, 6};                               // +=+-+=--
static const char vlist5653[] = { 2, 6, 0,16, 0,10,13};                                        // +=+-+=-=
static const char vlist5654[] = { 2, 6,12,16, 8,10,13};                                        // +=+-+=-+
static const char vlist5655[] = { 2, 6, 2,16, 2, 1,16};                                        // +=+-+==-
static const char vlist5656[] = { 1, 6, 0,16};                                                 // +=+-+===
static const char vlist5657[] = { 1, 6,12,16};                                                 // +=+-+==+
static const char vlist5658[] = { 2, 6, 2, 8, 6, 8,16};                                        // +=+-+=+-
static const char vlist5659[] = { 1, 6, 0,16};                                                 // +=+-+=+=
static const char vlist5660[] = { 1, 6,12,16};                                                 // +=+-+=++
static const char vlist5661[] = { 3,10,13, 9,13,16, 9, 9,16, 6};                               // +=+-++--
static const char vlist5662[] = { 2, 6, 0,16, 0,10,13};                                        // +=+-++-=
static const char vlist5663[] = { 2, 6,12,16, 8,10,13};                                        // +=+-++-+
static const char vlist5664[] = { 2, 9, 1,16, 6, 9,16};                                        // +=+-++=-
static const char vlist5665[] = { 1, 6, 0,16};                                                 // +=+-++==
static const char vlist5666[] = { 1, 6,12,16};                                                 // +=+-++=+
static const char vlist5667[] = { 2,16, 9, 8, 6, 9,16};                                        // +=+-+++-
static const char vlist5668[] = { 1, 6, 0,16};                                                 // +=+-+++=
static const char vlist5669[] = { 1, 6,12,16};                                                 // +=+-++++
static const char vlist5670[] = { 2, 4, 6,13, 6,15,13};                                        // +=+=----
static const char vlist5671[] = { 2,15, 0, 6,13, 0,15};                                        // +=+=---=
static const char vlist5672[] = { 3,13, 8,15, 8, 9,15,15, 9, 6};                               // +=+=---+
static const char vlist5673[] = { 2, 1, 4, 6,15, 1, 6};                                        // +=+=--=-
static const char vlist5674[] = { 2, 1, 0,15,15, 0, 6};                                        // +=+=--==
static const char vlist5675[] = { 2,15, 1, 9,15, 9, 6};                                        // +=+=--=+
static const char vlist5676[] = { 3, 8, 4, 6,10, 8, 6,15,10, 6};                               // +=+=--+-
static const char vlist5677[] = { 2,10, 0, 6,15,10, 6};                                        // +=+=--+=
static const char vlist5678[] = { 2,15, 9, 6,15,10, 9};                                        // +=+=--++
static const char vlist5679[] = { 2,13, 4, 2,13, 2,15};                                        // +=+=-=--
static const char vlist5680[] = { 2, 2,15,13, 0, 2,13};                                        // +=+=-=-=
static const char vlist5681[] = { 2, 2,15, 8, 8,15,13};                                        // +=+=-=-+
static const char vlist5682[] = { 2, 4, 2, 1, 1, 2,15};                                        // +=+=-==-
static const char vlist5683[] = { 1, 1, 2,15};                                                 // +=+=-===
static const char vlist5684[] = { 1, 1, 2,15};                                                 // +=+=-==+
static const char vlist5685[] = { 2, 4, 2, 8,10, 2,15};                                        // +=+=-=+-
static const char vlist5686[] = { 1,10, 2,15};                                                 // +=+=-=+=
static const char vlist5687[] = { 1,10, 2,15};                                                 // +=+=-=++
static const char vlist5688[] = { 3,13, 4, 9,13, 9,11,13,11,15};                               // +=+=-+--
static const char vlist5689[] = { 2, 0,11,13,11,15,13};                                        // +=+=-+-=
static const char vlist5690[] = { 2,11,13, 8,11,15,13};                                        // +=+=-+-+
static const char vlist5691[] = { 2, 4, 9, 1, 1,11,15};                                        // +=+=-+=-
static const char vlist5692[] = { 1, 1,11,15};                                                 // +=+=-+==
static const char vlist5693[] = { 1, 1,11,15};                                                 // +=+=-+=+
static const char vlist5694[] = { 2, 4, 9, 8,10,11,15};                                        // +=+=-++-
static const char vlist5695[] = { 1,10,11,15};                                                 // +=+=-++=
static const char vlist5696[] = { 1,10,11,15};                                                 // +=+=-+++
static const char vlist5697[] = { 2, 3, 4, 6,13, 4, 3};                                        // +=+==---
static const char vlist5698[] = { 2, 6, 3, 0, 0, 3,13};                                        // +=+==--=
static const char vlist5699[] = { 2, 6, 3, 9, 8, 3,13};                                        // +=+==--+
static const char vlist5700[] = { 2, 4, 3, 1, 3, 4, 6};                                        // +=+==-=-
static const char vlist5701[] = { 1, 3, 0, 6};                                                 // +=+==-==
static const char vlist5702[] = { 1, 3, 9, 6};                                                 // +=+==-=+
static const char vlist5703[] = { 2, 3, 8, 6, 8, 4, 6};                                        // +=+==-+-
static const char vlist5704[] = { 1, 3, 0, 6};                                                 // +=+==-+=
static const char vlist5705[] = { 1, 3, 9, 6};                                                 // +=+==-++
static const char vlist5706[] = { 2, 3,13, 2,13, 4, 2};                                        // +=+===--
static const char vlist5707[] = { 1, 0, 3,13};                                                 // +=+===-=
static const char vlist5708[] = { 1, 8, 3,13};                                                 // +=+===-+
static const char vlist5709[] = { 1, 2, 1, 4};                                                 // +=+====-
                                                                                               // +=+=====
                                                                                               // +=+====+
static const char vlist5712[] = { 1, 2, 8, 4};                                                 // +=+===+-
                                                                                               // +=+===+=
                                                                                               // +=+===++
static const char vlist5715[] = { 2, 9, 3,13, 4, 9,13};                                        // +=+==+--
static const char vlist5716[] = { 1, 0, 3,13};                                                 // +=+==+-=
static const char vlist5717[] = { 1, 8, 3,13};                                                 // +=+==+-+
static const char vlist5718[] = { 1, 9, 1, 4};                                                 // +=+==+=-
                                                                                               // +=+==+==
                                                                                               // +=+==+=+
static const char vlist5721[] = { 1, 9, 8, 4};                                                 // +=+==++-
                                                                                               // +=+==++=
                                                                                               // +=+==+++
static const char vlist5724[] = { 3,11, 4, 6,10, 4,11,13, 4,10};                               // +=+=+---
static const char vlist5725[] = { 2, 6,11, 0, 0,10,13};                                        // +=+=+--=
static const char vlist5726[] = { 2, 6,11, 9, 8,10,13};                                        // +=+=+--+
static const char vlist5727[] = { 2,11, 4, 6, 4,11, 1};                                        // +=+=+-=-
static const char vlist5728[] = { 1,11, 0, 6};                                                 // +=+=+-==
static const char vlist5729[] = { 1,11, 9, 6};                                                 // +=+=+-=+
static const char vlist5730[] = { 2,11, 8, 6, 8, 4, 6};                                        // +=+=+-+-
static const char vlist5731[] = { 1,11, 0, 6};                                                 // +=+=+-+=
static const char vlist5732[] = { 1,11, 9, 6};                                                 // +=+=+-++
static const char vlist5733[] = { 2, 4,10,13,10, 4, 2};                                        // +=+=+=--
static const char vlist5734[] = { 1, 0,10,13};                                                 // +=+=+=-=
static const char vlist5735[] = { 1, 8,10,13};                                                 // +=+=+=-+
static const char vlist5736[] = { 1, 2, 1, 4};                                                 // +=+=+==-
                                                                                               // +=+=+===
                                                                                               // +=+=+==+
static const char vlist5739[] = { 1, 2, 8, 4};                                                 // +=+=+=+-
                                                                                               // +=+=+=+=
                                                                                               // +=+=+=++
static const char vlist5742[] = { 2, 9,10,13, 4, 9,13};                                        // +=+=++--
static const char vlist5743[] = { 1, 0,10,13};                                                 // +=+=++-=
static const char vlist5744[] = { 1, 8,10,13};                                                 // +=+=++-+
static const char vlist5745[] = { 1, 9, 1, 4};                                                 // +=+=++=-
                                                                                               // +=+=++==
                                                                                               // +=+=++=+
static const char vlist5748[] = { 1, 9, 8, 4};                                                 // +=+=+++-
                                                                                               // +=+=+++=
                                                                                               // +=+=++++
static const char vlist5751[] = { 2, 6,15,12,15,13,12};                                        // +=++----
static const char vlist5752[] = { 2,15, 0, 6,13, 0,15};                                        // +=++---=
static const char vlist5753[] = { 3,13, 8,15, 8, 9,15,15, 9, 6};                               // +=++---+
static const char vlist5754[] = { 2, 1,12, 6,15, 1, 6};                                        // +=++--=-
static const char vlist5755[] = { 2, 1, 0,15,15, 0, 6};                                        // +=++--==
static const char vlist5756[] = { 2,15, 1, 9,15, 9, 6};                                        // +=++--=+
static const char vlist5757[] = { 3,10, 8, 6, 8,12, 6,15,10, 6};                               // +=++--+-
static const char vlist5758[] = { 2,10, 0, 6,15,10, 6};                                        // +=++--+=
static const char vlist5759[] = { 2,15, 9, 6,15,10, 9};                                        // +=++--++
static const char vlist5760[] = { 2,13,12, 2,13, 2,15};                                        // +=++-=--
static const char vlist5761[] = { 2, 2,15,13, 0, 2,13};                                        // +=++-=-=
static const char vlist5762[] = { 2, 2,15, 8, 8,15,13};                                        // +=++-=-+
static const char vlist5763[] = { 2, 1, 2,15,12, 2, 1};                                        // +=++-==-
static const char vlist5764[] = { 1, 1, 2,15};                                                 // +=++-===
static const char vlist5765[] = { 1, 1, 2,15};                                                 // +=++-==+
static const char vlist5766[] = { 2,10, 2,15,12, 2, 8};                                        // +=++-=+-
static const char vlist5767[] = { 1,10, 2,15};                                                 // +=++-=+=
static const char vlist5768[] = { 1,10, 2,15};                                                 // +=++-=++
static const char vlist5769[] = { 3,13,12, 9,13,11,15,13, 9,11};                               // +=++-+--
static const char vlist5770[] = { 2, 0,11,13,11,15,13};                                        // +=++-+-=
static const char vlist5771[] = { 2,11,13, 8,11,15,13};                                        // +=++-+-+
static const char vlist5772[] = { 2,12, 9, 1, 1,11,15};                                        // +=++-+=-
static const char vlist5773[] = { 1, 1,11,15};                                                 // +=++-+==
static const char vlist5774[] = { 1, 1,11,15};                                                 // +=++-+=+
static const char vlist5775[] = { 2,12, 9, 8,10,11,15};                                        // +=++-++-
static const char vlist5776[] = { 1,10,11,15};                                                 // +=++-++=
static const char vlist5777[] = { 1,10,11,15};                                                 // +=++-+++
static const char vlist5778[] = { 2, 3,12, 6,13,12, 3};                                        // +=++=---
static const char vlist5779[] = { 2, 6, 3, 0, 0, 3,13};                                        // +=++=--=
static const char vlist5780[] = { 2, 6, 3, 9, 8, 3,13};                                        // +=++=--+
static const char vlist5781[] = { 2, 3,12, 6,12, 3, 1};                                        // +=++=-=-
static const char vlist5782[] = { 1, 3, 0, 6};                                                 // +=++=-==
static const char vlist5783[] = { 1, 3, 9, 6};                                                 // +=++=-=+
static const char vlist5784[] = { 2, 3, 8, 6, 8,12, 6};                                        // +=++=-+-
static const char vlist5785[] = { 1, 3, 0, 6};                                                 // +=++=-+=
static const char vlist5786[] = { 1, 3, 9, 6};                                                 // +=++=-++
static const char vlist5787[] = { 2,12, 3,13, 3,12, 2};                                        // +=++==--
static const char vlist5788[] = { 1, 0, 3,13};                                                 // +=++==-=
static const char vlist5789[] = { 1, 8, 3,13};                                                 // +=++==-+
static const char vlist5790[] = { 1, 2, 1,12};                                                 // +=++===-
                                                                                               // +=++====
                                                                                               // +=++===+
static const char vlist5793[] = { 1, 2, 8,12};                                                 // +=++==+-
                                                                                               // +=++==+=
                                                                                               // +=++==++
static const char vlist5796[] = { 2, 9, 3,13,12, 9,13};                                        // +=++=+--
static const char vlist5797[] = { 1, 0, 3,13};                                                 // +=++=+-=
static const char vlist5798[] = { 1, 8, 3,13};                                                 // +=++=+-+
static const char vlist5799[] = { 1, 9, 1,12};                                                 // +=++=+=-
                                                                                               // +=++=+==
                                                                                               // +=++=+=+
static const char vlist5802[] = { 1, 9, 8,12};                                                 // +=++=++-
                                                                                               // +=++=++=
                                                                                               // +=++=+++
static const char vlist5805[] = { 3,11,12, 6,10,12,11,13,12,10};                               // +=+++---
static const char vlist5806[] = { 2, 6,11, 0, 0,10,13};                                        // +=+++--=
static const char vlist5807[] = { 2, 6,11, 9, 8,10,13};                                        // +=+++--+
static const char vlist5808[] = { 2,12,11, 1,11,12, 6};                                        // +=+++-=-
static const char vlist5809[] = { 1,11, 0, 6};                                                 // +=+++-==
static const char vlist5810[] = { 1,11, 9, 6};                                                 // +=+++-=+
static const char vlist5811[] = { 2,11,12, 6,11, 8,12};                                        // +=+++-+-
static const char vlist5812[] = { 1,11, 0, 6};                                                 // +=+++-+=
static const char vlist5813[] = { 1,11, 9, 6};                                                 // +=+++-++
static const char vlist5814[] = { 2,10,12, 2,12,10,13};                                        // +=+++=--
static const char vlist5815[] = { 1, 0,10,13};                                                 // +=+++=-=
static const char vlist5816[] = { 1, 8,10,13};                                                 // +=+++=-+
static const char vlist5817[] = { 1, 2, 1,12};                                                 // +=+++==-
                                                                                               // +=+++===
                                                                                               // +=+++==+
static const char vlist5820[] = { 1, 2, 8,12};                                                 // +=+++=+-
                                                                                               // +=+++=+=
                                                                                               // +=+++=++
static const char vlist5823[] = { 2,12,10,13, 9,10,12};                                        // +=++++--
static const char vlist5824[] = { 1, 0,10,13};                                                 // +=++++-=
static const char vlist5825[] = { 1, 8,10,13};                                                 // +=++++-+
static const char vlist5826[] = { 1, 9, 1,12};                                                 // +=++++=-
                                                                                               // +=++++==
                                                                                               // +=++++=+
static const char vlist5829[] = { 1, 9, 8,12};                                                 // +=+++++-
                                                                                               // +=+++++=
                                                                                               // +=++++++
static const char vlist5832[] = { 2,14,15,17,15,18,17};                                        // ++------
static const char vlist5833[] = { 3,18,17, 0,18, 0,15,15, 0,14};                               // ++-----=
static const char vlist5834[] = { 5,18,17,12,18, 8,15,18,12, 8,15, 8, 9,14,15, 9};             // ++-----+
static const char vlist5835[] = { 3, 1,17,14,18,17, 1,15, 1,14};                               // ++----=-
static const char vlist5836[] = { 4,17, 1,18, 1,17, 0, 1, 0,14,15, 1,14};                      // ++----==
static const char vlist5837[] = { 4,12, 1,18,15, 1, 9,17,12,18,14,15, 9};                      // ++----=+
static const char vlist5838[] = { 5, 8,17,14,13,17, 8,18,17,13, 8,14,10,14,15,10};             // ++----+-
static const char vlist5839[] = { 4,13,17, 0,17,13,18, 0,14,10,14,15,10};                      // ++----+=
static const char vlist5840[] = { 4,17,13,18,12,13,17,15,10,14,14,10, 9};                      // ++----++
static const char vlist5841[] = { 2, 2,18,17, 2,15,18};                                        // ++---=--
static const char vlist5842[] = { 3, 0, 2,15, 0,15,18,17, 0,18};                               // ++---=-=
static const char vlist5843[] = { 4, 8, 2,15,12, 8,18, 8,15,18,17,12,18};                      // ++---=-+
static const char vlist5844[] = { 3, 1, 2,15,17, 2, 1,17, 1,18};                               // ++---==-
static const char vlist5845[] = { 3, 1, 2,15, 1,18,17, 0, 1,17};                               // ++---===
static const char vlist5846[] = { 3, 1, 2,15, 1,18,12,12,18,17};                               // ++---==+
static const char vlist5847[] = { 4,10, 2,15,17, 2, 8,17, 8,13,17,13,18};                      // ++---=+-
static const char vlist5848[] = { 3, 0,13,17,13,18,17,10, 2,15};                               // ++---=+=
static const char vlist5849[] = { 3,13,17,12,13,18,17,10, 2,15};                               // ++---=++
static const char vlist5850[] = { 3,11,15, 9,15,18, 9,18,17, 9};                               // ++---+--
static const char vlist5851[] = { 3, 0,11,15, 0,15,18,17, 0,18};                               // ++---+-=
static const char vlist5852[] = { 4,12, 8,18, 8,15,18, 8,11,15,17,12,18};                      // ++---+-+
static const char vlist5853[] = { 3,17, 9, 1,17, 1,18, 1,11,15};                               // ++---+=-
static const char vlist5854[] = { 3, 1,11,15, 1,18,17, 0, 1,17};                               // ++---+==
static const char vlist5855[] = { 3, 1,11,15, 1,18,12,12,18,17};                               // ++---+=+
static const char vlist5856[] = { 4,17, 9, 8,17,13,18,17, 8,13,10,11,15};                      // ++---++-
static const char vlist5857[] = { 3, 0,13,17,13,18,17,10,11,15};                               // ++---++=
static const char vlist5858[] = { 3,13,17,12,13,18,17,10,11,15};                               // ++---+++
static const char vlist5859[] = { 2,14, 3,17, 3,18,17};                                        // ++--=---
static const char vlist5860[] = { 3, 0, 3,18,14, 3, 0,17, 0,18};                               // ++--=--=
static const char vlist5861[] = { 4, 8, 3,18,14, 3, 9,12, 8,18,17,12,18};                      // ++--=--+
static const char vlist5862[] = { 3,14, 3, 1,17,14, 1,17, 1,18};                               // ++--=-=-
static const char vlist5863[] = { 3, 1,18,17, 0, 1,17, 3, 0,14};                               // ++--=-==
static const char vlist5864[] = { 3, 1,18,12,12,18,17, 3, 9,14};                               // ++--=-=+
static const char vlist5865[] = { 4,14, 3, 8,17,14, 8,17,13,18,17, 8,13};                      // ++--=-+-
static const char vlist5866[] = { 3, 0,13,17,13,18,17, 3, 0,14};                               // ++--=-+=
static const char vlist5867[] = { 3,13,17,12,13,18,17, 3, 9,14};                               // ++--=-++
static const char vlist5868[] = { 2, 3,18,17, 2, 3,17};                                        // ++--==--
static const char vlist5869[] = { 2, 0,18,17, 0, 3,18};                                        // ++--==-=
static const char vlist5870[] = { 3, 8, 3,18,12, 8,18,12,18,17};                               // ++--==-+
static const char vlist5871[] = { 2, 2, 1,17, 1,18,17};                                        // ++--===-
static const char vlist5872[] = { 2, 1,18,17, 0, 1,17};                                        // ++--====
static const char vlist5873[] = { 2, 1,18,12,12,18,17};                                        // ++--===+
static const char vlist5874[] = { 3, 2, 8,17, 8,13,17,13,18,17};                               // ++--==+-
static const char vlist5875[] = { 2, 0,13,17,13,18,17};                                        // ++--==+=
static const char vlist5876[] = { 2,13,17,12,13,18,17};                                        // ++--==++
static const char vlist5877[] = { 2, 3,18, 9,18,17, 9};                                        // ++--=+--
static const char vlist5878[] = { 2, 0,18,17, 0, 3,18};                                        // ++--=+-=
static const char vlist5879[] = { 3, 8, 3,18,12, 8,18,12,18,17};                               // ++--=+-+
static const char vlist5880[] = { 2, 9, 1,17, 1,18,17};                                        // ++--=+=-
static const char vlist5881[] = { 2, 1,18,17, 0, 1,17};                                        // ++--=+==
static const char vlist5882[] = { 2, 1,18,12,12,18,17};                                        // ++--=+=+
static const char vlist5883[] = { 3, 8,13,17, 9, 8,17,13,18,17};                               // ++--=++-
static const char vlist5884[] = { 2, 0,13,17,13,18,17};                                        // ++--=++=
static const char vlist5885[] = { 2,13,17,12,13,18,17};                                        // ++--=+++
static const char vlist5886[] = { 3,14,11,10,17,14,10,18,17,10};                               // ++--+---
static const char vlist5887[] = { 3,14,11, 0,17, 0,18, 0,10,18};                               // ++--+--=
static const char vlist5888[] = { 4,14,11, 9,12, 8,18, 8,10,18,17,12,18};                      // ++--+--+
static const char vlist5889[] = { 3,14,11, 1,17,14, 1,17, 1,18};                               // ++--+-=-
static const char vlist5890[] = { 3, 1,18,17, 0, 1,17,11, 0,14};                               // ++--+-==
static const char vlist5891[] = { 3, 1,18,12,12,18,17,11, 9,14};                               // ++--+-=+
static const char vlist5892[] = { 4,14,11, 8,17,14, 8,17, 8,13,17,13,18};                      // ++--+-+-
static const char vlist5893[] = { 3, 0,13,17,13,18,17,11, 0,14};                               // ++--+-+=
static const char vlist5894[] = { 3,13,17,12,13,18,17,11, 9,14};                               // ++--+-++
static const char vlist5895[] = { 2,17, 2,10,18,17,10};                                        // ++--+=--
static const char vlist5896[] = { 2, 0,18,17, 0,10,18};                                        // ++--+=-=
static const char vlist5897[] = { 3, 8,10,18,12, 8,18,12,18,17};                               // ++--+=-+
static const char vlist5898[] = { 2, 2, 1,17, 1,18,17};                                        // ++--+==-
static const char vlist5899[] = { 2, 1,18,17, 0, 1,17};                                        // ++--+===
static const char vlist5900[] = { 2, 1,18,12,12,18,17};                                        // ++--+==+
static const char vlist5901[] = { 3, 2, 8,17, 8,13,17,13,18,17};                               // ++--+=+-
static const char vlist5902[] = { 2, 0,13,17,13,18,17};                                        // ++--+=+=
static const char vlist5903[] = { 2,13,17,12,13,18,17};                                        // ++--+=++
static const char vlist5904[] = { 2,10,18,17,10,17, 9};                                        // ++--++--
static const char vlist5905[] = { 2, 0,18,17, 0,10,18};                                        // ++--++-=
static const char vlist5906[] = { 3, 8,10,18,12, 8,18,12,18,17};                               // ++--++-+
static const char vlist5907[] = { 2, 9, 1,17, 1,18,17};                                        // ++--++=-
static const char vlist5908[] = { 2, 1,18,17, 0, 1,17};                                        // ++--++==
static const char vlist5909[] = { 2, 1,18,12,12,18,17};                                        // ++--++=+
static const char vlist5910[] = { 3, 8,13,17, 9, 8,17,13,18,17};                               // ++--+++-
static const char vlist5911[] = { 2, 0,13,17,13,18,17};                                        // ++--+++=
static const char vlist5912[] = { 2,13,17,12,13,18,17};                                        // ++--++++
static const char vlist5913[] = { 2,15, 4,14,18, 4,15};                                        // ++-=----
static const char vlist5914[] = { 3, 4, 0,18, 0,15,18, 0,14,15};                               // ++-=---=
static const char vlist5915[] = { 4, 4, 8,18, 8, 9,15, 8,15,18, 9,14,15};                      // ++-=---+
static const char vlist5916[] = { 3, 1, 4,18, 4,14, 1, 1,14,15};                               // ++-=--=-
static const char vlist5917[] = { 3, 4, 1,18,15, 1,14, 1, 0,14};                               // ++-=--==
static const char vlist5918[] = { 3, 4, 1,18,15, 1, 9,15, 9,14};                               // ++-=--=+
static const char vlist5919[] = { 4, 4,13,18, 4,14, 8, 8,14,10,10,14,15};                      // ++-=--+-
static const char vlist5920[] = { 3,10, 0,14,15,10,14, 4,13,18};                               // ++-=--+=
static const char vlist5921[] = { 3,14,10, 9,15,10,14, 4,13,18};                               // ++-=--++
static const char vlist5922[] = { 2, 4,15,18, 4, 2,15};                                        // ++-=-=--
static const char vlist5923[] = { 3, 0, 2,15, 0,15,18, 4, 0,18};                               // ++-=-=-=
static const char vlist5924[] = { 3, 8, 2,15, 4, 8,18, 8,15,18};                               // ++-=-=-+
static const char vlist5925[] = { 3, 4, 2, 1, 1, 2,15, 4, 1,18};                               // ++-=-==-
static const char vlist5926[] = { 2, 1, 2,15, 1,18, 4};                                        // ++-=-===
static const char vlist5927[] = { 2, 1, 2,15, 1,18, 4};                                        // ++-=-==+
static const char vlist5928[] = { 3, 4, 2, 8,10, 2,15, 4,13,18};                               // ++-=-=+-
static const char vlist5929[] = { 2,10, 2,15,13,18, 4};                                        // ++-=-=+=
static const char vlist5930[] = { 2,10, 2,15,13,18, 4};                                        // ++-=-=++
static const char vlist5931[] = { 3, 4, 9,18, 9,11,15,18, 9,15};                               // ++-=-+--
static const char vlist5932[] = { 3, 0,11,15, 0,15,18, 4, 0,18};                               // ++-=-+-=
static const char vlist5933[] = { 3, 4, 8,18, 8,15,18, 8,11,15};                               // ++-=-+-+
static const char vlist5934[] = { 3, 4, 9, 1, 4, 1,18, 1,11,15};                               // ++-=-+=-
static const char vlist5935[] = { 2, 1,11,15, 1,18, 4};                                        // ++-=-+==
static const char vlist5936[] = { 2, 1,11,15, 1,18, 4};                                        // ++-=-+=+
static const char vlist5937[] = { 3, 4, 9, 8,10,11,15, 4,13,18};                               // ++-=-++-
static const char vlist5938[] = { 2,10,11,15,13,18, 4};                                        // ++-=-++=
static const char vlist5939[] = { 2,10,11,15,13,18, 4};                                        // ++-=-+++
static const char vlist5940[] = { 2, 4, 3,18, 4,14, 3};                                        // ++-==---
static const char vlist5941[] = { 3, 3,18, 0, 3, 0,14,18, 4, 0};                               // ++-==--=
static const char vlist5942[] = { 3, 8, 3,18, 3, 9,14, 4, 8,18};                               // ++-==--+
static const char vlist5943[] = { 3,14, 3, 1, 4,14, 1, 4, 1,18};                               // ++-==-=-
static const char vlist5944[] = { 2, 1,18, 4, 3, 0,14};                                        // ++-==-==
static const char vlist5945[] = { 2, 1,18, 4, 3, 9,14};                                        // ++-==-=+
static const char vlist5946[] = { 3, 3, 8,14, 8, 4,14, 4,13,18};                               // ++-==-+-
static const char vlist5947[] = { 2, 3, 0,14,13,18, 4};                                        // ++-==-+=
static const char vlist5948[] = { 2, 3, 9,14,13,18, 4};                                        // ++-==-++
static const char vlist5949[] = { 2, 3, 4, 2, 4, 3,18};                                        // ++-===--
static const char vlist5950[] = { 2, 0,18, 4, 0, 3,18};                                        // ++-===-=
static const char vlist5951[] = { 2, 3,18, 8, 8,18, 4};                                        // ++-===-+
static const char vlist5952[] = { 2, 2, 1, 4, 1,18, 4};                                        // ++-====-
static const char vlist5953[] = { 1, 1,18, 4};                                                 // ++-=====
static const char vlist5954[] = { 1, 1,18, 4};                                                 // ++-====+
static const char vlist5955[] = { 2, 2, 8, 4,13,18, 4};                                        // ++-===+-
static const char vlist5956[] = { 1,13,18, 4};                                                 // ++-===+=
static const char vlist5957[] = { 1,13,18, 4};                                                 // ++-===++
static const char vlist5958[] = { 2, 3, 4, 9, 4, 3,18};                                        // ++-==+--
static const char vlist5959[] = { 2, 0,18, 4, 0, 3,18};                                        // ++-==+-=
static const char vlist5960[] = { 2, 3,18, 8, 8,18, 4};                                        // ++-==+-+
static const char vlist5961[] = { 2, 9, 1, 4, 1,18, 4};                                        // ++-==+=-
static const char vlist5962[] = { 1, 1,18, 4};                                                 // ++-==+==
static const char vlist5963[] = { 1, 1,18, 4};                                                 // ++-==+=+
static const char vlist5964[] = { 2, 9, 8, 4,13,18, 4};                                        // ++-==++-
static const char vlist5965[] = { 1,13,18, 4};                                                 // ++-==++=
static const char vlist5966[] = { 1,13,18, 4};                                                 // ++-==+++
static const char vlist5967[] = { 3, 4,10,18, 4,11,10, 4,14,11};                               // ++-=+---
static const char vlist5968[] = { 3, 4,10,18,10, 4, 0,11, 0,14};                               // ++-=+--=
static const char vlist5969[] = { 3, 8,10,18, 4, 8,18,11, 9,14};                               // ++-=+--+
static const char vlist5970[] = { 3,11, 4,14, 4,11, 1, 4, 1,18};                               // ++-=+-=-
static const char vlist5971[] = { 2, 1,18, 4,11, 0,14};                                        // ++-=+-==
static const char vlist5972[] = { 2, 1,18, 4,11, 9,14};                                        // ++-=+-=+
static const char vlist5973[] = { 3,11, 8,14, 8, 4,14, 4,13,18};                               // ++-=+-+-
static const char vlist5974[] = { 2,11, 0,14,13,18, 4};                                        // ++-=+-+=
static const char vlist5975[] = { 2,11, 9,14,13,18, 4};                                        // ++-=+-++
static const char vlist5976[] = { 2, 4,10,18,10, 4, 2};                                        // ++-=+=--
static const char vlist5977[] = { 2, 0,10, 4,10,18, 4};                                        // ++-=+=-=
static const char vlist5978[] = { 2,10,18, 8, 8,18, 4};                                        // ++-=+=-+
static const char vlist5979[] = { 2, 2, 1, 4, 1,18, 4};                                        // ++-=+==-
static const char vlist5980[] = { 1, 1,18, 4};                                                 // ++-=+===
static const char vlist5981[] = { 1, 1,18, 4};                                                 // ++-=+==+
static const char vlist5982[] = { 2, 2, 8, 4,13,18, 4};                                        // ++-=+=+-
static const char vlist5983[] = { 1,13,18, 4};                                                 // ++-=+=+=
static const char vlist5984[] = { 1,13,18, 4};                                                 // ++-=+=++
static const char vlist5985[] = { 2, 4,10,18,10, 4, 9};                                        // ++-=++--
static const char vlist5986[] = { 2, 0,10, 4,10,18, 4};                                        // ++-=++-=
static const char vlist5987[] = { 2,10,18, 8, 8,18, 4};                                        // ++-=++-+
static const char vlist5988[] = { 2, 9, 1, 4, 1,18, 4};                                        // ++-=++=-
static const char vlist5989[] = { 1, 1,18, 4};                                                 // ++-=++==
static const char vlist5990[] = { 1, 1,18, 4};                                                 // ++-=++=+
static const char vlist5991[] = { 2, 9, 8, 4,13,18, 4};                                        // ++-=+++-
static const char vlist5992[] = { 1,13,18, 4};                                                 // ++-=+++=
static const char vlist5993[] = { 1,13,18, 4};                                                 // ++-=++++
static const char vlist5994[] = { 3,14,15,12,15,16,12,15,18,16};                               // ++-+----
static const char vlist5995[] = { 3, 0,18,16, 0,15,18, 0,14,15};                               // ++-+---=
static const char vlist5996[] = { 4, 8, 9,15, 8,15,18,16, 8,18, 9,14,15};                      // ++-+---+
static const char vlist5997[] = { 3,12,14, 1, 1,14,15,16, 1,18};                               // ++-+--=-
static const char vlist5998[] = { 3,16, 1,18,15, 1,14, 1, 0,14};                               // ++-+--==
static const char vlist5999[] = { 3,16, 1,18,15, 1, 9,15, 9,14};                               // ++-+--=+
static const char vlist6000[] = { 4,12,14, 8,10,14,15, 8,14,10,16,13,18};                      // ++-+--+-
static const char vlist6001[] = { 3,10, 0,14,15,10,14,16,13,18};                               // ++-+--+=
static const char vlist6002[] = { 3,14,10, 9,15,10,14,16,13,18};                               // ++-+--++
static const char vlist6003[] = { 3,12, 2,15,12,15,16,16,15,18};                               // ++-+-=--
static const char vlist6004[] = { 3, 0, 2,15, 0,15,18, 0,18,16};                               // ++-+-=-=
static const char vlist6005[] = { 3, 8, 2,15,16, 8,18, 8,15,18};                               // ++-+-=-+
static const char vlist6006[] = { 3, 1, 2,15,12, 2, 1,16, 1,18};                               // ++-+-==-
static const char vlist6007[] = { 2, 1, 2,15, 1,18,16};                                        // ++-+-===
static const char vlist6008[] = { 2, 1, 2,15, 1,18,16};                                        // ++-+-==+
static const char vlist6009[] = { 3,10, 2,15,12, 2, 8,16,13,18};                               // ++-+-=+-
static const char vlist6010[] = { 2,10, 2,15,13,18,16};                                        // ++-+-=+=
static const char vlist6011[] = { 2,10, 2,15,13,18,16};                                        // ++-+-=++
static const char vlist6012[] = { 4, 9,11,12,12,11,15,12,15,16,16,15,18};                      // ++-+-+--
static const char vlist6013[] = { 3, 0,11,15, 0,15,18, 0,18,16};                               // ++-+-+-=
static const char vlist6014[] = { 3,16, 8,18, 8,15,18, 8,11,15};                               // ++-+-+-+
static const char vlist6015[] = { 3,12, 9, 1,16, 1,18, 1,11,15};                               // ++-+-+=-
static const char vlist6016[] = { 2, 1,11,15, 1,18,16};                                        // ++-+-+==
static const char vlist6017[] = { 2, 1,11,15, 1,18,16};                                        // ++-+-+=+
static const char vlist6018[] = { 3,12, 9, 8,10,11,15,16,13,18};                               // ++-+-++-
static const char vlist6019[] = { 2,10,11,15,13,18,16};                                        // ++-+-++=
static const char vlist6020[] = { 2,10,11,15,13,18,16};                                        // ++-+-+++
static const char vlist6021[] = { 3,16,12, 3,16, 3,18,12,14, 3};                               // ++-+=---
static const char vlist6022[] = { 3,16, 3,18, 3,16, 0, 3, 0,14};                               // ++-+=--=
static const char vlist6023[] = { 3, 8, 3,18, 3, 9,14,16, 8,18};                               // ++-+=--+
static const char vlist6024[] = { 3, 3,12,14,12, 3, 1,16, 1,18};                               // ++-+=-=-
static const char vlist6025[] = { 2, 1,18,16, 3, 0,14};                                        // ++-+=-==
static const char vlist6026[] = { 2, 1,18,16, 3, 9,14};                                        // ++-+=-=+
static const char vlist6027[] = { 3, 3, 8,14, 8,12,14,16,13,18};                               // ++-+=-+-
static const char vlist6028[] = { 2, 3, 0,14,13,18,16};                                        // ++-+=-+=
static const char vlist6029[] = { 2, 3, 9,14,13,18,16};                                        // ++-+=-++
static const char vlist6030[] = { 3,16, 3,18, 3,16,12, 3,12, 2};                               // ++-+==--
static const char vlist6031[] = { 2, 0, 3,16, 3,18,16};                                        // ++-+==-=
static const char vlist6032[] = { 2, 3,18, 8, 8,18,16};                                        // ++-+==-+
static const char vlist6033[] = { 2, 2, 1,12, 1,18,16};                                        // ++-+===-
static const char vlist6034[] = { 1, 1,18,16};                                                 // ++-+====
static const char vlist6035[] = { 1, 1,18,16};                                                 // ++-+===+
static const char vlist6036[] = { 2, 2, 8,12,13,18,16};                                        // ++-+==+-
static const char vlist6037[] = { 1,13,18,16};                                                 // ++-+==+=
static const char vlist6038[] = { 1,13,18,16};                                                 // ++-+==++
static const char vlist6039[] = { 3,16, 3,18, 3,16,12, 3,12, 9};                               // ++-+=+--
static const char vlist6040[] = { 2, 0, 3,16, 3,18,16};                                        // ++-+=+-=
static const char vlist6041[] = { 2, 3,18, 8, 8,18,16};                                        // ++-+=+-+
static const char vlist6042[] = { 2, 9, 1,12, 1,18,16};                                        // ++-+=+=-
static const char vlist6043[] = { 1, 1,18,16};                                                 // ++-+=+==
static const char vlist6044[] = { 1, 1,18,16};                                                 // ++-+=+=+
static const char vlist6045[] = { 2, 9, 8,12,13,18,16};                                        // ++-+=++-
static const char vlist6046[] = { 1,13,18,16};                                                 // ++-+=++=
static const char vlist6047[] = { 1,13,18,16};                                                 // ++-+=+++
static const char vlist6048[] = { 4,16,12,10,16,10,18,12,14,11,12,11,10};                      // ++-++---
static const char vlist6049[] = { 3,10,16, 0,16,10,18,11, 0,14};                               // ++-++--=
static const char vlist6050[] = { 3,16,10,18, 8,10,16,11, 9,14};                               // ++-++--+
static const char vlist6051[] = { 3,12,11, 1,11,12,14,16, 1,18};                               // ++-++-=-
static const char vlist6052[] = { 2, 1,18,16,11, 0,14};                                        // ++-++-==
static const char vlist6053[] = { 2, 1,18,16,11, 9,14};                                        // ++-++-=+
static const char vlist6054[] = { 3,11,12,14,11, 8,12,16,13,18};                               // ++-++-+-
static const char vlist6055[] = { 2,11, 0,14,13,18,16};                                        // ++-++-+=
static const char vlist6056[] = { 2,11, 9,14,13,18,16};                                        // ++-++-++
static const char vlist6057[] = { 3,10,16,12,16,10,18,10,12, 2};                               // ++-++=--
static const char vlist6058[] = { 2, 0,10,16,10,18,16};                                        // ++-++=-=
static const char vlist6059[] = { 2,10,18,16,10,16, 8};                                        // ++-++=-+
static const char vlist6060[] = { 2, 2, 1,12, 1,18,16};                                        // ++-++==-
static const char vlist6061[] = { 1, 1,18,16};                                                 // ++-++===
static const char vlist6062[] = { 1, 1,18,16};                                                 // ++-++==+
static const char vlist6063[] = { 2, 2, 8,12,13,18,16};                                        // ++-++=+-
static const char vlist6064[] = { 1,13,18,16};                                                 // ++-++=+=
static const char vlist6065[] = { 1,13,18,16};                                                 // ++-++=++
static const char vlist6066[] = { 3,10,16,12,16,10,18,10,12, 9};                               // ++-+++--
static const char vlist6067[] = { 2, 0,10,16,10,18,16};                                        // ++-+++-=
static const char vlist6068[] = { 2,10,18,16,10,16, 8};                                        // ++-+++-+
static const char vlist6069[] = { 2, 9, 1,12, 1,18,16};                                        // ++-+++=-
static const char vlist6070[] = { 1, 1,18,16};                                                 // ++-+++==
static const char vlist6071[] = { 1, 1,18,16};                                                 // ++-+++=+
static const char vlist6072[] = { 2, 9, 8,12,13,18,16};                                        // ++-++++-
static const char vlist6073[] = { 1,13,18,16};                                                 // ++-++++=
static const char vlist6074[] = { 1,13,18,16};                                                 // ++-+++++
static const char vlist6075[] = { 2, 5,17,14,15, 5,14};                                        // ++=-----
static const char vlist6076[] = { 3, 5, 0,15,17, 5, 0, 0,14,15};                               // ++=----=
static const char vlist6077[] = { 4, 5, 8,15, 5,17,12, 8, 9,15, 9,14,15};                      // ++=----+
static const char vlist6078[] = { 3, 5,17, 1, 1,17,14, 1,14,15};                               // ++=---=-
static const char vlist6079[] = { 3,15, 1,14, 1, 0,14, 0, 5,17};                               // ++=---==
static const char vlist6080[] = { 3,15, 1, 9,15, 9,14,12, 5,17};                               // ++=---=+
static const char vlist6081[] = { 4, 5,17, 8, 8,17,14, 8,14,10,10,14,15};                      // ++=---+-
static const char vlist6082[] = { 3,10, 0,14,15,10,14, 0, 5,17};                               // ++=---+=
static const char vlist6083[] = { 3,14,10, 9,15,10,14,12, 5,17};                               // ++=---++
static const char vlist6084[] = { 2, 5, 2,15, 5,17, 2};                                        // ++=--=--
static const char vlist6085[] = { 3, 2,15, 0,15, 5, 0, 0, 5,17};                               // ++=--=-=
static const char vlist6086[] = { 3, 8, 2,15, 5, 8,15,12, 5,17};                               // ++=--=-+
static const char vlist6087[] = { 3,17, 2, 1, 1, 2,15, 5,17, 1};                               // ++=--==-
static const char vlist6088[] = { 2, 1, 2,15,17, 0, 5};                                        // ++=--===
static const char vlist6089[] = { 2, 1, 2,15,17,12, 5};                                        // ++=--==+
static const char vlist6090[] = { 3, 2, 8,17,10, 2,15, 8, 5,17};                               // ++=--=+-
static const char vlist6091[] = { 2,17, 0, 5,10, 2,15};                                        // ++=--=+=
static const char vlist6092[] = { 2,17,12, 5,10, 2,15};                                        // ++=--=++
static const char vlist6093[] = { 3, 5,11,15, 5, 9,11, 5,17, 9};                               // ++=--+--
static const char vlist6094[] = { 3, 5,11,15,11, 5, 0, 0, 5,17};                               // ++=--+-=
static const char vlist6095[] = { 3, 8,11,15, 5, 8,15,12, 5,17};                               // ++=--+-+
static const char vlist6096[] = { 3, 9, 5,17, 5, 9, 1, 1,11,15};                               // ++=--+=-
static const char vlist6097[] = { 2, 1,11,15,17, 0, 5};                                        // ++=--+==
static const char vlist6098[] = { 2, 1,11,15,17,12, 5};                                        // ++=--+=+
static const char vlist6099[] = { 3, 9, 8,17, 8, 5,17,10,11,15};                               // ++=--++-
static const char vlist6100[] = { 2,17, 0, 5,10,11,15};                                        // ++=--++=
static const char vlist6101[] = { 2,17,12, 5,10,11,15};                                        // ++=--+++
static const char vlist6102[] = { 2, 5,14, 3, 5,17,14};                                        // ++=-=---
static const char vlist6103[] = { 3, 3, 5, 0, 3, 0,14, 0, 5,17};                               // ++=-=--=
static const char vlist6104[] = { 3, 3, 5, 8, 3, 9,14,12, 5,17};                               // ++=-=--+
static const char vlist6105[] = { 3, 3, 1,14,14, 1,17, 1, 5,17};                               // ++=-=-=-
static const char vlist6106[] = { 2,17, 0, 5, 3, 0,14};                                        // ++=-=-==
static const char vlist6107[] = { 2, 3, 9,14,17,12, 5};                                        // ++=-=-=+
static const char vlist6108[] = { 3, 3, 8,14, 8, 5,17,14, 8,17};                               // ++=-=-+-
static const char vlist6109[] = { 2,17, 0, 5, 3, 0,14};                                        // ++=-=-+=
static const char vlist6110[] = { 2, 3, 9,14,17,12, 5};                                        // ++=-=-++
static const char vlist6111[] = { 2, 3, 5, 2, 2, 5,17};                                        // ++=-==--
static const char vlist6112[] = { 2, 0, 3, 5,17, 0, 5};                                        // ++=-==-=
static const char vlist6113[] = { 2, 8, 3, 5,17,12, 5};                                        // ++=-==-+
static const char vlist6114[] = { 2, 2, 1,17,17, 1, 5};                                        // ++=-===-
static const char vlist6115[] = { 1,17, 0, 5};                                                 // ++=-====
static const char vlist6116[] = { 1,17,12, 5};                                                 // ++=-===+
static const char vlist6117[] = { 2,17, 2, 8,17, 8, 5};                                        // ++=-==+-
static const char vlist6118[] = { 1,17, 0, 5};                                                 // ++=-==+=
static const char vlist6119[] = { 1,17,12, 5};                                                 // ++=-==++
static const char vlist6120[] = { 2, 3, 5, 9, 9, 5,17};                                        // ++=-=+--
static const char vlist6121[] = { 2, 0, 3, 5,17, 0, 5};                                        // ++=-=+-=
static const char vlist6122[] = { 2, 8, 3, 5,17,12, 5};                                        // ++=-=+-+
static const char vlist6123[] = { 2, 9, 1, 5,17, 9, 5};                                        // ++=-=+=-
static const char vlist6124[] = { 1,17, 0, 5};                                                 // ++=-=+==
static const char vlist6125[] = { 1,17,12, 5};                                                 // ++=-=+=+
static const char vlist6126[] = { 2,17, 8, 5,17, 9, 8};                                        // ++=-=++-
static const char vlist6127[] = { 1,17, 0, 5};                                                 // ++=-=++=
static const char vlist6128[] = { 1,17,12, 5};                                                 // ++=-=+++
static const char vlist6129[] = { 3, 5,17,10,17,14,10,10,14,11};                               // ++=-+---
static const char vlist6130[] = { 3,10, 5, 0, 0, 5,17,11, 0,14};                               // ++=-+--=
static const char vlist6131[] = { 3,10, 5, 8,11, 9,14,12, 5,17};                               // ++=-+--+
static const char vlist6132[] = { 3,11, 1,14,14, 1,17, 1, 5,17};                               // ++=-+-=-
static const char vlist6133[] = { 2,17, 0, 5,11, 0,14};                                        // ++=-+-==
static const char vlist6134[] = { 2,11, 9,14,17,12, 5};                                        // ++=-+-=+
static const char vlist6135[] = { 3, 8, 5,17,14, 8,17,11, 8,14};                               // ++=-+-+-
static const char vlist6136[] = { 2,17, 0, 5,11, 0,14};                                        // ++=-+-+=
static const char vlist6137[] = { 2,11, 9,14,17,12, 5};                                        // ++=-+-++
static const char vlist6138[] = { 2,10, 5, 2, 2, 5,17};                                        // ++=-+=--
static const char vlist6139[] = { 2, 0,10, 5,17, 0, 5};                                        // ++=-+=-=
static const char vlist6140[] = { 2,17,12, 5, 8,10, 5};                                        // ++=-+=-+
static const char vlist6141[] = { 2, 2, 1,17,17, 1, 5};                                        // ++=-+==-
static const char vlist6142[] = { 1,17, 0, 5};                                                 // ++=-+===
static const char vlist6143[] = { 1,17,12, 5};                                                 // ++=-+==+
static const char vlist6144[] = { 2,17, 2, 8,17, 8, 5};                                        // ++=-+=+-
static const char vlist6145[] = { 1,17, 0, 5};                                                 // ++=-+=+=
static const char vlist6146[] = { 1,17,12, 5};                                                 // ++=-+=++
static const char vlist6147[] = { 2,10, 5, 9, 9, 5,17};                                        // ++=-++--
static const char vlist6148[] = { 2, 0,10, 5,17, 0, 5};                                        // ++=-++-=
static const char vlist6149[] = { 2,17,12, 5, 8,10, 5};                                        // ++=-++-+
static const char vlist6150[] = { 2, 9, 1, 5,17, 9, 5};                                        // ++=-++=-
static const char vlist6151[] = { 1,17, 0, 5};                                                 // ++=-++==
static const char vlist6152[] = { 1,17,12, 5};                                                 // ++=-++=+
static const char vlist6153[] = { 2,17, 8, 5,17, 9, 8};                                        // ++=-+++-
static const char vlist6154[] = { 1,17, 0, 5};                                                 // ++=-+++=
static const char vlist6155[] = { 1,17,12, 5};                                                 // ++=-++++
static const char vlist6156[] = { 2,15, 5,14, 5, 4,14};                                        // ++==----
static const char vlist6157[] = { 2,15, 0,14, 5, 0,15};                                        // ++==---=
static const char vlist6158[] = { 3, 5, 8,15, 8, 9,15,15, 9,14};                               // ++==---+
static const char vlist6159[] = { 2, 1, 4,14,15, 1,14};                                        // ++==--=-
static const char vlist6160[] = { 2,15, 1,14, 1, 0,14};                                        // ++==--==
static const char vlist6161[] = { 2,15, 1, 9,15, 9,14};                                        // ++==--=+
static const char vlist6162[] = { 3, 8, 4,14,10, 8,14,15,10,14};                               // ++==--+-
static const char vlist6163[] = { 2,10, 0,14,15,10,14};                                        // ++==--+=
static const char vlist6164[] = { 2,14,10, 9,15,10,14};                                        // ++==--++
static const char vlist6165[] = { 2, 5, 4, 2, 5, 2,15};                                        // ++==-=--
static const char vlist6166[] = { 2, 0, 2,15, 0,15, 5};                                        // ++==-=-=
static const char vlist6167[] = { 2, 2,15, 8, 8,15, 5};                                        // ++==-=-+
static const char vlist6168[] = { 2, 4, 2, 1, 1, 2,15};                                        // ++==-==-
static const char vlist6169[] = { 1, 1, 2,15};                                                 // ++==-===
static const char vlist6170[] = { 1, 1, 2,15};                                                 // ++==-==+
static const char vlist6171[] = { 2, 4, 2, 8,10, 2,15};                                        // ++==-=+-
static const char vlist6172[] = { 1,10, 2,15};                                                 // ++==-=+=
static const char vlist6173[] = { 1,10, 2,15};                                                 // ++==-=++
static const char vlist6174[] = { 3, 5, 4, 9, 5, 9,11, 5,11,15};                               // ++==-+--
static const char vlist6175[] = { 2, 0,11, 5,11,15, 5};                                        // ++==-+-=
static const char vlist6176[] = { 2, 8,15, 5,11,15, 8};                                        // ++==-+-+
static const char vlist6177[] = { 2, 4, 9, 1, 1,11,15};                                        // ++==-+=-
static const char vlist6178[] = { 1, 1,11,15};                                                 // ++==-+==
static const char vlist6179[] = { 1, 1,11,15};                                                 // ++==-+=+
static const char vlist6180[] = { 2, 4, 9, 8,10,11,15};                                        // ++==-++-
static const char vlist6181[] = { 1,10,11,15};                                                 // ++==-++=
static const char vlist6182[] = { 1,10,11,15};                                                 // ++==-+++
static const char vlist6183[] = { 2, 5, 4, 3, 3, 4,14};                                        // ++===---
static const char vlist6184[] = { 2,14, 3, 0, 0, 3, 5};                                        // ++===--=
static const char vlist6185[] = { 2,14, 3, 9, 8, 3, 5};                                        // ++===--+
static const char vlist6186[] = { 2,14, 3, 1, 4,14, 1};                                        // ++===-=-
static const char vlist6187[] = { 1, 3, 0,14};                                                 // ++===-==
static const char vlist6188[] = { 1, 3, 9,14};                                                 // ++===-=+
static const char vlist6189[] = { 2, 3, 8,14, 8, 4,14};                                        // ++===-+-
static const char vlist6190[] = { 1, 3, 0,14};                                                 // ++===-+=
static const char vlist6191[] = { 1, 3, 9,14};                                                 // ++===-++
static const char vlist6192[] = { 2, 3, 4, 2, 4, 3, 5};                                        // ++====--
static const char vlist6193[] = { 1, 0, 3, 5};                                                 // ++====-=
static const char vlist6194[] = { 1, 8, 3, 5};                                                 // ++====-+
static const char vlist6195[] = { 1, 2, 1, 4};                                                 // ++=====-
                                                                                               // ++======
                                                                                               // ++=====+
static const char vlist6198[] = { 1, 2, 8, 4};                                                 // ++====+-
                                                                                               // ++====+=
                                                                                               // ++====++
static const char vlist6201[] = { 2, 9, 3, 5, 4, 9, 5};                                        // ++===+--
static const char vlist6202[] = { 1, 0, 3, 5};                                                 // ++===+-=
static const char vlist6203[] = { 1, 8, 3, 5};                                                 // ++===+-+
static const char vlist6204[] = { 1, 9, 1, 4};                                                 // ++===+=-
                                                                                               // ++===+==
                                                                                               // ++===+=+
static const char vlist6207[] = { 1, 9, 8, 4};                                                 // ++===++-
                                                                                               // ++===++=
                                                                                               // ++===+++
static const char vlist6210[] = { 3, 5, 4,10,11, 4,14,10, 4,11};                               // ++==+---
static const char vlist6211[] = { 2,14,11, 0, 0,10, 5};                                        // ++==+--=
static const char vlist6212[] = { 2,14,11, 9, 8,10, 5};                                        // ++==+--+
static const char vlist6213[] = { 2,11, 4,14, 4,11, 1};                                        // ++==+-=-
static const char vlist6214[] = { 1,11, 0,14};                                                 // ++==+-==
static const char vlist6215[] = { 1,11, 9,14};                                                 // ++==+-=+
static const char vlist6216[] = { 2,11, 8,14, 8, 4,14};                                        // ++==+-+-
static const char vlist6217[] = { 1,11, 0,14};                                                 // ++==+-+=
static const char vlist6218[] = { 1,11, 9,14};                                                 // ++==+-++
static const char vlist6219[] = { 2, 4,10, 5,10, 4, 2};                                        // ++==+=--
static const char vlist6220[] = { 1, 0,10, 5};                                                 // ++==+=-=
static const char vlist6221[] = { 1, 8,10, 5};                                                 // ++==+=-+
static const char vlist6222[] = { 1, 2, 1, 4};                                                 // ++==+==-
                                                                                               // ++==+===
                                                                                               // ++==+==+
static const char vlist6225[] = { 1, 2, 8, 4};                                                 // ++==+=+-
                                                                                               // ++==+=+=
                                                                                               // ++==+=++
static const char vlist6228[] = { 2, 9,10, 5, 4, 9, 5};                                        // ++==++--
static const char vlist6229[] = { 1, 0,10, 5};                                                 // ++==++-=
static const char vlist6230[] = { 1, 8,10, 5};                                                 // ++==++-+
static const char vlist6231[] = { 1, 9, 1, 4};                                                 // ++==++=-
                                                                                               // ++==++==
                                                                                               // ++==++=+
static const char vlist6234[] = { 1, 9, 8, 4};                                                 // ++==+++-
                                                                                               // ++==+++=
                                                                                               // ++==++++
static const char vlist6237[] = { 2,15, 5,12,14,15,12};                                        // ++=+----
static const char vlist6238[] = { 2,15, 0,14, 5, 0,15};                                        // ++=+---=
static const char vlist6239[] = { 3, 5, 8,15, 8, 9,15,15, 9,14};                               // ++=+---+
static const char vlist6240[] = { 2, 1,12,14,15, 1,14};                                        // ++=+--=-
static const char vlist6241[] = { 2,15, 1,14, 1, 0,14};                                        // ++=+--==
static const char vlist6242[] = { 2,15, 1, 9,15, 9,14};                                        // ++=+--=+
static const char vlist6243[] = { 3,10, 8,14, 8,12,14,15,10,14};                               // ++=+--+-
static const char vlist6244[] = { 2,10, 0,14,15,10,14};                                        // ++=+--+=
static const char vlist6245[] = { 2,14,10, 9,15,10,14};                                        // ++=+--++
static const char vlist6246[] = { 2, 5,12, 2, 5, 2,15};                                        // ++=+-=--
static const char vlist6247[] = { 2, 0, 2,15, 0,15, 5};                                        // ++=+-=-=
static const char vlist6248[] = { 2, 2,15, 8, 8,15, 5};                                        // ++=+-=-+
static const char vlist6249[] = { 2, 1, 2,15,12, 2, 1};                                        // ++=+-==-
static const char vlist6250[] = { 1, 1, 2,15};                                                 // ++=+-===
static const char vlist6251[] = { 1, 1, 2,15};                                                 // ++=+-==+
static const char vlist6252[] = { 2,10, 2,15,12, 2, 8};                                        // ++=+-=+-
static const char vlist6253[] = { 1,10, 2,15};                                                 // ++=+-=+=
static const char vlist6254[] = { 1,10, 2,15};                                                 // ++=+-=++
static const char vlist6255[] = { 3, 5,12, 9, 5, 9,11, 5,11,15};                               // ++=+-+--
static const char vlist6256[] = { 2, 0,11, 5,11,15, 5};                                        // ++=+-+-=
static const char vlist6257[] = { 2, 8,15, 5,11,15, 8};                                        // ++=+-+-+
static const char vlist6258[] = { 2,12, 9, 1, 1,11,15};                                        // ++=+-+=-
static const char vlist6259[] = { 1, 1,11,15};                                                 // ++=+-+==
static const char vlist6260[] = { 1, 1,11,15};                                                 // ++=+-+=+
static const char vlist6261[] = { 2,12, 9, 8,10,11,15};                                        // ++=+-++-
static const char vlist6262[] = { 1,10,11,15};                                                 // ++=+-++=
static const char vlist6263[] = { 1,10,11,15};                                                 // ++=+-+++
static const char vlist6264[] = { 2, 3,12,14, 5,12, 3};                                        // ++=+=---
static const char vlist6265[] = { 2,14, 3, 0, 0, 3, 5};                                        // ++=+=--=
static const char vlist6266[] = { 2,14, 3, 9, 8, 3, 5};                                        // ++=+=--+
static const char vlist6267[] = { 2, 3,12,14,12, 3, 1};                                        // ++=+=-=-
static const char vlist6268[] = { 1, 3, 0,14};                                                 // ++=+=-==
static const char vlist6269[] = { 1, 3, 9,14};                                                 // ++=+=-=+
static const char vlist6270[] = { 2, 3, 8,14, 8,12,14};                                        // ++=+=-+-
static const char vlist6271[] = { 1, 3, 0,14};                                                 // ++=+=-+=
static const char vlist6272[] = { 1, 3, 9,14};                                                 // ++=+=-++
static const char vlist6273[] = { 2,12, 3, 5, 3,12, 2};                                        // ++=+==--
static const char vlist6274[] = { 1, 0, 3, 5};                                                 // ++=+==-=
static const char vlist6275[] = { 1, 8, 3, 5};                                                 // ++=+==-+
static const char vlist6276[] = { 1, 2, 1,12};                                                 // ++=+===-
                                                                                               // ++=+====
                                                                                               // ++=+===+
static const char vlist6279[] = { 1, 2, 8,12};                                                 // ++=+==+-
                                                                                               // ++=+==+=
                                                                                               // ++=+==++
static const char vlist6282[] = { 2, 9, 3, 5,12, 9, 5};                                        // ++=+=+--
static const char vlist6283[] = { 1, 0, 3, 5};                                                 // ++=+=+-=
static const char vlist6284[] = { 1, 8, 3, 5};                                                 // ++=+=+-+
static const char vlist6285[] = { 1, 9, 1,12};                                                 // ++=+=+=-
                                                                                               // ++=+=+==
                                                                                               // ++=+=+=+
static const char vlist6288[] = { 1, 9, 8,12};                                                 // ++=+=++-
                                                                                               // ++=+=++=
                                                                                               // ++=+=+++
static const char vlist6291[] = { 3,11,12,14,10,12,11, 5,12,10};                               // ++=++---
static const char vlist6292[] = { 2,14,11, 0, 0,10, 5};                                        // ++=++--=
static const char vlist6293[] = { 2,14,11, 9, 8,10, 5};                                        // ++=++--+
static const char vlist6294[] = { 2,12,11, 1,11,12,14};                                        // ++=++-=-
static const char vlist6295[] = { 1,11, 0,14};                                                 // ++=++-==
static const char vlist6296[] = { 1,11, 9,14};                                                 // ++=++-=+
static const char vlist6297[] = { 2,11,12,14,11, 8,12};                                        // ++=++-+-
static const char vlist6298[] = { 1,11, 0,14};                                                 // ++=++-+=
static const char vlist6299[] = { 1,11, 9,14};                                                 // ++=++-++
static const char vlist6300[] = { 2,10,12, 2,12,10, 5};                                        // ++=++=--
static const char vlist6301[] = { 1, 0,10, 5};                                                 // ++=++=-=
static const char vlist6302[] = { 1, 8,10, 5};                                                 // ++=++=-+
static const char vlist6303[] = { 1, 2, 1,12};                                                 // ++=++==-
                                                                                               // ++=++===
                                                                                               // ++=++==+
static const char vlist6306[] = { 1, 2, 8,12};                                                 // ++=++=+-
                                                                                               // ++=++=+=
                                                                                               // ++=++=++
static const char vlist6309[] = { 2,12,10, 5, 9,10,12};                                        // ++=+++--
static const char vlist6310[] = { 1, 0,10, 5};                                                 // ++=+++-=
static const char vlist6311[] = { 1, 8,10, 5};                                                 // ++=+++-+
static const char vlist6312[] = { 1, 9, 1,12};                                                 // ++=+++=-
                                                                                               // ++=+++==
                                                                                               // ++=+++=+
static const char vlist6315[] = { 1, 9, 8,12};                                                 // ++=++++-
                                                                                               // ++=++++=
                                                                                               // ++=+++++
static const char vlist6318[] = { 3,16,14,13,17,14,16,14,15,13};                               // +++-----
static const char vlist6319[] = { 3,16,17, 0, 0,14,15,13, 0,15};                               // +++----=
static const char vlist6320[] = { 4,16,17,12, 8, 9,15,13, 8,15, 9,14,15};                      // +++----+
static const char vlist6321[] = { 3, 1,16,17, 1,17,14, 1,14,15};                               // +++---=-
static const char vlist6322[] = { 3,15, 1,14, 1, 0,14, 0,16,17};                               // +++---==
static const char vlist6323[] = { 3,15, 1, 9,15, 9,14,12,16,17};                               // +++---=+
static const char vlist6324[] = { 4,16,17, 8, 8,17,14, 8,14,10,10,14,15};                      // +++---+-
static const char vlist6325[] = { 3,10, 0,14,15,10,14, 0,16,17};                               // +++---+=
static const char vlist6326[] = { 3,14,10, 9,15,10,14,12,16,17};                               // +++---++
static const char vlist6327[] = { 3,16, 2,13,16,17, 2,13, 2,15};                               // +++--=--
static const char vlist6328[] = { 3,13, 2,15, 2,13, 0, 0,16,17};                               // +++--=-=
static const char vlist6329[] = { 3, 8, 2,15,13, 8,15,12,16,17};                               // +++--=-+
static const char vlist6330[] = { 3, 2,16,17,16, 2, 1, 1, 2,15};                               // +++--==-
static const char vlist6331[] = { 2, 1, 2,15,17, 0,16};                                        // +++--===
static const char vlist6332[] = { 2, 1, 2,15,17,12,16};                                        // +++--==+
static const char vlist6333[] = { 3, 2, 8,17,10, 2,15, 8,16,17};                               // +++--=+-
static const char vlist6334[] = { 2,17, 0,16,10, 2,15};                                        // +++--=+=
static const char vlist6335[] = { 2,17,12,16,10, 2,15};                                        // +++--=++
static const char vlist6336[] = { 4,16, 9,13,16,17, 9,13, 9,11,13,11,15};                      // +++--+--
static const char vlist6337[] = { 3,11,13, 0,13,11,15, 0,16,17};                               // +++--+-=
static const char vlist6338[] = { 3,13,11,15, 8,11,13,12,16,17};                               // +++--+-+
static const char vlist6339[] = { 3,16, 9, 1, 9,16,17, 1,11,15};                               // +++--+=-
static const char vlist6340[] = { 2, 1,11,15,17, 0,16};                                        // +++--+==
static const char vlist6341[] = { 2, 1,11,15,17,12,16};                                        // +++--+=+
static const char vlist6342[] = { 3, 9,16,17, 9, 8,16,10,11,15};                               // +++--++-
static const char vlist6343[] = { 2,17, 0,16,10,11,15};                                        // +++--++=
static const char vlist6344[] = { 2,17,12,16,10,11,15};                                        // +++--+++
static const char vlist6345[] = { 3,13,16,14,13,14, 3,16,17,14};                               // +++-=---
static const char vlist6346[] = { 3, 3, 0,14, 3,13, 0, 0,16,17};                               // +++-=--=
static const char vlist6347[] = { 3, 3, 9,14, 3,13, 8,12,16,17};                               // +++-=--+
static const char vlist6348[] = { 3, 3, 1,14,14, 1,17,17, 1,16};                               // +++-=-=-
static const char vlist6349[] = { 2,17, 0,16, 3, 0,14};                                        // +++-=-==
static const char vlist6350[] = { 2, 3, 9,14,17,12,16};                                        // +++-=-=+
static const char vlist6351[] = { 3, 3, 8,14, 8,16,17,14, 8,17};                               // +++-=-+-
static const char vlist6352[] = { 2,17, 0,16, 3, 0,14};                                        // +++-=-+=
static const char vlist6353[] = { 2, 3, 9,14,17,12,16};                                        // +++-=-++
static const char vlist6354[] = { 3, 3,13, 2,13,16, 2, 2,16,17};                               // +++-==--
static const char vlist6355[] = { 2, 0, 3,13,17, 0,16};                                        // +++-==-=
static const char vlist6356[] = { 2, 8, 3,13,17,12,16};                                        // +++-==-+
static const char vlist6357[] = { 2,17, 2,16, 2, 1,16};                                        // +++-===-
static const char vlist6358[] = { 1,17, 0,16};                                                 // +++-====
static const char vlist6359[] = { 1,17,12,16};                                                 // +++-===+
static const char vlist6360[] = { 2,17, 2, 8,17, 8,16};                                        // +++-==+-
static const char vlist6361[] = { 1,17, 0,16};                                                 // +++-==+=
static const char vlist6362[] = { 1,17,12,16};                                                 // +++-==++
static const char vlist6363[] = { 3, 3,13, 9,13,16, 9, 9,16,17};                               // +++-=+--
static const char vlist6364[] = { 2, 0, 3,13,17, 0,16};                                        // +++-=+-=
static const char vlist6365[] = { 2, 8, 3,13,17,12,16};                                        // +++-=+-+
static const char vlist6366[] = { 2, 9, 1,16,17, 9,16};                                        // +++-=+=-
static const char vlist6367[] = { 1,17, 0,16};                                                 // +++-=+==
static const char vlist6368[] = { 1,17,12,16};                                                 // +++-=+=+
static const char vlist6369[] = { 2,16, 9, 8,17, 9,16};                                        // +++-=++-
static const char vlist6370[] = { 1,17, 0,16};                                                 // +++-=++=
static const char vlist6371[] = { 1,17,12,16};                                                 // +++-=+++
static const char vlist6372[] = { 4,13,16,14,13,14,11,10,13,11,16,17,14};                      // +++-+---
static const char vlist6373[] = { 3,10,13, 0, 0,16,17,11, 0,14};                               // +++-+--=
static const char vlist6374[] = { 3,10,13, 8,11, 9,14,12,16,17};                               // +++-+--+
static const char vlist6375[] = { 3,11, 1,14,14, 1,17,17, 1,16};                               // +++-+-=-
static const char vlist6376[] = { 2,17, 0,16,11, 0,14};                                        // +++-+-==
static const char vlist6377[] = { 2,11, 9,14,17,12,16};                                        // +++-+-=+
static const char vlist6378[] = { 3, 8,16,17,14, 8,17,11, 8,14};                               // +++-+-+-
static const char vlist6379[] = { 2,17, 0,16,11, 0,14};                                        // +++-+-+=
static const char vlist6380[] = { 2,11, 9,14,17,12,16};                                        // +++-+-++
static const char vlist6381[] = { 3,10,13, 2,13,16, 2, 2,16,17};                               // +++-+=--
static const char vlist6382[] = { 2, 0,10,13,17, 0,16};                                        // +++-+=-=
static const char vlist6383[] = { 2,17,12,16, 8,10,13};                                        // +++-+=-+
static const char vlist6384[] = { 2,17, 2,16, 2, 1,16};                                        // +++-+==-
static const char vlist6385[] = { 1,17, 0,16};                                                 // +++-+===
static const char vlist6386[] = { 1,17,12,16};                                                 // +++-+==+
static const char vlist6387[] = { 2,17, 2, 8,17, 8,16};                                        // +++-+=+-
static const char vlist6388[] = { 1,17, 0,16};                                                 // +++-+=+=
static const char vlist6389[] = { 1,17,12,16};                                                 // +++-+=++
static const char vlist6390[] = { 3,10,13, 9,13,16, 9, 9,16,17};                               // +++-++--
static const char vlist6391[] = { 2, 0,10,13,17, 0,16};                                        // +++-++-=
static const char vlist6392[] = { 2,17,12,16, 8,10,13};                                        // +++-++-+
static const char vlist6393[] = { 2, 9, 1,16,17, 9,16};                                        // +++-++=-
static const char vlist6394[] = { 1,17, 0,16};                                                 // +++-++==
static const char vlist6395[] = { 1,17,12,16};                                                 // +++-++=+
static const char vlist6396[] = { 2,16, 9, 8,17, 9,16};                                        // +++-+++-
static const char vlist6397[] = { 1,17, 0,16};                                                 // +++-+++=
static const char vlist6398[] = { 1,17,12,16};                                                 // +++-++++
static const char vlist6399[] = { 2, 4,14,13,14,15,13};                                        // +++=----
static const char vlist6400[] = { 2,15, 0,14,13, 0,15};                                        // +++=---=
static const char vlist6401[] = { 3,13, 8,15, 8, 9,15,15, 9,14};                               // +++=---+
static const char vlist6402[] = { 2, 1, 4,14,15, 1,14};                                        // +++=--=-
static const char vlist6403[] = { 2,15, 1,14, 1, 0,14};                                        // +++=--==
static const char vlist6404[] = { 2,15, 1, 9,15, 9,14};                                        // +++=--=+
static const char vlist6405[] = { 3, 8, 4,14,10, 8,14,15,10,14};                               // +++=--+-
static const char vlist6406[] = { 2,10, 0,14,15,10,14};                                        // +++=--+=
static const char vlist6407[] = { 2,14,10, 9,15,10,14};                                        // +++=--++
static const char vlist6408[] = { 2,13, 4, 2,13, 2,15};                                        // +++=-=--
static const char vlist6409[] = { 2, 2,15,13, 0, 2,13};                                        // +++=-=-=
static const char vlist6410[] = { 2, 2,15, 8, 8,15,13};                                        // +++=-=-+
static const char vlist6411[] = { 2, 4, 2, 1, 1, 2,15};                                        // +++=-==-
static const char vlist6412[] = { 1, 1, 2,15};                                                 // +++=-===
static const char vlist6413[] = { 1, 1, 2,15};                                                 // +++=-==+
static const char vlist6414[] = { 2, 4, 2, 8,10, 2,15};                                        // +++=-=+-
static const char vlist6415[] = { 1,10, 2,15};                                                 // +++=-=+=
static const char vlist6416[] = { 1,10, 2,15};                                                 // +++=-=++
static const char vlist6417[] = { 3,13, 4, 9,13, 9,11,13,11,15};                               // +++=-+--
static const char vlist6418[] = { 2, 0,11,13,11,15,13};                                        // +++=-+-=
static const char vlist6419[] = { 2,11,13, 8,11,15,13};                                        // +++=-+-+
static const char vlist6420[] = { 2, 4, 9, 1, 1,11,15};                                        // +++=-+=-
static const char vlist6421[] = { 1, 1,11,15};                                                 // +++=-+==
static const char vlist6422[] = { 1, 1,11,15};                                                 // +++=-+=+
static const char vlist6423[] = { 2, 4, 9, 8,10,11,15};                                        // +++=-++-
static const char vlist6424[] = { 1,10,11,15};                                                 // +++=-++=
static const char vlist6425[] = { 1,10,11,15};                                                 // +++=-+++
static const char vlist6426[] = { 2, 3, 4,14,13, 4, 3};                                        // +++==---
static const char vlist6427[] = { 2,14, 3, 0, 0, 3,13};                                        // +++==--=
static const char vlist6428[] = { 2,14, 3, 9, 8, 3,13};                                        // +++==--+
static const char vlist6429[] = { 2,14, 3, 1, 4,14, 1};                                        // +++==-=-
static const char vlist6430[] = { 1, 3, 0,14};                                                 // +++==-==
static const char vlist6431[] = { 1, 3, 9,14};                                                 // +++==-=+
static const char vlist6432[] = { 2, 3, 8,14, 8, 4,14};                                        // +++==-+-
static const char vlist6433[] = { 1, 3, 0,14};                                                 // +++==-+=
static const char vlist6434[] = { 1, 3, 9,14};                                                 // +++==-++
static const char vlist6435[] = { 2, 3,13, 2,13, 4, 2};                                        // +++===--
static const char vlist6436[] = { 1, 0, 3,13};                                                 // +++===-=
static const char vlist6437[] = { 1, 8, 3,13};                                                 // +++===-+
static const char vlist6438[] = { 1, 2, 1, 4};                                                 // +++====-
                                                                                               // +++=====
                                                                                               // +++====+
static const char vlist6441[] = { 1, 2, 8, 4};                                                 // +++===+-
                                                                                               // +++===+=
                                                                                               // +++===++
static const char vlist6444[] = { 2, 9, 3,13, 4, 9,13};                                        // +++==+--
static const char vlist6445[] = { 1, 0, 3,13};                                                 // +++==+-=
static const char vlist6446[] = { 1, 8, 3,13};                                                 // +++==+-+
static const char vlist6447[] = { 1, 9, 1, 4};                                                 // +++==+=-
                                                                                               // +++==+==
                                                                                               // +++==+=+
static const char vlist6450[] = { 1, 9, 8, 4};                                                 // +++==++-
                                                                                               // +++==++=
                                                                                               // +++==+++
static const char vlist6453[] = { 3,11, 4,14,10, 4,11,13, 4,10};                               // +++=+---
static const char vlist6454[] = { 2,14,11, 0, 0,10,13};                                        // +++=+--=
static const char vlist6455[] = { 2,14,11, 9, 8,10,13};                                        // +++=+--+
static const char vlist6456[] = { 2,11, 4,14, 4,11, 1};                                        // +++=+-=-
static const char vlist6457[] = { 1,11, 0,14};                                                 // +++=+-==
static const char vlist6458[] = { 1,11, 9,14};                                                 // +++=+-=+
static const char vlist6459[] = { 2,11, 8,14, 8, 4,14};                                        // +++=+-+-
static const char vlist6460[] = { 1,11, 0,14};                                                 // +++=+-+=
static const char vlist6461[] = { 1,11, 9,14};                                                 // +++=+-++
static const char vlist6462[] = { 2, 4,10,13,10, 4, 2};                                        // +++=+=--
static const char vlist6463[] = { 1, 0,10,13};                                                 // +++=+=-=
static const char vlist6464[] = { 1, 8,10,13};                                                 // +++=+=-+
static const char vlist6465[] = { 1, 2, 1, 4};                                                 // +++=+==-
                                                                                               // +++=+===
                                                                                               // +++=+==+
static const char vlist6468[] = { 1, 2, 8, 4};                                                 // +++=+=+-
                                                                                               // +++=+=+=
                                                                                               // +++=+=++
static const char vlist6471[] = { 2, 9,10,13, 4, 9,13};                                        // +++=++--
static const char vlist6472[] = { 1, 0,10,13};                                                 // +++=++-=
static const char vlist6473[] = { 1, 8,10,13};                                                 // +++=++-+
static const char vlist6474[] = { 1, 9, 1, 4};                                                 // +++=++=-
                                                                                               // +++=++==
                                                                                               // +++=++=+
static const char vlist6477[] = { 1, 9, 8, 4};                                                 // +++=+++-
                                                                                               // +++=+++=
                                                                                               // +++=++++
static const char vlist6480[] = { 2,15,13,14,14,13,12};                                        // ++++----
static const char vlist6481[] = { 2,15, 0,14,13, 0,15};                                        // ++++---=
static const char vlist6482[] = { 3,13, 8,15, 8, 9,15,15, 9,14};                               // ++++---+
static const char vlist6483[] = { 2, 1,12,14,15, 1,14};                                        // ++++--=-
static const char vlist6484[] = { 2,15, 1,14, 1, 0,14};                                        // ++++--==
static const char vlist6485[] = { 2,15, 1, 9,15, 9,14};                                        // ++++--=+
static const char vlist6486[] = { 3,10, 8,14, 8,12,14,15,10,14};                               // ++++--+-
static const char vlist6487[] = { 2,10, 0,14,15,10,14};                                        // ++++--+=
static const char vlist6488[] = { 2,14,10, 9,15,10,14};                                        // ++++--++
static const char vlist6489[] = { 2,13,12, 2,13, 2,15};                                        // ++++-=--
static const char vlist6490[] = { 2, 2,15,13, 0, 2,13};                                        // ++++-=-=
static const char vlist6491[] = { 2, 2,15, 8, 8,15,13};                                        // ++++-=-+
static const char vlist6492[] = { 2, 1, 2,15,12, 2, 1};                                        // ++++-==-
static const char vlist6493[] = { 1, 1, 2,15};                                                 // ++++-===
static const char vlist6494[] = { 1, 1, 2,15};                                                 // ++++-==+
static const char vlist6495[] = { 2,10, 2,15,12, 2, 8};                                        // ++++-=+-
static const char vlist6496[] = { 1,10, 2,15};                                                 // ++++-=+=
static const char vlist6497[] = { 1,10, 2,15};                                                 // ++++-=++
static const char vlist6498[] = { 3,13,12, 9,13,11,15,13, 9,11};                               // ++++-+--
static const char vlist6499[] = { 2, 0,11,13,11,15,13};                                        // ++++-+-=
static const char vlist6500[] = { 2,11,13, 8,11,15,13};                                        // ++++-+-+
static const char vlist6501[] = { 2,12, 9, 1, 1,11,15};                                        // ++++-+=-
static const char vlist6502[] = { 1, 1,11,15};                                                 // ++++-+==
static const char vlist6503[] = { 1, 1,11,15};                                                 // ++++-+=+
static const char vlist6504[] = { 2,12, 9, 8,10,11,15};                                        // ++++-++-
static const char vlist6505[] = { 1,10,11,15};                                                 // ++++-++=
static const char vlist6506[] = { 1,10,11,15};                                                 // ++++-+++
static const char vlist6507[] = { 2, 3,12,14,13,12, 3};                                        // ++++=---
static const char vlist6508[] = { 2,14, 3, 0, 0, 3,13};                                        // ++++=--=
static const char vlist6509[] = { 2,14, 3, 9, 8, 3,13};                                        // ++++=--+
static const char vlist6510[] = { 2, 3,12,14,12, 3, 1};                                        // ++++=-=-
static const char vlist6511[] = { 1, 3, 0,14};                                                 // ++++=-==
static const char vlist6512[] = { 1, 3, 9,14};                                                 // ++++=-=+
static const char vlist6513[] = { 2, 3, 8,14, 8,12,14};                                        // ++++=-+-
static const char vlist6514[] = { 1, 3, 0,14};                                                 // ++++=-+=
static const char vlist6515[] = { 1, 3, 9,14};                                                 // ++++=-++
static const char vlist6516[] = { 2,12, 3,13, 3,12, 2};                                        // ++++==--
static const char vlist6517[] = { 1, 0, 3,13};                                                 // ++++==-=
static const char vlist6518[] = { 1, 8, 3,13};                                                 // ++++==-+
static const char vlist6519[] = { 1, 2, 1,12};                                                 // ++++===-
                                                                                               // ++++====
                                                                                               // ++++===+
static const char vlist6522[] = { 1, 2, 8,12};                                                 // ++++==+-
                                                                                               // ++++==+=
                                                                                               // ++++==++
static const char vlist6525[] = { 2, 9, 3,13,12, 9,13};                                        // ++++=+--
static const char vlist6526[] = { 1, 0, 3,13};                                                 // ++++=+-=
static const char vlist6527[] = { 1, 8, 3,13};                                                 // ++++=+-+
static const char vlist6528[] = { 1, 9, 1,12};                                                 // ++++=+=-
                                                                                               // ++++=+==
                                                                                               // ++++=+=+
static const char vlist6531[] = { 1, 9, 8,12};                                                 // ++++=++-
                                                                                               // ++++=++=
                                                                                               // ++++=+++
static const char vlist6534[] = { 3,11,12,14,10,12,11,13,12,10};                               // +++++---
static const char vlist6535[] = { 2,14,11, 0, 0,10,13};                                        // +++++--=
static const char vlist6536[] = { 2,14,11, 9, 8,10,13};                                        // +++++--+
static const char vlist6537[] = { 2,12,11, 1,11,12,14};                                        // +++++-=-
static const char vlist6538[] = { 1,11, 0,14};                                                 // +++++-==
static const char vlist6539[] = { 1,11, 9,14};                                                 // +++++-=+
static const char vlist6540[] = { 2,11,12,14,11, 8,12};                                        // +++++-+-
static const char vlist6541[] = { 1,11, 0,14};                                                 // +++++-+=
static const char vlist6542[] = { 1,11, 9,14};                                                 // +++++-++
static const char vlist6543[] = { 2,10,12, 2,12,10,13};                                        // +++++=--
static const char vlist6544[] = { 1, 0,10,13};                                                 // +++++=-=
static const char vlist6545[] = { 1, 8,10,13};                                                 // +++++=-+
static const char vlist6546[] = { 1, 2, 1,12};                                                 // +++++==-
                                                                                               // +++++===
                                                                                               // +++++==+
static const char vlist6549[] = { 1, 2, 8,12};                                                 // +++++=+-
                                                                                               // +++++=+=
                                                                                               // +++++=++
static const char vlist6552[] = { 2,12,10,13, 9,10,12};                                        // ++++++--
static const char vlist6553[] = { 1, 0,10,13};                                                 // ++++++-=
static const char vlist6554[] = { 1, 8,10,13};                                                 // ++++++-+
static const char vlist6555[] = { 1, 9, 1,12};                                                 // ++++++=-
                                                                                               // ++++++==
                                                                                               // ++++++=+
static const char vlist6558[] = { 1, 9, 8,12};                                                 // +++++++-
                                                                                               // +++++++=
                                                                                               // ++++++++
const char *PolygonizerCubeArrayTable::s_isosurfaceLookup[6561] = {
  NULL     ,NULL     ,vlist0002,NULL     ,NULL     ,vlist0005,vlist0006,vlist0007,vlist0008,NULL     
 ,NULL     ,vlist0011,NULL     ,vlist0013,vlist0014,vlist0015,vlist0016,vlist0017,vlist0018,vlist0019
 ,vlist0020,vlist0021,vlist0022,vlist0023,vlist0024,vlist0025,vlist0026,NULL     ,NULL     ,vlist0029
 ,NULL     ,vlist0031,vlist0032,vlist0033,vlist0034,vlist0035,NULL     ,vlist0037,vlist0038,vlist0039
 ,vlist0040,vlist0041,vlist0042,vlist0043,vlist0044,vlist0045,vlist0046,vlist0047,vlist0048,vlist0049
 ,vlist0050,vlist0051,vlist0052,vlist0053,vlist0054,vlist0055,vlist0056,vlist0057,vlist0058,vlist0059
 ,vlist0060,vlist0061,vlist0062,vlist0063,vlist0064,vlist0065,vlist0066,vlist0067,vlist0068,vlist0069
 ,vlist0070,vlist0071,vlist0072,vlist0073,vlist0074,vlist0075,vlist0076,vlist0077,vlist0078,vlist0079
 ,vlist0080,NULL     ,NULL     ,vlist0083,NULL     ,vlist0085,vlist0086,vlist0087,vlist0088,vlist0089
 ,NULL     ,vlist0091,vlist0092,NULL     ,vlist0094,vlist0095,vlist0096,vlist0097,vlist0098,vlist0099
 ,vlist0100,vlist0101,vlist0102,vlist0103,vlist0104,vlist0105,vlist0106,vlist0107,NULL     ,NULL     
 ,vlist0110,NULL     ,vlist0112,vlist0113,vlist0114,vlist0115,vlist0116,NULL     ,vlist0118,vlist0119
 ,vlist0120,vlist0121,vlist0122,vlist0123,vlist0124,vlist0125,vlist0126,vlist0127,vlist0128,vlist0129
 ,vlist0130,vlist0131,vlist0132,vlist0133,vlist0134,vlist0135,vlist0136,vlist0137,vlist0138,vlist0139
 ,vlist0140,vlist0141,vlist0142,vlist0143,vlist0144,vlist0145,vlist0146,vlist0147,vlist0148,vlist0149
 ,vlist0150,vlist0151,vlist0152,vlist0153,vlist0154,vlist0155,vlist0156,vlist0157,vlist0158,vlist0159
 ,vlist0160,vlist0161,vlist0162,vlist0163,vlist0164,vlist0165,vlist0166,vlist0167,vlist0168,vlist0169
 ,vlist0170,vlist0171,vlist0172,vlist0173,vlist0174,vlist0175,vlist0176,vlist0177,vlist0178,vlist0179
 ,vlist0180,vlist0181,vlist0182,vlist0183,vlist0184,vlist0185,vlist0186,vlist0187,vlist0188,vlist0189
 ,vlist0190,vlist0191,vlist0192,vlist0193,vlist0194,vlist0195,vlist0196,vlist0197,vlist0198,vlist0199
 ,vlist0200,vlist0201,vlist0202,vlist0203,vlist0204,vlist0205,vlist0206,vlist0207,vlist0208,vlist0209
 ,vlist0210,vlist0211,vlist0212,vlist0213,vlist0214,vlist0215,vlist0216,vlist0217,vlist0218,vlist0219
 ,vlist0220,vlist0221,vlist0222,vlist0223,vlist0224,vlist0225,vlist0226,vlist0227,vlist0228,vlist0229
 ,vlist0230,vlist0231,vlist0232,vlist0233,vlist0234,vlist0235,vlist0236,vlist0237,vlist0238,vlist0239
 ,vlist0240,vlist0241,vlist0242,NULL     ,NULL     ,vlist0245,NULL     ,vlist0247,vlist0248,vlist0249
 ,vlist0250,vlist0251,NULL     ,NULL     ,vlist0254,NULL     ,vlist0256,vlist0257,vlist0258,vlist0259
 ,vlist0260,vlist0261,vlist0262,vlist0263,vlist0264,vlist0265,vlist0266,vlist0267,vlist0268,vlist0269
 ,NULL     ,NULL     ,vlist0272,vlist0273,vlist0274,vlist0275,vlist0276,vlist0277,vlist0278,NULL     
 ,vlist0280,vlist0281,vlist0282,vlist0283,vlist0284,vlist0285,vlist0286,vlist0287,vlist0288,vlist0289
 ,vlist0290,vlist0291,vlist0292,vlist0293,vlist0294,vlist0295,vlist0296,vlist0297,vlist0298,vlist0299
 ,vlist0300,vlist0301,vlist0302,vlist0303,vlist0304,vlist0305,vlist0306,vlist0307,vlist0308,vlist0309
 ,vlist0310,vlist0311,vlist0312,vlist0313,vlist0314,vlist0315,vlist0316,vlist0317,vlist0318,vlist0319
 ,vlist0320,vlist0321,vlist0322,vlist0323,NULL     ,vlist0325,vlist0326,vlist0327,vlist0328,vlist0329
 ,vlist0330,vlist0331,vlist0332,NULL     ,vlist0334,vlist0335,vlist0336,vlist0337,vlist0338,vlist0339
 ,vlist0340,vlist0341,vlist0342,vlist0343,vlist0344,vlist0345,vlist0346,vlist0347,vlist0348,vlist0349
 ,vlist0350,NULL     ,vlist0352,vlist0353,vlist0354,vlist0355,vlist0356,vlist0357,vlist0358,vlist0359
 ,NULL     ,vlist0361,vlist0362,vlist0363,vlist0364,vlist0365,vlist0366,vlist0367,vlist0368,vlist0369
 ,vlist0370,vlist0371,vlist0372,vlist0373,vlist0374,vlist0375,vlist0376,vlist0377,vlist0378,vlist0379
 ,vlist0380,vlist0381,vlist0382,vlist0383,vlist0384,vlist0385,vlist0386,vlist0387,vlist0388,vlist0389
 ,vlist0390,vlist0391,vlist0392,vlist0393,vlist0394,vlist0395,vlist0396,vlist0397,vlist0398,vlist0399
 ,vlist0400,vlist0401,vlist0402,vlist0403,vlist0404,vlist0405,vlist0406,vlist0407,vlist0408,vlist0409
 ,vlist0410,vlist0411,vlist0412,vlist0413,vlist0414,vlist0415,vlist0416,vlist0417,vlist0418,vlist0419
 ,vlist0420,vlist0421,vlist0422,vlist0423,vlist0424,vlist0425,vlist0426,vlist0427,vlist0428,vlist0429
 ,vlist0430,vlist0431,vlist0432,vlist0433,vlist0434,vlist0435,vlist0436,vlist0437,vlist0438,vlist0439
 ,vlist0440,vlist0441,vlist0442,vlist0443,vlist0444,vlist0445,vlist0446,vlist0447,vlist0448,vlist0449
 ,vlist0450,vlist0451,vlist0452,vlist0453,vlist0454,vlist0455,vlist0456,vlist0457,vlist0458,vlist0459
 ,vlist0460,vlist0461,vlist0462,vlist0463,vlist0464,vlist0465,vlist0466,vlist0467,vlist0468,vlist0469
 ,vlist0470,vlist0471,vlist0472,vlist0473,vlist0474,vlist0475,vlist0476,vlist0477,vlist0478,vlist0479
 ,vlist0480,vlist0481,vlist0482,vlist0483,vlist0484,vlist0485,vlist0486,vlist0487,vlist0488,vlist0489
 ,vlist0490,vlist0491,vlist0492,vlist0493,vlist0494,vlist0495,vlist0496,vlist0497,vlist0498,vlist0499
 ,vlist0500,vlist0501,vlist0502,vlist0503,vlist0504,vlist0505,vlist0506,vlist0507,vlist0508,vlist0509
 ,vlist0510,vlist0511,vlist0512,vlist0513,vlist0514,vlist0515,vlist0516,vlist0517,vlist0518,vlist0519
 ,vlist0520,vlist0521,vlist0522,vlist0523,vlist0524,vlist0525,vlist0526,vlist0527,vlist0528,vlist0529
 ,vlist0530,vlist0531,vlist0532,vlist0533,vlist0534,vlist0535,vlist0536,vlist0537,vlist0538,vlist0539
 ,vlist0540,vlist0541,vlist0542,vlist0543,vlist0544,vlist0545,vlist0546,vlist0547,vlist0548,vlist0549
 ,vlist0550,vlist0551,vlist0552,vlist0553,vlist0554,vlist0555,vlist0556,vlist0557,vlist0558,vlist0559
 ,vlist0560,vlist0561,vlist0562,vlist0563,vlist0564,vlist0565,vlist0566,vlist0567,vlist0568,vlist0569
 ,vlist0570,vlist0571,vlist0572,vlist0573,vlist0574,vlist0575,vlist0576,vlist0577,vlist0578,vlist0579
 ,vlist0580,vlist0581,vlist0582,vlist0583,vlist0584,vlist0585,vlist0586,vlist0587,vlist0588,vlist0589
 ,vlist0590,vlist0591,vlist0592,vlist0593,vlist0594,vlist0595,vlist0596,vlist0597,vlist0598,vlist0599
 ,vlist0600,vlist0601,vlist0602,vlist0603,vlist0604,vlist0605,vlist0606,vlist0607,vlist0608,vlist0609
 ,vlist0610,vlist0611,vlist0612,vlist0613,vlist0614,vlist0615,vlist0616,vlist0617,vlist0618,vlist0619
 ,vlist0620,vlist0621,vlist0622,vlist0623,vlist0624,vlist0625,vlist0626,vlist0627,vlist0628,vlist0629
 ,vlist0630,vlist0631,vlist0632,vlist0633,vlist0634,vlist0635,vlist0636,vlist0637,vlist0638,vlist0639
 ,vlist0640,vlist0641,vlist0642,vlist0643,vlist0644,vlist0645,vlist0646,vlist0647,vlist0648,vlist0649
 ,vlist0650,vlist0651,vlist0652,vlist0653,vlist0654,vlist0655,vlist0656,vlist0657,vlist0658,vlist0659
 ,vlist0660,vlist0661,vlist0662,vlist0663,vlist0664,vlist0665,vlist0666,vlist0667,vlist0668,vlist0669
 ,vlist0670,vlist0671,vlist0672,vlist0673,vlist0674,vlist0675,vlist0676,vlist0677,vlist0678,vlist0679
 ,vlist0680,vlist0681,vlist0682,vlist0683,vlist0684,vlist0685,vlist0686,vlist0687,vlist0688,vlist0689
 ,vlist0690,vlist0691,vlist0692,vlist0693,vlist0694,vlist0695,vlist0696,vlist0697,vlist0698,vlist0699
 ,vlist0700,vlist0701,vlist0702,vlist0703,vlist0704,vlist0705,vlist0706,vlist0707,vlist0708,vlist0709
 ,vlist0710,vlist0711,vlist0712,vlist0713,vlist0714,vlist0715,vlist0716,vlist0717,vlist0718,vlist0719
 ,vlist0720,vlist0721,vlist0722,vlist0723,vlist0724,vlist0725,vlist0726,vlist0727,vlist0728,NULL     
 ,NULL     ,vlist0731,NULL     ,NULL     ,vlist0734,vlist0735,vlist0736,vlist0737,NULL     ,vlist0739
 ,vlist0740,NULL     ,vlist0742,vlist0743,vlist0744,vlist0745,vlist0746,vlist0747,vlist0748,vlist0749
 ,vlist0750,vlist0751,vlist0752,vlist0753,vlist0754,vlist0755,NULL     ,NULL     ,vlist0758,NULL     
 ,vlist0760,vlist0761,vlist0762,vlist0763,vlist0764,vlist0765,vlist0766,vlist0767,vlist0768,vlist0769
 ,vlist0770,vlist0771,vlist0772,vlist0773,vlist0774,vlist0775,vlist0776,vlist0777,vlist0778,vlist0779
 ,vlist0780,vlist0781,vlist0782,vlist0783,vlist0784,vlist0785,vlist0786,vlist0787,vlist0788,vlist0789
 ,vlist0790,vlist0791,vlist0792,vlist0793,vlist0794,vlist0795,vlist0796,vlist0797,vlist0798,vlist0799
 ,vlist0800,vlist0801,vlist0802,vlist0803,vlist0804,vlist0805,vlist0806,vlist0807,vlist0808,vlist0809
 ,NULL     ,vlist0811,vlist0812,NULL     ,vlist0814,vlist0815,vlist0816,vlist0817,vlist0818,vlist0819
 ,vlist0820,vlist0821,vlist0822,vlist0823,vlist0824,vlist0825,vlist0826,vlist0827,vlist0828,vlist0829
 ,vlist0830,vlist0831,vlist0832,vlist0833,vlist0834,vlist0835,vlist0836,NULL     ,vlist0838,vlist0839
 ,NULL     ,vlist0841,vlist0842,vlist0843,vlist0844,vlist0845,vlist0846,vlist0847,vlist0848,vlist0849
 ,vlist0850,vlist0851,vlist0852,vlist0853,vlist0854,vlist0855,vlist0856,vlist0857,vlist0858,vlist0859
 ,vlist0860,vlist0861,vlist0862,vlist0863,vlist0864,vlist0865,vlist0866,vlist0867,vlist0868,vlist0869
 ,vlist0870,vlist0871,vlist0872,vlist0873,vlist0874,vlist0875,vlist0876,vlist0877,vlist0878,vlist0879
 ,vlist0880,vlist0881,vlist0882,vlist0883,vlist0884,vlist0885,vlist0886,vlist0887,vlist0888,vlist0889
 ,vlist0890,vlist0891,vlist0892,vlist0893,vlist0894,vlist0895,vlist0896,vlist0897,vlist0898,vlist0899
 ,vlist0900,vlist0901,vlist0902,vlist0903,vlist0904,vlist0905,vlist0906,vlist0907,vlist0908,vlist0909
 ,vlist0910,vlist0911,vlist0912,vlist0913,vlist0914,vlist0915,vlist0916,vlist0917,vlist0918,vlist0919
 ,vlist0920,vlist0921,vlist0922,vlist0923,vlist0924,vlist0925,vlist0926,vlist0927,vlist0928,vlist0929
 ,vlist0930,vlist0931,vlist0932,vlist0933,vlist0934,vlist0935,vlist0936,vlist0937,vlist0938,vlist0939
 ,vlist0940,vlist0941,vlist0942,vlist0943,vlist0944,vlist0945,vlist0946,vlist0947,vlist0948,vlist0949
 ,vlist0950,vlist0951,vlist0952,vlist0953,vlist0954,vlist0955,vlist0956,vlist0957,vlist0958,vlist0959
 ,vlist0960,vlist0961,vlist0962,vlist0963,vlist0964,vlist0965,vlist0966,vlist0967,vlist0968,vlist0969
 ,vlist0970,vlist0971,NULL     ,NULL     ,vlist0974,NULL     ,vlist0976,vlist0977,vlist0978,vlist0979
 ,vlist0980,NULL     ,vlist0982,vlist0983,NULL     ,vlist0985,vlist0986,vlist0987,vlist0988,vlist0989
 ,vlist0990,vlist0991,vlist0992,vlist0993,vlist0994,vlist0995,vlist0996,vlist0997,vlist0998,NULL     
 ,vlist1000,vlist1001,vlist1002,vlist1003,vlist1004,vlist1005,vlist1006,vlist1007,vlist1008,vlist1009
 ,vlist1010,vlist1011,vlist1012,vlist1013,vlist1014,vlist1015,vlist1016,vlist1017,vlist1018,vlist1019
 ,vlist1020,vlist1021,vlist1022,vlist1023,vlist1024,vlist1025,vlist1026,vlist1027,vlist1028,vlist1029
 ,vlist1030,vlist1031,vlist1032,vlist1033,vlist1034,vlist1035,vlist1036,vlist1037,vlist1038,vlist1039
 ,vlist1040,vlist1041,vlist1042,vlist1043,vlist1044,vlist1045,vlist1046,vlist1047,vlist1048,vlist1049
 ,vlist1050,vlist1051,vlist1052,vlist1053,vlist1054,vlist1055,vlist1056,vlist1057,vlist1058,vlist1059
 ,vlist1060,vlist1061,vlist1062,vlist1063,vlist1064,vlist1065,vlist1066,vlist1067,vlist1068,vlist1069
 ,vlist1070,vlist1071,vlist1072,vlist1073,vlist1074,vlist1075,vlist1076,vlist1077,vlist1078,vlist1079
 ,vlist1080,vlist1081,vlist1082,vlist1083,vlist1084,vlist1085,vlist1086,vlist1087,vlist1088,vlist1089
 ,vlist1090,vlist1091,vlist1092,vlist1093,vlist1094,vlist1095,vlist1096,vlist1097,vlist1098,vlist1099
 ,vlist1100,vlist1101,vlist1102,vlist1103,vlist1104,vlist1105,vlist1106,vlist1107,vlist1108,vlist1109
 ,vlist1110,vlist1111,vlist1112,vlist1113,vlist1114,vlist1115,vlist1116,vlist1117,vlist1118,vlist1119
 ,vlist1120,vlist1121,vlist1122,vlist1123,vlist1124,vlist1125,vlist1126,vlist1127,vlist1128,vlist1129
 ,vlist1130,vlist1131,vlist1132,vlist1133,vlist1134,vlist1135,vlist1136,vlist1137,vlist1138,vlist1139
 ,vlist1140,vlist1141,vlist1142,vlist1143,vlist1144,vlist1145,vlist1146,vlist1147,vlist1148,vlist1149
 ,vlist1150,vlist1151,vlist1152,vlist1153,vlist1154,vlist1155,vlist1156,vlist1157,vlist1158,vlist1159
 ,vlist1160,vlist1161,vlist1162,vlist1163,vlist1164,vlist1165,vlist1166,vlist1167,vlist1168,vlist1169
 ,vlist1170,vlist1171,vlist1172,vlist1173,vlist1174,vlist1175,vlist1176,vlist1177,vlist1178,vlist1179
 ,vlist1180,vlist1181,vlist1182,vlist1183,vlist1184,vlist1185,vlist1186,vlist1187,vlist1188,vlist1189
 ,vlist1190,vlist1191,vlist1192,vlist1193,vlist1194,vlist1195,vlist1196,vlist1197,vlist1198,vlist1199
 ,vlist1200,vlist1201,vlist1202,vlist1203,vlist1204,vlist1205,vlist1206,vlist1207,vlist1208,vlist1209
 ,vlist1210,vlist1211,vlist1212,vlist1213,vlist1214,vlist1215,vlist1216,vlist1217,vlist1218,vlist1219
 ,vlist1220,vlist1221,vlist1222,vlist1223,vlist1224,vlist1225,vlist1226,vlist1227,vlist1228,vlist1229
 ,vlist1230,vlist1231,vlist1232,vlist1233,vlist1234,vlist1235,vlist1236,vlist1237,vlist1238,vlist1239
 ,vlist1240,vlist1241,vlist1242,vlist1243,vlist1244,vlist1245,vlist1246,vlist1247,vlist1248,vlist1249
 ,vlist1250,vlist1251,vlist1252,vlist1253,vlist1254,vlist1255,vlist1256,vlist1257,vlist1258,vlist1259
 ,vlist1260,vlist1261,vlist1262,vlist1263,vlist1264,vlist1265,vlist1266,vlist1267,vlist1268,vlist1269
 ,vlist1270,vlist1271,vlist1272,vlist1273,vlist1274,vlist1275,vlist1276,vlist1277,vlist1278,vlist1279
 ,vlist1280,vlist1281,vlist1282,vlist1283,vlist1284,vlist1285,vlist1286,vlist1287,vlist1288,vlist1289
 ,vlist1290,vlist1291,vlist1292,vlist1293,vlist1294,vlist1295,vlist1296,vlist1297,vlist1298,vlist1299
 ,vlist1300,vlist1301,vlist1302,vlist1303,vlist1304,vlist1305,vlist1306,vlist1307,vlist1308,vlist1309
 ,vlist1310,vlist1311,vlist1312,vlist1313,vlist1314,vlist1315,vlist1316,vlist1317,vlist1318,vlist1319
 ,vlist1320,vlist1321,vlist1322,vlist1323,vlist1324,vlist1325,vlist1326,vlist1327,vlist1328,vlist1329
 ,vlist1330,vlist1331,vlist1332,vlist1333,vlist1334,vlist1335,vlist1336,vlist1337,vlist1338,vlist1339
 ,vlist1340,vlist1341,vlist1342,vlist1343,vlist1344,vlist1345,vlist1346,vlist1347,vlist1348,vlist1349
 ,vlist1350,vlist1351,vlist1352,vlist1353,vlist1354,vlist1355,vlist1356,vlist1357,vlist1358,vlist1359
 ,vlist1360,vlist1361,vlist1362,vlist1363,vlist1364,vlist1365,vlist1366,vlist1367,vlist1368,vlist1369
 ,vlist1370,vlist1371,vlist1372,vlist1373,vlist1374,vlist1375,vlist1376,vlist1377,vlist1378,vlist1379
 ,vlist1380,vlist1381,vlist1382,vlist1383,vlist1384,vlist1385,vlist1386,vlist1387,vlist1388,vlist1389
 ,vlist1390,vlist1391,vlist1392,vlist1393,vlist1394,vlist1395,vlist1396,vlist1397,vlist1398,vlist1399
 ,vlist1400,vlist1401,vlist1402,vlist1403,vlist1404,vlist1405,vlist1406,vlist1407,vlist1408,vlist1409
 ,vlist1410,vlist1411,vlist1412,vlist1413,vlist1414,vlist1415,vlist1416,vlist1417,vlist1418,vlist1419
 ,vlist1420,vlist1421,vlist1422,vlist1423,vlist1424,vlist1425,vlist1426,vlist1427,vlist1428,vlist1429
 ,vlist1430,vlist1431,vlist1432,vlist1433,vlist1434,vlist1435,vlist1436,vlist1437,vlist1438,vlist1439
 ,vlist1440,vlist1441,vlist1442,vlist1443,vlist1444,vlist1445,vlist1446,vlist1447,vlist1448,vlist1449
 ,vlist1450,vlist1451,vlist1452,vlist1453,vlist1454,vlist1455,vlist1456,vlist1457,vlist1458,vlist1459
 ,vlist1460,vlist1461,vlist1462,vlist1463,vlist1464,vlist1465,vlist1466,vlist1467,vlist1468,vlist1469
 ,vlist1470,vlist1471,vlist1472,vlist1473,vlist1474,vlist1475,vlist1476,vlist1477,vlist1478,vlist1479
 ,vlist1480,vlist1481,vlist1482,vlist1483,vlist1484,vlist1485,vlist1486,vlist1487,vlist1488,vlist1489
 ,vlist1490,vlist1491,vlist1492,vlist1493,vlist1494,vlist1495,vlist1496,vlist1497,vlist1498,vlist1499
 ,vlist1500,vlist1501,vlist1502,vlist1503,vlist1504,vlist1505,vlist1506,vlist1507,vlist1508,vlist1509
 ,vlist1510,vlist1511,vlist1512,vlist1513,vlist1514,vlist1515,vlist1516,vlist1517,vlist1518,vlist1519
 ,vlist1520,vlist1521,vlist1522,vlist1523,vlist1524,vlist1525,vlist1526,vlist1527,vlist1528,vlist1529
 ,vlist1530,vlist1531,vlist1532,vlist1533,vlist1534,vlist1535,vlist1536,vlist1537,vlist1538,vlist1539
 ,vlist1540,vlist1541,vlist1542,vlist1543,vlist1544,vlist1545,vlist1546,vlist1547,vlist1548,vlist1549
 ,vlist1550,vlist1551,vlist1552,vlist1553,vlist1554,vlist1555,vlist1556,vlist1557,vlist1558,vlist1559
 ,vlist1560,vlist1561,vlist1562,vlist1563,vlist1564,vlist1565,vlist1566,vlist1567,vlist1568,vlist1569
 ,vlist1570,vlist1571,vlist1572,vlist1573,vlist1574,vlist1575,vlist1576,vlist1577,vlist1578,vlist1579
 ,vlist1580,vlist1581,vlist1582,vlist1583,vlist1584,vlist1585,vlist1586,vlist1587,vlist1588,vlist1589
 ,vlist1590,vlist1591,vlist1592,vlist1593,vlist1594,vlist1595,vlist1596,vlist1597,vlist1598,vlist1599
 ,vlist1600,vlist1601,vlist1602,vlist1603,vlist1604,vlist1605,vlist1606,vlist1607,vlist1608,vlist1609
 ,vlist1610,vlist1611,vlist1612,vlist1613,vlist1614,vlist1615,vlist1616,vlist1617,vlist1618,vlist1619
 ,vlist1620,vlist1621,vlist1622,vlist1623,vlist1624,vlist1625,vlist1626,vlist1627,vlist1628,vlist1629
 ,vlist1630,vlist1631,vlist1632,vlist1633,vlist1634,vlist1635,vlist1636,vlist1637,vlist1638,vlist1639
 ,vlist1640,vlist1641,vlist1642,vlist1643,vlist1644,vlist1645,vlist1646,vlist1647,vlist1648,vlist1649
 ,vlist1650,vlist1651,vlist1652,vlist1653,vlist1654,vlist1655,vlist1656,vlist1657,vlist1658,vlist1659
 ,vlist1660,vlist1661,vlist1662,vlist1663,vlist1664,vlist1665,vlist1666,vlist1667,vlist1668,vlist1669
 ,vlist1670,vlist1671,vlist1672,vlist1673,vlist1674,vlist1675,vlist1676,vlist1677,vlist1678,vlist1679
 ,vlist1680,vlist1681,vlist1682,vlist1683,vlist1684,vlist1685,vlist1686,vlist1687,vlist1688,vlist1689
 ,vlist1690,vlist1691,vlist1692,vlist1693,vlist1694,vlist1695,vlist1696,vlist1697,vlist1698,vlist1699
 ,vlist1700,vlist1701,vlist1702,vlist1703,vlist1704,vlist1705,vlist1706,vlist1707,vlist1708,vlist1709
 ,vlist1710,vlist1711,vlist1712,vlist1713,vlist1714,vlist1715,vlist1716,vlist1717,vlist1718,vlist1719
 ,vlist1720,vlist1721,vlist1722,vlist1723,vlist1724,vlist1725,vlist1726,vlist1727,vlist1728,vlist1729
 ,vlist1730,vlist1731,vlist1732,vlist1733,vlist1734,vlist1735,vlist1736,vlist1737,vlist1738,vlist1739
 ,vlist1740,vlist1741,vlist1742,vlist1743,vlist1744,vlist1745,vlist1746,vlist1747,vlist1748,vlist1749
 ,vlist1750,vlist1751,vlist1752,vlist1753,vlist1754,vlist1755,vlist1756,vlist1757,vlist1758,vlist1759
 ,vlist1760,vlist1761,vlist1762,vlist1763,vlist1764,vlist1765,vlist1766,vlist1767,vlist1768,vlist1769
 ,vlist1770,vlist1771,vlist1772,vlist1773,vlist1774,vlist1775,vlist1776,vlist1777,vlist1778,vlist1779
 ,vlist1780,vlist1781,vlist1782,vlist1783,vlist1784,vlist1785,vlist1786,vlist1787,vlist1788,vlist1789
 ,vlist1790,vlist1791,vlist1792,vlist1793,vlist1794,vlist1795,vlist1796,vlist1797,vlist1798,vlist1799
 ,vlist1800,vlist1801,vlist1802,vlist1803,vlist1804,vlist1805,vlist1806,vlist1807,vlist1808,vlist1809
 ,vlist1810,vlist1811,vlist1812,vlist1813,vlist1814,vlist1815,vlist1816,vlist1817,vlist1818,vlist1819
 ,vlist1820,vlist1821,vlist1822,vlist1823,vlist1824,vlist1825,vlist1826,vlist1827,vlist1828,vlist1829
 ,vlist1830,vlist1831,vlist1832,vlist1833,vlist1834,vlist1835,vlist1836,vlist1837,vlist1838,vlist1839
 ,vlist1840,vlist1841,vlist1842,vlist1843,vlist1844,vlist1845,vlist1846,vlist1847,vlist1848,vlist1849
 ,vlist1850,vlist1851,vlist1852,vlist1853,vlist1854,vlist1855,vlist1856,vlist1857,vlist1858,vlist1859
 ,vlist1860,vlist1861,vlist1862,vlist1863,vlist1864,vlist1865,vlist1866,vlist1867,vlist1868,vlist1869
 ,vlist1870,vlist1871,vlist1872,vlist1873,vlist1874,vlist1875,vlist1876,vlist1877,vlist1878,vlist1879
 ,vlist1880,vlist1881,vlist1882,vlist1883,vlist1884,vlist1885,vlist1886,vlist1887,vlist1888,vlist1889
 ,vlist1890,vlist1891,vlist1892,vlist1893,vlist1894,vlist1895,vlist1896,vlist1897,vlist1898,vlist1899
 ,vlist1900,vlist1901,vlist1902,vlist1903,vlist1904,vlist1905,vlist1906,vlist1907,vlist1908,vlist1909
 ,vlist1910,vlist1911,vlist1912,vlist1913,vlist1914,vlist1915,vlist1916,vlist1917,vlist1918,vlist1919
 ,vlist1920,vlist1921,vlist1922,vlist1923,vlist1924,vlist1925,vlist1926,vlist1927,vlist1928,vlist1929
 ,vlist1930,vlist1931,vlist1932,vlist1933,vlist1934,vlist1935,vlist1936,vlist1937,vlist1938,vlist1939
 ,vlist1940,vlist1941,vlist1942,vlist1943,vlist1944,vlist1945,vlist1946,vlist1947,vlist1948,vlist1949
 ,vlist1950,vlist1951,vlist1952,vlist1953,vlist1954,vlist1955,vlist1956,vlist1957,vlist1958,vlist1959
 ,vlist1960,vlist1961,vlist1962,vlist1963,vlist1964,vlist1965,vlist1966,vlist1967,vlist1968,vlist1969
 ,vlist1970,vlist1971,vlist1972,vlist1973,vlist1974,vlist1975,vlist1976,vlist1977,vlist1978,vlist1979
 ,vlist1980,vlist1981,vlist1982,vlist1983,vlist1984,vlist1985,vlist1986,vlist1987,vlist1988,vlist1989
 ,vlist1990,vlist1991,vlist1992,vlist1993,vlist1994,vlist1995,vlist1996,vlist1997,vlist1998,vlist1999
 ,vlist2000,vlist2001,vlist2002,vlist2003,vlist2004,vlist2005,vlist2006,vlist2007,vlist2008,vlist2009
 ,vlist2010,vlist2011,vlist2012,vlist2013,vlist2014,vlist2015,vlist2016,vlist2017,vlist2018,vlist2019
 ,vlist2020,vlist2021,vlist2022,vlist2023,vlist2024,vlist2025,vlist2026,vlist2027,vlist2028,vlist2029
 ,vlist2030,vlist2031,vlist2032,vlist2033,vlist2034,vlist2035,vlist2036,vlist2037,vlist2038,vlist2039
 ,vlist2040,vlist2041,vlist2042,vlist2043,vlist2044,vlist2045,vlist2046,vlist2047,vlist2048,vlist2049
 ,vlist2050,vlist2051,vlist2052,vlist2053,vlist2054,vlist2055,vlist2056,vlist2057,vlist2058,vlist2059
 ,vlist2060,vlist2061,vlist2062,vlist2063,vlist2064,vlist2065,vlist2066,vlist2067,vlist2068,vlist2069
 ,vlist2070,vlist2071,vlist2072,vlist2073,vlist2074,vlist2075,vlist2076,vlist2077,vlist2078,vlist2079
 ,vlist2080,vlist2081,vlist2082,vlist2083,vlist2084,vlist2085,vlist2086,vlist2087,vlist2088,vlist2089
 ,vlist2090,vlist2091,vlist2092,vlist2093,vlist2094,vlist2095,vlist2096,vlist2097,vlist2098,vlist2099
 ,vlist2100,vlist2101,vlist2102,vlist2103,vlist2104,vlist2105,vlist2106,vlist2107,vlist2108,vlist2109
 ,vlist2110,vlist2111,vlist2112,vlist2113,vlist2114,vlist2115,vlist2116,vlist2117,vlist2118,vlist2119
 ,vlist2120,vlist2121,vlist2122,vlist2123,vlist2124,vlist2125,vlist2126,vlist2127,vlist2128,vlist2129
 ,vlist2130,vlist2131,vlist2132,vlist2133,vlist2134,vlist2135,vlist2136,vlist2137,vlist2138,vlist2139
 ,vlist2140,vlist2141,vlist2142,vlist2143,vlist2144,vlist2145,vlist2146,vlist2147,vlist2148,vlist2149
 ,vlist2150,vlist2151,vlist2152,vlist2153,vlist2154,vlist2155,vlist2156,vlist2157,vlist2158,vlist2159
 ,vlist2160,vlist2161,vlist2162,vlist2163,vlist2164,vlist2165,vlist2166,vlist2167,vlist2168,vlist2169
 ,vlist2170,vlist2171,vlist2172,vlist2173,vlist2174,vlist2175,vlist2176,vlist2177,vlist2178,vlist2179
 ,vlist2180,vlist2181,vlist2182,vlist2183,vlist2184,vlist2185,vlist2186,NULL     ,NULL     ,vlist2189
 ,NULL     ,NULL     ,vlist2192,vlist2193,vlist2194,vlist2195,NULL     ,NULL     ,vlist2198,NULL     
 ,vlist2200,vlist2201,vlist2202,vlist2203,vlist2204,vlist2205,vlist2206,vlist2207,vlist2208,vlist2209
 ,vlist2210,vlist2211,vlist2212,vlist2213,NULL     ,NULL     ,vlist2216,vlist2217,vlist2218,vlist2219
 ,vlist2220,vlist2221,vlist2222,vlist2223,vlist2224,vlist2225,vlist2226,vlist2227,vlist2228,vlist2229
 ,vlist2230,vlist2231,vlist2232,vlist2233,vlist2234,vlist2235,vlist2236,vlist2237,vlist2238,vlist2239
 ,vlist2240,vlist2241,vlist2242,vlist2243,vlist2244,vlist2245,vlist2246,vlist2247,vlist2248,vlist2249
 ,vlist2250,vlist2251,vlist2252,vlist2253,vlist2254,vlist2255,vlist2256,vlist2257,vlist2258,vlist2259
 ,vlist2260,vlist2261,vlist2262,vlist2263,vlist2264,vlist2265,vlist2266,vlist2267,NULL     ,NULL     
 ,vlist2270,NULL     ,vlist2272,vlist2273,vlist2274,vlist2275,vlist2276,NULL     ,vlist2278,vlist2279
 ,vlist2280,vlist2281,vlist2282,vlist2283,vlist2284,vlist2285,vlist2286,vlist2287,vlist2288,vlist2289
 ,vlist2290,vlist2291,vlist2292,vlist2293,vlist2294,NULL     ,NULL     ,vlist2297,vlist2298,vlist2299
 ,vlist2300,vlist2301,vlist2302,vlist2303,vlist2304,vlist2305,vlist2306,vlist2307,vlist2308,vlist2309
 ,vlist2310,vlist2311,vlist2312,vlist2313,vlist2314,vlist2315,vlist2316,vlist2317,vlist2318,vlist2319
 ,vlist2320,vlist2321,vlist2322,vlist2323,vlist2324,vlist2325,vlist2326,vlist2327,vlist2328,vlist2329
 ,vlist2330,vlist2331,vlist2332,vlist2333,vlist2334,vlist2335,vlist2336,vlist2337,vlist2338,vlist2339
 ,vlist2340,vlist2341,vlist2342,vlist2343,vlist2344,vlist2345,vlist2346,vlist2347,vlist2348,vlist2349
 ,vlist2350,vlist2351,vlist2352,vlist2353,vlist2354,vlist2355,vlist2356,vlist2357,vlist2358,vlist2359
 ,vlist2360,vlist2361,vlist2362,vlist2363,vlist2364,vlist2365,vlist2366,vlist2367,vlist2368,vlist2369
 ,vlist2370,vlist2371,vlist2372,vlist2373,vlist2374,vlist2375,vlist2376,vlist2377,vlist2378,vlist2379
 ,vlist2380,vlist2381,vlist2382,vlist2383,vlist2384,vlist2385,vlist2386,vlist2387,vlist2388,vlist2389
 ,vlist2390,vlist2391,vlist2392,vlist2393,vlist2394,vlist2395,vlist2396,vlist2397,vlist2398,vlist2399
 ,vlist2400,vlist2401,vlist2402,vlist2403,vlist2404,vlist2405,vlist2406,vlist2407,vlist2408,vlist2409
 ,vlist2410,vlist2411,vlist2412,vlist2413,vlist2414,vlist2415,vlist2416,vlist2417,vlist2418,vlist2419
 ,vlist2420,vlist2421,vlist2422,vlist2423,vlist2424,vlist2425,vlist2426,vlist2427,vlist2428,vlist2429
 ,NULL     ,NULL     ,vlist2432,vlist2433,vlist2434,vlist2435,vlist2436,vlist2437,vlist2438,NULL     
 ,NULL     ,vlist2441,vlist2442,vlist2443,vlist2444,vlist2445,vlist2446,vlist2447,vlist2448,vlist2449
 ,vlist2450,vlist2451,vlist2452,vlist2453,vlist2454,vlist2455,vlist2456,vlist2457,vlist2458,vlist2459
 ,vlist2460,vlist2461,vlist2462,vlist2463,vlist2464,vlist2465,vlist2466,vlist2467,vlist2468,vlist2469
 ,vlist2470,vlist2471,vlist2472,vlist2473,vlist2474,vlist2475,vlist2476,vlist2477,vlist2478,vlist2479
 ,vlist2480,vlist2481,vlist2482,vlist2483,vlist2484,vlist2485,vlist2486,vlist2487,vlist2488,vlist2489
 ,vlist2490,vlist2491,vlist2492,vlist2493,vlist2494,vlist2495,vlist2496,vlist2497,vlist2498,vlist2499
 ,vlist2500,vlist2501,vlist2502,vlist2503,vlist2504,vlist2505,vlist2506,vlist2507,vlist2508,vlist2509
 ,vlist2510,vlist2511,vlist2512,vlist2513,vlist2514,vlist2515,vlist2516,vlist2517,vlist2518,vlist2519
 ,vlist2520,vlist2521,vlist2522,vlist2523,vlist2524,vlist2525,vlist2526,vlist2527,vlist2528,vlist2529
 ,vlist2530,vlist2531,vlist2532,vlist2533,vlist2534,vlist2535,vlist2536,vlist2537,vlist2538,vlist2539
 ,vlist2540,vlist2541,vlist2542,vlist2543,vlist2544,vlist2545,vlist2546,vlist2547,vlist2548,vlist2549
 ,vlist2550,vlist2551,vlist2552,vlist2553,vlist2554,vlist2555,vlist2556,vlist2557,vlist2558,vlist2559
 ,vlist2560,vlist2561,vlist2562,vlist2563,vlist2564,vlist2565,vlist2566,vlist2567,vlist2568,vlist2569
 ,vlist2570,vlist2571,vlist2572,vlist2573,vlist2574,vlist2575,vlist2576,vlist2577,vlist2578,vlist2579
 ,vlist2580,vlist2581,vlist2582,vlist2583,vlist2584,vlist2585,vlist2586,vlist2587,vlist2588,vlist2589
 ,vlist2590,vlist2591,vlist2592,vlist2593,vlist2594,vlist2595,vlist2596,vlist2597,vlist2598,vlist2599
 ,vlist2600,vlist2601,vlist2602,vlist2603,vlist2604,vlist2605,vlist2606,vlist2607,vlist2608,vlist2609
 ,vlist2610,vlist2611,vlist2612,vlist2613,vlist2614,vlist2615,vlist2616,vlist2617,vlist2618,vlist2619
 ,vlist2620,vlist2621,vlist2622,vlist2623,vlist2624,vlist2625,vlist2626,vlist2627,vlist2628,vlist2629
 ,vlist2630,vlist2631,vlist2632,vlist2633,vlist2634,vlist2635,vlist2636,vlist2637,vlist2638,vlist2639
 ,vlist2640,vlist2641,vlist2642,vlist2643,vlist2644,vlist2645,vlist2646,vlist2647,vlist2648,vlist2649
 ,vlist2650,vlist2651,vlist2652,vlist2653,vlist2654,vlist2655,vlist2656,vlist2657,vlist2658,vlist2659
 ,vlist2660,vlist2661,vlist2662,vlist2663,vlist2664,vlist2665,vlist2666,vlist2667,vlist2668,vlist2669
 ,vlist2670,vlist2671,vlist2672,vlist2673,vlist2674,vlist2675,vlist2676,vlist2677,vlist2678,vlist2679
 ,vlist2680,vlist2681,vlist2682,vlist2683,vlist2684,vlist2685,vlist2686,vlist2687,vlist2688,vlist2689
 ,vlist2690,vlist2691,vlist2692,vlist2693,vlist2694,vlist2695,vlist2696,vlist2697,vlist2698,vlist2699
 ,vlist2700,vlist2701,vlist2702,vlist2703,vlist2704,vlist2705,vlist2706,vlist2707,vlist2708,vlist2709
 ,vlist2710,vlist2711,vlist2712,vlist2713,vlist2714,vlist2715,vlist2716,vlist2717,vlist2718,vlist2719
 ,vlist2720,vlist2721,vlist2722,vlist2723,vlist2724,vlist2725,vlist2726,vlist2727,vlist2728,vlist2729
 ,vlist2730,vlist2731,vlist2732,vlist2733,vlist2734,vlist2735,vlist2736,vlist2737,vlist2738,vlist2739
 ,vlist2740,vlist2741,vlist2742,vlist2743,vlist2744,vlist2745,vlist2746,vlist2747,vlist2748,vlist2749
 ,vlist2750,vlist2751,vlist2752,vlist2753,vlist2754,vlist2755,vlist2756,vlist2757,vlist2758,vlist2759
 ,vlist2760,vlist2761,vlist2762,vlist2763,vlist2764,vlist2765,vlist2766,vlist2767,vlist2768,vlist2769
 ,vlist2770,vlist2771,vlist2772,vlist2773,vlist2774,vlist2775,vlist2776,vlist2777,vlist2778,vlist2779
 ,vlist2780,vlist2781,vlist2782,vlist2783,vlist2784,vlist2785,vlist2786,vlist2787,vlist2788,vlist2789
 ,vlist2790,vlist2791,vlist2792,vlist2793,vlist2794,vlist2795,vlist2796,vlist2797,vlist2798,vlist2799
 ,vlist2800,vlist2801,vlist2802,vlist2803,vlist2804,vlist2805,vlist2806,vlist2807,vlist2808,vlist2809
 ,vlist2810,vlist2811,vlist2812,vlist2813,vlist2814,vlist2815,vlist2816,vlist2817,vlist2818,vlist2819
 ,vlist2820,vlist2821,vlist2822,vlist2823,vlist2824,vlist2825,vlist2826,vlist2827,vlist2828,vlist2829
 ,vlist2830,vlist2831,vlist2832,vlist2833,vlist2834,vlist2835,vlist2836,vlist2837,vlist2838,vlist2839
 ,vlist2840,vlist2841,vlist2842,vlist2843,vlist2844,vlist2845,vlist2846,vlist2847,vlist2848,vlist2849
 ,vlist2850,vlist2851,vlist2852,vlist2853,vlist2854,vlist2855,vlist2856,vlist2857,vlist2858,vlist2859
 ,vlist2860,vlist2861,vlist2862,vlist2863,vlist2864,vlist2865,vlist2866,vlist2867,vlist2868,vlist2869
 ,vlist2870,vlist2871,vlist2872,vlist2873,vlist2874,vlist2875,vlist2876,vlist2877,vlist2878,vlist2879
 ,vlist2880,vlist2881,vlist2882,vlist2883,vlist2884,vlist2885,vlist2886,vlist2887,vlist2888,vlist2889
 ,vlist2890,vlist2891,vlist2892,vlist2893,vlist2894,vlist2895,vlist2896,vlist2897,vlist2898,vlist2899
 ,vlist2900,vlist2901,vlist2902,vlist2903,vlist2904,vlist2905,vlist2906,vlist2907,vlist2908,vlist2909
 ,vlist2910,vlist2911,vlist2912,vlist2913,vlist2914,vlist2915,NULL     ,NULL     ,vlist2918,NULL     
 ,NULL     ,vlist2921,vlist2922,vlist2923,vlist2924,vlist2925,vlist2926,vlist2927,vlist2928,vlist2929
 ,vlist2930,vlist2931,vlist2932,vlist2933,vlist2934,vlist2935,vlist2936,vlist2937,vlist2938,vlist2939
 ,vlist2940,vlist2941,vlist2942,vlist2943,vlist2944,vlist2945,vlist2946,vlist2947,vlist2948,vlist2949
 ,vlist2950,vlist2951,vlist2952,vlist2953,vlist2954,vlist2955,vlist2956,vlist2957,vlist2958,vlist2959
 ,vlist2960,vlist2961,vlist2962,vlist2963,vlist2964,vlist2965,vlist2966,vlist2967,vlist2968,vlist2969
 ,vlist2970,vlist2971,vlist2972,vlist2973,vlist2974,vlist2975,vlist2976,vlist2977,vlist2978,vlist2979
 ,vlist2980,vlist2981,vlist2982,vlist2983,vlist2984,vlist2985,vlist2986,vlist2987,vlist2988,vlist2989
 ,vlist2990,vlist2991,vlist2992,vlist2993,vlist2994,vlist2995,vlist2996,vlist2997,vlist2998,vlist2999
 ,vlist3000,vlist3001,vlist3002,vlist3003,vlist3004,vlist3005,vlist3006,vlist3007,vlist3008,vlist3009
 ,vlist3010,vlist3011,vlist3012,vlist3013,vlist3014,vlist3015,vlist3016,vlist3017,vlist3018,vlist3019
 ,vlist3020,vlist3021,vlist3022,vlist3023,vlist3024,vlist3025,vlist3026,vlist3027,vlist3028,vlist3029
 ,vlist3030,vlist3031,vlist3032,vlist3033,vlist3034,vlist3035,vlist3036,vlist3037,vlist3038,vlist3039
 ,vlist3040,vlist3041,vlist3042,vlist3043,vlist3044,vlist3045,vlist3046,vlist3047,vlist3048,vlist3049
 ,vlist3050,vlist3051,vlist3052,vlist3053,vlist3054,vlist3055,vlist3056,vlist3057,vlist3058,vlist3059
 ,vlist3060,vlist3061,vlist3062,vlist3063,vlist3064,vlist3065,vlist3066,vlist3067,vlist3068,vlist3069
 ,vlist3070,vlist3071,vlist3072,vlist3073,vlist3074,vlist3075,vlist3076,vlist3077,vlist3078,vlist3079
 ,vlist3080,vlist3081,vlist3082,vlist3083,vlist3084,vlist3085,vlist3086,vlist3087,vlist3088,vlist3089
 ,vlist3090,vlist3091,vlist3092,vlist3093,vlist3094,vlist3095,vlist3096,vlist3097,vlist3098,vlist3099
 ,vlist3100,vlist3101,vlist3102,vlist3103,vlist3104,vlist3105,vlist3106,vlist3107,vlist3108,vlist3109
 ,vlist3110,vlist3111,vlist3112,vlist3113,vlist3114,vlist3115,vlist3116,vlist3117,vlist3118,vlist3119
 ,vlist3120,vlist3121,vlist3122,vlist3123,vlist3124,vlist3125,vlist3126,vlist3127,vlist3128,vlist3129
 ,vlist3130,vlist3131,vlist3132,vlist3133,vlist3134,vlist3135,vlist3136,vlist3137,vlist3138,vlist3139
 ,vlist3140,vlist3141,vlist3142,vlist3143,vlist3144,vlist3145,vlist3146,vlist3147,vlist3148,vlist3149
 ,vlist3150,vlist3151,vlist3152,vlist3153,vlist3154,vlist3155,vlist3156,vlist3157,vlist3158,vlist3159
 ,vlist3160,vlist3161,vlist3162,vlist3163,vlist3164,vlist3165,vlist3166,vlist3167,vlist3168,vlist3169
 ,vlist3170,vlist3171,vlist3172,vlist3173,vlist3174,vlist3175,vlist3176,vlist3177,vlist3178,vlist3179
 ,vlist3180,vlist3181,vlist3182,vlist3183,vlist3184,vlist3185,vlist3186,vlist3187,vlist3188,vlist3189
 ,vlist3190,vlist3191,vlist3192,vlist3193,vlist3194,vlist3195,vlist3196,vlist3197,vlist3198,vlist3199
 ,vlist3200,vlist3201,vlist3202,vlist3203,vlist3204,vlist3205,vlist3206,vlist3207,vlist3208,vlist3209
 ,vlist3210,vlist3211,vlist3212,vlist3213,vlist3214,vlist3215,vlist3216,vlist3217,vlist3218,vlist3219
 ,vlist3220,vlist3221,vlist3222,vlist3223,vlist3224,vlist3225,vlist3226,vlist3227,vlist3228,vlist3229
 ,vlist3230,vlist3231,vlist3232,vlist3233,vlist3234,vlist3235,vlist3236,vlist3237,vlist3238,vlist3239
 ,vlist3240,vlist3241,vlist3242,vlist3243,vlist3244,vlist3245,vlist3246,vlist3247,vlist3248,vlist3249
 ,vlist3250,vlist3251,vlist3252,vlist3253,vlist3254,vlist3255,vlist3256,vlist3257,vlist3258,vlist3259
 ,vlist3260,vlist3261,vlist3262,vlist3263,vlist3264,vlist3265,vlist3266,vlist3267,vlist3268,vlist3269
 ,vlist3270,vlist3271,vlist3272,vlist3273,vlist3274,vlist3275,vlist3276,vlist3277,vlist3278,vlist3279
 ,NULL     ,NULL     ,vlist3282,NULL     ,NULL     ,vlist3285,vlist3286,vlist3287,vlist3288,NULL     
 ,NULL     ,vlist3291,NULL     ,NULL     ,vlist3294,vlist3295,vlist3296,vlist3297,vlist3298,vlist3299
 ,vlist3300,vlist3301,vlist3302,vlist3303,vlist3304,vlist3305,vlist3306,NULL     ,NULL     ,vlist3309
 ,NULL     ,NULL     ,vlist3312,vlist3313,vlist3314,vlist3315,NULL     ,NULL     ,vlist3318,NULL     
 ,NULL     ,vlist3321,vlist3322,vlist3323,vlist3324,vlist3325,vlist3326,vlist3327,vlist3328,vlist3329
 ,vlist3330,vlist3331,vlist3332,vlist3333,vlist3334,vlist3335,vlist3336,vlist3337,vlist3338,vlist3339
 ,vlist3340,vlist3341,vlist3342,vlist3343,vlist3344,vlist3345,vlist3346,vlist3347,vlist3348,vlist3349
 ,vlist3350,vlist3351,vlist3352,vlist3353,vlist3354,vlist3355,vlist3356,vlist3357,vlist3358,vlist3359
 ,vlist3360,NULL     ,NULL     ,vlist3363,NULL     ,NULL     ,vlist3366,vlist3367,vlist3368,vlist3369
 ,NULL     ,NULL     ,vlist3372,NULL     ,NULL     ,vlist3375,vlist3376,vlist3377,vlist3378,vlist3379
 ,vlist3380,vlist3381,vlist3382,vlist3383,vlist3384,vlist3385,vlist3386,vlist3387,NULL     ,NULL     
 ,vlist3390,NULL     ,NULL     ,vlist3393,vlist3394,vlist3395,vlist3396,NULL     ,NULL     ,vlist3399
 ,NULL     ,NULL     ,vlist3402,vlist3403,vlist3404,vlist3405,vlist3406,vlist3407,vlist3408,vlist3409
 ,vlist3410,vlist3411,vlist3412,vlist3413,vlist3414,vlist3415,vlist3416,vlist3417,vlist3418,vlist3419
 ,vlist3420,vlist3421,vlist3422,vlist3423,vlist3424,vlist3425,vlist3426,vlist3427,vlist3428,vlist3429
 ,vlist3430,vlist3431,vlist3432,vlist3433,vlist3434,vlist3435,vlist3436,vlist3437,vlist3438,vlist3439
 ,vlist3440,vlist3441,vlist3442,vlist3443,vlist3444,vlist3445,vlist3446,vlist3447,vlist3448,vlist3449
 ,vlist3450,vlist3451,vlist3452,vlist3453,vlist3454,vlist3455,vlist3456,vlist3457,vlist3458,vlist3459
 ,vlist3460,vlist3461,vlist3462,vlist3463,vlist3464,vlist3465,vlist3466,vlist3467,vlist3468,vlist3469
 ,vlist3470,vlist3471,vlist3472,vlist3473,vlist3474,vlist3475,vlist3476,vlist3477,vlist3478,vlist3479
 ,vlist3480,vlist3481,vlist3482,vlist3483,vlist3484,vlist3485,vlist3486,vlist3487,vlist3488,vlist3489
 ,vlist3490,vlist3491,vlist3492,vlist3493,vlist3494,vlist3495,vlist3496,vlist3497,vlist3498,vlist3499
 ,vlist3500,vlist3501,vlist3502,vlist3503,vlist3504,vlist3505,vlist3506,vlist3507,vlist3508,vlist3509
 ,vlist3510,vlist3511,vlist3512,vlist3513,vlist3514,vlist3515,vlist3516,vlist3517,vlist3518,vlist3519
 ,vlist3520,vlist3521,vlist3522,NULL     ,NULL     ,vlist3525,NULL     ,NULL     ,vlist3528,vlist3529
 ,vlist3530,vlist3531,NULL     ,NULL     ,vlist3534,NULL     ,NULL     ,vlist3537,vlist3538,vlist3539
 ,vlist3540,vlist3541,vlist3542,vlist3543,vlist3544,vlist3545,vlist3546,vlist3547,vlist3548,vlist3549
 ,NULL     ,NULL     ,vlist3552,NULL     ,NULL     ,vlist3555,vlist3556,vlist3557,vlist3558,NULL     
 ,NULL     ,vlist3561,NULL     ,NULL     ,vlist3564,vlist3565,vlist3566,vlist3567,vlist3568,vlist3569
 ,vlist3570,vlist3571,vlist3572,vlist3573,vlist3574,vlist3575,vlist3576,vlist3577,vlist3578,vlist3579
 ,vlist3580,vlist3581,vlist3582,vlist3583,vlist3584,vlist3585,vlist3586,vlist3587,vlist3588,vlist3589
 ,vlist3590,vlist3591,vlist3592,vlist3593,vlist3594,vlist3595,vlist3596,vlist3597,vlist3598,vlist3599
 ,vlist3600,vlist3601,vlist3602,vlist3603,NULL     ,NULL     ,vlist3606,NULL     ,NULL     ,vlist3609
 ,vlist3610,vlist3611,vlist3612,NULL     ,NULL     ,vlist3615,NULL     ,NULL     ,vlist3618,vlist3619
 ,vlist3620,vlist3621,vlist3622,vlist3623,vlist3624,vlist3625,vlist3626,vlist3627,vlist3628,vlist3629
 ,vlist3630,NULL     ,NULL     ,vlist3633,NULL     ,NULL     ,vlist3636,vlist3637,vlist3638,vlist3639
 ,NULL     ,NULL     ,vlist3642,NULL     ,NULL     ,vlist3645,vlist3646,vlist3647,vlist3648,vlist3649
 ,vlist3650,vlist3651,vlist3652,vlist3653,vlist3654,vlist3655,vlist3656,vlist3657,vlist3658,vlist3659
 ,vlist3660,vlist3661,vlist3662,vlist3663,vlist3664,vlist3665,vlist3666,vlist3667,vlist3668,vlist3669
 ,vlist3670,vlist3671,vlist3672,vlist3673,vlist3674,vlist3675,vlist3676,vlist3677,vlist3678,vlist3679
 ,vlist3680,vlist3681,vlist3682,vlist3683,vlist3684,vlist3685,vlist3686,vlist3687,vlist3688,vlist3689
 ,vlist3690,vlist3691,vlist3692,vlist3693,vlist3694,vlist3695,vlist3696,vlist3697,vlist3698,vlist3699
 ,vlist3700,vlist3701,vlist3702,vlist3703,vlist3704,vlist3705,vlist3706,vlist3707,vlist3708,vlist3709
 ,vlist3710,vlist3711,vlist3712,vlist3713,vlist3714,vlist3715,vlist3716,vlist3717,vlist3718,vlist3719
 ,vlist3720,vlist3721,vlist3722,vlist3723,vlist3724,vlist3725,vlist3726,vlist3727,vlist3728,vlist3729
 ,vlist3730,vlist3731,vlist3732,vlist3733,vlist3734,vlist3735,vlist3736,vlist3737,vlist3738,vlist3739
 ,vlist3740,vlist3741,vlist3742,vlist3743,vlist3744,vlist3745,vlist3746,vlist3747,vlist3748,vlist3749
 ,vlist3750,vlist3751,vlist3752,vlist3753,vlist3754,vlist3755,vlist3756,vlist3757,vlist3758,vlist3759
 ,vlist3760,vlist3761,vlist3762,vlist3763,vlist3764,vlist3765,vlist3766,vlist3767,vlist3768,vlist3769
 ,vlist3770,vlist3771,vlist3772,vlist3773,vlist3774,vlist3775,vlist3776,vlist3777,vlist3778,vlist3779
 ,vlist3780,vlist3781,vlist3782,vlist3783,vlist3784,vlist3785,vlist3786,vlist3787,vlist3788,vlist3789
 ,vlist3790,vlist3791,vlist3792,vlist3793,vlist3794,vlist3795,vlist3796,vlist3797,vlist3798,vlist3799
 ,vlist3800,vlist3801,vlist3802,vlist3803,vlist3804,vlist3805,vlist3806,vlist3807,vlist3808,vlist3809
 ,vlist3810,vlist3811,vlist3812,vlist3813,vlist3814,vlist3815,vlist3816,vlist3817,vlist3818,vlist3819
 ,vlist3820,vlist3821,vlist3822,vlist3823,vlist3824,vlist3825,vlist3826,vlist3827,vlist3828,vlist3829
 ,vlist3830,vlist3831,vlist3832,vlist3833,vlist3834,vlist3835,vlist3836,vlist3837,vlist3838,vlist3839
 ,vlist3840,vlist3841,vlist3842,vlist3843,vlist3844,vlist3845,vlist3846,vlist3847,vlist3848,vlist3849
 ,vlist3850,vlist3851,vlist3852,vlist3853,vlist3854,vlist3855,vlist3856,vlist3857,vlist3858,vlist3859
 ,vlist3860,vlist3861,vlist3862,vlist3863,vlist3864,vlist3865,vlist3866,vlist3867,vlist3868,vlist3869
 ,vlist3870,vlist3871,vlist3872,vlist3873,vlist3874,vlist3875,vlist3876,vlist3877,vlist3878,vlist3879
 ,vlist3880,vlist3881,vlist3882,vlist3883,vlist3884,vlist3885,vlist3886,vlist3887,vlist3888,vlist3889
 ,vlist3890,vlist3891,vlist3892,vlist3893,vlist3894,vlist3895,vlist3896,vlist3897,vlist3898,vlist3899
 ,vlist3900,vlist3901,vlist3902,vlist3903,vlist3904,vlist3905,vlist3906,vlist3907,vlist3908,vlist3909
 ,vlist3910,vlist3911,vlist3912,vlist3913,vlist3914,vlist3915,vlist3916,vlist3917,vlist3918,vlist3919
 ,vlist3920,vlist3921,vlist3922,vlist3923,vlist3924,vlist3925,vlist3926,vlist3927,vlist3928,vlist3929
 ,vlist3930,vlist3931,vlist3932,vlist3933,vlist3934,vlist3935,vlist3936,vlist3937,vlist3938,vlist3939
 ,vlist3940,vlist3941,vlist3942,vlist3943,vlist3944,vlist3945,vlist3946,vlist3947,vlist3948,vlist3949
 ,vlist3950,vlist3951,vlist3952,vlist3953,vlist3954,vlist3955,vlist3956,vlist3957,vlist3958,vlist3959
 ,vlist3960,vlist3961,vlist3962,vlist3963,vlist3964,vlist3965,vlist3966,vlist3967,vlist3968,vlist3969
 ,vlist3970,vlist3971,vlist3972,vlist3973,vlist3974,vlist3975,vlist3976,vlist3977,vlist3978,vlist3979
 ,vlist3980,vlist3981,vlist3982,vlist3983,vlist3984,vlist3985,vlist3986,vlist3987,vlist3988,vlist3989
 ,vlist3990,vlist3991,vlist3992,vlist3993,vlist3994,vlist3995,vlist3996,vlist3997,vlist3998,vlist3999
 ,vlist4000,vlist4001,vlist4002,vlist4003,vlist4004,vlist4005,vlist4006,vlist4007,vlist4008,NULL     
 ,NULL     ,vlist4011,NULL     ,NULL     ,vlist4014,vlist4015,vlist4016,vlist4017,NULL     ,NULL     
 ,vlist4020,NULL     ,NULL     ,vlist4023,vlist4024,vlist4025,vlist4026,vlist4027,vlist4028,vlist4029
 ,vlist4030,vlist4031,vlist4032,vlist4033,vlist4034,vlist4035,NULL     ,NULL     ,vlist4038,NULL     
 ,NULL     ,vlist4041,vlist4042,vlist4043,vlist4044,NULL     ,NULL     ,vlist4047,NULL     ,NULL     
 ,vlist4050,vlist4051,vlist4052,vlist4053,vlist4054,vlist4055,vlist4056,vlist4057,vlist4058,vlist4059
 ,vlist4060,vlist4061,vlist4062,vlist4063,vlist4064,vlist4065,vlist4066,vlist4067,vlist4068,vlist4069
 ,vlist4070,vlist4071,vlist4072,vlist4073,vlist4074,vlist4075,vlist4076,vlist4077,vlist4078,vlist4079
 ,vlist4080,vlist4081,vlist4082,vlist4083,vlist4084,vlist4085,vlist4086,vlist4087,vlist4088,vlist4089
 ,NULL     ,NULL     ,vlist4092,NULL     ,NULL     ,vlist4095,vlist4096,vlist4097,vlist4098,NULL     
 ,NULL     ,vlist4101,NULL     ,NULL     ,vlist4104,vlist4105,vlist4106,vlist4107,vlist4108,vlist4109
 ,vlist4110,vlist4111,vlist4112,vlist4113,vlist4114,vlist4115,vlist4116,NULL     ,NULL     ,vlist4119
 ,NULL     ,NULL     ,vlist4122,vlist4123,vlist4124,vlist4125,NULL     ,NULL     ,vlist4128,NULL     
 ,NULL     ,vlist4131,vlist4132,vlist4133,vlist4134,vlist4135,vlist4136,vlist4137,vlist4138,vlist4139
 ,vlist4140,vlist4141,vlist4142,vlist4143,vlist4144,vlist4145,vlist4146,vlist4147,vlist4148,vlist4149
 ,vlist4150,vlist4151,vlist4152,vlist4153,vlist4154,vlist4155,vlist4156,vlist4157,vlist4158,vlist4159
 ,vlist4160,vlist4161,vlist4162,vlist4163,vlist4164,vlist4165,vlist4166,vlist4167,vlist4168,vlist4169
 ,vlist4170,vlist4171,vlist4172,vlist4173,vlist4174,vlist4175,vlist4176,vlist4177,vlist4178,vlist4179
 ,vlist4180,vlist4181,vlist4182,vlist4183,vlist4184,vlist4185,vlist4186,vlist4187,vlist4188,vlist4189
 ,vlist4190,vlist4191,vlist4192,vlist4193,vlist4194,vlist4195,vlist4196,vlist4197,vlist4198,vlist4199
 ,vlist4200,vlist4201,vlist4202,vlist4203,vlist4204,vlist4205,vlist4206,vlist4207,vlist4208,vlist4209
 ,vlist4210,vlist4211,vlist4212,vlist4213,vlist4214,vlist4215,vlist4216,vlist4217,vlist4218,vlist4219
 ,vlist4220,vlist4221,vlist4222,vlist4223,vlist4224,vlist4225,vlist4226,vlist4227,vlist4228,vlist4229
 ,vlist4230,vlist4231,vlist4232,vlist4233,vlist4234,vlist4235,vlist4236,vlist4237,vlist4238,vlist4239
 ,vlist4240,vlist4241,vlist4242,vlist4243,vlist4244,vlist4245,vlist4246,vlist4247,vlist4248,vlist4249
 ,vlist4250,vlist4251,NULL     ,NULL     ,vlist4254,NULL     ,NULL     ,vlist4257,vlist4258,vlist4259
 ,vlist4260,NULL     ,NULL     ,vlist4263,NULL     ,NULL     ,vlist4266,vlist4267,vlist4268,vlist4269
 ,vlist4270,vlist4271,vlist4272,vlist4273,vlist4274,vlist4275,vlist4276,vlist4277,vlist4278,NULL     
 ,NULL     ,vlist4281,NULL     ,NULL     ,vlist4284,vlist4285,vlist4286,vlist4287,NULL     ,NULL     
 ,vlist4290,NULL     ,NULL     ,vlist4293,vlist4294,vlist4295,vlist4296,vlist4297,vlist4298,vlist4299
 ,vlist4300,vlist4301,vlist4302,vlist4303,vlist4304,vlist4305,vlist4306,vlist4307,vlist4308,vlist4309
 ,vlist4310,vlist4311,vlist4312,vlist4313,vlist4314,vlist4315,vlist4316,vlist4317,vlist4318,vlist4319
 ,vlist4320,vlist4321,vlist4322,vlist4323,vlist4324,vlist4325,vlist4326,vlist4327,vlist4328,vlist4329
 ,vlist4330,vlist4331,vlist4332,NULL     ,NULL     ,vlist4335,NULL     ,NULL     ,vlist4338,vlist4339
 ,vlist4340,vlist4341,NULL     ,NULL     ,vlist4344,NULL     ,NULL     ,vlist4347,vlist4348,vlist4349
 ,vlist4350,vlist4351,vlist4352,vlist4353,vlist4354,vlist4355,vlist4356,vlist4357,vlist4358,vlist4359
 ,NULL     ,NULL     ,vlist4362,NULL     ,NULL     ,vlist4365,vlist4366,vlist4367,vlist4368,NULL     
 ,NULL     ,vlist4371,NULL     ,NULL     ,vlist4374,vlist4375,vlist4376,vlist4377,vlist4378,vlist4379
 ,vlist4380,vlist4381,vlist4382,vlist4383,vlist4384,vlist4385,vlist4386,vlist4387,vlist4388,vlist4389
 ,vlist4390,vlist4391,vlist4392,vlist4393,vlist4394,vlist4395,vlist4396,vlist4397,vlist4398,vlist4399
 ,vlist4400,vlist4401,vlist4402,vlist4403,vlist4404,vlist4405,vlist4406,vlist4407,vlist4408,vlist4409
 ,vlist4410,vlist4411,vlist4412,vlist4413,vlist4414,vlist4415,vlist4416,vlist4417,vlist4418,vlist4419
 ,vlist4420,vlist4421,vlist4422,vlist4423,vlist4424,vlist4425,vlist4426,vlist4427,vlist4428,vlist4429
 ,vlist4430,vlist4431,vlist4432,vlist4433,vlist4434,vlist4435,vlist4436,vlist4437,vlist4438,vlist4439
 ,vlist4440,vlist4441,vlist4442,vlist4443,vlist4444,vlist4445,vlist4446,vlist4447,vlist4448,vlist4449
 ,vlist4450,vlist4451,vlist4452,vlist4453,vlist4454,vlist4455,vlist4456,vlist4457,vlist4458,vlist4459
 ,vlist4460,vlist4461,vlist4462,vlist4463,vlist4464,vlist4465,vlist4466,vlist4467,vlist4468,vlist4469
 ,vlist4470,vlist4471,vlist4472,vlist4473,vlist4474,vlist4475,vlist4476,vlist4477,vlist4478,vlist4479
 ,vlist4480,vlist4481,vlist4482,vlist4483,vlist4484,vlist4485,vlist4486,vlist4487,vlist4488,vlist4489
 ,vlist4490,vlist4491,vlist4492,vlist4493,vlist4494,vlist4495,vlist4496,vlist4497,vlist4498,vlist4499
 ,vlist4500,vlist4501,vlist4502,vlist4503,vlist4504,vlist4505,vlist4506,vlist4507,vlist4508,vlist4509
 ,vlist4510,vlist4511,vlist4512,vlist4513,vlist4514,vlist4515,vlist4516,vlist4517,vlist4518,vlist4519
 ,vlist4520,vlist4521,vlist4522,vlist4523,vlist4524,vlist4525,vlist4526,vlist4527,vlist4528,vlist4529
 ,vlist4530,vlist4531,vlist4532,vlist4533,vlist4534,vlist4535,vlist4536,vlist4537,vlist4538,vlist4539
 ,vlist4540,vlist4541,vlist4542,vlist4543,vlist4544,vlist4545,vlist4546,vlist4547,vlist4548,vlist4549
 ,vlist4550,vlist4551,vlist4552,vlist4553,vlist4554,vlist4555,vlist4556,vlist4557,vlist4558,vlist4559
 ,vlist4560,vlist4561,vlist4562,vlist4563,vlist4564,vlist4565,vlist4566,vlist4567,vlist4568,vlist4569
 ,vlist4570,vlist4571,vlist4572,vlist4573,vlist4574,vlist4575,vlist4576,vlist4577,vlist4578,vlist4579
 ,vlist4580,vlist4581,vlist4582,vlist4583,vlist4584,vlist4585,vlist4586,vlist4587,vlist4588,vlist4589
 ,vlist4590,vlist4591,vlist4592,vlist4593,vlist4594,vlist4595,vlist4596,vlist4597,vlist4598,vlist4599
 ,vlist4600,vlist4601,vlist4602,vlist4603,vlist4604,vlist4605,vlist4606,vlist4607,vlist4608,vlist4609
 ,vlist4610,vlist4611,vlist4612,vlist4613,vlist4614,vlist4615,vlist4616,vlist4617,vlist4618,vlist4619
 ,vlist4620,vlist4621,vlist4622,vlist4623,vlist4624,vlist4625,vlist4626,vlist4627,vlist4628,vlist4629
 ,vlist4630,vlist4631,vlist4632,vlist4633,vlist4634,vlist4635,vlist4636,vlist4637,vlist4638,vlist4639
 ,vlist4640,vlist4641,vlist4642,vlist4643,vlist4644,vlist4645,vlist4646,vlist4647,vlist4648,vlist4649
 ,vlist4650,vlist4651,vlist4652,vlist4653,vlist4654,vlist4655,vlist4656,vlist4657,vlist4658,vlist4659
 ,vlist4660,vlist4661,vlist4662,vlist4663,vlist4664,vlist4665,vlist4666,vlist4667,vlist4668,vlist4669
 ,vlist4670,vlist4671,vlist4672,vlist4673,vlist4674,vlist4675,vlist4676,vlist4677,vlist4678,vlist4679
 ,vlist4680,vlist4681,vlist4682,vlist4683,vlist4684,vlist4685,vlist4686,vlist4687,vlist4688,vlist4689
 ,vlist4690,vlist4691,vlist4692,vlist4693,vlist4694,vlist4695,vlist4696,vlist4697,vlist4698,vlist4699
 ,vlist4700,vlist4701,vlist4702,vlist4703,vlist4704,vlist4705,vlist4706,vlist4707,vlist4708,vlist4709
 ,vlist4710,vlist4711,vlist4712,vlist4713,vlist4714,vlist4715,vlist4716,vlist4717,vlist4718,vlist4719
 ,vlist4720,vlist4721,vlist4722,vlist4723,vlist4724,vlist4725,vlist4726,vlist4727,vlist4728,vlist4729
 ,vlist4730,vlist4731,vlist4732,vlist4733,vlist4734,vlist4735,vlist4736,vlist4737,vlist4738,vlist4739
 ,vlist4740,vlist4741,vlist4742,vlist4743,vlist4744,vlist4745,vlist4746,vlist4747,vlist4748,vlist4749
 ,vlist4750,vlist4751,vlist4752,vlist4753,vlist4754,vlist4755,vlist4756,vlist4757,vlist4758,vlist4759
 ,vlist4760,vlist4761,vlist4762,vlist4763,vlist4764,vlist4765,vlist4766,vlist4767,vlist4768,vlist4769
 ,vlist4770,vlist4771,vlist4772,vlist4773,vlist4774,vlist4775,vlist4776,vlist4777,vlist4778,vlist4779
 ,vlist4780,vlist4781,vlist4782,vlist4783,vlist4784,vlist4785,vlist4786,vlist4787,vlist4788,vlist4789
 ,vlist4790,vlist4791,vlist4792,vlist4793,vlist4794,vlist4795,vlist4796,vlist4797,vlist4798,vlist4799
 ,vlist4800,vlist4801,vlist4802,vlist4803,vlist4804,vlist4805,vlist4806,vlist4807,vlist4808,vlist4809
 ,vlist4810,vlist4811,vlist4812,vlist4813,vlist4814,vlist4815,vlist4816,vlist4817,vlist4818,vlist4819
 ,vlist4820,vlist4821,vlist4822,vlist4823,vlist4824,vlist4825,vlist4826,vlist4827,vlist4828,vlist4829
 ,vlist4830,vlist4831,vlist4832,vlist4833,vlist4834,vlist4835,vlist4836,vlist4837,vlist4838,vlist4839
 ,vlist4840,vlist4841,vlist4842,vlist4843,vlist4844,vlist4845,vlist4846,vlist4847,vlist4848,vlist4849
 ,vlist4850,vlist4851,vlist4852,vlist4853,vlist4854,vlist4855,vlist4856,vlist4857,vlist4858,vlist4859
 ,vlist4860,vlist4861,vlist4862,vlist4863,vlist4864,vlist4865,vlist4866,vlist4867,vlist4868,vlist4869
 ,vlist4870,vlist4871,vlist4872,vlist4873,vlist4874,vlist4875,vlist4876,vlist4877,vlist4878,vlist4879
 ,vlist4880,vlist4881,vlist4882,vlist4883,vlist4884,vlist4885,vlist4886,vlist4887,vlist4888,vlist4889
 ,vlist4890,vlist4891,vlist4892,vlist4893,vlist4894,vlist4895,vlist4896,vlist4897,vlist4898,vlist4899
 ,vlist4900,vlist4901,vlist4902,vlist4903,vlist4904,vlist4905,vlist4906,vlist4907,vlist4908,vlist4909
 ,vlist4910,vlist4911,vlist4912,vlist4913,vlist4914,vlist4915,vlist4916,vlist4917,vlist4918,vlist4919
 ,vlist4920,vlist4921,vlist4922,vlist4923,vlist4924,vlist4925,vlist4926,vlist4927,vlist4928,vlist4929
 ,vlist4930,vlist4931,vlist4932,vlist4933,vlist4934,vlist4935,vlist4936,vlist4937,vlist4938,vlist4939
 ,vlist4940,vlist4941,vlist4942,vlist4943,vlist4944,vlist4945,vlist4946,vlist4947,vlist4948,vlist4949
 ,vlist4950,vlist4951,vlist4952,vlist4953,vlist4954,vlist4955,vlist4956,vlist4957,vlist4958,vlist4959
 ,vlist4960,vlist4961,vlist4962,vlist4963,vlist4964,vlist4965,vlist4966,vlist4967,vlist4968,vlist4969
 ,vlist4970,vlist4971,vlist4972,vlist4973,vlist4974,vlist4975,vlist4976,vlist4977,vlist4978,vlist4979
 ,vlist4980,vlist4981,vlist4982,vlist4983,vlist4984,vlist4985,vlist4986,vlist4987,vlist4988,vlist4989
 ,vlist4990,vlist4991,vlist4992,vlist4993,vlist4994,vlist4995,vlist4996,vlist4997,vlist4998,vlist4999
 ,vlist5000,vlist5001,vlist5002,vlist5003,vlist5004,vlist5005,vlist5006,vlist5007,vlist5008,vlist5009
 ,vlist5010,vlist5011,vlist5012,vlist5013,vlist5014,vlist5015,vlist5016,vlist5017,vlist5018,vlist5019
 ,vlist5020,vlist5021,vlist5022,vlist5023,vlist5024,vlist5025,vlist5026,vlist5027,vlist5028,vlist5029
 ,vlist5030,vlist5031,vlist5032,vlist5033,vlist5034,vlist5035,vlist5036,vlist5037,vlist5038,vlist5039
 ,vlist5040,vlist5041,vlist5042,vlist5043,vlist5044,vlist5045,vlist5046,vlist5047,vlist5048,vlist5049
 ,vlist5050,vlist5051,vlist5052,vlist5053,vlist5054,vlist5055,vlist5056,vlist5057,vlist5058,vlist5059
 ,vlist5060,vlist5061,vlist5062,vlist5063,vlist5064,vlist5065,vlist5066,vlist5067,vlist5068,vlist5069
 ,vlist5070,vlist5071,vlist5072,vlist5073,vlist5074,vlist5075,vlist5076,vlist5077,vlist5078,vlist5079
 ,vlist5080,vlist5081,vlist5082,vlist5083,vlist5084,vlist5085,vlist5086,vlist5087,vlist5088,vlist5089
 ,vlist5090,vlist5091,vlist5092,vlist5093,vlist5094,vlist5095,vlist5096,vlist5097,vlist5098,vlist5099
 ,vlist5100,vlist5101,vlist5102,vlist5103,vlist5104,vlist5105,vlist5106,vlist5107,vlist5108,vlist5109
 ,vlist5110,vlist5111,vlist5112,vlist5113,vlist5114,vlist5115,vlist5116,vlist5117,vlist5118,vlist5119
 ,vlist5120,vlist5121,vlist5122,vlist5123,vlist5124,vlist5125,vlist5126,vlist5127,vlist5128,vlist5129
 ,vlist5130,vlist5131,vlist5132,vlist5133,vlist5134,vlist5135,vlist5136,vlist5137,vlist5138,vlist5139
 ,vlist5140,vlist5141,vlist5142,vlist5143,vlist5144,vlist5145,vlist5146,vlist5147,vlist5148,vlist5149
 ,vlist5150,vlist5151,vlist5152,vlist5153,vlist5154,vlist5155,vlist5156,vlist5157,vlist5158,vlist5159
 ,vlist5160,vlist5161,vlist5162,vlist5163,vlist5164,vlist5165,vlist5166,vlist5167,vlist5168,vlist5169
 ,vlist5170,vlist5171,vlist5172,vlist5173,vlist5174,vlist5175,vlist5176,vlist5177,vlist5178,vlist5179
 ,vlist5180,vlist5181,vlist5182,vlist5183,vlist5184,vlist5185,vlist5186,vlist5187,vlist5188,vlist5189
 ,vlist5190,vlist5191,vlist5192,vlist5193,vlist5194,vlist5195,vlist5196,vlist5197,vlist5198,vlist5199
 ,vlist5200,vlist5201,vlist5202,vlist5203,vlist5204,vlist5205,vlist5206,vlist5207,vlist5208,vlist5209
 ,vlist5210,vlist5211,vlist5212,vlist5213,vlist5214,vlist5215,vlist5216,vlist5217,vlist5218,vlist5219
 ,vlist5220,vlist5221,vlist5222,vlist5223,vlist5224,vlist5225,vlist5226,vlist5227,vlist5228,vlist5229
 ,vlist5230,vlist5231,vlist5232,vlist5233,vlist5234,vlist5235,vlist5236,vlist5237,vlist5238,vlist5239
 ,vlist5240,vlist5241,vlist5242,vlist5243,vlist5244,vlist5245,vlist5246,vlist5247,vlist5248,vlist5249
 ,vlist5250,vlist5251,vlist5252,vlist5253,vlist5254,vlist5255,vlist5256,vlist5257,vlist5258,vlist5259
 ,vlist5260,vlist5261,vlist5262,vlist5263,vlist5264,vlist5265,vlist5266,vlist5267,vlist5268,vlist5269
 ,vlist5270,vlist5271,vlist5272,vlist5273,vlist5274,vlist5275,vlist5276,vlist5277,vlist5278,vlist5279
 ,vlist5280,vlist5281,vlist5282,vlist5283,vlist5284,vlist5285,vlist5286,vlist5287,vlist5288,vlist5289
 ,vlist5290,vlist5291,vlist5292,vlist5293,vlist5294,vlist5295,vlist5296,vlist5297,vlist5298,vlist5299
 ,vlist5300,vlist5301,vlist5302,vlist5303,vlist5304,vlist5305,vlist5306,vlist5307,vlist5308,vlist5309
 ,vlist5310,vlist5311,vlist5312,vlist5313,vlist5314,vlist5315,vlist5316,vlist5317,vlist5318,vlist5319
 ,vlist5320,vlist5321,vlist5322,vlist5323,vlist5324,vlist5325,vlist5326,vlist5327,vlist5328,vlist5329
 ,vlist5330,vlist5331,vlist5332,vlist5333,vlist5334,vlist5335,vlist5336,vlist5337,vlist5338,vlist5339
 ,vlist5340,vlist5341,vlist5342,vlist5343,vlist5344,vlist5345,vlist5346,vlist5347,vlist5348,vlist5349
 ,vlist5350,vlist5351,vlist5352,vlist5353,vlist5354,vlist5355,vlist5356,vlist5357,vlist5358,vlist5359
 ,vlist5360,vlist5361,vlist5362,vlist5363,vlist5364,vlist5365,vlist5366,vlist5367,vlist5368,vlist5369
 ,vlist5370,vlist5371,vlist5372,vlist5373,vlist5374,vlist5375,vlist5376,vlist5377,vlist5378,vlist5379
 ,vlist5380,vlist5381,vlist5382,vlist5383,vlist5384,vlist5385,vlist5386,vlist5387,vlist5388,vlist5389
 ,vlist5390,vlist5391,vlist5392,vlist5393,vlist5394,vlist5395,vlist5396,vlist5397,vlist5398,vlist5399
 ,vlist5400,vlist5401,vlist5402,vlist5403,vlist5404,vlist5405,vlist5406,vlist5407,vlist5408,vlist5409
 ,vlist5410,vlist5411,vlist5412,vlist5413,vlist5414,vlist5415,vlist5416,vlist5417,vlist5418,vlist5419
 ,vlist5420,vlist5421,vlist5422,vlist5423,vlist5424,vlist5425,vlist5426,vlist5427,vlist5428,vlist5429
 ,vlist5430,vlist5431,vlist5432,vlist5433,vlist5434,vlist5435,vlist5436,vlist5437,vlist5438,vlist5439
 ,vlist5440,vlist5441,vlist5442,vlist5443,vlist5444,vlist5445,vlist5446,vlist5447,vlist5448,vlist5449
 ,vlist5450,vlist5451,vlist5452,vlist5453,vlist5454,vlist5455,vlist5456,vlist5457,vlist5458,vlist5459
 ,vlist5460,vlist5461,vlist5462,vlist5463,vlist5464,vlist5465,vlist5466,NULL     ,NULL     ,vlist5469
 ,NULL     ,NULL     ,vlist5472,vlist5473,vlist5474,vlist5475,NULL     ,NULL     ,vlist5478,NULL     
 ,NULL     ,vlist5481,vlist5482,vlist5483,vlist5484,vlist5485,vlist5486,vlist5487,vlist5488,vlist5489
 ,vlist5490,vlist5491,vlist5492,vlist5493,NULL     ,NULL     ,vlist5496,NULL     ,NULL     ,vlist5499
 ,vlist5500,vlist5501,vlist5502,NULL     ,NULL     ,vlist5505,NULL     ,NULL     ,vlist5508,vlist5509
 ,vlist5510,vlist5511,vlist5512,vlist5513,vlist5514,vlist5515,vlist5516,vlist5517,vlist5518,vlist5519
 ,vlist5520,vlist5521,vlist5522,vlist5523,vlist5524,vlist5525,vlist5526,vlist5527,vlist5528,vlist5529
 ,vlist5530,vlist5531,vlist5532,vlist5533,vlist5534,vlist5535,vlist5536,vlist5537,vlist5538,vlist5539
 ,vlist5540,vlist5541,vlist5542,vlist5543,vlist5544,vlist5545,vlist5546,vlist5547,NULL     ,NULL     
 ,vlist5550,NULL     ,NULL     ,vlist5553,vlist5554,vlist5555,vlist5556,NULL     ,NULL     ,vlist5559
 ,NULL     ,NULL     ,vlist5562,vlist5563,vlist5564,vlist5565,vlist5566,vlist5567,vlist5568,vlist5569
 ,vlist5570,vlist5571,vlist5572,vlist5573,vlist5574,NULL     ,NULL     ,vlist5577,NULL     ,NULL     
 ,vlist5580,vlist5581,vlist5582,vlist5583,NULL     ,NULL     ,vlist5586,NULL     ,NULL     ,vlist5589
 ,vlist5590,vlist5591,vlist5592,vlist5593,vlist5594,vlist5595,vlist5596,vlist5597,vlist5598,vlist5599
 ,vlist5600,vlist5601,vlist5602,vlist5603,vlist5604,vlist5605,vlist5606,vlist5607,vlist5608,vlist5609
 ,vlist5610,vlist5611,vlist5612,vlist5613,vlist5614,vlist5615,vlist5616,vlist5617,vlist5618,vlist5619
 ,vlist5620,vlist5621,vlist5622,vlist5623,vlist5624,vlist5625,vlist5626,vlist5627,vlist5628,vlist5629
 ,vlist5630,vlist5631,vlist5632,vlist5633,vlist5634,vlist5635,vlist5636,vlist5637,vlist5638,vlist5639
 ,vlist5640,vlist5641,vlist5642,vlist5643,vlist5644,vlist5645,vlist5646,vlist5647,vlist5648,vlist5649
 ,vlist5650,vlist5651,vlist5652,vlist5653,vlist5654,vlist5655,vlist5656,vlist5657,vlist5658,vlist5659
 ,vlist5660,vlist5661,vlist5662,vlist5663,vlist5664,vlist5665,vlist5666,vlist5667,vlist5668,vlist5669
 ,vlist5670,vlist5671,vlist5672,vlist5673,vlist5674,vlist5675,vlist5676,vlist5677,vlist5678,vlist5679
 ,vlist5680,vlist5681,vlist5682,vlist5683,vlist5684,vlist5685,vlist5686,vlist5687,vlist5688,vlist5689
 ,vlist5690,vlist5691,vlist5692,vlist5693,vlist5694,vlist5695,vlist5696,vlist5697,vlist5698,vlist5699
 ,vlist5700,vlist5701,vlist5702,vlist5703,vlist5704,vlist5705,vlist5706,vlist5707,vlist5708,vlist5709
 ,NULL     ,NULL     ,vlist5712,NULL     ,NULL     ,vlist5715,vlist5716,vlist5717,vlist5718,NULL     
 ,NULL     ,vlist5721,NULL     ,NULL     ,vlist5724,vlist5725,vlist5726,vlist5727,vlist5728,vlist5729
 ,vlist5730,vlist5731,vlist5732,vlist5733,vlist5734,vlist5735,vlist5736,NULL     ,NULL     ,vlist5739
 ,NULL     ,NULL     ,vlist5742,vlist5743,vlist5744,vlist5745,NULL     ,NULL     ,vlist5748,NULL     
 ,NULL     ,vlist5751,vlist5752,vlist5753,vlist5754,vlist5755,vlist5756,vlist5757,vlist5758,vlist5759
 ,vlist5760,vlist5761,vlist5762,vlist5763,vlist5764,vlist5765,vlist5766,vlist5767,vlist5768,vlist5769
 ,vlist5770,vlist5771,vlist5772,vlist5773,vlist5774,vlist5775,vlist5776,vlist5777,vlist5778,vlist5779
 ,vlist5780,vlist5781,vlist5782,vlist5783,vlist5784,vlist5785,vlist5786,vlist5787,vlist5788,vlist5789
 ,vlist5790,NULL     ,NULL     ,vlist5793,NULL     ,NULL     ,vlist5796,vlist5797,vlist5798,vlist5799
 ,NULL     ,NULL     ,vlist5802,NULL     ,NULL     ,vlist5805,vlist5806,vlist5807,vlist5808,vlist5809
 ,vlist5810,vlist5811,vlist5812,vlist5813,vlist5814,vlist5815,vlist5816,vlist5817,NULL     ,NULL     
 ,vlist5820,NULL     ,NULL     ,vlist5823,vlist5824,vlist5825,vlist5826,NULL     ,NULL     ,vlist5829
 ,NULL     ,NULL     ,vlist5832,vlist5833,vlist5834,vlist5835,vlist5836,vlist5837,vlist5838,vlist5839
 ,vlist5840,vlist5841,vlist5842,vlist5843,vlist5844,vlist5845,vlist5846,vlist5847,vlist5848,vlist5849
 ,vlist5850,vlist5851,vlist5852,vlist5853,vlist5854,vlist5855,vlist5856,vlist5857,vlist5858,vlist5859
 ,vlist5860,vlist5861,vlist5862,vlist5863,vlist5864,vlist5865,vlist5866,vlist5867,vlist5868,vlist5869
 ,vlist5870,vlist5871,vlist5872,vlist5873,vlist5874,vlist5875,vlist5876,vlist5877,vlist5878,vlist5879
 ,vlist5880,vlist5881,vlist5882,vlist5883,vlist5884,vlist5885,vlist5886,vlist5887,vlist5888,vlist5889
 ,vlist5890,vlist5891,vlist5892,vlist5893,vlist5894,vlist5895,vlist5896,vlist5897,vlist5898,vlist5899
 ,vlist5900,vlist5901,vlist5902,vlist5903,vlist5904,vlist5905,vlist5906,vlist5907,vlist5908,vlist5909
 ,vlist5910,vlist5911,vlist5912,vlist5913,vlist5914,vlist5915,vlist5916,vlist5917,vlist5918,vlist5919
 ,vlist5920,vlist5921,vlist5922,vlist5923,vlist5924,vlist5925,vlist5926,vlist5927,vlist5928,vlist5929
 ,vlist5930,vlist5931,vlist5932,vlist5933,vlist5934,vlist5935,vlist5936,vlist5937,vlist5938,vlist5939
 ,vlist5940,vlist5941,vlist5942,vlist5943,vlist5944,vlist5945,vlist5946,vlist5947,vlist5948,vlist5949
 ,vlist5950,vlist5951,vlist5952,vlist5953,vlist5954,vlist5955,vlist5956,vlist5957,vlist5958,vlist5959
 ,vlist5960,vlist5961,vlist5962,vlist5963,vlist5964,vlist5965,vlist5966,vlist5967,vlist5968,vlist5969
 ,vlist5970,vlist5971,vlist5972,vlist5973,vlist5974,vlist5975,vlist5976,vlist5977,vlist5978,vlist5979
 ,vlist5980,vlist5981,vlist5982,vlist5983,vlist5984,vlist5985,vlist5986,vlist5987,vlist5988,vlist5989
 ,vlist5990,vlist5991,vlist5992,vlist5993,vlist5994,vlist5995,vlist5996,vlist5997,vlist5998,vlist5999
 ,vlist6000,vlist6001,vlist6002,vlist6003,vlist6004,vlist6005,vlist6006,vlist6007,vlist6008,vlist6009
 ,vlist6010,vlist6011,vlist6012,vlist6013,vlist6014,vlist6015,vlist6016,vlist6017,vlist6018,vlist6019
 ,vlist6020,vlist6021,vlist6022,vlist6023,vlist6024,vlist6025,vlist6026,vlist6027,vlist6028,vlist6029
 ,vlist6030,vlist6031,vlist6032,vlist6033,vlist6034,vlist6035,vlist6036,vlist6037,vlist6038,vlist6039
 ,vlist6040,vlist6041,vlist6042,vlist6043,vlist6044,vlist6045,vlist6046,vlist6047,vlist6048,vlist6049
 ,vlist6050,vlist6051,vlist6052,vlist6053,vlist6054,vlist6055,vlist6056,vlist6057,vlist6058,vlist6059
 ,vlist6060,vlist6061,vlist6062,vlist6063,vlist6064,vlist6065,vlist6066,vlist6067,vlist6068,vlist6069
 ,vlist6070,vlist6071,vlist6072,vlist6073,vlist6074,vlist6075,vlist6076,vlist6077,vlist6078,vlist6079
 ,vlist6080,vlist6081,vlist6082,vlist6083,vlist6084,vlist6085,vlist6086,vlist6087,vlist6088,vlist6089
 ,vlist6090,vlist6091,vlist6092,vlist6093,vlist6094,vlist6095,vlist6096,vlist6097,vlist6098,vlist6099
 ,vlist6100,vlist6101,vlist6102,vlist6103,vlist6104,vlist6105,vlist6106,vlist6107,vlist6108,vlist6109
 ,vlist6110,vlist6111,vlist6112,vlist6113,vlist6114,vlist6115,vlist6116,vlist6117,vlist6118,vlist6119
 ,vlist6120,vlist6121,vlist6122,vlist6123,vlist6124,vlist6125,vlist6126,vlist6127,vlist6128,vlist6129
 ,vlist6130,vlist6131,vlist6132,vlist6133,vlist6134,vlist6135,vlist6136,vlist6137,vlist6138,vlist6139
 ,vlist6140,vlist6141,vlist6142,vlist6143,vlist6144,vlist6145,vlist6146,vlist6147,vlist6148,vlist6149
 ,vlist6150,vlist6151,vlist6152,vlist6153,vlist6154,vlist6155,vlist6156,vlist6157,vlist6158,vlist6159
 ,vlist6160,vlist6161,vlist6162,vlist6163,vlist6164,vlist6165,vlist6166,vlist6167,vlist6168,vlist6169
 ,vlist6170,vlist6171,vlist6172,vlist6173,vlist6174,vlist6175,vlist6176,vlist6177,vlist6178,vlist6179
 ,vlist6180,vlist6181,vlist6182,vlist6183,vlist6184,vlist6185,vlist6186,vlist6187,vlist6188,vlist6189
 ,vlist6190,vlist6191,vlist6192,vlist6193,vlist6194,vlist6195,NULL     ,NULL     ,vlist6198,NULL     
 ,NULL     ,vlist6201,vlist6202,vlist6203,vlist6204,NULL     ,NULL     ,vlist6207,NULL     ,NULL     
 ,vlist6210,vlist6211,vlist6212,vlist6213,vlist6214,vlist6215,vlist6216,vlist6217,vlist6218,vlist6219
 ,vlist6220,vlist6221,vlist6222,NULL     ,NULL     ,vlist6225,NULL     ,NULL     ,vlist6228,vlist6229
 ,vlist6230,vlist6231,NULL     ,NULL     ,vlist6234,NULL     ,NULL     ,vlist6237,vlist6238,vlist6239
 ,vlist6240,vlist6241,vlist6242,vlist6243,vlist6244,vlist6245,vlist6246,vlist6247,vlist6248,vlist6249
 ,vlist6250,vlist6251,vlist6252,vlist6253,vlist6254,vlist6255,vlist6256,vlist6257,vlist6258,vlist6259
 ,vlist6260,vlist6261,vlist6262,vlist6263,vlist6264,vlist6265,vlist6266,vlist6267,vlist6268,vlist6269
 ,vlist6270,vlist6271,vlist6272,vlist6273,vlist6274,vlist6275,vlist6276,NULL     ,NULL     ,vlist6279
 ,NULL     ,NULL     ,vlist6282,vlist6283,vlist6284,vlist6285,NULL     ,NULL     ,vlist6288,NULL     
 ,NULL     ,vlist6291,vlist6292,vlist6293,vlist6294,vlist6295,vlist6296,vlist6297,vlist6298,vlist6299
 ,vlist6300,vlist6301,vlist6302,vlist6303,NULL     ,NULL     ,vlist6306,NULL     ,NULL     ,vlist6309
 ,vlist6310,vlist6311,vlist6312,NULL     ,NULL     ,vlist6315,NULL     ,NULL     ,vlist6318,vlist6319
 ,vlist6320,vlist6321,vlist6322,vlist6323,vlist6324,vlist6325,vlist6326,vlist6327,vlist6328,vlist6329
 ,vlist6330,vlist6331,vlist6332,vlist6333,vlist6334,vlist6335,vlist6336,vlist6337,vlist6338,vlist6339
 ,vlist6340,vlist6341,vlist6342,vlist6343,vlist6344,vlist6345,vlist6346,vlist6347,vlist6348,vlist6349
 ,vlist6350,vlist6351,vlist6352,vlist6353,vlist6354,vlist6355,vlist6356,vlist6357,vlist6358,vlist6359
 ,vlist6360,vlist6361,vlist6362,vlist6363,vlist6364,vlist6365,vlist6366,vlist6367,vlist6368,vlist6369
 ,vlist6370,vlist6371,vlist6372,vlist6373,vlist6374,vlist6375,vlist6376,vlist6377,vlist6378,vlist6379
 ,vlist6380,vlist6381,vlist6382,vlist6383,vlist6384,vlist6385,vlist6386,vlist6387,vlist6388,vlist6389
 ,vlist6390,vlist6391,vlist6392,vlist6393,vlist6394,vlist6395,vlist6396,vlist6397,vlist6398,vlist6399
 ,vlist6400,vlist6401,vlist6402,vlist6403,vlist6404,vlist6405,vlist6406,vlist6407,vlist6408,vlist6409
 ,vlist6410,vlist6411,vlist6412,vlist6413,vlist6414,vlist6415,vlist6416,vlist6417,vlist6418,vlist6419
 ,vlist6420,vlist6421,vlist6422,vlist6423,vlist6424,vlist6425,vlist6426,vlist6427,vlist6428,vlist6429
 ,vlist6430,vlist6431,vlist6432,vlist6433,vlist6434,vlist6435,vlist6436,vlist6437,vlist6438,NULL     
 ,NULL     ,vlist6441,NULL     ,NULL     ,vlist6444,vlist6445,vlist6446,vlist6447,NULL     ,NULL     
 ,vlist6450,NULL     ,NULL     ,vlist6453,vlist6454,vlist6455,vlist6456,vlist6457,vlist6458,vlist6459
 ,vlist6460,vlist6461,vlist6462,vlist6463,vlist6464,vlist6465,NULL     ,NULL     ,vlist6468,NULL     
 ,NULL     ,vlist6471,vlist6472,vlist6473,vlist6474,NULL     ,NULL     ,vlist6477,NULL     ,NULL     
 ,vlist6480,vlist6481,vlist6482,vlist6483,vlist6484,vlist6485,vlist6486,vlist6487,vlist6488,vlist6489
 ,vlist6490,vlist6491,vlist6492,vlist6493,vlist6494,vlist6495,vlist6496,vlist6497,vlist6498,vlist6499
 ,vlist6500,vlist6501,vlist6502,vlist6503,vlist6504,vlist6505,vlist6506,vlist6507,vlist6508,vlist6509
 ,vlist6510,vlist6511,vlist6512,vlist6513,vlist6514,vlist6515,vlist6516,vlist6517,vlist6518,vlist6519
 ,NULL     ,NULL     ,vlist6522,NULL     ,NULL     ,vlist6525,vlist6526,vlist6527,vlist6528,NULL     
 ,NULL     ,vlist6531,NULL     ,NULL     ,vlist6534,vlist6535,vlist6536,vlist6537,vlist6538,vlist6539
 ,vlist6540,vlist6541,vlist6542,vlist6543,vlist6544,vlist6545,vlist6546,NULL     ,NULL     ,vlist6549
 ,NULL     ,NULL     ,vlist6552,vlist6553,vlist6554,vlist6555,NULL     ,NULL     ,vlist6558,NULL     
 ,NULL     
};

};