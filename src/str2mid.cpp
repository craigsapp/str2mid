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
void convertSpineToMidiTrack (MidiFile& mfile, HTp sstart, int track,
                              int channel, vector<int>& attacks,
                              HumdrumFile& infile);
void insertTempo             (MidiFile& midifile, HumdrumFile& infile);
void getAttackList           (vector<int>& attacks, HumdrumFile& infile);

string tie_signifier = "L";
int tpq = 240;
int miditype = 0;

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
	if (miditype != 0) {
		midifile.addTracks(scount);
	}
	if (miditype == 0) {
		midifile.addTimbre(0, 0, 0, 24);
	}
	vector<int> attacks;
	getAttackList(attacks, infile);
	for (int i=0; i<scount; i++) {
		if (miditype != 0) {
			midifile.addTimbre(i+1, 0, i, 24);
		}
		convertSpineToMidiTrack(midifile, strspines[scount - i - 1], i+1, i,
				attacks, infile);
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

void convertSpineToMidiTrack(MidiFile& mfile, HTp sstart, int track,
		int channel, vector<int>& attacks, HumdrumFile& infile) {
	if (miditype == 0) {
		track = 0;
		channel = 0;
	}
	HTp current = sstart->getNextToken();
	int basepitch = 60;
	HumRegex hre;
	int velocity = 64;
	int midinote = 0;
	int lastmidinote = 0;
	HTp tiednote = NULL;
	HTp lastnote = NULL;
	while (current) {
		if (current->isInterpretation()) {
			if (current->compare(0, 4, "*AT:") == 0) {
				string pitch = current->substr(4);
				if (hre.search(pitch, "^(\\d)$")) {
					basepitch = hre.getMatchInt(1);
				} else if (hre.search(pitch, "^([A-Ga-g])([0-9])$")) {
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
						case  '0': basepitch += 12*2; break;
						case  '1': basepitch += 12*3; break;
						case  '2': basepitch += 12*4; break;
						case  '3': basepitch += 12*5; break;
						case  '4': basepitch += 12*6; break;
						case  '5': basepitch += 12*7; break;
						case  '6': basepitch += 12*8; break;
						case  '7': basepitch += 12*9; break;
						default:   basepitch += 12*5;
					}
				} else {
					cerr << "UNKNOWN ABSOLUTE TUNING " << current << endl;
				}
			} else if (current->compare(0, 4, "*CH:") == 0) {
				// MIDI channel
				string chan = current->substr(4);
				if (hre.search(chan, "^(\\d+)$")) {
					channel = hre.getMatchInt(1);
					channel--;
					if (channel < 0) {
						channel = 0;
					} else if (channel > 15) {
						channel = 15;
					}
				}
			}
		}
		if (!current->isData()) {	
			current = current->getNextToken();
			continue;
		}

		if (current->isNull()) {
			if (attacks[current->getLineIndex()] && (lastmidinote > 0) &&
					(lastnote != NULL) && !tiednote) {
				// turn off previous note if not a tied note.
				HumNum etime = current->getDurationFromStart();
				int etick = (int)(etime.getFloat() * tpq + 0.5);
				mfile.addNoteOff(track, etick, channel, lastmidinote);
				lastmidinote = 0;
				lastnote = NULL;
			}
			current = current->getNextToken();
			continue;
		}

		int snum = 0;  // string number
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
		if (!tiednote) {
			if (lastnote) {
				HumNum etime = current->getDurationFromStart();
				int etick = (int)(etime.getFloat() * tpq + 0.5);
				mfile.addNoteOff(track, etick, channel, lastmidinote);
				lastmidinote = 0;
				lastnote = NULL;
			}
			HumNum stime = current->getDurationFromStart();
			int stick = (int)(stime.getFloat() * tpq + 0.5);
			mfile.addNoteOn(track, stick, channel, midinote, velocity);
			lastnote = current;
			lastmidinote = midinote;
		}
		if (tieQ) {
			tiednote = current;
		} else {
			tiednote = NULL;
		}
		current = current->getNextToken();
	}

	if ((lastnote != NULL) && (lastmidinote > 0)) {
		// turn off the last note
		HumNum etime = infile[infile.getLineCount()-1].getDurationFromStart();
		int etick = (int)(etime.getFloat() * tpq + 0.5);
		mfile.addNoteOff(track, etick, channel, lastmidinote);
		lastmidinote = 0;
		lastnote = NULL;
	}
}



//////////////////////////////
//
// getAttackList --
//

void getAttackList(vector<int>& attacks, HumdrumFile& infile) {
	attacks.resize(infile.getLineCount());
	std::fill(attacks.begin(), attacks.end(), 0);
	vector<int> states(infile.getMaxTrack()+1, 0);
	for (int i=0; i<infile.getLineCount(); i++) {
		if (!infile[i].isData()) {
			continue;
		}
		for (int j=0; j<infile[i].getFieldCount(); j++) {
			HTp token = infile.token(i, j);
			if (token->isNull()) {
				continue;
			}
			if (!token->isDataType("**str")) {
				continue;
			}
			int track = token->getTrack();
			bool lasttieQ = states[track];
			bool tieQ = false;
			if (token->find(tie_signifier) != std::string::npos) {
				tieQ = true;
			}
			if (lasttieQ) {
				if (!tieQ) {
					states[track] = 0;
				}
				continue;
			}
			if (tieQ) {
				states[track] = 1;
			}
			attacks[i]++;
		}
	}
	attacks.push_back(1);  // force note off at end (probably not needed).
}



