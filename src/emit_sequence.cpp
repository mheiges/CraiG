/*****************************************************************************
 * This is the main of the emit_sequence program. It generates a sequence of 
 * an arbitrary length given as argument, using a context IMM, which must have
 * been previously computed with train_cimm tool
 *
 *   Copyright (C) 2002-2007  Axel E. Bernal (abernal@seas.upenn.edu)
 *   
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version.
 *   
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *   
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307, USA.
 *    
 *   The GNU General Public License is contained in the file COPYING.
 *   
 ****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#include "Gene.h"
#include "Utils.h"
#include "IMM.h"
#include "ContextIMM.h"
#include "Sequence.h"
#include "Organism.h"
#include "ArgParseUtils.h"
#include <time.h>
#include <streambuf>
#include <list>

// The following variables are organism dependent. When built, organism class will encapsulate all of them


void printHelp(const char * pname, ::ofstream &fd) {
  fd << "CRAIG v. " << CRAIG_VERSION << " tool for training a context IMM given an input sequence.\n";
  fd << "Written by Axel E. Bernal (" << AUTHOR_EMAIL << ")\n\n";
  fd << "Usage :" << pname << " [options] CIMM_MODEL > SEQUENCE\n\n";
  fd << "positional arguments:\n";
  ArgParseUtils::displayOption(fd, "CIMM_MODEL", "Name of the file containing the model generated with train_content_sensor");
  fd << "optional arguments:\n";
  ArgParseUtils::displayOption(fd, "--version", "Print version name and license information");
  ArgParseUtils::displayOption(fd, "-h --help", "Show this message and exit");
  ArgParseUtils::displayOption(fd, "-v --verbose", "Turns on output of debugging information");
  ArgParseUtils::displayOption(fd, "--length=LENGTH", "Length of the emited sequence [1000]");
  ArgParseUtils::displayOption(fd, "--subchain=SUBCHAIN", "The IMM subchain to use for generating the first character of sequence. Useful only for inhomogeneus cimm's [1]");
  ArgParseUtils::displayOption(fd, "--context=CONTEXT", "The context to be used [0]");
  ArgParseUtils::displayOption(fd, "SEQUENCE", "The sequence generated by cimm_model");
  fd << "Report bugs to <" << AUTHOR_EMAIL << ">\n";
}

bool verbose = false;

int main(int argc, char *argv[]) {
  std::string modelFile = "";
  int length = 1000, pos = 1;
  int i, cls = 0;

  try {
    for(i = 1; i < argc; i++) {
      if(!strncmp(argv[i], "--context=", 10)) {
        sscanf(argv[i] + 10, "%d", &cls);
      }
      else if(!strncmp(argv[i], "--verbose", 9) 
         || !strncmp(argv[i], "-v", 2)) {  
        verbose = true;  
      }
      else if(!strncmp(argv[i], "--help", 6) 
         || !strncmp(argv[i], "-h", 2)) {
        printHelp("emit_sequence", (std::ofstream &)cout); 
        exit(0);  
      }
      else if(!strncmp(argv[i], "--version", 9)) { 
        PRINT_VERSION(cerr, "emit_sequence", "tool for training a context IMM given an input sequence");
        PRINT_DISCLAIMER(cerr, "emit_sequence"); 
        exit(0);  
      }
      else if(!strncmp(argv[i], "--length=", 9)) {
        sscanf(argv[i] + 9, "%d", &length);
      }
      else if(!strncmp(argv[i], "--subchain=", 11)) {
        sscanf(argv[i] + 11, "%d", &pos);
      }
      else
        break;
    } 

    if(argc - i < 1)
      throw EXCEPTION(BAD_USAGE, "insufficient arguments");

    if(length < 0 || pos < 1 || cls < 0)
      throw EXCEPTION(BAD_USAGE, "numeric parameter(s) is(are) out of bounds");


    modelFile = std::string(argv[i++]);

    ::ifstream immStream(modelFile.c_str());
    
    if(!immStream.is_open())
      throw EXCEPTION(FILE_UNAVAILABLE, modelFile);
    
    srand ( time(NULL) );

    DNASigma sigma;
    ContextIMM *cimm = new ContextIMM(immStream, &sigma);

    char *seq = new char[length + 1];
    cimm->emitSequence(cls, seq, pos, length);
    cout << seq << "\n";
    // deleting some unnecesesary structures
    delete [] seq;
    delete cimm;   
    return 0;
  }
  catch(exception *e) { 
    perror(e->what());

    GenLibExcept *gle = (GenLibExcept *)e;
    if(gle->error() == BAD_USAGE)
      cerr << "use --help for more information\n";
    delete e;
    exit(1);

  }   
}
