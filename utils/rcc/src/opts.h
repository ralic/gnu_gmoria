/*
   Copyright (C) 2014 Ben Asselstine
   Written by Ben Asselstine
  
   rcc is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   rcc is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with rcc; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef RCC_OPTS_H
#define RCC_OPTS_H

#include <stdio.h>
struct rcc_arguments_t
{
  int debug;
  char *infile;
  FILE *outfile;
  int consistency_check;
  int only_generate_constants;
};

struct arguments_t 
{
  struct rcc_arguments_t rcc;
};

void parse_opts(int argc, char **argv, struct arguments_t *arguments);

extern struct arguments_t arguments;
#endif
