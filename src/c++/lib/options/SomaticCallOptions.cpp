// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Manta
// Copyright (c) 2013 Illumina, Inc.
//
// This software is provided under the terms and conditions of the
// Illumina Open Source Software License 1.
//
// You should have received a copy of the Illumina Open Source
// Software License 1 along with this program. If not, see
// <https://github.com/downloads/sequencing/licenses/>.
//

///
/// \author Chris Saunders
///

#include "options/SomaticCallOptions.hh"


boost::program_options::options_description
getOptionsDescription(SomaticCallOptions& opt)
{
    namespace po = boost::program_options;
    po::options_description desc("somatic-calling");
    desc.add_options()
    ("max-depth-factor", po::value(&opt.maxDepthFactor)->default_value(opt.maxDepthFactor),
     "Variants where the non-tumor depth around the breakpoint is greater than this factor x the chromosomal mean will be filtered out")
    ("min-somatic-score", po::value(&opt.minOutputSomaticScore)->default_value(opt.minOutputSomaticScore),
     "minimum somatic score for variants included in the somatic output vcf")
    ;

    return desc;
}

