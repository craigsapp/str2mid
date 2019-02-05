str2mid
========

The str2mid program converts Humdrum files in the
`**str` format into Standard MIDI Files.  Typically data
will be converted from ASCII guitar TAB data using
[gptab2str](https://github.com/craigsapp/humextra/blob/master/scripts/perl/gptab2str).  

Downloading and compiling
-------------------------

If you have git installed, then in a terminal type the commands:

```bash
   cd               # Go to installation directory (home directory in this example).
   git clone https://github.com/craigsapp/str2mid
   cd str2mid       # Go into repository to run make commands.
   make update      # Make sure you have the most recent humlib/midifile libraries.
   make install     # Copy executable to /usr/local/bin
```

Updating 
--------

If you want to download the latest version of the code, but have an existing
installation, then use these commands to update:

```bash
   cd str2mid   # Go to the location wherever you placed the repository.
   make update
   make install
```

Example 
--------


<img width="1054" alt="screen shot 2019-02-04 at 3 55 10 pm" src="https://user-images.githubusercontent.com/3487289/52236495-032c8c00-2895-11e9-95fb-204129240f83.png">

```
!!!COM: Mozart, Wolfgang Amadeus
!!!OTL: Opening of the theme, "Ah, vous dirai-je, Maman"
!!!CDT: 1778
!!!SCT: KV 265
!!!LAR: Sapp, Craig Stuart
**recip	**str	**str	**str	**str	**str	**str
*	*AT:E1	*AT:A1	*AT:D2	*AT:G2	*AT:B2	*AT:E3
*M4/4	*	*	*	*	*	*
*MM84	*	*	*	*	*	*
=1	=1	=1	=1	=1	=1	=1
4	8	.	.	.	.	8
4	.	.	10	.	.	8
4	.	.	14	.	.	15
4	.	.	10	.	.	15
=2	=2	=2	=2	=2	=2	=2
4	.	.	15	.	.	17
4	.	.	10	.	.	17
4	.	.	14	.	.	15
4	.	.	10	.	.	15
=3	=3	=3	=3	=3	=3	=3
4	.	.	12	.	.	13
4	.	.	9	.	.	13
4	.	.	10	.	.	12
4	.	12	.	.	.	12
=4	=4	=4	=4	=4	=4	=4
4	.	8	.	.	.	10
8.	.	[10	.	.	.	10
16	.	10]	.	.	.	12
2	8	.	.	.	.	8
==	==	==	==	==	==	==
*-	*-	*-	*-	*-	*-	*-
```


To convert the above example to MIDI:

```bash
   str2mid examples/twinkle.txt
```

The file `twinkle.mid` will be created in the current directory.



