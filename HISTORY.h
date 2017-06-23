/*
 * ----------------------------------------------
 * osd2web Plugin - Revision History
 * ----------------------------------------------
 *
 *  (c) 2017 Dirk Hermes, Jörg Wendel
 *
 */

#define _VERSION      "0.1.13"
#define VERSION_DATE  "23.06.2017"

#ifdef GIT_REV
#  define VERSION _VERSION "-GIT" GIT_REV
#else
#  define VERSION _VERSION
#endif

/*
 * ------------------------------------

2017-06-23: version 0.1.13 (horchi)
   - fixed:  lock sequence
   - fixed:  disappear of replay control
   - change: removed obsolete libarchive
   - change: 'started' to implement replay view for skin horchiTft

2017-06-21: version 0.1.12 (horchi)
   - Change: further improvements of skin horchiTft
              - menu color
              - display running timer in 'red'
              - fixed epg image scaling

2017-06-13: version 0.1.11 (rechner)
   - Change: compileroptions for skins
   - fixed:  skin horchiTft/theme blue missed

2017-06-13: version 0.1.10 (horchi)
   - Change: further improvements of skin horchiTft

2017-06-13: version 0.1.9 (horchi)
   - Bugfix: Fixed possible crash on missing event

2017-06-12: version 0.1.8 (horchi)
   - Change: Improved auto size of theme horchiTft to fit different resolutions

2017-06-09: version 0.1.7 (horchi)
   - Bugfix: Fixed lock sequence

2017-06-09: version 0.1.6 (horchi)
   - Bugfix: Fixed PATCHED switch

2017-06-07: version 0.1.5 (horchi)
   - Added: TFT skin horchi (under development)

2017-06-06: version 0.1.4 (horchi)
   - Change: Further less lib dependencies
   - Bugfix: Fixed compile problem

2017-06-06: version 0.1.3 (horchi)
   - Bugfix: Fixed possible crash at replay

2017-06-06: version 0.1.2 (horchi)
   - Change: removed unused mysql dependency

2017-06-02: version 0.1.1 (horchi)
   - Bugfix: Fxed possible crash

2017-06-01: version 0.1.0 (horchi)
   - Added: First official Beta Release

2017-05-03: version 0.0.3 (horchi)
   - Change: VDR 2.3.4 porting

2017-02-17: version 0.0.2 (rechner)
   - Initial version client

2017-02-13: version 0.0.1 (horchi)
   - Initial version

 * ------------------------------------
 */
