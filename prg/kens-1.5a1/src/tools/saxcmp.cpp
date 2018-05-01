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
#include "saxman.h"

//here we initialize variables for use with libenigma
long pointer;        //for -x switch with Decomp
int arg_id=1;        //for parsing arguments
bool extract;        //for -x switch
bool help;           //for -? flag
int error;           //for error handling
bool withsize;       //for use with -S switch in compression
bool setsize;        //for use with -s switch in extraction
bool verbose;  //for -v switch
unsigned short size; //for use with -s switch in extraction
int inttous;
std::string param;
std::string aptr;
std::string asize;

//entry point
int main( int argc, char *argv[] )
{
    withsize = true;
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
	    else if (param == "-S")
	    {
		withsize = false;
	    }
	    else if (param == "-s")
	    {
		setsize = true;
		arg_id++;
		inttous = atoi(argv[arg_id]);
		size = (unsigned short)inttous;
	    }
	    else if (param == "--size")
	    {
		setsize = true;
		arg_id++;
		inttous = atoi(argv[arg_id]);
		size = (unsigned short)inttous;
	    }
	    else if (param == "--without-size")
	    {
		withsize = false;
	    }
	    else if (param == "--help")
	    {
		help = true;
	    }
	    else if (param == "-x")
	    {
		extract = true;
		arg_id++;
		pointer = atol(argv[arg_id]);
		aptr = argv[arg_id];
	    }
	    else if (param == "--extract")
	    {
		extract = true;
		arg_id++;
		pointer = atol(argv[arg_id]);
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
	    printf("sega 0.3 saxman compressor/decompressor\nUsage: saxcmp [-x|--extract {pointer}] [-v|--verbose] [-S|--without-size] [-s|--size {size}] {input_filename} {output_filename}\n\n-x,--extract  extract from {pointer} address in file.\n-v,--verbose  display verbose output.\n-s,--size  specify size.\n-S,--without-size  compress without size.\n");
	    return 0;
	}
	if (error > 0)
	{
	    switch ( error )
	    {
		case 1:
		printf("saxcmp: invalid switch. stop.\n");
		return error;
		break;
		case 2:
		printf("saxcmp: too many switches. stop.\n");
		return error;
		break;
		case 3:
		printf("saxcmp: unknown error. stop.\n");
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
	    Decomp2((char *)infile, (char *)outfile, pointer, size);
	    if (verbose == true)
	    {
		printf ("extracted: ");
		printf (infile);
		printf (":");
		printf (aptr.c_str());
		printf (" -> ");
		printf (outfile);
		if (setsize == true)
		{
		    printf (" [Size= ");
		    printf (asize.c_str());
		    printf ("]");
		}
		printf ("\n");
	    }
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
	    Comp2((char *)infile, (char *)outfile, withsize);
	    if (verbose == true)
	    {
		printf ("compressed: ");
		printf (infile);
		printf (" -> ");
		printf (outfile);
		if (withsize == false)
		{
		    printf (" [WithSize= FALSE]");
		}
	    }
	    return 0;
	}
    }
     else if (argc < 2)
    {
	    printf("sega 0.3 saxman compressor/decompressor\nUsage: saxcmp [-x|--extract {pointer}] [-v|--verbose] [-S|--without-size] [-s|--size {size}] {input_filename} {output_filename}\n\n-x,--extract  extract from {pointer} address in file.\n-v,--verbose  display verbose output.\n-s,--size  specify size.\n-S,--without-size  compress without size.\n");
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
	    Comp2((char *)infile, (char *)outfile, true);
	    if (verbose == true)
	    {
		printf ("compressed: ");
		printf (infile);
		printf (" -> ");
		printf (outfile);
	    }
	    return 0;
    }
}
