/*
  Copyright (c) 2009 Stefan Kurtz <kurtz@zbh.uni-hamburg.de>
  Copyright (c) 2009 Center for Bioinformatics, University of Hamburg

  Permission to use, copy, modify, and distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "core/option.h"
#include "core/ma.h"
#include "core/unused_api.h"
#include "core/tool.h"
#include "gt_chain2dim.h"

static void *gt_chain2dim_arguments_new (void)
{
  return gt_malloc (sizeof (GtChain2dimoptions));
}

static void gt_chain2dim_arguments_delete (void *tool_arguments)
{
  GtChain2dimoptions *arguments = tool_arguments;

  if (!arguments)
  {
    return;
  }
  gt_str_delete (arguments->matchfile);
  gt_str_array_delete (arguments->globalargs);
  gt_str_array_delete (arguments->localargs);
  gt_free (arguments);
}

static GtOptionParser *gt_chain2dim_option_parser_new (void *tool_arguments)
{
  GtChain2dimoptions *arguments = tool_arguments;
  GtOptionParser *op;
  GtOption *option, *optionglobal, *optionlocal;

  gt_assert (arguments != NULL);
  arguments->matchfile = gt_str_new ();
  arguments->globalargs = gt_str_array_new();
  arguments->localargs = gt_str_array_new();

  op = gt_option_parser_new("[options] -m matchfile",
                            "Chain pairwise matches.");

  gt_option_parser_set_mailaddress (op, "<kurtz@zbh.uni-hamburg.de>");
  option = gt_option_new_filename("m","Specify file containing the matches\n"
                                  "mandatory options",
                                  arguments->matchfile);
  gt_option_parser_add_option(op, option);
  gt_option_is_mandatory (option);

  optionglobal = gt_option_new_stringarray("global",
                   "perform global chaining\n"
                   "optional parameter gc switches\n"
                   "on gap costs (according to L1-model)\n"
                   "optional parameter ov means\n"
                   "that overlaps between matches are allowed",
                   arguments->globalargs);
  gt_option_parser_add_option(op, optionglobal);

  optionlocal = gt_option_new_stringarray("local",
                   "perform local chaining\n"
                   "compute local chains (according to L1-model).\n"
                   "If no parameter is given, compute local chains with\n"
                   "maximums score.\n"
                   "If parameter is given, this must be a positive number\n"
                   "optionally followed by the character b or p.\n"
                   "If only the number, say k, is given, this is the minimum\n"
                   "score of the chains output.\n"
                   "If a number is followed by character b, then output all\n"
                   "chains with the largest k scores.\n"
                   "If a number is followed by character p, then output all\n"
                   "chains with scores at most k percent away\n"
                   "from the best score.",
                   arguments->localargs);
  gt_option_parser_add_option(op, optionlocal);
  gt_option_exclude(optionlocal,optionglobal);
  option = gt_option_new_double("wf","specify weight factor > 0.0 to obtain "
                                     "score of a fragment\nrequires one of "
                                     "the options\n-localconst\n-global "
                                     "gc\n-global ov",
                                     &arguments->weightfactor,0.0);
  gt_option_parser_add_option(op, option);
  option = gt_option_new_ulong("maxgap","specify maximal width of gap in chain",
                                         &arguments->maxgap,0);
  gt_option_parser_add_option(op, option);
  option = gt_option_new_bool("silent","do not output the chains but only "
                                       "report their lengths and scores",
                                       &arguments->silent,false);
  gt_option_parser_add_option(op, option);
  option = gt_option_new_verbose(&arguments->verbose);
  gt_option_parser_add_option(op, option);
  return op;
}

/*
  The following string is used to trigger the usage of gap costs
  for global chaining.
*/

#define GAPCOSTSWITCH        "gc"

/*
  The following string is used to trigger the use of a chaining algorithm
  allowing for overlaps between the hits.
*/

#define OVERLAPSWITCH        "ov"

static int gt_chain2dim_arguments_check (GT_UNUSED int rest_argc,
                                         GT_UNUSED void *tool_arguments,
                                         GT_UNUSED GtError * err)
{
  return 0;
}

static int gt_chain2dim_runner (GT_UNUSED int argc,
                                GT_UNUSED const char **argv,
                                GT_UNUSED int parsed_args,
                                void *tool_arguments,
                                GT_UNUSED GtError * err)
{
  GtChain2dimoptions *arguments = tool_arguments;

  gt_error_check (err);
  gt_assert (arguments != NULL);
  gt_assert (parsed_args == argc);
  return 0;
}

GtTool *gt_chain2dim (void)
{
  return gt_tool_new (gt_chain2dim_arguments_new,
                      gt_chain2dim_arguments_delete,
                      gt_chain2dim_option_parser_new,
                      gt_chain2dim_arguments_check,
                      gt_chain2dim_runner);
}
