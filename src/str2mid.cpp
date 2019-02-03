//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Feb  2 21:12:42 PST 2019
// Last Modified: Sat Feb  2 21:58:18 PST 2019
// Filename:      str2mid.cpp
// URL:           https://github.com/craigsapp/str2mid/src/str2mid.cpp
// Syntax:        C++11
// vim:           ts=3 noexpandtab
//
// Description:   Convert a **str score to MIDI.
//

#include "humlib.h"
#include "MidiFile.h"

using namespace std;
using namespace hum;
using namespace smf;

void processFile             (HumdrumFile& infile, const string filename);
void convertToMidi           (MidiFile& midifile, HumdrumFile& infile);
void convertSpineToMidiTrack (MidiFile& mfile, HTp sstart, int track, int channel);
void insertTempo             (MidiFile& midifile, HumdrumFile& infile);

string tie_signifier = "L";
int tpq = 240;

int main(int argc, char** argv) {
	Options options;
	options.process(argc, argv);
	HumdrumFile infile;
	for (int i=1; i<= options.getArgCount(); i++) {
		infile.read(options.getArg(i));
		processFile(infile, options.getArg(i));
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// processFile --
//

void processFile(HumdrumFile& infile, const string filename) {
	auto slash = filename.rfind("/");
	string outname = filename;
	if (slash != std::string::npos) {
		outname = filename.substr(slash+1);
	}
	auto dot = outname.rfind(".");
	if (dot != std::string::npos) {
		outname = outname.substr(0, dot);
	}
	outname += ".mid";
	MidiFile midifile;
	midifile.setTPQ(tpq);
	convertToMidi(midifile, infile);
	midifile.write(outname);
}



//////////////////////////////
//
// convertToMidi --
//

void convertToMidi(MidiFile& midifile, HumdrumFile& infile) {
	vector<HTp> strspines;
	infile.getSpineStartList(strspines, "**str");
	int scount = (int)strspines.size();
	midifile.addTracks(scount);
	for (int i=0; i<scount; i++) {
		midifile.addTimbre(i+1, 0, i, 24);
		convertSpineToMidiTrack(midifile, strspines[scount - i - 1], i+1, i);
	}
	insertTempo(midifile, infile);
	midifile.sortTracks();
}



//////////////////////////////
//
// insertTempo --
//

void insertTempo(MidiFile& midifile, HumdrumFile& infile) {
	vector<HTp> recip;
	infile.getSpineStartList(recip, "**recip");
	if (recip.empty()) {
		return;
	}
	HumRegex hre;
	double tempo = -1.0;
	HTp current = recip[0]->getNextToken();
	while (current && !current->isData()) {
		if (!current->isInterpretation()) {
			current = current->getNextToken();
			continue;
		}
		if (!hre.search(current, "^\\*MM([\\d.]+)")) {
			current = current->getNextToken();
			continue;
		}
		tempo = hre.getMatchDouble(1);
		break;
	}
	if (tempo < 1) {
		return;
	}
	midifile.addTempo(0, 0, tempo);
}



//////////////////////////////
//
// convertSpineToMidiTrack --  Channel cannot be 9 or greater than 15.
//

void convertSpineToMidiTrack(MidiFile& mfile, HTp sstart, int track, int channel) {
	HTp current = sstart->getNextToken();
	int basepitch = 60;
	HumRegex hre;
	int velocity = 64;
	int midinote = -1;
	HTp tiednote = NULL;
	HTp lastnote = NULL;
	while (current) {
		if (current->isNull()) {
			current = current->getNextToken();
			continue;
		}
		if (current->isInterpretation()) {
			if (current->compare(0, 4, "*AT:") == 0) {
				string pitch = current->substr(4);
				if (pitch.size() == 2) {
					switch (pitch[0]) {
						case 'C': basepitch = 0; break;
						case 'D': basepitch = 2; break;
						case 'E': basepitch = 4; break;
						case 'F': basepitch = 5; break;
						case 'G': basepitch = 7; break;
						case 'A': basepitch = 9; break;
						case 'B': basepitch = 11; break;
						default:  basepitch = 0;
					}
					switch (pitch[1]) {
						case  '0': basepitch += 12*1; break;
						case  '1': basepitch += 12*2; break;
						case  '2': basepitch += 12*3; break;
						case  '3': basepitch += 12*4; break;
						case  '4': basepitch += 12*5; break;
						case  '5': basepitch += 12*6; break;
						case  '6': basepitch += 12*7; break;
						case  '7': basepitch += 12*8; break;
						case  '8': basepitch += 12*9; break;
						default:   basepitch += 12*5;
					}
				}
			}
		}
		if (!current->isData()) {	
			current = current->getNextToken();
			continue;
		}
		int snum = -1;
		if (!hre.search(current, "(\\d+)")) {
			current = current->getNextToken();
			continue;
		}
		snum = hre.getMatchInt(1);
		midinote = basepitch + snum;

		bool tieQ = false;
		if (current->find(tie_signifier) != std::string::npos) {
			tieQ = true;
		}
		if (!tieQ && lastnote) {
			HumNum etime = current->getDurationFromStart() + current->getDuration();
			if (etime > 4) {
				etime = 4;
			}
			int etick = (int)(etime.getFloat() * tpq + 0.5);
			mfile.addNoteOff(track, etick, channel, midinote);
		}
		if (!tiednote) {
			HumNum stime = current->getDurationFromStart();
			int stick = (int)(stime.getFloat() * tpq + 0.5);
			mfile.addNoteOn(track, stick, channel, midinote, velocity);
		}
		if (tieQ) {
			tiednote = current;
		} else {
			tiednote = NULL;
		}
		current = current->getNextToken();
	}
}


