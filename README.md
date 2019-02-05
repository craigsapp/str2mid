# str2mid #

The str2mid program converts Humdrum files in the
`**str` format into Standard MIDI Files.  Typically data
will be converted from ASCII guitar TAB data using
[gptab2str](https://github.com/craigsapp/humextra/blob/master/scripts/perl/gptab2str).  

## Downloading and compiling ##

If you have git installed, then in a terminal type the commands:

```bash
   cd               # Go to installation directory (home directory in this example).
   git clone https://github.com/craigsapp/str2mid
   cd str2mid       # Go into repository to run make commands.
   make update      # Make sure you have the most recent humlib/midifile libraries.
   make install     # Copy executable to /usr/local/bin
```

## Updating ##

If you want to download the latest version of the code, but have an existing
installation, then use these commands to update:

```bash
   cd str2mid   # Go to the location wherever you placed the repository.
   make update
   make install
```

## Example ##


<img width="1054" alt="screen shot 2019-02-04 at 3 55 10 pm" src="https://user-images.githubusercontent.com/3487289/52236495-032c8c00-2895-11e9-95fb-204129240f83.png">

```
!!!COM: Mozart, Wolfgang Amadeus
!!!OTL: Opening of the theme, "Ah, vous dirai-je, Maman"
!!!CDT: 1778
!!!SCT: KV 265
!!!LAR: Sapp, Craig Stuart
**recip	**str	**str	**str	**str	**str	**str
*	*AT:E2	*AT:A2	*AT:D3	*AT:G3	*AT:B3	*AT:E4
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


## Brief overview of the `**str` syntax ##

The `**str` representation is a string
tablature format that is similar to Humdrum
[fret](http://www.humdrum.org/Humdrum/representations/fret.rep.html)
representation,  but for `**str` case, each string is in a separate
spine, which makes it easier to edit and manipulate the data.  The `**str`
representation is intended to be roughly equivalent to [ASCII
Guitar TAB](https://en.wikipedia.org/wiki/ASCII_tab),
such as used in Guitar Pro and [Ultimate
Guitar](https://www.ultimate-guitar.com/contribution/help/rubric)

Each string is represented by a different spine (column), starting
with `**str` and ending with `*-`.  The strings should be in order from
"lowest" string on the left to "highest" string on the right. Before the
first notes on a string, the tuning of the open string must be given.
This is done in the form `*AT:C4`, where `C4` is middle C.  The strings of
a standard six-string guitar, from lowest to highest string are: `*AT:E2`,
`*AT:A2`, `*AT:D3`, `*AT:G3`, `*AT:B3`, `*AT:E4`.   MIDI note numbers can
also be used instead of pitch names, which would be: `*AT:40`, `*AT:45`,
`*AT:50`, `*AT:55`, `*AT:59`, `*AT:64` for a standard guitars.

Percussion instruments can be specified by setting the tuning to 
`*AT:0` and using General MIDI Percussion key numbers for the fret
numbers.  When representing percussion instruments, also set the MIDI 
channel to 10, using the interpretation `*CH:10`.

To the left of all strings for a particular instrument, a time-line using
the `**recip` formate needs to be given in order to convert the tablature
to MIDI with the str2mid program.  Meter and tempo markings (such as
`*M4/4` and `*MM84` in the above example) should be placed in the tandem
`**recip` spine, and this sort of information can also be stored in each
`**str` spine (but the mid2str program will ignore such extract data.
Similarly the key signature and key designation can be added to the
`**recip` spine.

Barlines behave similar to the `**kern` representation.

The str2mid program will convert notes with a default loudness of 64.   You
can also change the loudness of individual strings with an interpretation
like `*VEL:64`.  The range for `*VEL:` values is 1 to 127 (standard MIDI range).

Ties can be encoded in a manner similar to Humdrum `**kern` data, using
`[` to start a tie, `_` for the middle note in a tied group and `]`
for the last note in a tied group.  If a note is not tied, it will be
ended automatically at the next data line that has a note attack in any
of the other strings.


## Humdrum tool integration ##

The `**str` represetation can work with many [Humdrum
tools](https://github.com/humdrum-tools/humdrum-tools).  Here are some
example uses with some of the tools.

### Uniform spacing of lines ###

Use the [timebase](http://www.humdrum.org/Humdrum/commands/timebase.html)
tool to space out the data lines evenly.  Another tool called
[minrhy](htt://extras.humdrum.org/man/minrhy) can also be used to 
calculate the minimum integral rhythmic unit needed for spacing the
lines without losing any short-duration lines.  For example, running the
above example data through the minrhy tool will identify 16th notes as the
shortest note time values (other than grace notes):

```bash
   $ minrhy twinkle.str
   16
```

This value can be used to space every data line so that it represents
a 16th note duration:

```bash
   $ timebase -t 16 twinkle.str > twinkle.tb16
```

The result of the above command:

```
!!!COM: Mozart, Wolfgang Amadeus
!!!OTL: Opening of the theme, "Ah, vous dirai-je, Maman"
!!!CDT: 1778
!!!SCT: KV 265
!!!LAR: Sapp, Craig Stuart
**recip	**str	**str	**str	**str	**str	**str
*	*AT:E2	*AT:A2	*AT:D3	*AT:G3	*AT:B3	*AT:E4
*M4/4	*	*	*	*	*	*
*MM84	*	*	*	*	*	*
*tb16	*	*	*	*	*	*
=1	=1	=1	=1	=1	=1	=1
4	8	.	.	.	.	8
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	.	10	.	.	8
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	.	14	.	.	15
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	.	10	.	.	15
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
=2	=2	=2	=2	=2	=2	=2
4	.	.	15	.	.	17
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	.	10	.	.	17
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	.	14	.	.	15
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	.	10	.	.	15
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
=3	=3	=3	=3	=3	=3	=3
4	.	.	12	.	.	13
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	.	9	.	.	13
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	.	10	.	.	12
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
4	.	12	.	.	.	12
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
=4	=4	=4	=4	=4	=4	=4
4	.	8	.	.	.	10
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
8.	.	[10	.	.	.	10
.	.	.	.	.	.	.
.	.	.	.	.	.	.
16	.	10]	.	.	.	12
2	8	.	.	.	.	8
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
.	.	.	.	.	.	.
==	==	==	==	==	==	==
*-	*-	*-	*-	*-	*-	*-
```

This is similar to Guitar Pro TABS, although they will evenly space
notes in the minimal time unit on a measure-by-measure basis.


### Extracting measures ###

Individual measures can be extracted using the [myank](http://extras.humdrum.org/man/myank) tool, here is an example of extracting meaasures 2 through 3:

```
!!!COM: Mozart, Wolfgang Amadeus
!!!OTL: Opening of the theme, "Ah, vous dirai-je, Maman"
!!!CDT: 1778
!!!SCT: KV 265
!!!LAR: Sapp, Craig Stuart
**recip	**str	**str	**str	**str	**str	**str
=2-	=2-	=2-	=2-	=2-	=2-	=2-
4	.	.	15	.	.	17
4	.	.	10	.	.	17
4	.	.	14	.	.	15
4	.	.	10	.	.	15
=3	=3	=3	=3	=3	=3	=3
4	.	.	12	.	.	13
4	.	.	9	.	.	13
4	.	.	10	.	.	12
4	.	12	.	.	.	12
=	=	=	=	=	=	=
*-	*-	*-	*-	*-	*-	*-
```

### Removing a string ###

Use the [extract](http://www.humdrum.org/Humdrum/commands/extract.html) or
[extractx](http://extras.humdrum.org/man/extractx) tool to extract or add
spines to/from the score.


Remove the `**recip` spine by extracting only the `**str` spines:

```bash
   $ extractx -i str twinkle.str
```

Results in:

```
!!!COM: Mozart, Wolfgang Amadeus
!!!OTL: Opening of the theme, "Ah, vous dirai-je, Maman"
!!!CDT: 1778
!!!SCT: KV 265
!!!LAR: Sapp, Craig Stuart
**str	**str	**str	**str	**str	**str
*AT:E2	*AT:A2	*AT:D3	*AT:G3	*AT:B3	*AT:E4
*	*	*	*	*	*
*	*	*	*	*	*
=1	=1	=1	=1	=1	=1
8	.	.	.	.	8
.	.	10	.	.	8
.	.	14	.	.	15
.	.	10	.	.	15
=2	=2	=2	=2	=2	=2
.	.	15	.	.	17
.	.	10	.	.	17
.	.	14	.	.	15
.	.	10	.	.	15
=3	=3	=3	=3	=3	=3
.	.	12	.	.	13
.	.	9	.	.	13
.	.	10	.	.	12
.	12	.	.	.	12
=4	=4	=4	=4	=4	=4
.	8	.	.	.	10
.	[10	.	.	.	10
.	10]	.	.	.	12
8	.	.	.	.	8
==	==	==	==	==	==
*-	*-	*-	*-	*-	*-
```

Extract the E4 string:

```bash
   $ extractx -g AT:E4 twinkle.str
```

```
!!!COM: Mozart, Wolfgang Amadeus
!!!OTL: Opening of the theme, "Ah, vous dirai-je, Maman"
!!!CDT: 1778
!!!SCT: KV 265
!!!LAR: Sapp, Craig Stuart
**str
*AT:E4
*
*
=1
8
8
15
15
=2
17
17
15
15
=3
13
13
12
12
=4
10
10
12
8
==
*-
```

Count the number of times the E4 string is plucked:

```bash
   $ extractx -g AT:E4 | ridx -H | sortcount -th
```

```
**count	**data
4	15
3	12
3	8
2	17
2	10
2	13
*-	*-
!!TOTAL:	16
```

Plot a histogram of all frets used on all strings:

```bash
   $ extractx -i str twinkle.str | serialize | ridx -H |
	 sed 's/[]]//g' | grep -v []_] |\
	 sortcount -v  -T "Frets used" -x "fret number" > plot.html
```

Then open plot.html in a web browser to view the plot:

<img width="1147" alt="screen shot 2019-02-04 at 10 20 12 pm" src="https://user-images.githubusercontent.com/3487289/52251429-c5e2f100-28ca-11e9-8dfc-6e8dfb0026e0.png">






