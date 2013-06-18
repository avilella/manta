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

#include "manta/SVLocusSet.hh"

#include "boost/foreach.hpp"

#include <algorithm>
#include <iostream>



void
SVLocusSet::
merge(SVLocus& inputLocus)
{
    const unsigned startLocusIndex(_loci.size());
    unsigned locusIndex(startLocusIndex);

    // test each node for intersection and insert/join to existing node:
    BOOST_FOREACH(SVLocusNode* inputNodePtr, inputLocus)
    {
        typedef ins_type::iterator in_iter;

        ins_type intersect;

        // get all existing nodes which intersect with this one:
        {
            ins_type::value_type search(std::make_pair(inputNodePtr,0));
            in_iter it(std::lower_bound(_inodes.begin(), _inodes.end(), search));

            // first look forward and extend to find all nodes which this inputNode intersects:
            for(in_iter it_fwd(it); it_fwd !=_inodes.end(); ++it_fwd)
            {
                SVLocusNode* nodePtr(it_fwd->first);
                if(! inputNodePtr->interval.range.is_range_intersect(nodePtr->interval.range)) break;
                intersect[nodePtr] = it_fwd->second;
            }

            // now find all intersecting nodes in reverse direction:
            for(in_iter it_rev(it); it_rev !=_inodes.begin(); )
            {
                --it_rev;
                SVLocusNode* nodePtr(it_rev->first);
                if(! inputNodePtr->interval.range.is_range_intersect(nodePtr->interval.range)) break;
                intersect[nodePtr] = it_rev->second;
            }
        }

        if(intersect.empty())
        {
            // if no nodes intersect, then insert inputNode into inputLocus set:
            if(locusIndex>=_loci.size())
            {
                _loci.resize(locusIndex+1);
            }

            _loci[locusIndex].copyNode(inputLocus,inputNodePtr);
            _inodes[inputNodePtr] = locusIndex;
        }
        else
        {
            // otherwise, merge inputNode to the first intersecting inputNode, then merge remaining nodes with each other

            {
                //
                // first step is to assign all intersect clusters to the lowest index number
                //

                // get lowest index number:
                BOOST_FOREACH(const ins_type::value_type& val, intersect)
                {
                    if(val.second < locusIndex)
                    {
                        locusIndex = val.second;
                    }
                }

                // assign all intersect clusters to the lowest index number
                combineLoci(startLocusIndex,locusIndex);
                BOOST_FOREACH(const ins_type::value_type& val, intersect)
                {
                    combineLoci(val.second,locusIndex);
                }
            }

            {
                //
                // second step is to add this inputNode into the graph
                //
                _loci[locusIndex].copyNode(inputLocus,inputNodePtr);
                _inodes[inputNodePtr] = locusIndex;
            }

            {
                //
                // third step is to merge this inputNode with each intersecting inputNode:
                //
#if 0
                BOOST_FOREACH(const ins_type::value_type& val, intersect)
                {
                    val.first
                    combineLoci(val.second,locusIndex);
                }
                _loci[locusIndex]._graph.push_back(inputLocus._graph[i]);

                inode key;
                key.nodePtr=nodePtr;
                _inodes[key] = locusIndex;
#endif
            }
        }
    }
}



void
SVLocusSet::
combineLoci(
        const unsigned fromIndex,
        const unsigned toIndex) {

    assert(toIndex<_loci.size());

    if(fromIndex>=_loci.size()) return;

    SVLocus& fromLocus(_loci[fromIndex]);
    if(fromLocus.empty()) return;

    SVLocus& toLocus(_loci[toIndex]);
    BOOST_FOREACH(SVLocusNode* fromNodePtr, fromLocus)
    {
        toLocus.copyNode(fromLocus,fromNodePtr);

        // update indexing structure:
        assert(_inodes[fromNodePtr] == fromIndex);
        _inodes[fromNodePtr] = toIndex;
    }
    fromLocus.clear();
}


void
SVLocusSet::
write(std::ostream& os) const
{
    os << "FOO";
}