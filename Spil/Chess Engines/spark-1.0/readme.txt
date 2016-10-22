Spark Copyright (C) 2009-2010 AJ Siemelink. All rights reserved.
==============================================================================


License
==============================================================================
See license.txt for the license that applies to this software.


Installation
==============================================================================
To install Spark, follow these steps:

1. extract the zip-file to a directory of your choice

2. configure your gui to execute the file 'spark-[version]-[os]*' to start Spark:
   spark-[version]-win64*-corei.exe: for Intel coreI(3/5/7) cpu's
                                      on 64-bit Windows (XP, Vista, 7)
   spark-[version]-win64*.exe      : for 64-bit Windows (XP, Vista, 7),all cpu's
   spark-[version]-win32*.exe      : for 32-bit Windows (XP, Vista, 7)
   spark-[version]-linux64*        : for 64-bit Linux
   spark-[version]-osx64*          : for 64-bit Maxc OSX (Intel only)

   If your operating system supports multiple versions, you should pick the one
   that runs with the highest nps (nodes per second) for the best performance.

   If you do not yet have a chess GUI, you can download a free one here:
   http://www.playwitharena.com.

3. configure your GUI to use the UCI-protocol for communications with Spark.
   If your GUI does not support the UCI-protocol, but it does support the 
   Winboard-protocol, you can use Polyglot by Fabien Letouzey to run Spark.
   PolyGlot can be downloaded here:
   http://wbec-ridderkerk.nl/html/details/PolyGlot.html



Multi Processor (MP) Support
==============================================================================
1. Configure your gui to start 'spark-[version]-[os]-mp' in stead of 'spark-[version]-[os]'

2. Set the UCI option 'Threads' to the desired number of cores/processors in your gui


Opening Book Support
==============================================================================
Spark support's Bright's "bbk" book format (created with Bright 0.5c or higher)
Book settings can be changed in Sparks configuration file: spark.ini
See spark.ini for details on the various options.
The default book, spark.bbk, covers some major openings.
A much deeper book by Wael Deeb can be downloaded here: http://www.mediafire.com/?wyzwmwytgjo


Acknowledgements
==============================================================================
CCRL
http://computerchess.org.uk/ccrl/
Computer Chess Rating List

CEGT
http://www.husvankempen.de/nunn/
Chess Engines Grand Tournament: 

Olivier Deville
http://www.open-aurec.com/chesswar/ 
Chesswar tournament

Pradu Kanan
http://www.pradu.us/old/Nov27_2008/Buzz/
Magic bitboards

Swaminathan
Operator in the CCT12 tournament
http://www.cctchess.com/

Wael Deeb
Opening book for the CCT12 tournament
http://www.mediafire.com/?wyzwmwytgjo
