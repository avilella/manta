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

#include "GenerateSVCandidates.hh"
#include "GSCOptions.hh"
#include "EdgeRetriever.hh"
#include "SVFinder.hh"
#include "SVScorer.hh"

#include "blt_util/log.hh"
#include "common/OutStream.hh"
#include "manta/ReadGroupStatsSet.hh"
#include "format/VcfWriterCandidateSV.hh"
#include "format/VcfWriterSomaticSV.hh"

#include "boost/foreach.hpp"

#include <iostream>




static
void
runGSC(
    const GSCOptions& opt,
    const char* progName,
    const char* progVersion)
{
    const bool isSomatic(! opt.somaticOutputFilename.empty());

    SVFinder svFind(opt);
    const SVLocusSet& cset(svFind.getSet());

    SVScorer svScore(opt, cset.header);

    EdgeRetriever edger(cset, opt.binCount, opt.binIndex);

    OutStream candfs(opt.candidateOutputFilename);
    OutStream somfs(opt.somaticOutputFilename);

    VcfWriterCandidateSV candWriter(opt.referenceFilename,cset,candfs.getStream());
    VcfWriterSomaticSV somWriter(opt.somaticOpt, (! opt.chromDepthFilename.empty()),
                                 opt.referenceFilename,cset,somfs.getStream());

    if (0 == opt.binIndex)
    {
        candWriter.writeHeader(progName, progVersion);
        if (isSomatic) somWriter.writeHeader(progName, progVersion);
    }

    SVCandidateData svData;
    std::vector<SVCandidate> svs;
    SomaticSVScoreInfo ssInfo;
    while (edger.next())
    {
        const EdgeInfo& edge(edger.getEdge());

        try
        {
            // find number, type and breakend range of SVs on this edge:
            svFind.findCandidateSV(edge,svData,svs);

            candWriter.writeSV(edge, svData, svs);

            if (isSomatic)
            {
                unsigned svIndex(0);
                BOOST_FOREACH(const SVCandidate& sv, svs)
                {
                    svScore.scoreSomaticSV(svData, svIndex, sv, ssInfo);
                    somWriter.writeSV(edge, svData, svIndex, sv, ssInfo);
                    svIndex++;
                }
            }
        }
        catch (...)
        {
            log_os << "Exception caught while processing graph component: " << edge << "\n";
            log_os << "\tnode1:" << cset.getLocus(edge.locusIndex).getNode(edge.nodeIndex1);
            log_os << "\tnode2:" << cset.getLocus(edge.locusIndex).getNode(edge.nodeIndex2);

            throw;
        }
    }

}



void
GenerateSVCandidates::
runInternal(int argc, char* argv[]) const
{

    GSCOptions opt;

    parseGSCOptions(*this,argc,argv,opt);
    runGSC(opt, name(), version());
}
