/*
  Copyright (c) 2006-2007 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2006-2007 Center for Bioinformatics, University of Hamburg
  See LICENSE file or http://genometools.org/license.html for license details.
*/

#include "gt.h"

static int parse_options(unsigned int *be_tolerant, int argc, char **argv)
{
  int parsed_args;
  OptionParser *op;
  Option *option;

  op = option_parser_new("[gtf_file]",
                         "Parse GTF2.2 file and show it as GFF3.");

  /* -tolerant */
  option = option_new_boolean("tolerant",
                              "be tolerant when parsing the GTF file",
                              be_tolerant, 0);
  option_parser_add_option(op, option);

  /* parse */
  parsed_args = option_parser_parse_max_args(op, argc, argv, versionfunc, 1);
  option_parser_free(op);

  return parsed_args;
}

int gt_gtf2gff3(int argc, char *argv[])
{
  Genome_stream *gtf_in_stream,
                *gff3_out_stream;
  Genome_node *gn;
  int parsed_args;
  unsigned int be_tolerant;

  /* option parsing */
  parsed_args = parse_options(&be_tolerant, argc, argv);

  /* create a gtf input stream */
  gtf_in_stream = gtf_in_stream_new(argv[parsed_args], be_tolerant);

  /* create a gff3 output stream */
  gff3_out_stream = gff3_out_stream_new(gtf_in_stream, stdout);

  /* pull the features through the stream and free them afterwards */
  while ((gn = genome_stream_next_tree(gff3_out_stream, NULL)))
    genome_node_rec_free(gn);

  /* free */
  genome_stream_free(gff3_out_stream);
  genome_stream_free(gtf_in_stream);

  return EXIT_SUCCESS;
}
