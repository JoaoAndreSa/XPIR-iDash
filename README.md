XPIR: Private Information Retrieval for Everyone
=================================================

### XPIR v0.2.0-beta is released please get it [here](https://github.com/XPIR-team/XPIR/releases).

This version introduces three major changes:
  * Compilation with cmake instead of the classic autotools
  * Dependencies are no longer included (the user can install them by himself or use a script to download and install them)
  * An API allowing to use XPIR as a library has been released (see below)

The original client/server applications are still available with the associated optimization tools. These can still be used for example to do some tests on PIR without developping an application or to use the optimization process in order to get interesting cryptographic and PIR parameters for a given setting.

**If you have compilation/execution issues please contact us.** The old version is still available in the branch old-master and of course through previous [releases](https://github.com/XPIR-team/XPIR/releases). 

Introduction:
=============

XPIR allows a user to privately download an element from a database. This means that the database server knows that she has sent a database element to the user but does not know which one. The scientific term for the underlying protocol is Private Information Retrieval (PIR). This library is described and studied in the paper:

Carlos Aguilar-Melchor, Joris Barrier, Laurent Fousse, Marc-Olivier Killijian, "XPIR: Private Information Retrieval for Everyone", Proceedings on Privacy Enhancing Technologies. Volume 2016, Issue 2, Pages 155–174, ISSN (Online) 2299-0984, DOI: 10.1515/popets-2016-0010, December 2015. 

If you use our library, or a sub-part, such a NFLlib, please cite this paper on your work.

This project is closely related to another project available at GitHub: [NFLlib](https://github.com/quarkslab/NFLlib). The NTT-based Fast Lattice library, which allows fast cryptographic computations over ideal lattices. Adapting this project to a more recent version of NFLlib would provide a performance boost but would be a lot of work so we are not planning to do it immediately. 

*Important Note 1*: A PIR implementation for a specific application can be much simpler and more compact than this library. Indeed, much of the code of XPIR is aimed to ensure that the library delivers good performance in a large span of applications *without user interaction* so that the user does not need to be an expert on PIR or cryptography to use it with good performance results. **If you want to use a PIR protocol for a very specific setting feel free to contact us for building up a collaboration !**

*Important Note 2*: For publication issues, a small part of the code is missing. From a technical point of view this correspond to the gaussian noise generator for LWE which is replaced by a uniform noise generator until some scientific results are published. Replacing our uniform noise generator with our gaussian noise generator does not impact performance in an observable way.

*Important Note 3*: This software cannot provide reliable privacy without more scrutiny on many details. We have tried to provide some resiliance to timing tests, but haven't tested them thoroughly. The random seed generation and pseudorandom generation use strong functions but we haven't done a thorough analysis of whether an idiotic fault is present in those critical sections of the code or not. No input verification is done by the server or client so many buffer overflows are potentially possible, etc. As is, the software *shows that privacy is possible* but cannot guarantee it against strong active adversaries (using timing attacks, introducing malformed entries, etc.) until it gets enough scrutiny.


Installation:
=============

Requirements: 
- 64-bits Linux OS: g++>=4.8, gcc>=4.8

Get a copy of the project with:
- https://github.com/JoaoAndreSa/XPIR-iDash.git
- or by downloading from https://github.com/JoaoAndreSa/XPIR-iDash/archive/master.zip
 
You need cmake, GMP (version 6) Mpfr (version 3.1.2), and some boost modules (atomic, chrono, date_time, exception, program_options, regex, system, thread, all in version 1.55.0) to build XPIR. You can install them by yourself on your system (if you know how to do it this will be the fastest option). You can also use the (slower but safer) script helper_script.sh that is in the root directory of the repository to retrieve the exact versions and compile them in a local directory (namely ./local/). If two versions of the required libraries exist (one local and one system-wide) the local will be taken preferently.

To build, and test XPIR, run the following:

```
$> mkdir _build
$> cd _build
$> cmake .. -DCMAKE_BUILD_TYPE=Release 
$> make
$> make check
```
The first test should be pretty long (to build initial caches) and then a set of tests should be display CORRECT or "Skipping test...". If you get INCORRECT tests or core dump notifications then something went wrong ...

The following CMake options are relevant:

Option                             | Description
-----------------------------------|---------------------------------
`-DSEND_CATALOG=OFF`               | Do not send the catalog to client (default is send catalog if |catalog|<1000)
`-DMULTI_THREAD=OFF`               | Do not use multi-threading
`-DPERF_TIMERS=OFF`                | Do not show performance measurements during execution
`-DCMAKE_BUILD_TYPE=Debug`         | Add debugging options and remove optimization

**Our application source files are located in the /apps/XPIR-hash/ folder.** To obtain the executable, you need to install XPIR by following the above instructions. The *make* step automatically creates a folder XPIR/_build/apps/XPIR-hash/ from where you can run this application. If you wish to change any of the parameters, or specify other address and/or port, erase this folder, change the files inside XPIR/apps/XPIR-hash/Constants/ as you see fit and re-make (execute \textsc{make} in the _build/ folder).

NOTE: If you wish to examine and execute our alternative application, which does not include the subtraction step, simply change branch to noSub (git checkout subDev).\\

CAUTION: Incorrectly changing the parameters might cause the application to stop working properly (keep a backup of the default settings).


Usage of XPIR as a library:
===========================



Contributors:
=============

This project has been imported to GitHub once mature enough by Joao Sa Sousa.........  

Affiliations:
=============
 
