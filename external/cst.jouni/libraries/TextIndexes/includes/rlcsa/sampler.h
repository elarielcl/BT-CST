#ifndef SAMPLER_H
#define SAMPLER_H


#include <deque>

#include "utils.h"


namespace TextIndexes
{


typedef std::pair<uint, uint> node_type;


class WeightedSampler
{
  public:
    // We assume that weights are non-negative.
    // 'weights' will be deleted in the constructor.
    WeightedSampler(int* weights, uint _size, bool _use_psi);
    ~WeightedSampler();

    // Call this to build the samples. Returns 'true' if ok.
    // Set 'initial_adjustment' to nonzero if you know the correct value.
    bool buildSamples(uint number, sint initial_adjustment);

    // Returns the given number of samples as sorted (index, value) pairs.
    // The user must delete the samples.
    pair_type* getSamples(uint* inverse_sa, uint number_of_sequences);

    const static uint NOT_READY = 0;
    const static uint READY = 1;
    const static uint SAMPLED = 2;

    inline uint getStatus() const { return this->status; }
    inline uint getSize() const { return this->size; }
    inline uint getItems() const { return this->number_of_samples; }

  private:
    uint size;
    bool use_psi;

    sint* path_weights;
    uint* predecessors;

    /*
      When using LF: Edges (V_i, V_n)
        edge_weights[i] is \sum_{j = i}^{n - 1} (j - i) weights[j]
        edge_totals[i]  is \sum_{j = i}^{n - 1} weights[j]

      When using Psi: Edges (V_0, V_i)
        edge_weights[i] is \sum_{j = 1}^{i} (i - j) weights[j]
        edge_totals[i]  is \sum_{j = 1}^{i} weights[j]
    */
    sint* edge_weights;
    sint* edge_totals;  // FIXME do not store explicitly?

    pair_type* samples;
    uint       number_of_samples;

    uint status;

    void cleanUp();

    // Build a path of 'links' edges that is no heavier than 'path_a' and 'path_b'.
    // Store it in 'path_a', which is assumed to be the longer one.
    void buildPath(uint links, uint* path_a, uint length_a, uint* path_b, uint length_b);

    // Finds the shortest or the longest path of minimum weight.
    uint minimumWeightPath(sint adjustment, bool shortest);
    node_type link(std::deque<node_type>& active_nodes, uint to, sint adjustment, bool shortest);

    // Is 'a' or 'c' always strictly better than 'b'?
    bool bridge(uint a, uint b, uint c, sint adjustment);

    void calculateEdges(int* weights);

    sint getEdgeWeight(uint from, uint to, sint adjustment);
    sint getPathWeight(uint through, uint to, sint adjustment);

    // Is the path through 'a' to 'to' (strictly) better than through 'b' to 'to'?
    bool isBetter(uint a, uint b, uint to, sint adjustment);
    bool isStrictlyBetter(uint a, uint b, uint to, sint adjustment);

    // These are not allowed.
    WeightedSampler();
    WeightedSampler(const WeightedSampler&);
    WeightedSampler& operator = (const WeightedSampler&);
};


}; // namespace TextIndexes


#endif // SAMPLER_H
