/***************************************************************************
 *   Copyright (C) 2007 by Mark A. Toman   *
 *   kram1024@localhost.localdomain   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <iostream>
#include <malloc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "kosinski.h"

//here we declare the needed variables...
char *infile;    //input file
char *outfile;   //output file
long pointer;    //for -x switch with Decomp
long slidewin;   //used with -c switch with CompEx
long reclen;     //used with -c switch with CompEx
bool modulated;  //used with -m switch
bool extract;    //for -x switch
bool extend;     //used for -c switch
bool help;       //for -? flag
bool verbose;    //for -v switch
int error;       //for error handling
int arg_id=1;    //for parsing arguments
std::string param;
std::string aptr;
std::string asw;
std::string arl;

//entry point
int main( int argc, char *argv[] )
{
    if (argc > 2)
    {
	int sw_top = argc - 2;
	while ( arg_id < sw_top )
	{
	    param = argv[arg_id];
	    
	    if (param == "-?")
	    {
		help = true;
	    }
	    else if (param == "--help")
	    {
		help = true;
	    }
	    else if (param == "-m")
	    {
		modulated = true;
	    }
	    else if (param == "--modulated")
	    {
		modulated = true;
	    }
	    else if (param == "-c")
	    {
		extend = true;
		arg_id++;
		slidewin = atol(argv[arg_id]);
		asw = argv[arg_id];
		arg_id++;
		reclen = atol(argv[arg_id]);
		arl = argv[arg_id];
	    }
	    else if (param == "--extend")
	    {
		extend = true;
		arg_id++;
		slidewin = atol(argv[arg_id]);
		asw = argv[arg_id];
		arg_id++;
		reclen = atol(argv[arg_id]);
		arl = argv[arg_id];
	    }
	    else if ((param == "-x")
	             || (param == "--extract"))
	    {
		extract = true;
		arg_id++;
		std::string str = argv[arg_id];
		if ((str.size() >= 3)
		    && (str[0] == '0')
		    && (str[1] == 'x')) {
  		sscanf(str.c_str() + 2, "%x", &pointer);
		}
		else {
  		pointer = atol(argv[arg_id]);
  	}
		aptr = argv[arg_id];

	    }
	    else if (param == "-v")
	    {
		verbose = true;
	    }
	    else if (param == "--verbose")
	    {
		verbose = true;
	    }
	    else
	    {
		error = 1;
	    }
	    arg_id++;
	}
	if (help == true)
	{
	    printf("sega 0.3 kosinski compressor/decompressor\nUsage: koscmp [-x|--extract {pointer}] [-v|--verbose] [-c|--extend {slidewin} {reclen}] [-m|--modulated] {input_filename} {output_filename}\n\n-x,--extract  extract from {pointer} address in file.\n-v,--verbose  display verbose output.\n-c,--extend  set recursion length and sliding window.\n-m,--modulated  use moduled compression.\n");
	    return 0;
	}
	if (error > 0)
	{
	    switch ( error )
	    {
		case 1:
		printf("koscmp: invalid switch. stop.\n");
		return error;
		break;
		case 2:
		printf("koscmp: too many switches. stop.\n");
		return error;
		break;
		case 3:
		printf("koscmp: unknown error. stop.\n");
		return error;
		break;
	    }
	}
	if (extract == true)
	{
	    std::string input="";
	    std::string output="";
	    input = argv[argc - 2];
	    const char *infile = input.c_str();
	    output = argv[argc -1];
	    const char *outfile = output.c_str();
	    if ( extend == true)
	    {
		printf("koscmp: extend option cannot be used in extraction. stop.\n");
		return 4;
	    }
	    else
	    {
		Decomp((char *)infile, (char *)outfile, pointer, modulated);
	    if (verbose == true)
	    {
		printf ("extracted: ");
		printf (infile);
		printf (":");
		printf (aptr.c_str());
		printf (" -> ");
		printf (outfile);
		if (modulated == true)
		{
		    printf (" [modulated = TRUE]");
	        }
		printf ("]\n");
	    }
		return 0;
	    }
	}
	else
	{
	    std::string input="";
	    std::string output="";
	    input = argv[argc - 2];
	    const char *infile = input.c_str();
	    output = argv[argc -1];
	    const char *outfile = output.c_str();
	    if ( extend == true)
	    {
		CompEx((char *)infile, (char *)outfile, slidewin, reclen, modulated);
	    if (verbose == true)
	    {
		printf ("compressed: ");
		printf (infile);
		printf (" -> ");
		printf (outfile);
		printf (" [SlideWin = ");
		printf (asw.c_str());
		printf (", RecLen= ");
		printf (arl.c_str());
		if (modulated == true)
		{
		    printf (", modulated = TRUE");
	        }
		printf ("]\n");
	    }
		return 0;
	    }
	    else
	    {
		Comp((char *)infile, (char *)outfile, modulated);
	    if (verbose == true)
	    {
		printf ("compressed: ");
		printf (infile);
		printf (" -> ");
		printf (outfile);
		if (modulated == true)
		{
		    printf (" [modulated = TRUE]");
	        }
		printf ("\n");
	    }
		return 0;
	    }
	}
    }
     else if (argc < 2)
    {
	    printf("sega 0.3 kosinski compressor/decompressor\nUsage: koscmp [-x|--extract {pointer}] [-v|--verbose] [-c|--extend {slidewin} {reclen}] [-m|--modulated] {input_filename} {output_filename}\n\n-x,--extract  extract from {pointer} address in file.\n-v,--verbose  display verbose output.\n-c,--extend  set recursion length and sliding window.\n-m,--modulated  use moduled compression.\n");
	    return 0;
    }
   else
    {
	    std::string input="";
	    std::string output="";
	    input = argv[argc - 2];
	    const char *infile = input.c_str();
	    output = argv[argc -1];
	    const char *outfile = output.c_str();
	    printf ("trace\n");
	    Comp((char *)infile, (char *)outfile, false);
	    if (verbose == true)
	    {
		printf ("compressed: ");
		printf (infile);
		printf (" -> ");
		printf (outfile);
		printf ("\n");
	    }
	    return 0;
    }
}
