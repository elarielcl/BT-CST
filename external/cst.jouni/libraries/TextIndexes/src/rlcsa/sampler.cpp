#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <limits>

#include <rlcsa/sampler.h>


namespace TextIndexes
{


WeightedSampler::WeightedSampler(int* weights, uint _size, bool _use_psi) :
  size(_size), use_psi(_use_psi),
  path_weights(0), predecessors(0),
  edge_weights(0), edge_totals(0),
  samples(0), number_of_samples(0),
  status(NOT_READY)
{
  if(weights == 0 || this->size == 0)
  {
    delete[] weights; return;
  }

  this->edge_weights = new sint[this->size + 1];
  this->edge_totals  = new sint[this->size + 1];
  this->calculateEdges(weights);
  delete[] weights;

  this->path_weights = new sint[this->size + 1];
  this->predecessors = new uint[this->size + 1];

  this->status = READY;
}

WeightedSampler::~WeightedSampler()
{
  this->cleanUp();
  delete[] this->samples; this->samples = 0;
}

void
WeightedSampler::cleanUp()
{
  delete[] this->path_weights; this->path_weights = 0;
  delete[] this->predecessors; this->predecessors = 0;
  delete[] this->edge_weights; this->edge_weights = 0;
  delete[] this->edge_totals; this->edge_totals = 0;
}

//--------------------------------------------------------------------------

bool
WeightedSampler::buildSamples(uint number, sint initial_adjustment)
{
  if(number == 0 || number > this->size || this->status != READY) { return false; }

  delete[] this->samples; this->samples = 0;
  this->number_of_samples = number;

  sint max_weight;
  if(this->use_psi) { max_weight = this->edge_weights[this->size]; }
  else              { max_weight = this->edge_weights[0]; }

  // 'left' should be -3 * max_weight, if we allow negative weights.
  sint left = 0, right = 3 * max_weight, mid = (this->size / number) * (this->size / number);
  if(initial_adjustment > 0) { mid = initial_adjustment; }
  uint shortest = 0, longest = 0;
  uint* shortest_path = 0;
  uint* longest_path = 0;

  // Find adjustment 'mid' such that the shortest minimum weight path has at most
  // 'number' links, while the longest minimum weight path has at least 'number' links.
  std::cout << "left: " << left << ", right: " << right << std::endl;
  while(true)
  {
    std::cout << "Adjustment: " << mid << std::endl;

    shortest = this->minimumWeightPath(mid, true);
    std::cout << "  shortest: " << shortest << " (weight " << this->path_weights[this->size] << ")" << std::endl;
    if(shortest > this->number_of_samples)
    {
      sint diff = mid - left;
      left = mid + 1;
      mid = std::min(left + 2 * diff, left + (right - left) / 2);
      continue;
    }
    shortest_path = new uint[shortest + 1];
    for(uint i = shortest + 1, cur = this->size; i > 0; i--, cur = this->predecessors[cur])
    {
      shortest_path[i - 1] = cur;
    }

    longest = this->minimumWeightPath(mid, false);
    std::cout << "  longest:  " << longest << " (weight " << this->path_weights[this->size] << ")" << std::endl;
    if(longest < this->number_of_samples)
    {
      right = mid - 1;
      mid = left + (right - left) / 2;
      delete[] shortest_path; shortest_path = 0;
      continue;
    }
    longest_path = new uint[longest + 1];
    for(uint i = longest + 1, cur = this->size; i > 0; i--, cur = this->predecessors[cur])
    {
      longest_path[i - 1] = cur;
    }

    break;
  }
  std::cout << std::endl;
  this->cleanUp();

  // Build a 'number'-link path from the saved paths.
  this->buildPath(this->number_of_samples, longest_path, longest, shortest_path, shortest);
  delete[] shortest_path; shortest_path = 0;

  this->samples = new pair_type[this->number_of_samples];
  for(uint i = 0; i < this->number_of_samples; i++)
  {
    samples[i].second = longest_path[i];
  }
  delete[] longest_path; longest_path = 0;

  this->status = SAMPLED;
  return true;
}

pair_type*
WeightedSampler::getSamples(uint* inverse_sa, uint number_of_sequences)
{
  if(inverse_sa == 0 || this->samples == 0 || this->status != SAMPLED) { return 0; }

  for(uint i = 0; i < this->number_of_samples; i++)
  {
    // FIXME should be "- number_of_sequences - 1" if there are multiple sequences.
    this->samples[i].first = inverse_sa[this->samples[i].second] - number_of_sequences;
  }
  std::sort(samples, samples + this->number_of_samples);

  pair_type* result = this->samples; this->samples = 0;
  this->status = NOT_READY;
  return result;
}

void
WeightedSampler::buildPath(uint links, uint* path_a, uint length_a, uint* path_b, uint length_b)
{
  uint cur_a = 0, cur_b = 0;
  uint adj = length_a - links;

  while(true)
  {
    if(cur_a == cur_b + adj && path_b[cur_b] <= path_a[cur_a] && path_b[cur_b + 1] >= path_a[cur_a + 1])
    {
      break;
    }

    if(path_a[cur_a] <= path_b[cur_b]) { cur_a++; }
    else                               { cur_b++; }
  }

  // Combine the prefix of 'path_b' until 'cur_b - 1' with the suffix of 'path_a' from 'cur_a'.
  for(uint i = 0; i <= cur_b; i++) { path_a[i] = path_b[i]; }
  for(uint i = cur_b + 1, j = cur_a + 1; j < length_a; i++, j++) { path_a[i] = path_a[j]; }
}

//--------------------------------------------------------------------------

uint // FIXME closer to the published algorithm?
WeightedSampler::minimumWeightPath(sint adjustment, bool shortest)
{
  this->path_weights[0] = 0;
  this->predecessors[0] = 0;

  std::deque<node_type> active_nodes;
  active_nodes.push_back(node_type(0, 0));

  for(uint i = 1; i < this->size; i++)
  {
    node_type current = this->link(active_nodes, i, adjustment, shortest);

    // Retire the active nodes that are before the linked predecessor.
    while(active_nodes.front().first < this->predecessors[i])
    {
      active_nodes.pop_front();
    }

    // Retire the first active nodes, if the following ones are strictly better.
    while(active_nodes.size() > 1 &&
          this->isStrictlyBetter(active_nodes[1].first, active_nodes[0].first, i + 1, adjustment))
    {
      active_nodes.pop_front();
    }

    // Retire the last active nodes, if the preceding node or the current node is always
    // strictly better.
    while(active_nodes.size() > 1 &&
          this->bridge(active_nodes[active_nodes.size() - 2].first, active_nodes.back().first, i, adjustment))
    {
      active_nodes.pop_back();
    }

    // Add the current node to active nodes, if it is at least as good as the active nodes
    // for some future node.
    if(isBetter(i, active_nodes.back().first, this->size, adjustment))
    {
      active_nodes.push_back(current);
    }
  }

  node_type result = this->link(active_nodes, this->size, adjustment, shortest);
  return result.second;
}

node_type
WeightedSampler::link(std::deque<node_type>& active_nodes, uint to, sint adjustment, bool shortest)
{
  this->path_weights[to] = std::numeric_limits<sint>::max();
  node_type current(to, 0);
  if(shortest) { current.second = std::numeric_limits<uint>::max(); }

  for(std::deque<node_type>::iterator iter = active_nodes.begin(); iter != active_nodes.end(); ++iter)
  {
    sint weight = this->getPathWeight((*iter).first, to, adjustment);
    if(weight > this->path_weights[to]) { return current; }
    if(weight < this->path_weights[to] ||
       (shortest && (*iter).second + 1 < current.second) ||
       (!shortest && (*iter).second + 1 > current.second))
    {
      this->path_weights[to] = weight;
      this->predecessors[to] = (*iter).first;
      current.second = (*iter).second + 1;
    }
  }

  return current;
}

bool
WeightedSampler::bridge(uint a, uint b, uint c, sint adjustment)
{
/*
  Note that 'c' is always less than 'this->size', and 'b' will not be added to active nodes,
  unless it offers a strictly better path to 'this->size' than 'a'.
*/

  uint low = c + 1, high = this->size;
  while(low < high)
  {
    uint mid = low + (high - low) / 2;
    if(this->isStrictlyBetter(a, b, mid, adjustment)) { low = mid + 1; }
    else { high = mid; }
  }
  // 'high' will now be the first position for which 'a' is not strictly better than 'b'.

  return this->isStrictlyBetter(c, b, high, adjustment);
}

//--------------------------------------------------------------------------

void
WeightedSampler::calculateEdges(int* weights)
{
  if(this->use_psi)
  {
    this->edge_totals[0] = 0;
    for(uint i = 1; i < this->size; i++)
    {
      this->edge_totals[i] = weights[i];
    }
    this->edge_totals[this->size] = 0;

    this->edge_weights[0] = 0;
    for(uint i = 1; i <= this->size; i++)
    {
      this->edge_weights[i] = this->edge_weights[i - 1] + this->edge_totals[i - 1];
      this->edge_totals[i] += this->edge_totals[i - 1];
    }
  }
  else
  {
    for(uint i = 0; i < this->size; i++)
    {
      this->edge_totals[i] = weights[i];
    }
    this->edge_totals[this->size] = 0;

    this->edge_weights[this->size] = 0;
    for(uint i = this->size; i > 0; i--)
    {
      this->edge_weights[i - 1] = this->edge_weights[i] + this->edge_totals[i];
      this->edge_totals[i - 1] += this->edge_totals[i];
    }
  }
}

sint
WeightedSampler::getEdgeWeight(uint from, uint to, sint adjustment)
{
  if(this->use_psi)
  {
    return this->edge_weights[to] - this->edge_weights[from] -
           (to - from) * this->edge_totals[from] + adjustment;
  }
  else
  {
    return this->edge_weights[from] - this->edge_weights[to] -
           (to - from) * this->edge_totals[to] + adjustment;
  }
}

sint
WeightedSampler::getPathWeight(uint through, uint to, sint adjustment)
{
  return this->path_weights[through] + this->getEdgeWeight(through, to, adjustment);
}

bool
WeightedSampler::isBetter(uint a, uint b, uint to, sint adjustment)
{
  return (this->getPathWeight(a, to, adjustment) <= this->getPathWeight(b, to, adjustment));
}

bool
WeightedSampler::isStrictlyBetter(uint a, uint b, uint to, sint adjustment)
{
  return (this->getPathWeight(a, to, adjustment) < this->getPathWeight(b, to, adjustment));
}

//--------------------------------------------------------------------------

}; // namespace TextIndexes
